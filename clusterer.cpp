#include "cluster.h"
#include "stdio.h"
#include "string"
#include "iostream"

FILE *fileorstdin = NULL;

int fileorstdinit(const char *filename) {
  fileorstdin = fopen(filename, "r");
  return fileorstdin == NULL;
}

char fileorstdinchar() {
  return fileorstdin == NULL ? getc(stdin) : getc(fileorstdin);
}

int fileorstdinline(std::string &buffer) {
  char c;
  char c2str[2] = {'\0', '\0'};
  while((c = fileorstdinchar()) != EOF) {
    if (c == '\n') return 1;
    c2str[0] = c;
    buffer.append(c2str);
  }
  return 0;
}

void fileorstdinclose() {
  if (fileorstdin == NULL) return;
  fclose(fileorstdin);
}

int main(int argc, char **argv) {
  if (argc >= 2) fileorstdinit(argv[1]);
  std::string buffer;
  while(fileorstdinline(buffer)) {
    std::cout << buffer << std::endl;
    buffer = "";
  }
  return 0;
}
