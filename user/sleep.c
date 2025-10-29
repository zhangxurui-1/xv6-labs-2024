#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(2, "usage: sleep ticks\n");
    exit(1);
  }

  char* n_str;
  int n;
  n_str = argv[1];
  n = atoi(n_str);
  sleep(n);
  exit(0);
}