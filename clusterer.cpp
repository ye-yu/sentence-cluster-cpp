#include "cluster.h"
#include "stdio.h"
#include "string"
#include "iostream"
#include "vector"
#include <boost/filesystem.hpp>
#include "cargs.h"

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
  std::string strategy_string = "strategy";
  std::string strategy_default_string = "stem";
  
  str_arg *cluster_strategy = define_str_arg('s', strategy_string.c_str(), strategy_default_string.c_str());

  std::string buffer;
  std::vector<std::string> docs;
  while(fileorstdinline(buffer)) {
    docs.push_back(buffer);
    buffer.clear();
  }

  Cluster cluster(docs, 0.85, false, cluster_strategy->value);

  cluster.print_settings();
  cluster.print_analytics();

  return 0;
}
