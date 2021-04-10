include_directories(include)
add_subdirectory(test)
add_library(hwLockCtrlService include/hwLockCtrlService.h src/hwLockCtrlService.c)
target_link_libraries(hwLockCtrlService hwLockCtrl)
target_include_directories(hwLockCtrlService PUBLIC
        include
        ../../core/include
        ../../services/include
        ../../core/activeObject/include
        ../../core/flatStateMachine/include)
