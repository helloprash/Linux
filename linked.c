#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/time.h>

struct timeval tv;

typedef struct node
{
     char SensorName[50];
     int data;
     int time;
     struct node *next;
} NODE;

NODE *head = NULL;
NODE *scanPtr;
int count = 0;

void DisplayList();

void CreateList()
{
    NODE *temp;
    int value;
    char Sensor[50];

    temp = (NODE*)malloc(sizeof(NODE));
   
    printf("\nEnter the Sensor name:");
    scanf("%s",Sensor);
    printf("Enter the value:"); 
    scanf("%d",&value);
    gettimeofday(&tv,NULL);

    if(head == NULL)
	    head = temp;
    else
    {
	scanPtr = head;
        while(scanPtr->next!=NULL)
        {
		scanPtr = scanPtr->next;
        }
        scanPtr->next = temp;
    }
    strcpy(temp->SensorName,Sensor);
    temp->data = value;
    temp->time = tv.tv_sec;
    temp->next = NULL;
    count++;
}

void AddNode()
{
    NODE *ptr = head;
    NODE *temp,*currPtr;
    int pos,idx,value;
    char Sensor[50];

    printf("Enter the position:");
    scanf("%d",&pos);

    if(pos == 0 || pos>=count+2)
    {
	printf("Position does not exist. Please enter a valid position\n");
	return;
    }
	    
    printf("\nEnter the Sensor name:");
    scanf("%s",Sensor);
    printf("Enter the value:"); 
    scanf("%d",&value);
    gettimeofday(&tv,NULL);

    temp = (NODE*)malloc(sizeof(NODE));

    if(pos == 1)
    {
	if(count == 0)
	{
	    head = temp;
	    temp->next = NULL;
	}
	else if(count>0)
	{
	    temp->next = head;
	    head = temp;
	}
    }
    else
    {
	for(idx=2; idx<pos; idx++)
	{
            ptr = ptr->next;
	}
	currPtr = ptr->next;
	ptr->next = temp;
	temp->next = currPtr;
    }
    strcpy(temp->SensorName,Sensor);
    temp->data = value;
    temp->time = tv.tv_sec;
    count++;
}

void DeleteNode()
{
     int idx,pos;
     NODE *ptr = head;
     NODE *temp;

     if(head == NULL)
     {
	 DisplayList();
	 return;
     }

     printf("Enter the position to be deleted:");
     scanf("%d",&pos);

     if(pos-count>=1 || pos==0)
     {
	  printf("\nInvalid Position\n");
	  return;
     }

     if(pos == 1)
     {
	  head = ptr->next;
	  ptr->next = NULL;
	  free(ptr);
	  count--;
	  return;
     }
     else
     {
	  for(idx=1; idx<pos-1; idx++)
		  ptr = ptr->next;

	  temp = ptr->next;
	  ptr->next=temp->next;
	  temp->next = NULL;
	  free(temp);
	  count--;
     }
}

void DisplayList()
{
    if(head == NULL)
    {
	 printf("\nList is empty\n");
	 return;
    }

    NODE *ptr = head;

    while(ptr != NULL)
    {
	 printf("\n%s ",ptr->SensorName);
	 printf("%d ",ptr->data);
	 printf("%d\n",ptr->time);
	 ptr = ptr->next;
    }
}

void DeleteList()
{
    if(head == NULL)
    {
	 DisplayList();
	 return;
    }

    NODE *ptr = head;
    NODE *temp;
    head = NULL;

    while(ptr != NULL)
    {
         temp = ptr->next;
	 ptr->next = NULL;
         free(ptr);	 
	 ptr=temp;
    }
    free(ptr);
    count = 0;
}

int main()
{
  int flag;
  int option;

     flag = 1;
     while(flag)
     {
             printf("\n1.Create list\n");
             printf("2.Add Node\n");
             printf("3.Delete Node\n");
             printf("4.Display list\n");
             printf("5.Delete list\n");
             printf("6.Exit\n");

	     printf("\nEnter an option:");
	     scanf("%d",&option);
             
	     switch(option)
	     {
		  case 1:{
				 printf("Create List\n");
				 CreateList();
				 break;
			 }

	          case 2:{
				 printf("Add Node\n");
				 AddNode();
				 break;
			 }

		  case 3:{
                                 printf("Delete Node\n");
				 DeleteNode();
				 break;
			 }

	          case 4:{
                                 printf("Display list\n");
				 DisplayList();
				 break;
			 }

		  case 5:{
                                 printf("Delete list\n");
				 DeleteList();
				 break;
			 }

		  case 6:{
                                 printf("Exit\n");
				 flag = 0;
				 break;
			 }

		  default: {
				   printf("Enter a proper option\n");
				   break;
			   }
	     }
     }

     return 0;
}


