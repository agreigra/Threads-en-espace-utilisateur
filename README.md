# Operating system project

This project aims at creating a library to manipulate thread with an interface inspired by the pthread library.

## The project structure

.
/Makefile
/README.md
/thread.c
/thread.h
/queue.h
/tests
--/create_many_once.c
--/create_many_recursive.c
--/create_many.c
--/fibonacci.c
--/join_main.c
--/join.c
--/main.c
--/switch_many_join.c
--/switch_many.c
--/switch.c
--/example.c
--/mutex.c
--/mutex-2.c
--/merge-sort_no_thread.c
--/merge-sort.c
--/quicksort_no_threads.c
--/quicksort.c
--/sum-rec_no_threads.c
--/sum-rec.c
/graph
--/create_graph.sh
requirements.txt

* The makefile implements commands that are described in the next section.
* The thread.c/thread.h files contain our implementation of the thread library.
* The queue.h file contains various implementations of queues, we use only use it, we have not implemented it(see copyright in the file).
* The tests repository contains various examples of what could be asked to this  kind of library. We use them to verify if our implementation works correctly and test its performances.
* The script create_graph.sh is used to generate graphs comparing the performances of our implementation --with and without the preemption-- and the ones of pthread.
* The files requirements.txt contains the command lines to install the packages needed to use this project.

## Usage

To compile our library and the tests you can use the commands *make* or *make install*.

The tests can be launch on our implementation using *make check*.

If you want to compile or execute a single test you can find the corresponding command in the makefile. You can also edit the makefile to desactivate the preemption or to change the value used for the tests.

It is also possible to launch those tests with the pthread library for comparison using *make check_pthread*.

If you want a visual comparison of the result with our implementation and the pthread library you can use the command *make graph*.
You can then find the resulting images in the repository graph/png.

If you want to check the tests with valgrind simply use *make valgrind*.

The compilation will create a repository install with four other subdirectory :
* a bin subdirectory for binary files,
* a lib subdirectory for our library,
* an object subdirectory for object files,
* a pthread subdirectory for the binary files using pthread.

Those directories can be removed using *make clean*.

## Authors
Corentin Le Métayer
Ahmed Greigra
Esther Félix
Sarah Larroze-Jardiné
Jérôme Faure
