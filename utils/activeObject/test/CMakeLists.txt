set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(TEST_APP_NAME cmsActiveObjectTests)
set(TEST_SOURCES cmsActiveObjectTests.cpp ../../../testCommon/cpputest_main.cpp TestStdActiveObject.cpp TestStdActiveObject.hpp)

include(../../../testCommon/cpputestCMake.txt)

target_link_libraries(${TEST_APP_NAME} Threads::Threads)
