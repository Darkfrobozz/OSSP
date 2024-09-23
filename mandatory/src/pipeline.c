#include <stdio.h>    // puts(), printf(), perror(), getchar()
#include <stdlib.h>   // exit(), EXIT_SUCCESS, EXIT_FAILURE
#include <sys/wait.h> // wait()
#include <unistd.h>   // getpid(), getppid(),fork()

#define READ 0
#define WRITE 1

void child_a(int fd[]) {
  dup2(fd[WRITE], STDOUT_FILENO);
  // We can close the unused descriptors
  close(fd[READ]);
  close(fd[WRITE]);

  // This should be done last leaves pipeline.c
  execlp("ls", "ls", "-F", "-1", NULL);
  perror("Could not switch executable, child a");
  exit(EXIT_FAILURE);
}

void child_b(int fd[]) {
  dup2(fd[READ], STDIN_FILENO);
  // We can close the unused
  close(fd[WRITE]);
  close(fd[READ]);

  // Same as in child a
  execlp("nl", "nl", NULL);
  perror("Could not switch executable, child b");
  exit(EXIT_FAILURE);
}

int main(void) {
  int fd[2];
  if (pipe(fd) == -1) {
    perror("Could not create pipe");
    exit(EXIT_FAILURE);
  }

  switch (fork()) {
  case 0:
    child_a(fd);
    break;
  case -1:
    perror("Could not fork");
    exit(EXIT_FAILURE);
    break;
  }

  switch (fork()) {
  case 0:
    child_b(fd);
    break;
  case -1:
    perror("Done");
    exit(EXIT_FAILURE);
    break;
  }

  // Parent does not need pipes, so close them
  close(fd[0]);
  close(fd[1]);

  // Reuse fd for status
  wait(NULL);
  // printf("%d\n", *fd);
  wait(NULL);
  // printf("%d\n", *(fd + 1));
}
