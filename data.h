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

struct timeval  tv1, tv2;

struct entry{
  int data;  // entry's id
  int writers; //writers who have visit this entry 
  int readers; //writers who have visit this entry 
};

struct SharedMemory{
  int memId; 
  int arrayId;
  struct entry* entryArray;   // list form structs Entry
  int *readersArray;		// array for the current readers,inside in the entries
};

union semun {
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INFO
                               (Linux-specific) */
};

void initializeSemaphores(int semid,int index,int val);
int semP(int semid,int index);
int semV(int semid,int index);
void removeSemaphores(int sem_id);

void detachArray(struct entry *pointer);
void detachReadersArray(int *pointer);
void detachMemory(struct SharedMemory *ptr);
void deleteMemory(int memId);