#include <stdio.h>    // puts(), printf(), perror(), getchar()
#include <stdlib.h>   // exit(), EXIT_SUCCESS, EXIT_FAILURE
#include <sys/wait.h> // wait()
#include <unistd.h>   // getpid(), getppid(),fork()

#define READ 0
#define WRITE 1

void child_a(int fd[]) {
  dup2(fd[WRITE], STDOUT_FILENO);
  // We can close the pipe reader
  close(fd[READ]);

  // This should be done last leaves pipeline.c
  execlp("ls", "ls", "-F", "-1", NULL);
}

void child_b(int fd[]) {
  dup2(fd[READ], STDIN_FILENO);
  // We can close the pipe reader
  close(fd[WRITE]);

  // Same as in child a
  execlp("nl", "nl", NULL);
}

int main(void) {
  int fd[2];
  if (pipe(fd) == -1) {
    perror("Could not create pipe");
  }

  switch (fork()) {
  case 0:
    child_a(fd);
  case -1:
    perror("Could not fork");
  }

  switch (fork()) {
  case 0:
    child_b(fd);
  case -1:
    perror("Done");
  }

  // Parent does not need pipes, so close them
  close(fd[0]);
  close(fd[1]);

  // Reuse fd for status
  wait(fd);
  // printf("%d\n", *fd);
  wait(fd + 1);
  // printf("%d\n", *(fd + 1));
}
