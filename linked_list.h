/***************************
* FILE: linked_list.h      *
* AUTHOR: Thomas Di Pietro *
* DATE: 05/05/2019         *      
***************************/
typedef struct 
{   /*Time in seconds*/
    int* arrivalTime; 
    int* serviceTime;
    int* compTime;
    int* cpuNum;
} TaskInfo;

typedef struct LinkedListNode
{
    int* taskNum; /*task# (1 to 100)*/
    int* cpu_burst; /*cpu_burst (1 to 50)*/
    TaskInfo* curTask;
    struct LinkedListNode* next;
} LinkedListNode;

typedef struct 
{
    LinkedListNode* head;
    LinkedListNode* tail;
    int count;
} LinkedList;


LinkedList* newList();
void insertLast(LinkedList* list, LinkedListNode* newNode);
void removeFirst(LinkedList* list);