#include "kernel/types.h"
#include "user/user.h"

void primes(int filter, int fd) __attribute__((noreturn));
void primes(int filter, int fd) {
  if (filter > 280) {
    exit(0);
  }

  int n, val;
  int p[2] = {-1, -1};
  while (1) {
    n = read(fd, &val, sizeof(int));
    if (n == 0) {
      break;
    };
    if (val == filter) {
      printf("prime %d\n", val);
    } else if (val > filter) {
      if (p[0] == -1) {
        pipe(p);
        if (fork() == 0) {
          close(fd);
          int new_fd = dup(p[0]);
          close(p[0]);
          close(p[1]);
          primes(val, new_fd);
        } else {
          close(p[0]);
        }
      }

      if (val % filter != 0) {
        write(p[1], &val, sizeof(int));
      }
    }
  }
  if (p[1] != -1) {
    close(p[1]);
  }
  close(fd);
  wait(0);
  exit(0);
}

int main(int argc, char *argv[]) {
  if (argc != 1) {
    fprintf(2, "primes: no arguments expected\n");
    exit(1);
  }
  int p[2];
  pipe(p);
  if (fork() == 0) {
    close(p[1]);
    primes(2, p[0]);
  } else {
    close(p[0]);
    for (int i = 2; i <= 280; i++) {
      write(p[1], &i, sizeof(int));
    }
    close(p[1]);
    wait(0);
    exit(0);
  }
}
