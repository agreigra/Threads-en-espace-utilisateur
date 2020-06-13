#----------FOLDERS-------------
INSTALL_DIR=install
BIN_DIR = $(INSTALL_DIR)/bin
PTHREAD_DIR = $(INSTALL_DIR)/pthread
LIB_DIR=$(INSTALL_DIR)/lib
TST_DIR = tests
GRAPH_DIR = graph
DAT_DIR = $(GRAPH_DIR)/dat
PNG_DIR= $(GRAPH_DIR)/png

#----------PARAMETERS-------------
THREAD_NUMBER = 50
YIELD_NUMBER = 25
FIBO = 15
SUM = 50

#----------FLAGS-------------
CC=gcc
DEBUG=no #yes
ifeq ($(DEBUG),yes)
	CFLAGS=-Wall -Wextra -g -O0
else
	CFLAGS=-Wall -Wextra
endif
STDFLAGS = -DUSE_PTHREAD -pthread
VALGRINDFLAGS = --leak-check=full --track-origins=yes --show-leak-kinds=all
FLIB=-L$(LIB_DIR) -lthread -lrt
PRE=-DPREEMPTION

#----------OTHER-------------
BINARIES = 01-main 02-switch 11-join 12-join-main 21-create-many 22-create-many-recursive 23-create-many-once 31-switch-many 32-switch-many-join 33-switch-many-cascade 51-fibonacci 61-mutex 62-mutex 71-example 72-sum-rec 73-quicksort 74-merge-sort 75-signal 76-preemption
EXEC1 = check_main check_switch check_join check_join-main check_create-many check_create-many-recursive check_create-many-once check_switch-many check_switch-many-join check_switch-many-cascade check_mutex check_mutex-2 check_example
EXEC2 = check_sum-rec check_quicksort check_merge-sort
EXEC1_STD = $(EXEC1:=_std)
EXEC2_STD = $(EXEC2:=_std)
GRAPH=graph_create_many graph_create_many_recursive graph_create_many_once graph_switch_many graph_switch_many_join graph_switch_many_cascade graph_sum_rec graph_quicksort graph_merge_sort

$(LIB_DIR)/libthread.so: thread.c create_dir
	@$(CC) $(PRE) -o $@ $< -fPIC -shared

all: install

# compile with project code
install: $(BINARIES)

# check with project code
check: $(EXEC1) $(EXEC2) check_fibonacci check_signal check_preemption

# check with std library
check_pthread: $(EXEC1_STD) $(EXEC2_STD) check_fibonacci_std

graph: $(GRAPH) graph_fibonacci

create_dir:
	@mkdir -p $(BIN_DIR) $(PTHREAD_DIR) $(LIB_DIR) $(DAT_DIR) $(PNG_DIR)

clean:
	@rm -rf $(INSTALL_DIR) $(DAT_DIR) $(PNG_DIR)


###-----------------------------------------------------------------------------
### using the project code :
###-----------------------------------------------------------------------------

# Compilation :

