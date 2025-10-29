#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  if (argc != 1) {
    fprintf(2, "pingpong: no arguments expected\n");
    exit(1);
  }

  int p1[2], p2[2];
  pipe(p1);
  pipe(p2);
  if (fork() == 0) {
    char buf[2];
    close(p1[1]);
    close(p2[0]);
    if (read(p1[0], buf, sizeof(buf)) > 0) {
      printf("%d: received ping\n", getpid());
      write(p2[1], "b", 1);
    }
    close(p1[0]);
    close(p2[1]);
  } else {
    char buf[2];
    close(p1[0]);
    close(p2[1]);
    write(p1[1], "b", 1);
    if (read(p2[0], buf, sizeof(buf)) > 0) {
      printf("%d: received pong\n", getpid());
    }
    close(p1[1]);
    close(p2[0]);
    wait(0);
    exit(0);
  }
}