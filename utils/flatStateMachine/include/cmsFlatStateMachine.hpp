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

#ifndef CMSFLATSTATEMACHINE_HPP
#define CMSFLATSTATEMACHINE_HPP

#include <cassert>
#include <type_traits>
#include "cmsStandardSignals.hpp"
#include "cmsBaseEvent.hpp"
#include "cmsTypeUtils.hpp"

namespace CMS
{

/**
 * @brief The FlatStateMachine class provides for simple flat statemachine
 *        functionality.
 *
 * @param template param EventT: event type processed by this state machine
 *
 * @note derived state machines must provide a concrete implementation of
 *       the initial pseudo state via the method InitialPseudoState().
 */
template<typename EventT>
class FlatStateMachine
{
public:
    static_assert(true == is_base_of_any<CMS::BaseEvent, EventT>::value,
      "The EventT type must be derived from the type CMS::BaseEvent.");
    /**
     * @brief Initialize - method to call to initialize the state machine.
     *                     Must be called before any event processing.
     */
    void Initialize();

    /**
     * @brief ProcessEvent - process a single event.
     *
     * @param event - ptr to the event to process
     *
     */
    void ProcessEvent(const EventT* const event);

protected:
    struct StateRtn;
    using StateMethodHandler = StateRtn (FlatStateMachine<EventT>::*)(const EventT* const event);
    struct StateRtn
    {
        StateRtn() = default;

        explicit StateRtn(StateMethodHandler method) : mMethod(method)
        {
        }

        StateMethodHandler mMethod;
    };

    /**
     * @brief InitialPseudoState - pure virtual. Derived class must provide
     *                             a concrete implementation.
     *
     * @return The state to transition to
     *
     * @note: the implementation MUST return a handler via the TransitionTo() method
     */
    virtual StateRtn InitialPseudoState(const EventT* const event) = 0;

    /**
     * @brief TransitionTo - Request a Local Transition to the desired target state
     * @param targetState - pointer to member, i.e the handler for the target state
     */
    template<typename StateMethodT>
    StateRtn TransitionTo(StateMethodT targetState)
    {
        return StateRtn(static_cast<StateMethodHandler>(targetState));
    };

    /**
     * @brief Handled - The event was handled.
     * @return
     */
    StateRtn Handled()
    {
        return StateRtn(static_cast<StateMethodHandler>(mCurrent));
    }

private:
    const EventT EnterEvent = EventT(SM_ENTER);
    const EventT ExitEvent = EventT(SM_EXIT);

    StateMethodHandler mCurrent = nullptr;
};

template<typename EventT>
void FlatStateMachine<EventT>::Initialize()
{
    assert(mCurrent == nullptr);

    //get the initial desired state
    mCurrent = &FlatStateMachine<EventT>::InitialPseudoState;
    auto rtn = ((this)->*(mCurrent))(&EnterEvent);

    //now enter the initial desired state
    mCurrent = rtn.mMethod;
    ((this)->*(mCurrent))(&EnterEvent);
}

template<typename EventT>
void FlatStateMachine<EventT>::ProcessEvent(const EventT* const event)
{
    auto rtn = ((this)->*(mCurrent))(event);
    if (rtn.mMethod != mCurrent)
    {
        ((this)->*(mCurrent))(&ExitEvent);
        mCurrent = rtn.mMethod;
        ((this)->*(mCurrent))(&EnterEvent);
    }
}

} //namespace CMS

#endif //CMSFLATSTATEMACHINE_HPP
