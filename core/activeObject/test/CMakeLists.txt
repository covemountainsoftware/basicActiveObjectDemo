set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(TEST_APP_NAME cmsActiveObjectTests)
set(TEST_SOURCES cmsActiveObjectTests.cpp ../../../test/common/cpputestMain.cpp TestStdActiveObject.cpp TestStdActiveObject.hpp)

include(../../../test/common/cpputestCMake.txt)

target_link_libraries(${TEST_APP_NAME} Threads::Threads)
