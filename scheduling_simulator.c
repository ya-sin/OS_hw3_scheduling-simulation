#include "scheduling_simulator.h"
static ucontext_t shell_context;
static ucontext_t start;
void simulator(){
	int test;
	printf("im in simulator");
	scanf("%d",&test);
	if(test)
		swapcontext(&start,&shell_context);
}
int main()
{
	// creat job Q and ready Q
	front = rear = (Node*)malloc(sizeof(Node));
	front->next = rear->next = NULL;
	lfront = lrear = (Node*)malloc(sizeof(Node));
	lfront->lnext = lrear->lnext = NULL;

	// setting the start context
	char *stack = (char*)malloc(10000);
	getcontext(&start);
	start.uc_stack.ss_flags = 0;
	start.uc_stack.ss_size = sizeof(stack);
	start.uc_stack.ss_sp = stack;
	start.uc_link = NULL;
	makecontext(&start,simulator,0);
	shell();
	return 0;
}

void shell(void)
{
	getcontext(&shell_context);
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
			rmjobq(pid);
		} else if(!strcmp(tmp,"start")) {
			// keep the time at this point in the task struct->S-time
			set_S_time();
			printf("simulating...\n");
			// swap to start mode,store the shell mode
			// right after that,executing the first task in the ready queue
			swapcontext(&shell_context,&start);
		} else if(!strcmp(tmp,"ps")) {
			printjobq();
		} else {
			printf(" %s: command not found \n",tmp);
			printf("command: add, remove, ps,and start ");
			continue;
		}
	}
}

void add2ready(Node *newnode)
{

	struct timeval now;

	if(lfront->next == NULL) {
		lfront->next = newnode;
	}
	newnode->next = NULL;
	lrear->next = newnode;
	lrear = newnode;
	gettimeofday(&now,NULL);
	strcpy(newnode->task_state, "TASK_READY");
	newnode->S_time = (now.tv_sec)*1000 + (now.tv_usec)/1000;
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
			add2ready(newnode);
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

void rmjobq(int pid)
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
			// renode is the last one node in the job queue
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
void printjobq()
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
	tmpnode = lfront -> next;
	while(tmpnode != NULL) {
		printf("%-3d  %s    %-18s%lld         %d       %d\n",tmpnode->pid,tmpnode->task_name,
		       tmpnode->task_state,
		       tmpnode->queuing_T, tmpnode->prior, tmpnode->time_Quant);
		tmpnode = tmpnode->next;
	}

}

void set_S_time(){
	struct timeval start_T;
	Node * tmpnode;
	gettimeofday(&start_T,NULL);
	tmpnode = lfront->next;
	while(tmpnode != NULL){
		tmpnode->S_time = (start_T.tv_sec)*1000 + (start_T.tv_usec)/1000;
		printf("pid: %d start time: %lld\n",tmpnode->pid,tmpnode->S_time);
		tmpnode = tmpnode->next;
	}
}
// long get_time(){
// 	struct timeval now;
// 	gettimeofday(&now,NULL);
// 	return (now.tv_sec)*1000 + (now.tv_usec)/1000;
// }
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

