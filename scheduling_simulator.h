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
	char task_name[100];
	char task_state[20];
	int time_Quant; //time quantum
	int prior;
	long long int S_time; //enter ready Q time
	long long int queuing_T; //queueing time
	int Sleep_time; //suspend time
	ucontext_t task;
	struct node* next;
	struct node* lnext;
} Node;

Node* front, *rear; // job queuq
Node* lfront, *lrear;// ready queue
int PID;
struct itimerval new_value,old_value;

void simulator();
void add2ready(Node *newnode);
void add2jobq( char* name, int time_Quant, int prior);
void rmjobq(int pid);
void printjobq();


void set_S_time();
void get_time();

void hw_suspend(int msec_10);
void hw_wakeup_pid(int pid);
int hw_wakeup_taskname(char *task_name);
int hw_task_create(char *task_name);

void shell();

#endif
