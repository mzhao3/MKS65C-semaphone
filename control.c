#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define KEY 0xDEADBEEF

union semun {
  int              val;   // Value for SETVAL /
  struct semid_ds *buf;   // Buffer for IPC_STAT, IPC_SET /
  unsigned short  *array; // Array for GETALL, SETALL /
  struct seminfo  *__buf; // Buffer for IPC_INFO /
};


int main(int argc, char* argv[]) {
  if (argc < 2){
    printf("No arguments \n");
    exit(0);
  }

  //printf("What would you like to do with the data? Create? Remove? or View? \n");
  char * args = argv[1];

  if(strcmp(args,"-c") == 0){
    int shmid;
    char * data;

    shmid = shmget(KEY,1024,0644 | IPC_CREAT);
    data = shmat(shmid,(void *)0,0);
    if(data == (char*)(-1)){
      perror("shmat");
    }


    printf("Created shared memory \n");

    int semd;
    semd= semget(KEY,1,IPC_CREAT | IPC_EXCL| 0777);
    if(semd== -1){
      printf("semaphore already exists\n");
      semd = semget(KEY,1,0);
      int v;
      v = semctl(semd,0,GETVAL,0);
      printf("semctl: %d \n",v);
    }

    else {
      union semun us;
      us.val = 1;
      int r;
      r =semctl(semd, 0, SETVAL, us);
      printf("Created semaphore: %d \n",r);
    }

    int file = open("file.txt", O_CREAT | O_TRUNC);
    if(file < 0){
      printf("file error");
    }
    printf("Created file \n");

  }


  else if(strcmp(args,"-r") == 0){
    int shmid;
    shmid = shmget(0,200,0644 | IPC_CREAT);
    int semd = semget(KEY,1,0);

    printf("waiting, semaphore unavailable at the moment\n");
    while(semctl(semd,0,GETVAL)==0);

    printf("Removing \n");

    shmctl(semd,0,IPC_RMID);

    int file = open("./file.txt", O_RDONLY);
    char readin[1000000];
    if(read(file,readin,1000000) < 0){
      printf("error viewing \n");
      return 0;
    }
    else{
      printf("Story time! \n%s \n",readin);
    }

    //remove
    shmctl(shmid, IPC_RMID, NULL);
    remove("./file.txt");

  }


  else if(strcmp(args,"-v") == 0){
    int file = open("./file.txt", O_RDONLY);

    char readin[1000000];
    if(read(file,readin,1000000) < 0){
      printf("error viewing \n");
      return 0;
    }
    else{
      printf("Story time! \n%s \n",readin);
    }

  }

  else{
    printf("Wrong format\n");
  }



  return 0;
}
