# Active Object and State Machine Unit Testing Demo
C and C++ project demonstrating active object and state machine unit testing, using CppUTest.

## Platform
This project and associated code were developed and tested in Ubuntu 16.04 using CLion 2020.1.

The project requires an installation of CppUTest version 3.8. 

## Build Targets
### HwLockCtrlServiceTests
The primary focus should be the "HwLockCtrlServiceTests" build target and associated code. See the
folder "services/hwLockCtrlService" and associated code and tests.

### cmsActiveObjectTests
Secondary focus may be given to this target and associated code. 
These classes create the foundation needed for the primary code/focus.
 
### cmsFlatStateMachineTests
Secondary focus may be given to this target and associated code. 
This trivial flat state machine class create the foundation for this project's 
demonstration active object class.

### demoPcApp
This target is a trivial terminal demo app showing the target service in action "for real."

## References and Inspiration
* [1] Sutter, Herb. Prefer Using Active Objects Instead of Naked Threads. Dr. Dobbs, June 2010. https://www.drdobbs.com/parallel/prefer-using-active-objects-instead-of-n/225700095
* [2] Grenning, James. Test Driven Development for Embedded C. https://amzn.to/2YbANIG 
* [3] Samek, Miro. Practical UML Statecharts in C/C++: Event-Driven Programming for Embedded Systems. https://amzn.to/3eXdhoz
* [4] Just Software Solutions Ltd blog: Implementing a Thread-Safe Queue using Condition Variables. https://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html
* [5] Jason Turner's C++ starter kit: https://github.com/lefticus/cpp_starter_project
* [6] Lars Melchior's C++ starter kit: https://github.com/TheLartians/ModernCppStarter
* [7] Guy Morand (r4nd0m6uy) blog on cmake and cpputest: https://r4nd0m6uy.ch/cmake-and-cpputest.html
* [8] Davis Ford's github repo on cmake and cpputest: https://github.com/davisford/cmake-cpputest

## Questions?
Contact information is available here:
https://covemountainsoftware.com/services/consulting/
