set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(demoPcApp main.cpp)

target_link_libraries(demoPcApp Threads::Threads hwLockCtrlService)
