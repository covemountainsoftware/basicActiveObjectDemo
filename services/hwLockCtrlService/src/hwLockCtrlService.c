/*
MIT License

Copyright (c) <2019-2020> <Matthew Eshleman - https://covemountainsoftware.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <stdatomic.h>
#include <assert.h>
#include <stdio.h>
#include "hwLockCtrlService.h"
#include "hwLockCtrl.h"
#include "fauxQueue.h"
#include "fauxThread.h"

typedef enum Signal
{
    SM_ENTER,
    SM_EXIT,
    SIG_REQUEST_LOCKED,
    SIG_REQUEST_UNLOCKED,
    SIG_REQUEST_SELF_TEST,
    SIG_REQUEST_THREAD_EXIT
} SignalT;

typedef struct HLCS_EventType
{
    SignalT signal;
} HLCS_EventTypeT;

//internal prototypes
typedef void* StateRtn;
typedef StateRtn (*HLCS_StateMachineFunc)(const HLCS_EventTypeT * const event);
static void HLCS_PerformSelfTest();
static void HLCS_NotifyChangedState(HLCS_LockStateT state);
static void HLCS_PushEvent(SignalT sig);
static void HLCS_PushUrgentEvent(SignalT sig);
static void HLCS_SmProcess(const HLCS_EventTypeT * event);
static void  HLCS_SmInitialize();
static void* HLCS_SmInitialPseudoState(const HLCS_EventTypeT* const event);
static void* HLCS_SmLocked(const HLCS_EventTypeT* const event);
static void* HLCS_SmUnlocked(const HLCS_EventTypeT* const event);
static void* HLCS_SmSelfTest(const HLCS_EventTypeT* const event);
static void HLCS_Task(void);

//constants
static const size_t QueueDepth = 10;
static const HLCS_EventTypeT ExitEvent = { .signal = SM_EXIT};
static const HLCS_EventTypeT EnterEvent = { .signal = SM_ENTER};

//module static variables
static _Atomic HLCS_LockStateT s_lockState = HLCS_LOCK_STATE_UNKNOWN;
static atomic_bool s_exitThread = false;
static TaskHandle_t s_thread = NULL;
static QueueHandle_t s_eventQueue = NULL;
static HLCS_ChangeStateCallback s_stateChangedCallback = NULL;
static HLCS_SelfTestResultCallback s_selfTestResultCallback = NULL;
static HLCS_StateMachineFunc s_currentState = NULL;
static HLCS_StateMachineFunc s_stateHistory = NULL;

//internal macros for state machine readability
#define TransitionTo(x) (x)
#define Handled() (s_currentState)

void HLCS_Init()
{
    //ensure Init is being called appropriately
    assert(s_lockState == HLCS_LOCK_STATE_UNKNOWN);
    assert(s_thread == NULL);
    assert(s_eventQueue == NULL);
    assert(s_stateChangedCallback == NULL);
    assert(s_selfTestResultCallback == NULL);
    assert(s_currentState == NULL);
    assert(s_stateHistory == NULL);
    assert(s_exitThread == false);

    s_eventQueue = xQueueCreate(QueueDepth, sizeof(HLCS_EventTypeT));

    //thread is created in Start()
}

void HLCS_Destroy()
{
    if (s_eventQueue != NULL)
    {
        s_exitThread = true;
        HLCS_PushUrgentEvent(SIG_REQUEST_THREAD_EXIT);
        vTaskDelete(s_thread);
        vQueueDelete(s_eventQueue);
    }
    s_lockState = HLCS_LOCK_STATE_UNKNOWN;
    s_eventQueue = NULL;
    s_stateChangedCallback = NULL;
    s_selfTestResultCallback = NULL;
    s_currentState = NULL;
    s_stateHistory = NULL;
    s_exitThread = false;
    s_thread = NULL;
}

void HLCS_Start(ExecutionOptionT option)
{
    assert(s_currentState == NULL);
    assert(s_thread == NULL);

    if (EXECUTION_OPTION_NORMAL == option)
    {
        bool ok = xTaskCreate(HLCS_Task, "HLCS", 2000, &s_thread);
        assert(ok == true);
    }
    else
    {
        HLCS_SmInitialize();
    }
}

HLCS_LockStateT HLCS_GetState()
{
    return atomic_load(&s_lockState);
}

void HLCS_RegisterChangeStateCallback(HLCS_ChangeStateCallback callback)
{
    s_stateChangedCallback = callback;
}

void HLCS_RegisterSelfTestResultCallback(HLCS_SelfTestResultCallback callback)
{
    s_selfTestResultCallback = callback;
}

void HLCS_RequestLockedAsync()
{
    HLCS_PushEvent(SIG_REQUEST_LOCKED);
}

void HLCS_RequestUnlockedAsync()
{
    HLCS_PushEvent(SIG_REQUEST_UNLOCKED);
}

void HLCS_RequestSelfTestAsync()
{
    HLCS_PushEvent(SIG_REQUEST_SELF_TEST);
}

bool HLCS_ProcessOneEvent(ExecutionOptionT option)
{
    if ((EXECUTION_OPTION_UNIT_TEST == option) &&
        (0 == uxQueueMessagesWaiting(s_eventQueue)))
    {
        return false;
    }

    HLCS_EventTypeT event;
    bool ok = xQueueReceive(s_eventQueue, &event);
    if (!ok)
    {
        return false;
    }

    if (event.signal == SIG_REQUEST_THREAD_EXIT)
    {
        s_exitThread = true;
        return false;
    }

    HLCS_SmProcess(&event);
    return true;
}

void HLCS_PushEvent(SignalT sig)
{
    HLCS_EventTypeT event =
      {
        .signal = sig
      };
    bool ok = xQueueSendToBack(s_eventQueue, &event);
    if (!ok)
    {
        fprintf(stderr, "HLCS queue send failed for sig %d!\n", sig);
        assert(false);
    }
}

void HLCS_PushUrgentEvent(SignalT sig)
{
    HLCS_EventTypeT event =
      {
        .signal = sig
      };
    bool ok = xQueueSendToFront(s_eventQueue, &event);
    if (!ok)
    {
        fprintf(stderr, "HLCS queue send failed for sig %d!\n", sig);
        assert(false);
    }
}

void HLCS_SmProcess(const HLCS_EventTypeT * event)
{
    void* rtn = s_currentState(event);
    if (rtn != (void*)s_currentState)
    {
        s_currentState(&ExitEvent);
        s_currentState = rtn;
        s_currentState(&EnterEvent);
    }
}

void HLCS_NotifyChangedState(HLCS_LockStateT state)
{
    s_lockState = state;
    if (s_stateChangedCallback)
    {
        s_stateChangedCallback(s_lockState);
    }
}

void HLCS_NotifySelfTestResult(HLCS_SelfTestResultT result)
{
    if (s_selfTestResultCallback)
    {
        s_selfTestResultCallback(result);
    }
}

void  HLCS_SmInitialize()
{
    //get the initial desired state
    s_currentState = HLCS_SmInitialPseudoState;
    s_currentState = s_currentState(&EnterEvent);

    //now enter the initial desired state
    s_currentState(&EnterEvent);
}

void* HLCS_SmInitialPseudoState(const HLCS_EventTypeT* const event)
{
    (void)event;

    HwLockCtrlInit();
    return TransitionTo(HLCS_SmLocked);
}

void* HLCS_SmLocked(const HLCS_EventTypeT* const event)
{
    StateRtn rtn;
    switch (event->signal)
    {
    case SM_ENTER:
        HwLockCtrlLock();
        HLCS_NotifyChangedState(HLCS_LOCK_STATE_LOCKED);
        rtn = Handled();
        break;
    case SM_EXIT:
        rtn = Handled();
        s_stateHistory = HLCS_SmLocked;
        break;
    case SIG_REQUEST_LOCKED:
        rtn = Handled();
        break;
    case SIG_REQUEST_UNLOCKED:
        rtn = TransitionTo(HLCS_SmUnlocked);
        break;
    case SIG_REQUEST_SELF_TEST:
        rtn = TransitionTo(HLCS_SmSelfTest);
        break;
    default:
        rtn = Handled();
        break;
    }

    return rtn;
}

void* HLCS_SmUnlocked(const HLCS_EventTypeT* const event)
{
    StateRtn rtn;
    switch (event->signal)
    {
    case SM_ENTER:
        rtn = Handled();
        HwLockCtrlUnlock();
        HLCS_NotifyChangedState(HLCS_LOCK_STATE_UNLOCKED);
        break;
    case SM_EXIT:
        rtn = Handled();
        s_stateHistory = HLCS_SmUnlocked;
        break;
    case SIG_REQUEST_LOCKED:
        rtn = TransitionTo(HLCS_SmLocked);
        break;
    case SIG_REQUEST_UNLOCKED:
        rtn = Handled();
        break;
    case SIG_REQUEST_SELF_TEST:
        rtn = TransitionTo(HLCS_SmSelfTest);
        break;
    default:
        rtn = Handled();
        break;
    }

    return rtn;
}

void* HLCS_SmSelfTest(const HLCS_EventTypeT* const event)
{
    StateRtn rtn;
    switch (event->signal)
    {
    case SM_ENTER:
        HLCS_PerformSelfTest();
        rtn = Handled();
        break;
    case SIG_REQUEST_LOCKED:
        rtn = TransitionTo(HLCS_SmLocked);
        break;
    case SIG_REQUEST_UNLOCKED:
        rtn = TransitionTo(HLCS_SmUnlocked);
        break;

    case SIG_REQUEST_SELF_TEST: //purposeful fallthrough
    default:
        rtn = Handled();
        break;
    }

    return rtn;
}

void HLCS_PerformSelfTest()
{
    HwLockCtrlSelfTestResultT result;
    bool ok = HwLockCtrlSelfTest(&result);
    if (ok && (result == HW_LOCK_CTRL_SELF_TEST_PASSED))
    {
        HLCS_NotifySelfTestResult(HLCS_SELF_TEST_RESULT_PASS);
    }
    else
    {
        HLCS_NotifySelfTestResult(HLCS_SELF_TEST_RESULT_FAIL);
    }

    //remind self to transition back to
    //history per this service's requirements
    //note the use of "PostUrgent" as per:
    //
    // https://covemountainsoftware.com/2020/03/08/uml-statechart-handling-errors-when-entering-a-state/
    //
    if (s_stateHistory == HLCS_SmUnlocked)
    {
        HLCS_PushUrgentEvent(SIG_REQUEST_UNLOCKED);
    }
    else
    {
        HLCS_PushUrgentEvent(SIG_REQUEST_LOCKED);
    }
}

void HLCS_Task(void)
{
    HLCS_SmInitialize();
    while (!s_exitThread)
    {
        HLCS_ProcessOneEvent(EXECUTION_OPTION_NORMAL);
    }
}
