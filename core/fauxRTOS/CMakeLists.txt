include_directories(include)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
add_library(fauxRTOS
            src/fauxQueue.cpp src/fauxThread.cpp)

target_link_libraries(fauxRTOS Threads::Threads)
