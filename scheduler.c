/***************************
* FILE: scheduler.c        *
* AUTHOR: Thomas Di Pietro *
* DATE: 05/05/2019         *
*                          *
* PURPOSE: To simulate     *
* multiple processor       * 
* scheduling through       *
* multithreading and       *
* synchronization          *      
***************************/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include "linked_list.h"

/*GLOBAL VARIABLES*/

/*Ready-Queue*/
LinkedList* readyQueue = NULL;

int num_tasks = 0;
int total_waiting_time = 0;
int total_turnaround_time = 0;

/*init values, cpu begins asleep
task begins awake*/
int cpuSleep = 1;
int taskSleep = 0;

/*cpu task count*/
int cpu1tasks = 0;
int cpu2tasks = 0;
int cpu3tasks = 0;

/*how many tasks have been queued*/
int queuedTasks = 0;

/*cmd line arg for filename
saves the hassle of passing it
through functions*/
char filename[20];

/*Mutex & conditionals*/
pthread_mutex_t mutex;

pthread_cond_t cpu1wake;
pthread_cond_t cpu2wake;
pthread_cond_t cpu3wake;
pthread_cond_t taskWake;

/*Simulates sleep*/
void wait(int seconds)
{
    time_t startTime, endTime;

    startTime = time(NULL);
    endTime = time(NULL) + (time_t)seconds;

    while (startTime != endTime)
    {
        startTime = time(NULL);
    }
}

/*Grabs number of lines from file*/
int getNumLines()
{
    FILE* readTasks = fopen(filename, "r");
    int length = 1, ch = 0;
    while ((ch = fgetc(readTasks)) != EOF)
    {
        if (ch == '\n')
        {
            length++;
        }
    }
    fclose(readTasks);
    return length;
}

void getCurrentTime(int* timeSec)
{
    int hour, min, sec;
    time_t mytime = time(NULL);
    char* time_str = ctime(&mytime);
    sscanf(time_str, "%*s %*s %*d %d:%d:%d %*d", &hour, &min, &sec);

    *timeSec = (hour*60*60)+(min*60)+sec;
}

void convSecToTime(int* timeSec, int* hour, int* min, int* sec)
{
    double tempHr, tempMin, tempSec;

    /*for hours*/
    *hour = *timeSec / 3600; /*as they ints, truncates to hour*/
    
    /*for minutes*/
    tempHr = (double)*timeSec / 3600;
    tempMin = (tempHr - *hour) * 60;
    *min = (int)tempMin;

    /*for seconds*/
    tempSec = (tempMin - *min) * 60;
    *sec = (int)tempSec;
}

