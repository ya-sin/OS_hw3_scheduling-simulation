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
void addQ( char* name, int Q_time, int prior)
{
	if(strlen(name)!=5) {
		printf("TASK_NAME ERROR.\n");
		return ;
	}
	if(!strncmp(name,"Task",4)||!strncmp(name,"task",4)) {
		if(name[4]>='1'&&name[4]<='6') {
			Node *newnode;
			newnode = (Node*)malloc(sizeof(Node));
			if(front->next == NULL) {
				front->next = newnode;
			}
			newnode->pid = ++PID;
			newnode->Q_time = Q_time;
			newnode->prior = prior;
			newnode->W_time = 0;
			strcpy(newnode->name,name);
			strcpy(newnode->state, "TASK_READY");
			newnode->next = NULL;
			rear->next = newnode;
			rear = newnode;
		} else {
			printf("TASK_NAME ERROR.\n");
			return ;
		}
	} else {
		printf("TASK_NAME ERROR.\n");
		return ;
	}
}

void removeQ(int pid)
{
	Node* rmnode;
	Node* tmpnode;
	if(front->next == NULL) {
		printf("List is empty, you have nothing to remove!\n ");
		return;
	}
	tmpnode = front;
	while(tmpnode->next != NULL) {
		rmnode = tmpnode->next;
		if(rmnode->pid == pid && front->next->next == NULL) {
			front->next=NULL;
			rear = (Node*)malloc(sizeof(Node));
			rear->next = NULL;
			free(rmnode);
			return;
		} else if(rmnode->pid == pid) {
			tmpnode->next = rmnode->next;
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
		printf("%-3d  %s    %-18s%lld         %d       %d\n",tmpnode->pid,tmpnode->name,
		       tmpnode->state,
		       tmpnode->W_time, tmpnode->prior, tmpnode->Q_time);
		tmpnode = tmpnode->next;
	}

}

void shell(void)
{
	char tmp[100];
	char name[100];
	int Q_time=0;
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
				if(!strcmp(time,"L"))    Q_time=20;
				else if(!strcmp(time,"S"))    Q_time=10;
			} else if(strlen(opt_T)==0&&strlen(time)==0)    Q_time=10;
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
			addQ(name, Q_time, prior);
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

