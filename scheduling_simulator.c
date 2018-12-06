#include "scheduling_simulator.h"
static ucontext_t shell_context;// (record shell status)
static ucontext_t start;
static ucontext_t end;
static ucontext_t current;
int main()
{
    // init signal(for time quntum and ctrl-Z)
    struct sigaction act;
    act.sa_handler = &sighandler;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGALRM);
    sigaddset(&act.sa_mask, SIGTSTP);
    act.sa_flags = 0;
    assert(sigaction(SIGALRM, &act, NULL)<=0);
    assert(sigaction(SIGTSTP, &act, NULL)<=0);

    // creat job Q and ready Q
    front = rear = (Node*)malloc(sizeof(Node));
    front->next = rear->next = NULL;
    lfront = lrear = (Node*)malloc(sizeof(Node));
    lfront->lnext = lrear->lnext = NULL;

    // setting the end context(if tassk finish  the function in task.c, calling terminal()
    char *stack = (char*)malloc(8192);
    getcontext(&end);
    end.uc_stack.ss_flags = 0;
    end.uc_stack.ss_size = 8192;
    end.uc_stack.ss_sp = stack;
    end.uc_link = NULL;
    makecontext(&end,terminal,0);

    // setting the start context(record simulation status)
    char *stack2 = (char*)malloc(8192);
    getcontext(&start);
    start.uc_stack.ss_flags = 0;
    start.uc_stack.ss_size = 8192;
    start.uc_stack.ss_sp = stack2;
    start.uc_link = NULL;
    makecontext(&start,simulator,0);

    shell();

    return 0;
}

