# Basic Active Object Demo
C (and behind the scenes C++) project demonstrating a basic C module using the active object pattern.

This project was created to provide an example code base supporting the following post:

TODO:

The following post provided the origin for this repository.
https://covemountainsoftware.com/2020/04/17/unit-testing-active-objects-and-state-machines/

## Platform
This project and associated code were developed and tested in Ubuntu 16.04 using CLion 2020.1.
Recently confirmed in OSX 10.15.7 , again using CLion 2020.1.

The project requires an installation of CppUTest version 3.8.

## Build Targets
### HwLockCtrlServiceTests
Project demonstrating my approach to unit test an event driven active object.

### demoPcApp
This target is a trivial terminal demo app showing the target service in action "for real."

## References and Inspiration
* [0] The blog post for this code: 
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
