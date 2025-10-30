#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    fprintf(2, "Usage: xargs command [args...]\n");
    exit(1);
  }

  char* arg_buf[MAXARG];
  for (int i = 1; i < argc; i++) {
    int len = strlen(argv[i]);
    arg_buf[i - 1] = malloc(len + 1);
    memmove(arg_buf[i - 1], argv[i], len);
    arg_buf[i - 1][len] = 0;
  }

  int j = argc - 1;
  arg_buf[j] = malloc(128);
  char* p = arg_buf[j];
  while (read(0, p, sizeof(char)) != 0) {
    ++p;
    if (p - arg_buf[j] >= 128) {
      fprintf(2, "xargs: argument too long\n");
      exit(1);
    }
    if (*(p - 1) == '\n') {
      *(p - 1) = 0;
      if (j + 1 >= MAXARG) {
        fprintf(2, "xargs: too many arguments\n");
        exit(1);
      }
      ++j;
      arg_buf[j] = 0;

      if (fork() == 0) {
        // for (int k = 0; k < j; k++) {
        //   printf("arg %d: %s\n", k, arg_buf[k]);
        // }
        exec(arg_buf[0], arg_buf);
      } else {
        wait(0);
        for (int k = argc; k < j; k++) {
          free(arg_buf[k]);
        }
        j = argc - 1;
        arg_buf[j] = malloc(128);
        p = arg_buf[j];
      }
    } else if (*(p - 1) == ' ') {
      *(p - 1) = 0;
      ++j;
      if (j >= MAXARG - 1) {  // leave space for NULL
        fprintf(2, "xargs: too many arguments\n");
        exit(1);
      }
      arg_buf[j] = malloc(128);
      p = arg_buf[j];
    }
  }
  for (int k = 0; k <= argc - 1; k++) {
    free(arg_buf[k]);
  }
  exit(0);
}