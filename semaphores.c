#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h> 
#include <time.h>
#include <math.h>
#include "data.h"

int createSemaphores(int num){
  int semid;
  semid = semget((key_t) IPC_PRIVATE,num, IPC_CREAT | 0660);
  if (semid < 0){
    perror("Fail to create semaphores");
    return -1;
  }
  return semid;
}

void initializeSemaphores(int semid,int index,int val){  //initialise semaphores ind the index "inxed"  from the array sem_id ,with val
  int init = semctl(semid, index, SETVAL,val);
  if (init < 0){
    perror("Fail to initialize semaphores");
    exit(1);
  }
}

int semP(int semid,int index){ //sem down
    struct sembuf sem_b; 
    int op;
    sem_b.sem_num = index;
    sem_b.sem_op = -1;
    sem_b.sem_flg = 0;
  op = semop(semid,&sem_b,1);
  return op;
}

int semV(int semid,int index){ //sem up
  int op;
    struct sembuf sem_b;  
    sem_b.sem_num = index;
    sem_b.sem_op = 1;
    sem_b.sem_flg = 0;
    op = semop(semid,&sem_b,1);
    return op;
}

void removeSemaphores(int sem_id){  //remove semaphores form the system
  int rem = semctl(sem_id,0,IPC_RMID);
  if (rem < 0)
   {  perror ("error in removing semaphore from the system");
      exit (1);
   }
}