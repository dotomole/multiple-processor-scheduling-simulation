/***************************
* FILE: linked_list.c      *
* AUTHOR: Thomas Di Pietro *
* DATE: 05/05/2019         *
*                          *
* PURPOSE: Linked List data*
* structure, with the      *
* appropriate methods      *
* needed for program       *      
***************************/
#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"

/*Creates new LinkedList*/
LinkedList* newList()
{
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
    return list;
}

/*inserts at tail and pushes current tail up*/
void insertLast(LinkedList* list, LinkedListNode* newNode)
{
    if (list->head == NULL)
    {
        list->head = newNode;
        list->tail = newNode;
    }
    else
    {
        LinkedListNode* temp = list->tail;
        list->tail = newNode;
        temp->next = list->tail;
    }
    list->count++;
}

/*removes first element of imported LinkedList*/
void removeFirst(LinkedList* list)
{
    LinkedListNode* temp = list->head;
    if (temp != NULL)
    {
        list->head = temp->next; /*sets head to next element*/
        /*Frees (inside out)*/
        free(temp->curTask->cpuNum);
        free(temp->curTask->compTime);
        free(temp->curTask->serviceTime);
        free(temp->curTask->arrivalTime);
        free(temp->curTask);
        free(temp->taskNum);
        free(temp->cpu_burst);
        free(temp);
        list->count--;
    }
    else
    {
        printf("Error: No head in Linked List.");
    }
}