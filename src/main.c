#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  printf("$ ");

  char command[1024];
  fgets(command, sizeof(command), stdin);
  command[strspn(command, "\n")] = '\0';

  return 0;
}
