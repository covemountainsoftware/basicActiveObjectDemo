
#todo: learn cmake better to find a way to
#      include the dependencies better/naturally.
include_directories(../../core/include
        ../../services/include
        ../../drivers/hwLockCtrl/include
        ../../core/activeObject/include
        ../../services/lockCtrlService/include
        ../../core/flatStateMachine/include)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(demoPcApp
        main.cpp
        ../../services/lockCtrlService/src/hwLockCtrlService.cpp
        ../../drivers/hwLockCtrl/src/hwLockCtrl.c)

target_link_libraries(demoPcApp Threads::Threads)