$(BINARIES): create_dir $(LIB_DIR)/libthread.so
	@echo -n "Compilation de $@ : "; $(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $(TST_DIR)/$@.c $(FLIB); echo "Réussie"

# Exécution :

check_main: 01-main
check_switch: 02-switch
check_join: 11-join
check_join-main: 12-join-main
check_create-many: 21-create-many
check_create-many-recursive: 22-create-many-recursive
check_create-many-once: 23-create-many-once
check_switch-many: 31-switch-many
check_switch-many-join: 32-switch-many-join
check_switch-many-cascade: 33-switch-many-cascade
check_mutex: 61-mutex
check_mutex-2: 62-mutex
check_example: 71-example
check_signal: 75-signal
check_preemption: 76-preemption
$(EXEC1) check_signal check_preemption:
	@echo "\nExécution de $< :";	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$$PWD/install/lib $(BIN_DIR)/$< $(THREAD_NUMBER) $(YIELD_NUMBER); echo "\n"

check_sum-rec: 72-sum-rec
check_quicksort: 73-quicksort
check_merge-sort: 74-merge-sort
$(EXEC2):
	@echo "\nExécution de $< :\n";	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$$PWD/install/lib $(BIN_DIR)/$< $(SUM) $(PRE); echo "\n"

check_fibonacci: 51-fibonacci
	@echo "\nExécution de $< :\n";	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$$PWD/install/lib $(BIN_DIR)/$< $(FIBO) $(PRE); echo "\n"

# execute with valgrind
valgrind: install
	@for t in '01-main' '02-switch' '11-join' '12-join-main' '21-create-many' '22-create-many-recursive' '23-create-many-once' '31-switch-many' '32-switch-many-join' '33-switch-many-cascade' '51-fibonacci' '61-mutex' '62-mutex' '71-example' '72-sum-rec' '73-quicksort' '74-merge-sort' '75-signal' '76-preemption'; \
	do \
        echo "\nLancement de valgrind sur $$t :\n" ; LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$$PWD/install/lib valgrind $(VALGRINDFLAGS) $(BIN_DIR)/$$t; \
  done

###-----------------------------------------------------------------------------
### using std library :
###-----------------------------------------------------------------------------

# Compilation :

01-main_std: $(TST_DIR)/01-main.c create_dir
02-switch_std: $(TST_DIR)/02-switch.c create_dir
11-join_std: $(TST_DIR)/11-join.c create_dir
12-join-main_std: $(TST_DIR)/12-join-main.c create_dir
21-create-many_std: $(TST_DIR)/21-create-many.c create_dir
22-create-many-recursive_std: $(TST_DIR)/22-create-many-recursive.c create_dir
23-create-many-once_std: $(TST_DIR)/23-create-many-once.c create_dir
31-switch-many_std: $(TST_DIR)/31-switch-many.c create_dir
32-switch-many-join_std: $(TST_DIR)/32-switch-many-join.c create_dir
33-switch-many-cascade_std: $(TST_DIR)/33-switch-many-cascade.c create_dir
51-fibonacci_std: $(TST_DIR)/51-fibonacci.c create_dir
61-mutex_std: $(TST_DIR)/61-mutex.c create_dir
62-mutex_std: $(TST_DIR)/62-mutex.c create_dir
71-example_std: $(TST_DIR)/71-example.c create_dir
72-sum-rec_std: $(TST_DIR)/72-sum-rec.c create_dir
73-quicksort_std: $(TST_DIR)/73-quicksort.c create_dir
74-merge-sort_std: $(TST_DIR)/74-merge-sort.c create_dir
%_std:
	@echo "Compilation de $@"; $(CC) $< -o $(PTHREAD_DIR)/$@ $(STDFLAGS);

# Exécution :

check_main_std: 01-main_std
check_switch_std: 02-switch_std
check_join_std: 11-join_std
check_join-main_std: 12-join-main_std
check_create-many_std: 21-create-many_std
check_create-many-recursive_std: 22-create-many-recursive_std
check_create-many-once_std: 23-create-many-once_std
check_switch-many_std: 31-switch-many_std
check_switch-many-join_std: 32-switch-many-join_std
check_switch-many-cascade_std: 33-switch-many-cascade_std
check_mutex_std: 61-mutex_std
check_mutex-2_std: 62-mutex_std
check_example_std: 71-example_std
$(EXEC1_STD):
	@echo "\nExécution de $< :\n"; $(PTHREAD_DIR)/$< $(THREAD_NUMBER) $(YIELD_NUMBER)

check_sum-rec_std: 72-sum-rec_std
check_quicksort_std: 73-quicksort_std
check_merge-sort_std: 74-merge-sort_std
$(EXEC2_STD):
	@echo "\nExécution de $< :\n"; $(PTHREAD_DIR)/$< $(SUM)

check_fibonacci_std: 51-fibonacci_std
	@echo "\nExécution de $< :\n"; $(PTHREAD_DIR)/$< $(FIBO)

###-----------------------------------------------------------------------------
###                                Graph
###-----------------------------------------------------------------------------

graph_create_many: 21-create-many 21-create-many_std $(GRAPH_DIR)/create_graph.sh
graph_create_many_recursive: 22-create-many-recursive 22-create-many-recursive_std $(GRAPH_DIR)/create_graph.sh
graph_create_many_once: 23-create-many-once 23-create-many-once_std $(GRAPH_DIR)/create_graph.sh
graph_switch_many: 31-switch-many 31-switch-many_std $(GRAPH_DIR)/create_graph.sh
graph_switch_many_join: 32-switch-many-join 32-switch-many-join_std $(GRAPH_DIR)/create_graph.sh
graph_switch_many_cascade: 33-switch-many-cascade 33-switch-many-cascade_std $(GRAPH_DIR)/create_graph.sh
graph_sum_rec: 72-sum-rec 72-sum-rec_std $(GRAPH_DIR)/create_graph.sh
graph_quicksort: 73-quicksort 73-quicksort_std $(GRAPH_DIR)/create_graph.sh
graph_merge_sort: 74-merge-sort 74-merge-sort_std $(GRAPH_DIR)/create_graph.sh
$(GRAPH):
	@echo -n "\nCréation du graph pour $< : "; $(GRAPH_DIR)/create_graph.sh $(BIN_DIR)/$< $(PTHREAD_DIR)/$<_std $(DAT_DIR)/$<.dat $(DAT_DIR)/$<_std.dat $(PNG_DIR)/$<.png "$<" $(THREAD_NUMBER) $(DAT_DIR)/$<_premp.dat; echo "terminée\n"

graph_fibonacci: 51-fibonacci 51-fibonacci_std $(GRAPH_DIR)/create_graph.sh
	@echo -n "\nCréation du graph pour $< : "; $(GRAPH_DIR)/create_graph.sh $(BIN_DIR)/$< $(PTHREAD_DIR)/$<_std $(DAT_DIR)/$<.dat $(DAT_DIR)/$<_std.dat $(PNG_DIR)/$<.png "$<" $(FIBO) $(DAT_DIR)/$<_premp.dat; echo "terminée\n"
