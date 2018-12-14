# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/shm.h>
# include <sys/ipc.h>
# include <sys/types.h>
# include <sys/sem.h>
# include <fcntl.h>
# include <errno.h>
# include <unistd.h>

# define KEY 0xFAD

// UNCCOMENT WHEN ON LINUX
// union semun{
//   int val; // used for SETVAL
//   struct semid_ds *buf; // used for IPC_STAT and IPC_SET
//   unsigned short *array; // used for SETALL
//   struct seminfo *__buf; // buffer for IPC_INFO
// };

int main(int argc, char const *argv[]) {
  /* code */
  const char * command;
  if (!argv[1]){
    printf("No arg inputted! Program exiting.\n");
    exit(0);
  }
  command = argv[1];
  int file;
  // printf("argc: %d command: %s\n", argc, command);
  // printf("strcmp(command, \"-v\"): %d\n", strcmp(command, "-v"));

  // create mode
  if (!strcmp(command, "-c")){
    int shmid = shmget(KEY, 4, 0644 | IPC_CREAT);
    // if connection fails
    if (shmid == -1){
      printf("Shared Memory Get Error: %s\n", strerror(errno));
    }

    file = open("./story.txt", O_CREAT | O_TRUNC, 0644);
    // if connection fails
    if (file == -1){
      printf("File Error: %s\n", strerror(errno));
    }

    int semaphore = semget(KEY, 1, IPC_CREAT | 0644);
    // if connection fails
    if (semaphore == -1){
      printf("Semaphore Get Error: %s\n", strerror(errno));
      semaphore = semget(KEY, 1, 0);
      int v = semctl(semaphore, 0, GETVAL, 0);
      /* printf("semctl returned: %d\n", v); */
    }
    else {
      union semun data;
      data.val = 1;
      int r = semctl(semaphore, 0, SETVAL, data);
      /* printf("semctl returned: %d\n", r); */
    }
  }
  // view mode
  else if (!strcmp(command, "-v")){
    file = open("./story.txt", O_RDONLY);
    // if connection fails
    if (file == -1){
      printf("File Error: %s\n", strerror(errno));
    }

    char text[1000];
    read(file, text, 999);
    close(file);
    if (errno){ //possible problems here
      printf("Read Error: %s\n", strerror(errno));
    }
    else{
      printf("Story Below:\n%s\n", text);
    }
  }
  // remove mode
  else if (!strcmp(command, "-r")){
    int shmid = shmget(KEY, 4, 0644);
    // if connection fails
    if (shmid == -1){
      printf("Shared Memory Get Error: %s\n", strerror(errno));
    }
    shmctl(shmid, IPC_RMID, NULL);

    char text[1000];
    file = open("./story.txt", O_RDONLY);
    read(file, text, 999);
    close(file);
    if (errno){ //possible problems here
      printf("Read Error: %s\n", strerror(errno));
    }
    else{
      printf("Deleted Story Below:\n%s\n", text);
    }

    if (remove("./story.txt")){
      printf("Error! Unable to delete story or story does not exist!\n");
    }

    int semaphore = semget(KEY, 1, IPC_CREAT | 0644);
    // if connection fails
    if (semaphore == -1){
      printf("Semaphore Get Error: %s\n", strerror(errno));
    }
    union semun data;
    if (semctl(semaphore, 1, IPC_RMID, data) == -1){
      printf("Semaphore Delete Error: %s\n", strerror(errno));
    }
  }
  else {
    printf("Error: Please input a valid parameter (-v, -r, or -c)!\n");
  }
  return 0;
}