void shell(void)
{
    getcontext(&shell_context);
    char tmp[100];
    char dst[100];
    char name[100];
    int pid;
    char *token;
    // t -> flag = 1;
    // p -> flag = 2;
    while(printf("$")) {
        int time_Quant=10;
        int prior = 0;
        int j = 0,i = 0,flag = 0;
        token = NULL;
        scanf("%s",tmp);
        if(!strcmp(tmp,"add")) {
            scanf("%s",name);
            fgets(tmp,sizeof(tmp),stdin);
            if(tmp[0]==' ')
                i = 1;
            for(i; tmp[i] != '\0'; i++) {
                if(tmp[i]=='\n')
                    continue;
                dst[j] = tmp[i];
                j++;
            }
            token = strtok(dst," ");
            while(token!=NULL) {
                if(!strcmp(token,"-t"))
                    flag =1;
                if(!strcmp(token,"-p"))
                    flag =2;
                if(!strcmp(token,"S")&&flag==1)
                    time_Quant = 10;
                if(!strcmp(token,"L")&&flag==1)
                    time_Quant = 20;
                if(!strcmp(token,"H")&&flag==2) {
                    prior = 1;
                }
                if(!strcmp(token,"L")&&flag==2) {
                    prior = 0;
                }
                token = strtok(NULL," ");
            }
            printf("prio:%d Q:%d\n",prior,time_Quant);
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
        } else if(!strcmp(tmp,"tm")) {
            trerminateall();
        } else {
            printf(" %s: command not found \n",tmp);
            printf("command: add, remove, ps,and start ");
            continue;
        }
    }
}
void set_timer(int time_Quant)
{
    // setting one time timer
    new_value.it_value.tv_sec = 0;
    new_value.it_value.tv_usec = time_Quant*1000;
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_usec = 0;
    if(setitimer(ITIMER_REAL,&new_value,&old_value)<0) {
        printf("set timer error");
        exit(EXIT_FAILURE);
    }
}
Node * pop_readyq()
{
    Node *popnode = lfront->lnext;
    if(popnode==NULL)
        return NULL;
    if(popnode->lnext==NULL) {
        lrear = lfront;
        lfront->lnext = NULL;
    } else {
        lfront -> lnext = popnode -> lnext;
    }
    popnode->lnext = NULL;
    // printf("qT: %lld start: %lld\n",popnode->queuing_T,popnode->S_time);
    long a = get_time();
    // printf("get time %ld\n",a);
    popnode->queuing_T = popnode->queuing_T + (a-popnode->S_time);
    return popnode;
}
void trerminateall()
{
    Node *tmpnode;
    tmpnode = front->next;
    while(tmpnode!=NULL) {
        strcpy(tmpnode->task_state, "TASK_TERMINATED");
        tmpnode = tmpnode -> next;
    }
}
bool check_terminate()
{
    Node *tmpnode;
    tmpnode = front->next;
    while(tmpnode!=NULL) {
        if(strcmp(tmpnode->task_state,"TASK_TERMINATED"))
            return 0;// have task can do
        tmpnode = tmpnode -> next;
    }
    return 1;//no task can do
}
void terminal()
{
    while(1) {
        // clear the timer
        set_timer(0);
        if(runnode==NULL)
            return;
        strcpy(runnode->task_state,"TASK_TERMINATED");
        runnode = NULL;
        swapcontext(&end,&start);
    }
}
void simulator()
{
    int test;
    printf("im in simulator");
    scanf("%d",&test);
    while(1) {
        if(test) {
            if(check_terminate()) { // all the task in the job q are terminate
                swapcontext(&start,&shell_context);
            }
            // pop a task from  the ready queue
            Node *runtask;
            runtask = pop_readyq();


            if(runtask==NULL) { // has no task in the ready q
                printf("has no task in the ready ");
                // swapcontext(&start,&shell_context);
                exit(EXIT_FAILURE);
            } else {
                // printf("%d %lld\n",runtask->pid,runtask->queuing_T);

                runnode = runtask;
                strcpy(runtask->task_state, "TASK_RUNNING");
                set_timer(runtask->time_Quant);
                swapcontext(&start,&runtask->task);
            }

            // printf("soo");
            // swapcontext(&start,&shell_context);
            // printf("hi");
        }
    }
}
void timeout()
{
    // everytime when the time quantum time out will call this function
    // 1. decrease  sleep time of all the task in the wating Q
    // 2. change thhe runnode
    Node *tmpnode;
    tmpnode = front->next;
    while(tmpnode!=NULL) {
        if(!strcmp(tmpnode->task_state,"TASK_WAITING")) {
            tmpnode->suspendT = tmpnode->suspendT - runnode->time_Quant;
            if(tmpnode->suspendT<=0)
                add2ready(tmpnode);
        }
        tmpnode = tmpnode->next;
    }
    add2ready(runnode);
    Node *task = pop_readyq();
    strcpy(task->task_state, "TASK_RUNNIG");
    runnode = task;
    set_timer(task->time_Quant);
    swapcontext(&lrear->task,&runnode->task);

}
void sighandler(int mode)
{
    switch(mode) {
    // time quntum
    case SIGALRM:
        timeout();
        break;
    // ctrl+Z
    case SIGTSTP:
        printf("Ctrl+Z\n");
        if(runnode!=NULL) {
            add2ready(runnode);
            runnode = NULL;
            set_timer(0);
        }
        getcontext(&current);
        swapcontext(&current,&shell_context);
        break;
    default:
        printf("SIGNAL ERROR.\n");
        break;
    }
}

