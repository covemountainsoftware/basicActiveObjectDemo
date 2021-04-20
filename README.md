# Basic Active Object Demo
This project demonstrates a basic C module using an active object pattern.

This project was created to provide an example code base supporting the following post: TODO

The associated blog post details three elements required to implement an active object:

* An external API or method to send asynchronous requests
* A concurrency safe queue to hold the requests
* A thread to receive and execute the requests

In this example, the C module hwLockCtrlService (HLCS) provides these three elements.
In this module we find:
* A C based external API providing an abstraction for external users, where primary requests are all async and push events onto the internal queue.
* A private internal queue, modeled after FreeRTOS
* A private internal thread, modeled after FreeRTOS

Like the original source project, the HLCS implements a simple flat state machine to process the events.

This earlier post provided the origin for this repository and the example active object behavior.
https://covemountainsoftware.com/2020/04/17/unit-testing-active-objects-and-state-machines/

## Platform
This project should build and execute on any reasonable modern PC with the following:
* Toolchain supporting C11 and C++17.
* CMake VERSION 3.16 or newer.
* Installation of CppUTest version 3.8.

This project was confirmed in the following environments:
* macOS Catalina Version 10.15.7
* Ubuntu 20.04

## Build Targets
### HwLockCtrlServiceTests
Project demonstrating my approach to unit testing an event driven active object.

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
* [9] Earlier blog post on unit testing active objects: https://covemountainsoftware.com/2020/04/17/unit-testing-active-objects-and-state-machines/
* [10] The all C++ repo that originated this code: https://github.com/covemountainsoftware/activeObjectUnitTestingDemo

## Questions?
Contact information is available here:
https://covemountainsoftware.com/services/consulting/
