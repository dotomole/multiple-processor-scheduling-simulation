#OS ASSIGNMENT - Multiple-Processor Scheduling Simulation

The goal of this assignment was to gain experience in using the POSIX Pthreads library for thread creations and synchronizations by simulating a Multithreaded CPU scheduler.

1. run makefile

correct usage:
`make`

this will remove any .o and exec files and compile program.

2. create task_file with create_tasks

correct usage:
`./create_tasks <num-tasks> <cpu-burst-limit>`
`./create_tasks 100 50`
(For testing purposes recommend less tasks and lower limit)
Will output to file called "task_file"

3. simulate program with scheduler

correct usage:
`./scheduler <filename> <queue-capacity>`
`./scheduler task_file 4`

Will output to file called "simulation_log"

4. Can then view simulation_log by any means... :)