void add2ready(Node *newnode)
{

    struct timeval now;

    if(lfront->lnext == NULL) {
        lfront->lnext = newnode;
    }
    newnode->lnext = NULL;
    lrear->lnext = newnode;
    lrear = newnode;
    gettimeofday(&now,NULL);
    strcpy(newnode->task_state, "TASK_READY");
    newnode->S_time = (now.tv_sec)*1000 + (now.tv_usec)/1000;
    // printf("add2readyq %lld\n",newnode->S_time);
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
            char *stack = (char*)malloc(8192);
            getcontext(&newnode->task);
            newnode->task.uc_stack.ss_sp = stack;
            newnode->task.uc_stack.ss_size = 8192;
            newnode->task.uc_stack.ss_flags = 0;
            newnode->task.uc_link = &end;// when task function finish go to the terminal()
            switch(name[4]) {
            case '1':
                makecontext(&newnode->task,task1,0);
                break;
            case '2':
                makecontext(&newnode->task,task2,0);
                break;
            case '3':
                makecontext(&newnode->task,task3,0);
                break;
            case '4':
                makecontext(&newnode->task,task4,0);
                break;
            case '5':
                makecontext(&newnode->task,task5,0);
                break;
            case '6':
                makecontext(&newnode->task,task6,0);
                break;
            default:
                printf("TASK_NAME ERROR.\n");
                break;
            }
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
void rmreadyq(int pid)
{
    Node* rmnode;
    Node* tmpnode;
    // ready queue is empty
    if(lfront->lnext == NULL) {
        printf("List is empty, you have nothing to remove!\n ");
        return;
    }
    // ready queue is not empty
    tmpnode = lfront;
    while(tmpnode->lnext != NULL) {
        rmnode = tmpnode->lnext;
        // job queue has only one node
        if(rmnode->pid == pid && lfront->lnext->lnext == NULL) {
            lfront->lnext=NULL;
            lrear = lfront;
            return;
        } else if(rmnode->pid == pid) {
            tmpnode->lnext = rmnode->lnext;
            // renode is the last one node in the job queue
            if(rmnode->lnext == NULL)
                lrear = tmpnode;
            else
                rmnode->lnext = NULL;
            return;
        }
        tmpnode = tmpnode->lnext;
    }
}
void rmjobq(int pid)
{
    Node* rmnode;
    Node* tmpnode;
    rmreadyq(pid);
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
            rear -> next = NULL;
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
    char P,T;
    printf("PID TASK_NAME TASK_STATE QUEUEING_TIME PRIORITY QUANTUM\n");
    while(tmpnode != NULL) {
        if(tmpnode->prior)
            P = 'H';
        else
            P = 'L';
        if(tmpnode->time_Quant==20)
            T = 'L';
        else
            T = 'S';
        printf("%-3d  %s    %-18s%lld         %c       %c\n",tmpnode->pid,tmpnode->task_name,
               tmpnode->task_state,
               tmpnode->queuing_T, P, T);
        tmpnode = tmpnode->next;
    }
    printf("ready queue\n");
    tmpnode = lfront -> lnext;
    while(tmpnode != NULL) {
        printf("%-3d  %s    %-18s%lld         %d       %d\n",tmpnode->pid,tmpnode->task_name,
               tmpnode->task_state,
               tmpnode->queuing_T, tmpnode->prior, tmpnode->time_Quant);
        tmpnode = tmpnode->lnext;
    }

}

void set_S_time()
{
    struct timeval start_T;
    Node * tmpnode;
    tmpnode = lfront->next;
    while(tmpnode != NULL) {
        gettimeofday(&start_T,NULL);
        tmpnode->S_time = (start_T.tv_sec)*1000 + (start_T.tv_usec)/1000;
        // printf("pid: %d start time: %lld\n",tmpnode->pid,tmpnode->S_time);
        tmpnode = tmpnode->next;
    }
}
long get_time()
{
    struct timeval now;
    gettimeofday(&now,NULL);
    return (now.tv_sec)*1000 + (now.tv_usec)/1000;
}
void hw_suspend(int msec_10)
{
    // clear timer
    set_timer(0);
    Node *tmpnode;
    runnode->suspendT = msec_10*10;// ms
    strcpy(runnode->task_state,"TASK_WAITING");
    tmpnode = runnode;
    runnode = NULL;
    swapcontext(&tmpnode->task,&start);
    return;
}

void hw_wakeup_pid(int pid)
{
    Node *tmpnode;
    tmpnode = front->next;
    while(tmpnode!=NULL) {
        if(tmpnode->pid == pid)
            add2ready(tmpnode);
        tmpnode = tmpnode->next;
    }
    return;
}

int hw_wakeup_taskname(char *task_name)
{
    Node *tmpnode;
    int count = 0;
    tmpnode = front->next;
    while(tmpnode!=NULL) {
        if(!strcmp(tmpnode->task_name,task_name)) {
            add2ready(tmpnode);
            count++;
        }
        tmpnode = tmpnode->next;
    }
    return count;
}

int hw_task_create(char *task_name)
{
    add2jobq(task_name,10,0);
    return PID;
}

