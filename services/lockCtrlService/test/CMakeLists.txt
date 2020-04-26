set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(TEST_APP_NAME LockCtrlServiceTests)

#note: we are building and linking with the MOCK LockCtrl module, instead
#      of the actual LockCtrl driver.
set(TEST_SOURCES lockCtrlServiceTests.cpp ../../../test/common/cpputestMain.cpp ../src/lockCtrlService.cpp ../../../test/mocks/lockCtrl/mockLockCtrl.cpp)

include(../../../test/common/cpputestCMake.txt)

target_link_libraries(${TEST_APP_NAME} Threads::Threads)