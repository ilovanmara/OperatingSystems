#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "a2_helper.h"
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

typedef struct {
    int process;
    int th;
    sem_t* log;
    sem_t* sem;
}TH_STRUCT;

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

sem_t logSem1, logSem2;
sem_t *logSem3, *logSem4;
sem_t sem1;

int found = 0;
int k = 0;
int ok = 0;
int count = 0;
int threads= 0;

void thread_functionP4th1(void* arg)
{
    TH_STRUCT* s = (TH_STRUCT*)arg;
      
    info(BEGIN, s->process, s->th);
    sem_post(&logSem1);
    sem_wait(&logSem2);
    info(END, s->process, s->th);
  
}

void thread_functionP4th3(void* arg)
{
    TH_STRUCT* s = (TH_STRUCT*)arg;

    sem_wait(&logSem1);
    info(BEGIN, s->process, s->th);
    info(END, s->process, s->th);
    sem_post(&logSem2);
    
}
void thread_functionP4(void* arg)
{
    TH_STRUCT* s = (TH_STRUCT*)arg;
    
    info(BEGIN, s->process, s->th);
    info(END, s->process, s->th);
    
}

// p5 creaza 39 de thread uri
//cel mult 5 thread uri pot rula simultan
//t14 nu are voie sa se termine doar daca in timpul acesta 4 thread uri ruleaza(cu tot cu el)

void thread_functionP5_1(void* arg){
    TH_STRUCT* s = (TH_STRUCT*)arg;

    sem_wait(s->log);
    if(s->th != 14){
        count++;
        info(BEGIN, s->process, s->th);
         if(ok == 0 && count == 3){
            sem_wait(s->log);
            info(BEGIN, 5, 14);
            ok = 1;
            info(END, 5, 14);
            sem_post(s->log);
        }
        info(END, s->process, s->th);
    }
    sem_post(s->log);
}

void thread_functionP5(void* arg)
{
    TH_STRUCT* s = (TH_STRUCT*)arg;
    
    sem_wait(s->sem);
    //regiune critica 1
    pthread_mutex_lock(&mutex1);
    found++;
    printf("%d %d ", found, ok);
    pthread_mutex_unlock(&mutex1);
    //vad daca l-am gasit
    if(s->th == 14){
       ok = 1;
    } else
        info(BEGIN, s->process, s->th);
    sem_wait(s->log);
    //regiune critica 2
    pthread_mutex_lock(&mutex2);
    threads++;
    pthread_mutex_unlock(&mutex2);
    
    if(s->th != 14) {
        info(END, s->process, s->th);
        pthread_mutex_lock(&mutex1);
        found--;
        pthread_mutex_unlock(&mutex1);
        if(ok == 1 && k == 0 && found == 4){
            info(BEGIN, s->process, 14);
            info(END, s->process, 14);
            k = 1;
        }  
    }
    sem_post(s->log);
    
    sem_post(s->sem);
}

void create_4threadsP5(pthread_t* tids, int nrThreads, TH_STRUCT params[]){
    sem_t logSem;
    if(sem_init(&logSem, 0, 1) != 0) {
        perror("Could not init the semaphore");
        return;
    }
    sem_t sem;
    if(sem_init(&sem, 0, 4) != 0) {
        perror("Could not init the semaphore");
        return;
    }
    for (int i=0; i < 39; i++){
        params[i].process = 5;
        params[i].th = i + 1;
        params[i].log = &logSem;
        params[i].sem = &sem;
        pthread_create(&tids[i], 0, (void* (*)(void *))thread_functionP5, &params[i]);
    } 
    for (int i = 0; i < 39; i++)
		pthread_join(tids[i], 0); 
    
    sem_destroy(&logSem);
    sem_destroy(&sem);
    
}

