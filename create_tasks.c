/***************************
* FILE: create_tasks.c     *
* AUTHOR: Thomas Di Pietro *
* DATE: 05/05/2019         *
*                          *
* PURPOSE: Creates random  *
* task_file of 100 tasks   *      
***************************/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("Error: command-line parameters wrong.\n");
        printf("       e.g. ./create_tasks <num-tasks> <cpu-burst-limit>\n");
    }
    else
    {
        FILE* taskFile = fopen("task_file", "w");
        int i, tasks, burstLim;
        srand(time(NULL));

        sscanf(argv[1], "%d", &tasks);
        sscanf(argv[2], "%d", &burstLim);

        for (i = 1; i < tasks; i++)
        {
            fprintf(taskFile, "%d %d\n", i, (rand() % burstLim + 1));
        }
        fprintf(taskFile, "%d %d", i, (rand() % burstLim + 1));

        fclose(taskFile);

        printf("task_file created!\n");
    }
    return 0;
}