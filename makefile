############################
#         makefile         #
#     for scheduler and    #
#       create_tasks       #
############################

CC = gcc
CCFLAGS = -Wall -pedantic -ansi -g
OBJ = scheduler.o linked_list.o
OBJ2 = create_tasks.o
EXEC = scheduler
EXEC2 = create_tasks

default : clean $(EXEC) $(EXEC2)

create_tasks : $(OBJ2)
	$(CC) $(OBJ2) -o $(EXEC2)

scheduler : $(OBJ)
	$(CC) -pthread $(OBJ) -o $(EXEC)

scheduler.o : scheduler.c linked_list.h
	$(CC) -c scheduler.c $(CCFLAGS)

linked_list.o : linked_list.c linked_list.h
	$(CC) -c linked_list.c $(CCFLAGS)

create_tasks.o : create_tasks.c
	$(CC) -c create_tasks.c $(CCFLAGS)

clean:
	rm -f $(EXEC) $(OBJ) $(EXEC2) $(OBJ2)
