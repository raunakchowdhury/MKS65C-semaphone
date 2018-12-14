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

#define KEY 0xFAD

int main(int argc, char const *argv[]) {
 //attempt to access resources using semaphore -- DOWNING
 int *data;
 char text[256];
 int file;
 int shmid = shmget(KEY, 4, 0644 | IPC_CREAT);
 if (shmid == -1){
   printf("Shared Memory Get Error: %s\n", strerror(errno));
 }
 int semd = semget(KEY, 1, 0);

 struct sembuf sb;
 sb.sem_num = 0;
 sb.sem_flg = SEM_UNDO;
 sb.sem_op = -1;
 semop(semd, &sb, 1);

 printf("Last line:\n" );
 data = shmat(shmid, (void *)0, 0);

 //display last line added to file
 file = open("./story.txt", O_RDONLY);
 if (file == -1){
    printf("File Error: %s\n", strerror(errno));
 }
 lseek(file, 0 - *data, SEEK_END);
 if ( file == -1){ //possible problems here
   printf("lseek Error: %s\n", strerror(errno));
 }

 read(file, text, *data);
 close(file);
 if (errno){ //possible problems here
   printf("Read Error: %s\n", strerror(errno));
 }
 else{
   printf("%s", text);
 }

 //prompt user for next line
 printf("Next line (max 2048 char):\n");
 char *input = calloc(2048, sizeof(char));
 fgets(input, 2049, stdin);
 *data = (int)strlen(input);
 free(input);

 //write to file, update shared memory
 file = open("./story.txt", O_APPEND | O_WRONLY);
 if (file == -1){
    printf("File Error: %s\n", strerror(errno));
 }

 if (write(file, input, strlen(input)) == -1){
    printf("Write Error: %s\n", strerror(errno));
 }
 close(file);

 shmid = shmget( KEY, 4, 0644|IPC_CREAT);

 //release semaphore -- UPPING
 sb.sem_op = 1;
 semop(semd, &sb, 1);

 return 0;
}
