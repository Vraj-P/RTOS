#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "freelist.h"


int main()
{
    FL_Node_t *head = NULL;

    FL_Node_t *n1 = malloc(sizeof(FL_Node_t));
    printf("Node 1: %p\n", n1);
    FL_Node_t *n2 = malloc(sizeof(FL_Node_t));
    printf("Node 2: %p\n", n2);
    FL_Node_t *n3 = malloc(sizeof(FL_Node_t));
    printf("Node 3: %p\n", n3);
    FL_Node_t *n4 = malloc(sizeof(FL_Node_t));
    printf("Node 4: %p\n", n4);
    FL_Node_t *n5 = malloc(sizeof(FL_Node_t));
    printf("Node 5: %p\n", n5);

    printf("------------------------------\n");
    FL_Insert(&head, n3);
    FL_Print(head, 3);
    printf("--------------\n");
    FL_Insert(&head, n5);
    FL_Print(head, 5);
printf("--------------\n");
    FL_Insert(&head, n1);
    FL_Print(head, 1);
printf("--------------\n");
    FL_Insert(&head, n4);
    FL_Print(head, 4);
printf("--------------\n");
    FL_Insert(&head, n2);
    FL_Print(head, 2);

    printf("------------------------------\n");

    FL_Remove(&head, n1);
    FL_Print(head, 1);
printf("--------------\n");
    FL_Remove(&head, n4);
    FL_Print(head, 4);
printf("--------------\n");
    FL_Remove(&head, n2);
    FL_Print(head, 2);
printf("--------------\n");
    FL_Remove(&head, n5);
    FL_Print(head, 5);
printf("--------------\n");
    FL_Remove(&head, n3);
    FL_Print(head, 3);
    
    printf("------------------------------\n");

}