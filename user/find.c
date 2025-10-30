#include "kernel/types.h"

#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

const char* trim_dir(const char* path) {
  const char* p = path + strlen(path);
  while (*p != '/' && p >= path) {
    --p;
  }
  ++p;
  return p;
}

void find(char* path, char* filename) {
  char buf[512], *p;
  int fd;
  struct stat st;
  struct dirent de;
  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type) {
    case T_DEVICE:
    case T_FILE:
      //   printf("find file: %s\n, after trim: %s\n", path, trim_dir(path));
      if (strcmp(trim_dir(path), filename) == 0) {
        printf("%s\n", path);
      }
      break;
    case T_DIR:
      if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
        printf("find: path too long\n");
        break;
      }
      strcpy(buf, path);
      p = buf + strlen(buf);
      *p++ = '/';
      while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0 || strcmp(de.name, ".") == 0 ||
            strcmp(de.name, "..") == 0) {
          continue;
        }
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        find(buf, filename);
      }
      break;
  }
  close(fd);
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(2, "usage: find path filename\n");
    exit(1);
  }

  find(argv[1], argv[2]);
  exit(0);
}