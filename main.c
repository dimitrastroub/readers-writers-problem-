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

#define l 5

int main(int argc, char* argv[]){

  if (argc < 5){
    perror("not enough parametrs given\n");
    exit(34);
  }
  int readers , writers;
  pid_t pid;  
  int i, entries = atoi(argv[1]);
  int peers = atoi(argv[2]);
  float ratio = atof(argv[3]);
  float iterations = atoi(argv[4]);

  printf("number of peers is %d\n",peers);
  printf("number of entries is  %d\n",entries);
  printf("ratio betwwen readers and writers is  %f\n",ratio);
  printf("number of iterations is %f\n",iterations);

  //create 2 semaphores

  int sem_readers = createSemaphores(entries);
  int sem_writers = createSemaphores(entries);

  for(i=0; i<entries; i++){
    initializeSemaphores(sem_readers,i,1);
    initializeSemaphores(sem_writers,i,1);
  }

  //allocate shared memory
  int memId = shmget((key_t) IPC_PRIVATE, sizeof(struct SharedMemory),IPC_CREAT | 0660);
    if (memId == -1){
	     perror("allocation");
	 }
  //attach shared memory
  struct SharedMemory* ptr;
  ptr = (struct SharedMemory*)shmat(memId,NULL,0); 
  if (ptr == NULL)
    perror("attachment");
  else
    printf("attached at address %p\n", ptr);
  ptr->memId = shmget((key_t)IPC_PRIVATE,entries*sizeof(struct entry),IPC_CREAT | 0660);
  ptr->entryArray = shmat(ptr->memId,NULL,0);
  ptr->arrayId = shmget((key_t)IPC_PRIVATE,entries*sizeof(int),IPC_CREAT | 0660 );
  ptr->readersArray = shmat(ptr->arrayId, NULL ,0);

  for(i=0;i<entries;i++){
    ptr->entryArray[i].readers = 0; 
    ptr->entryArray[i].writers = 0;
    ptr->entryArray[i].data = i;
  }

  //create process
  int status;
  for (i=0; i<peers; i++){
    pid = fork();
    double time_spent = 0;
    srand(time(NULL) ^ (getpid() << 8));
    if (pid <0 ){
      perror("fork failed");
      exit(EXIT_FAILURE);
    }
    readers = 0;
    writers = 0;
    if (pid == 0){// eimai sto paidi
      for(int j=0; j<iterations; j++){ //each child "runs" for specific iterations
          float choice = rand()%11;     // choice if in the current moment the child is a reader or a writer
          choice = choice/10;
          if (choice <= ratio){  // is a reader
              readers++ ;
              int chooseEntryToRead = rand()%entries; // which entry reader will try to visit
              gettimeofday(&tv1, NULL); //start waiting time

              semP(sem_readers,chooseEntryToRead);
              ptr->readersArray[chooseEntryToRead]++;
              if ( ptr->readersArray[chooseEntryToRead] == 1) 
                semP(sem_writers,chooseEntryToRead);
              semV(sem_readers,chooseEntryToRead);

              gettimeofday(&tv2, NULL); //end waiting time

              semP(sem_readers,chooseEntryToRead);
              ptr->entryArray[chooseEntryToRead].readers++;
              //sleep time
              float Uni, Exp;
              do{
                Uni = rand()%11;
                }while(Uni == 0 || Uni == 10);
              Uni = Uni/10;
              Exp = -log(Uni)/l;
              Exp = Exp*20;
              //printf("%f\n", Exp);
              sleep(Exp);

              semV(sem_readers,chooseEntryToRead);
              ptr->readersArray[chooseEntryToRead]--;
              if(ptr->readersArray[chooseEntryToRead] == 0)
                semV(sem_writers,chooseEntryToRead);
              semV(sem_readers,chooseEntryToRead);

              time_spent = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec);
           }

           else if(choice> ratio){ // is a writer
            writers ++;
            int chooseEntryToWrite = rand()%(entries); //which entry the writer will visit

            gettimeofday(&tv1, NULL);

            semP(sem_writers,chooseEntryToWrite);
            ptr->entryArray[chooseEntryToWrite].writers++;

            gettimeofday(&tv2, NULL);
            //sleep time
            float Uni, Exp;;
            do{
              Uni = rand()%11;
              }while(Uni == 0 || Uni == 10);
            Uni = Uni/10;
            Exp = -log(Uni)/l;
            Exp = Exp*20;
            sleep(Exp);

            semV(sem_writers,chooseEntryToWrite); 

            time_spent = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec);
            }
       } 

       printf("Average time of waiting for this child is: %f\n",time_spent/iterations);
       printf("This child made %d readings\n", readers);
       printf("This child made %d writings\n", writers);
       exit(EXIT_SUCCESS);
     }
  }
  while (wait(&status) > 0);// Wait all chlidren to finish
  wait(&status); 

  for (i=0; i<entries; i++)
      printf("entry %d has %d readers and %d writers\n", ptr->entryArray[i].data, ptr->entryArray[i].readers, ptr->entryArray[i].writers);

  //remove semaphores from the system
  removeSemaphores(sem_writers);
  removeSemaphores(sem_readers);

  detachArray(ptr->entryArray);
  detachReadersArray(ptr->readersArray);
  deleteMemory(ptr->arrayId);
  detachMemory(ptr);
  deleteMemory(memId);
  return 0;
 }