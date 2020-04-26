
include_directories(../../core/include ../../services/include ../../drivers/lockCtrl/include ../../core/activeObject/include ../../services/lockCtrlService/include ../../core/flatStateMachine/include)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(demoPcApp main.cpp ../../services/lockCtrlService/src/lockCtrlService.cpp ../../drivers/lockCtrl/src/lockCtrl.cpp)
target_link_libraries(demoPcApp Threads::Threads)