/*Will import node (that has all times) and type var
to determine which type of data to write to log*/
/*Added: cpuNum param for specific CPU terminate line*/
void writeToLog(LinkedListNode* taskNode, int type, int* cpuNum)
{
    /*appends to logFile*/
    FILE* logFile = fopen("simulation_log", "a");

    /*Type 1:
    task#: cpu_burst
    Arrival time: (time)*/
    if (type == 1)
    {
        /*arrival time vars*/
        int atHr, atMin, atSec;

        /*Converts total time in seconds to seperate variables (for log)*/
        convSecToTime(taskNode->curTask->arrivalTime, &atHr, &atMin, &atSec);

        fprintf(logFile, "%d: %d\n", *(taskNode->taskNum), *(taskNode->cpu_burst));
        fprintf(logFile, "Arrival time: %02d:%02d:%02d\n\n", atHr,atMin,atSec);
        fflush(logFile);
    }
    /*Type 2:
    Statistics for CPU-1:
    Task #n
    Arrival time: (time)
    Service time: (time)*/
    else if (type == 2)
    {   
        /*arrival/service time vars*/
        int atHr, atMin, atSec, serHr, serMin, serSec;

        /*Converts total time in seconds to seperate variables (for log)*/
        convSecToTime(taskNode->curTask->arrivalTime, &atHr, &atMin, &atSec);
        convSecToTime(taskNode->curTask->serviceTime, &serHr, &serMin, &serSec);

        fprintf(logFile, "Statistics for CPU-%d\n", *(taskNode->curTask->cpuNum));
        fprintf(logFile, "Task %d\n", *(taskNode->taskNum));
        fprintf(logFile, "Arrival time: %02d:%02d:%02d\n", atHr,atMin,atSec);
        fprintf(logFile, "Service time: %02d:%02d:%02d\n\n", serHr,serMin,serSec);
        fflush(logFile);
    }
    /*Type 3:
    Statistics for CPU-1:
    Task #n
    Arrival time: (time)
    Completion time: (time)*/
    else if (type == 3)
    {
        /*arrival/comp time vars*/
        int atHr, atMin, atSec, compHr, compMin, compSec;

        /*Converts total time in seconds to seperate variables (for log)*/
        convSecToTime(taskNode->curTask->arrivalTime, &atHr, &atMin, &atSec);
        convSecToTime(taskNode->curTask->compTime, &compHr, &compMin, &compSec);       

        fprintf(logFile, "Statistics for CPU-%d\n", *(taskNode->curTask->cpuNum));
        fprintf(logFile, "Task %d\n", *(taskNode->taskNum));
        fprintf(logFile, "Arrival time: %02d:%02d:%02d\n", atHr,atMin,atSec);
        fprintf(logFile, "Completion time: %02d:%02d:%02d\n\n", compHr,compMin,compSec);
        fflush(logFile);
    }
    /*Type 4:
    Number of tasks put into Ready-Queue: #of tasks
    Terminate at time: (time)*/
    else if (type == 4)
    {
        int timeSec, hour, min, sec;
        getCurrentTime(&timeSec);
        convSecToTime(&timeSec, &hour, &min, &sec);

        fprintf(logFile, "Number of tasks put into Ready-Queue: %d\n", queuedTasks);
        fprintf(logFile, "Terminate at time: %02d:%02d:%02d\n\n", hour, min, sec);
        fflush(logFile);

    }
    /*Type 5:
    Number of tasks: #of tasks
    Average waiting time: (time)
    Average turn around time: (time)*/
    else if (type == 5)
    {
        fprintf(logFile, "Number of tasks: %d\n", num_tasks);
        fprintf(logFile, "Average waiting time: %d seconds\n", (total_waiting_time / num_tasks));
        fprintf(logFile, "Average turn around time: %d seconds\n", (total_turnaround_time / num_tasks));
        fflush(logFile);
    }
    /*Type 6:
    CPU-X terminates after servicing #of tasks*/
    else if (type == 6)
    {
        /*Take the cpuNum not from TaskInfo struct
        as that is current info, this is from the 
        end of the thread, from cpu's args*/
        if (*cpuNum == 1)
        {
            fprintf(logFile, "CPU-1 terminates after servicing %d tasks\n\n", cpu1tasks);
        }
        else if (*cpuNum == 2)
        {
            fprintf(logFile, "CPU-2 terminates after servicing %d tasks\n\n", cpu2tasks);
        }
        else if (*cpuNum == 3)
        {
            fprintf(logFile, "CPU-3 terminates after servicing %d tasks\n\n", cpu3tasks);
        }
        fflush(logFile);

    }
    fclose(logFile);

}
void taskToQueue(FILE* readTasks)
{
    LinkedListNode* taskNode = NULL;
    int timeSec;
    char line[10];

    /*Mallocs new task node for placement in readyQueue*/
    taskNode = calloc(1, sizeof(LinkedListNode));
    taskNode->taskNum = calloc(1,sizeof(int));
    taskNode->cpu_burst = calloc(1,sizeof(int));

    /*TaskNode's TaskInfo struct for times malloc all vars too*/
    taskNode->curTask = malloc(sizeof(TaskInfo));
    taskNode->curTask->arrivalTime = malloc(sizeof(int));
   
    /*read task from file*/
    fgets(line, 10, readTasks);

    /*put values from task_file into taskNode*/
    sscanf(line, "%d %d", taskNode->taskNum, taskNode->cpu_burst);
    
    /*Gets current time*/
    getCurrentTime(&timeSec);    

    /*Store arrival time*/
    *taskNode->curTask->arrivalTime = timeSec;
    
    /*Inserts task in ready queue*/
    insertLast(readyQueue, taskNode);

    /*writes activity to simulation_log*/
    writeToLog(taskNode, 1, NULL);
}

