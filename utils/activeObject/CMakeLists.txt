include_directories(include)
include_directories(../flatStateMachine/include)
add_subdirectory(test)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
