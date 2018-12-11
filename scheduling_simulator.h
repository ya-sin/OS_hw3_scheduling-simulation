#ifndef SCHEDULING_SIMULATOR_H
#define SCHEDULING_SIMULATOR_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <ucontext.h>
#include <signal.h>
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
    int suspendT; //suspend time
    ucontext_t task;
    struct node* next; // job queue ptr
    struct node* lnext; // ready queue ptr
} Node;

Node* front, *rear; // job queue
Node* lfront, *lrear;// ready queue
Node* runnode;
int PID; // the current maximum pid in this system
struct itimerval new_value,old_value; // for set_timer()

// shell
void shell();

// simulator
void simulator();
void set_S_time();
bool check_terminate();

// signal
void sighandler(int mode);
void timeout();
void set_timer(int time_Quant);

// queuing time
long get_time();

// Q organize
void add2jobq( char* name, int time_Quant, int prior);
void add2ready(Node *newnode);
void rmjobq(int pid);
void rmreadyq(int pid);
void printjobq();
Node * pop_readyq();

// terminate fnished task
void terminal();

// for test
void trerminateall();

void hw_suspend(int msec_10);
void hw_wakeup_pid(int pid);
int hw_wakeup_taskname(char *task_name);
int hw_task_create(char *task_name);


#endif
