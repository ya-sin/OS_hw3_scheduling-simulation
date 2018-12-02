#ifndef SCHEDULING_SIMULATOR_H
#define SCHEDULING_SIMULATOR_H

#include <stdio.h>
#include <ucontext.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>

#include "task.h"

enum TASK_STATE {
	TASK_RUNNING,
	TASK_READY,
	TASK_WAITING,
	TASK_TERMINATED
};

typedef struct node {
	int pid;
	char name[100];
	char state[20];
	int Q_time; //time quantum
	int prior;
	long long int S_time; //enter ready Q time
	long long int W_time; //queueing time
	int Sleep_time; //suspend time
	ucontext_t task;
	struct node* next;
	struct node* lnext;
} Node;

Node* front, *rear;
Node* lfront, *lrear;
int PID;
struct itimerval new_value,old_value;

void creatQ();
void addQ( char* name, int Q_time, int prior);
void removeQ(int pid);
void showQ();

void hw_suspend(int msec_10);
void hw_wakeup_pid(int pid);
int hw_wakeup_taskname(char *task_name);
int hw_task_create(char *task_name);

void shell();

#endif
