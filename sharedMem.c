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

void detachArray(struct entry *pointer){
  int err = shmdt((void*)pointer);
  if (err == -1){
    perror("Detached");
    exit(1);
  }
}

void detachReadersArray(int *pointer){
  int err = shmdt((void*)pointer);
  if (err == -1){
    perror("Detached");
    exit(1);
  }
}

void detachMemory(struct SharedMemory *ptr){
  int err = shmdt((void*)ptr);
  if (err == -1){
    perror("Detached");
    exit(1);
  }
  else
    printf("Shared memory in adress %p has been Detached\n",ptr);
}


void deleteMemory(int memId){
  int err = shmctl(memId,IPC_RMID,0);
  if (err == -1){
    perror("Detached");
    exit(1);
  }
}