void* cpu(void* cpuArgs)
{
    /*cpuArgsFin[0] is #tasks in task_file
      cpuArgsFin[1] is CPU #*/
    int timeSec, hour, min, sec;
    int* cpuArgsFin;
    cpuArgsFin = (int*)cpuArgs;

    /*MUTEX LOCK - ensures no race conditions*/
    pthread_mutex_lock(&mutex);

    /*Loop until ALL tasks have been executed*/
    while (num_tasks < cpuArgsFin[0])
    { 
        /*while queue empty put cpu threads to sleep (Zzzzzz)*/
        while (readyQueue->count == 0 && num_tasks != cpuArgsFin[0])
        {

            /*if queue empty and task thread 
            sleeping, wakeup task thread*/
            if (readyQueue->count == 0 && taskSleep == 1)
            {
                pthread_cond_signal(&taskWake); 
            }

            /*Had issues using broadcast or one signal,
            this made it more clear to me as well.
            --Each cpu thread has own condition*/
            if (cpuArgsFin[1] == 1)
            {
                pthread_cond_wait(&cpu1wake, &mutex);
            }
            else if (cpuArgsFin[1] == 2)
            {
                pthread_cond_wait(&cpu2wake, &mutex);
            }
            else if (cpuArgsFin[1] == 3)
            {
                pthread_cond_wait(&cpu3wake, &mutex);
            }
        }

        /*Take tasks from ready queue and simulate
        cpu bursts then removes from queue
        until none are left (FIFO queue)*/
        if (readyQueue->count != 0)
        {
            /*Malloc all time values*/
            readyQueue->head->curTask->serviceTime = malloc(sizeof(int));
            readyQueue->head->curTask->compTime = malloc(sizeof(int));
            readyQueue->head->curTask->cpuNum = malloc(sizeof(int));
           
            /*Grab CPU thread number (1, 2, or 3)*/
            *(readyQueue->head->curTask->cpuNum) = cpuArgsFin[1];

            /*Grab service time and put in current TaskInfo struct
            and then write to log*/
            getCurrentTime(&timeSec);
            *(readyQueue->head->curTask->serviceTime) = timeSec;
            convSecToTime(&timeSec, &hour, &min, &sec);
            writeToLog(readyQueue->head, 2, NULL);

            /*SIMULATE CPU BURST - SLEEP/WAIT*/
            wait(*(readyQueue->head->cpu_burst));

            /*Completion time is "computed from Service Time + cpu_burst"*/
            *(readyQueue->head->curTask->compTime) = *(readyQueue->head->curTask->serviceTime) + *(readyQueue->head->cpu_burst);

            writeToLog(readyQueue->head, 3, NULL);
            
            /*Update global variables shared between threads*/
            total_turnaround_time += *(readyQueue->head->curTask->compTime) - *(readyQueue->head->curTask->arrivalTime);
            total_waiting_time += *(readyQueue->head->curTask->serviceTime) - *(readyQueue->head->curTask->arrivalTime);
            num_tasks++; 

            /*individual cpu task count update (global)*/
            if (cpuArgsFin[1] == 1)
            {
                cpu1tasks++;
            }
            else if (cpuArgsFin[1] == 2)
            {
                cpu2tasks++;
            }
            else if (cpuArgsFin[1] == 3)
            {
                cpu3tasks++;
            }

            /*Then remove task (and all its info) from queue
            which shuffles next tasks to top*/
            removeFirst(readyQueue); 
        }
    }
    pthread_mutex_unlock(&mutex);

    /*when cpu threads finish executing tasks,
    write how many tasks each individual
    cpu thread serviced*/
    writeToLog(NULL, 6, &cpuArgsFin[1]);
    return NULL;
}

