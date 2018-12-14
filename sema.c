#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>

#define KEY 0xDEADBEEF


int main() {

  int semd;
  semd = semget(KEY, 1, 0);


  //DOWNING
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_flg = SEM_UNDO;
  sb.sem_op = -1;
  semop(semd, &sb, 1);

  int shmid = shmget(KEY,4,0644 | IPC_CREAT);
  if (shmid == -1) {
    perror("shmget");
    return 1;
  }

  int * data =  (int *) shmat(shmid,(void *)0,0);
  //printf("%p\n", data);
  //printf("%d\n", *data);
  int length = *data;
  //printf("length of last line: %d\n", length);

  int file = open("./file.txt", O_CREAT|O_RDWR|O_APPEND, 0777);
  if (file < 0){
    perror("file error");
  }

  char story_text[1000000];
  int rstatus = read(file, story_text, 1000000);

  if (rstatus == -1) {
   perror("reading story");
   return 1;
  }
  //printf("%s\n", story_text);

  if (strcmp(story_text, "") == 0) {
    printf("You have nothing here. Start your story!\n");
  }
  else {
    char *story = story_text + lseek(file, (length) * -1, SEEK_END) - 2;
    // print last line
    printf("Last line in story:\n%s\n", story);
  }

  printf("What do you want the next line to be? \n");
  char string[1000000];
  fgets(string, sizeof(string), stdin);
  string[strlen(string)] = 0;
  int line = strlen(string);
  //printf("size of new line: %d\n", line);

  int w = write(file, &string, line);
  if (w < 0)
    perror("write error");

  close(file);
  printf("Your line was added to the story.\n");

  line -= 2;
  *data = line;
  int x = shmdt(data);
  //printf("%p\n", data);
  //printf("Value in stored memory: %d\n", *data);

  //UPPING
  sb.sem_op = 1;
  semop(semd, &sb, 1);

  return 0;
}
