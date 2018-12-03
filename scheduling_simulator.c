#include "scheduling_simulator.h"

int main()
{
	creatQ();
	shell();
	return 0;
}

void creatQ()
{
	front = rear = (Node*)malloc(sizeof(Node));
	front->next = rear->next = NULL;
	lfront = lrear = (Node*)malloc(sizeof(Node));
	lfront->lnext = lrear->lnext = NULL;
}
void shell(void)
{
	char tmp[100];
	char name[100];
	int time_Quant=0;
	int prior = 0;
	int pid;
	char opt_T[50];
	char opt_P[50];
	char time[50];
	char priority[50];
	while(printf("$")) {
		scanf("%s",tmp);
		if(!strcmp(tmp,"add")) {
			scanf("%s",name);
			fgets(tmp,sizeof(tmp),stdin);
			memset(opt_T,0,50);
			memset(time,0,50);
			memset(opt_P,0,50);
			memset(priority,0,50);
			sscanf(tmp,"%s%s%s%s",opt_T,time,opt_P,priority);
			if(!strcmp(opt_T,"-t")) {
				if(!strcmp(time,"L"))    time_Quant=20;
				else if(!strcmp(time,"S"))    time_Quant=10;
			} else if(strlen(opt_T)==0&&strlen(time)==0)    time_Quant=10;
			else {
				printf("Input error.\n");
				continue;
			}
			if(!strcmp(opt_P,"-p")) {
				if(!strcmp(priority,"H"))    prior=1;
				else if(!strcmp(priority,"L"))    prior=0;
			} else if(strlen(opt_P)==0&&strlen(priority)==0)    prior=0;
			else {
				printf("Input error.\n");
				continue;
			}
			printf("%d\n",prior);
			// add the new task to the job queue
			add2jobq(name, time_Quant, prior);
		} else if(!strcmp(tmp,"remove")) {
			scanf("%d",&pid);
			removeQ(pid);
		} else if(!strcmp(tmp,"start")) {
			printf("simulating...\n");
		} else if(!strcmp(tmp,"ps")) {
			showQ();
		} else {
			printf(" %s: command not found \n",tmp);
			continue;
		}
	}
}

void add2jobq( char* name, int time_Quant, int prior)
{
	// the length of task's name should be equal to 5 
	if(strlen(name)!=5) {
		printf("TASK_NAME ERROR.\n");
		return ;
	}
	// the task's name should be in the "taskx/Taskx "format
	// x = 1~6
	if(!strncmp(name,"Task",4)||!strncmp(name,"task",4)) {
		if(name[4]>='1'&&name[4]<='6') {
			// melloc a new node
			Node *newnode;
			newnode = (Node*)malloc(sizeof(Node));
			// setting the new node(except start_time, lnext and sleep_time )
			if(front->next == NULL) {
				front->next = newnode;
			}
			newnode->pid = ++PID;
			newnode->time_Quant = time_Quant;
			newnode->prior = prior;
			newnode->queuing_T = 0;
			strcpy(newnode->task_name,name);
			strcpy(newnode->task_state, "TASK_READY");
			// change the new node's context

			//
			newnode->next = NULL;
			rear->next = newnode;
			rear = newnode;
			// add the new node to the ready queue
			// add2ready()
		} else {
			printf("ERROR TASK_NUMBER.\n");
			printf("the range of number is 1~6 ");
			return ;
		}
	} else {
		printf("TASK_NAME ERROR.\n");
		printf("EX: task1 / Task1\n");
		return ;
	}
}

void removeQ(int pid)
{
	Node* rmnode;
	Node* tmpnode;
	// job queue is empty
	if(front->next == NULL) {
		printf("List is empty, you have nothing to remove!\n ");
		return;
	}
	// job queue is not empty
	tmpnode = front;
	while(tmpnode->next != NULL) {
		rmnode = tmpnode->next;
		// job queue has only one node
		if(rmnode->pid == pid && front->next->next == NULL) {
			front->next=NULL;
			rear = (Node*)malloc(sizeof(Node));
			rear->next = NULL;
			free(rmnode);
			return;
		} else if(rmnode->pid == pid) {
			tmpnode->next = rmnode->next;
			if(rmnode->next == NULL)
				rear = tmpnode;
			else
				rmnode->next = NULL;
			free(rmnode);
			return;
		}
		tmpnode = tmpnode->next;
	}
}
void showQ()
{
	Node* tmpnode;
	tmpnode = front->next;
	printf("PID TASK_NAME TASK_STATE QUEUEING_TIME PRIORITY QUANTUM\n");
	while(tmpnode != NULL) {
		printf("%-3d  %s    %-18s%lld         %d       %d\n",tmpnode->pid,tmpnode->task_name,
		       tmpnode->task_state,
		       tmpnode->queuing_T, tmpnode->prior, tmpnode->time_Quant);
		tmpnode = tmpnode->next;
	}

}


void hw_suspend(int msec_10)
{
	return;
}

void hw_wakeup_pid(int pid)
{
	return;
}

int hw_wakeup_taskname(char *task_name)
{
    return 0;
}

int hw_task_create(char *task_name)
{
    return 0; // the pid of created task name
}