void thread_functionP8(void* arg)
{
    TH_STRUCT* s = (TH_STRUCT*)arg;
    //t8.4 sa se incheie inainte ca t4.4 sa pornesaca
    if(s->process == 8 && s->th == 4){
        info(BEGIN, s->process, s->th);
        info(END, s->process, s->th);
        sem_post(logSem3);
    }
    else if(s->process == 4 && s->th == 4){
        sem_wait(logSem3);
        info(BEGIN, s->process, s->th);
        info(END, s->process, s->th);
        sem_post(logSem4);
    } 
    else if(s->process == 8 && s->th == 2){
        sem_wait(logSem4);
        info(BEGIN, s->process, s->th);
        info(END, s->process, s->th);
    }
    else{
        info(BEGIN, s->process, s->th);
        info(END, s->process, s->th);
    }
    
}
void create_4threadsP4(pthread_t* tids, int nrThreads, TH_STRUCT params[]){

    for (int i=0; i < nrThreads; i++){
        params[i].process = 4;
        params[i].th = i + 1;
        if(params[i].th == 2 || params[i].th == 4)
		    pthread_create(&tids[i], 0, (void* (*)(void *))thread_functionP8, &params[i]);
        if(params[i].th == 3)
		    pthread_create(&tids[i], 0, (void* (*)(void *))thread_functionP4th3, &params[i]);
        if(params[i].th == 1)
		    pthread_create(&tids[i], 0, (void* (*)(void *))thread_functionP4th1, &params[i]);
    }    
	for (int i = 0; i < nrThreads; i++)
		pthread_join(tids[i], 0);
}

//t8.4 trebuie sa se incheie inainte ca thread ul t4.4
//t8.2 nu poate incepe decat dupa terminarea t4.4

void create_4threadsP8(pthread_t* tids, int nrThreads, TH_STRUCT params[]){
    
    for (int i=0; i < nrThreads; i++){
        params[i].process = 8;
        params[i].th = i + 1;
		pthread_create(&tids[i], 0, (void* (*)(void *))thread_functionP8, &params[i]);
    }    
	for (int i = 0; i < nrThreads; i++)
		pthread_join(tids[i], 0);
}

pid_t p2 = -1, p3 = -1, p4 = -1, p5 = -1, p6 = -1, p7 = -1, p8 = -1;

void create_processes(){

    p2 = fork();
    if(p2 == 0) {
        //P2
        info(BEGIN, 2, 0);
        p7 = fork();
        if(p7 == 0) {
            info(BEGIN, 7, 0);
            info(END, 7, 0);
            exit(1);
        } 
        waitpid(p7, NULL, 0);
        info(END, 2, 0);
        exit(1);  
    }
    p3 = fork();
    if(p3 == 0) {
        //P3
        info(BEGIN, 3, 0);
        info(END, 3, 0);
        exit(1);
    }
    p4 = fork();
    if(p4 == 0) {
        //P4
        info(BEGIN, 4, 0);
        int nrThreads = 4;
        TH_STRUCT params[4];
        pthread_t tids[4];
      
        create_4threadsP4(tids, nrThreads, params);
        info(END, 4, 0);
        exit(1);
    }
    p5 = fork();
    if(p5 == 0) {
        //P5
        info(BEGIN, 5, 0);
        int nrThreads = 39;
        TH_STRUCT params[39];
        pthread_t tids[39];
        create_4threadsP5(tids, nrThreads, params);

        p8 = fork();
        if(p8 == 0) {
            info(BEGIN, 8, 0);
            logSem3 = sem_open("log_semaphore3", 0);
            logSem4 = sem_open("log_semaphore4", 0);
            int nrThreads1 = 4;
            TH_STRUCT params1[4];
            pthread_t tids1[4];
            create_4threadsP8(tids1, nrThreads1, params1);

            info(END, 8, 0);
            exit(1);
        } 
        waitpid(p8, NULL, 0);
        info(END, 5, 0);
        exit(1);
    }
    p6 = fork();
    if(p6 == 0) {
        //P6
        info(BEGIN, 6, 0);
        info(END, 6, 0);
        exit(1);
    }
    waitpid(p8, NULL, 0);
}

int main(int argc, char** argv)
{
    //procesul principal p1
    init();

    info(BEGIN, 1, 0);
    
    logSem3 = sem_open("log_semaphore3", O_CREAT, 0644, 0);
    logSem4 = sem_open("log_semaphore4", O_CREAT, 0644, 0);

    create_processes();
    waitpid(p2, NULL, 0);
    waitpid(p3, NULL, 0);
    waitpid(p4, NULL, 0);
    waitpid(p5, NULL, 0);
    waitpid(p6, NULL, 0);

    sem_unlink("log_semaphore3");
    sem_unlink("log_semaphore4");

    sem_destroy(&logSem1);
    sem_destroy(&logSem2);

    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);

    info(END, 1, 0);

    return 0;
}