void* task(void* taskArgs)
{
    /*taskArgsFin[0] is #tasks in task_file
      taskArgsFin[1] is readyQueue capacity*/
    FILE* readTasks = fopen(filename, "r");
    if (readTasks == NULL)
    {
        perror("Error: File empty!");
    }
    else
    {
        int* taskArgsFin;
        taskArgsFin = (int*)taskArgs;
        
        /*If multiple cpu threads at once
        wakeup task thread, this prevents
        problems task thread could face*/
        pthread_mutex_lock(&mutex);

        /*Loop until all tasks are handled*/
        while (queuedTasks < taskArgsFin[0])
        {

            /*While capacity full, task thread sleeps (Zzzz)
            Added: If all tasks are done - NEVER SLEEP*/
            while (readyQueue->count == taskArgsFin[1] && queuedTasks != taskArgsFin[0])
            {
                taskSleep = 1;
                pthread_cond_wait(&taskWake, &mutex);           
            }

            /*If only one task left, get only one task
            else get 2 tasks at a time. This will 
            obviously only be the case for 
            odd numbered capacities, ie. 9
            Added: OR if last task (fixed few scenarios)*/
            if (readyQueue->count == taskArgsFin[1] - 1 || queuedTasks == taskArgsFin[0] - 1)
            {
                taskToQueue(readTasks);
                queuedTasks++;
            }
            else
            {
                taskToQueue(readTasks);
                taskToQueue(readTasks);         
                queuedTasks++;
                queuedTasks++;
            }

            /*Signal cpu threads they can wakeup
            as there are tasks in ready queue
            --Also task thread can sleep*/
            /*If capacity full or all tasks have been 'taken out' of task_file, wakeup cpu*/
            if (readyQueue->count == taskArgsFin[1] || queuedTasks == taskArgsFin[0])
            {
                pthread_cond_signal(&cpu1wake);
                pthread_cond_signal(&cpu2wake);
                pthread_cond_signal(&cpu3wake);
            }

        }
        pthread_mutex_unlock(&mutex);
        fclose(readTasks);
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("Error: command-line parameters wrong.\n");
        printf("       e.g. ./scheduler <filename> <queue-capacity>\n");
    }
    else
    {
        /*Opens logFile in main, basically just to 
        empty it out from previous runs*/
        FILE* logFile = fopen("simulation_log", "w");
        
        int capacity;
        int taskArgs[2];
        int cpu1Args[2];
        int cpu2Args[2];
        int cpu3Args[2];
        int task_file_length;

        pthread_t taskT;
        pthread_t cpu1;
        pthread_t cpu2;
        pthread_t cpu3;
        pthread_attr_t attr;

        sscanf(argv[1], "%[^\n]", filename);
        sscanf(argv[2], "%d", &capacity);

        if (capacity > 1 && capacity < 10)
        {
            task_file_length = getNumLines();

            taskArgs[0] = task_file_length;
            taskArgs[1] = capacity;

            cpu1Args[0] = task_file_length;
            cpu1Args[1] = 1;/*CPU 1*/
            cpu2Args[0] = task_file_length;
            cpu2Args[1] = 2;/*CPU 2*/
            cpu3Args[0] = task_file_length;
            cpu3Args[1] = 3;/*CPU 3*/

            printf("SIMULATING...\n");

            /*creates empty ready queue*/
            readyQueue = newList();

            /*initialize mutex, conditionals & attributes*/
            pthread_mutex_init(&mutex, NULL);
            pthread_cond_init (&taskWake, NULL);
            pthread_cond_init (&cpu1wake, NULL);
            pthread_cond_init (&cpu2wake, NULL);
            pthread_cond_init (&cpu3wake, NULL);

            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

            /*Start threads*/
            pthread_create(&taskT, NULL, task, (void*)taskArgs);
            pthread_create(&cpu1, &attr, cpu, (void*)cpu1Args);
            pthread_create(&cpu2, &attr, cpu, (void*)cpu2Args);
            pthread_create(&cpu3, &attr, cpu, (void*)cpu3Args);

            pthread_join(taskT, NULL);
            writeToLog(NULL, 4, NULL); /*once task() terminates*/
         
            pthread_join(cpu1, NULL);  
            pthread_join(cpu2, NULL);  
            pthread_join(cpu3, NULL);  
            writeToLog(NULL, 5, NULL); /*once cpu's terminate*/

            if (logFile != NULL)
            {
                printf("SUCCESS - Simulation written to \"simulation_log\"\n");
            }

            fclose(logFile);
            free(readyQueue);
        }
        else
        {
            printf("Error: queue-capacity must be between 1 and 10!\n");
        }
    }
    return 0;
}