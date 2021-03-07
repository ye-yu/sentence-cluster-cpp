#include "cluster.h"
#include "stdio.h"
#include "string"
#include "iostream"
#include "vector"
#include "filesystem"
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
  std::string outdir_string = "out";
  std::string stdout_string = "stdout";
  std::string analytics_string = "analytics";
  
  str_arg *cluster_strategy = define_str_arg('s', strategy_string.c_str(), strategy_default_string.c_str());
  str_arg *out_directory = define_str_arg('o', outdir_string.c_str(), NULL);
  flag_arg *stdout_cluster = define_flag_arg('O', stdout_string.c_str(), false);
  flag_arg *print_analytics = define_flag_arg('a', analytics_string.c_str(), false);

  process_args(argc, argv);

  std::string outdir(out_directory->value);
  if (!outdir.empty()) {
    std::filesystem::path f{outdir};
    if (!std::filesystem::is_directory(f)) {
      fprintf(stderr, "%s is not a directory!\n", outdir.c_str());
      return 1;
    }
  }

  std::string buffer;
  std::vector<std::string> docs;
  while(fileorstdinline(buffer)) {
    docs.push_back(buffer);
    buffer.clear();
  }

  Cluster cluster(docs, 0.85, false, cluster_strategy->value);

  cluster.print_settings();
  if (print_analytics->value) {
    std::cout << std::endl;
    cluster.print_analytics();
  } else {
    std::cout << std::endl
              << "Done (" << ((cluster.times.end() - 1) -> time).count() 
              << " ms)" << std::endl;
  }

  if (stdout_cluster->value) {
    std::cout << std::endl;
    for(auto cluster: cluster.clusters) {
      std::cout << "id: " << cluster.id << std::endl;
      for(auto cinstance: cluster.datapoints) {
        std::cout << "  - " <<  cinstance << std::endl;
      }
      std::cout << std::endl;
    }
  }

  if (!outdir.empty()) {
    std::filesystem::path target_dir{outdir};
    for(auto cluster: cluster.clusters) {
      auto filename = target_dir / std::to_string(cluster.id);
      FILE *f = fopen(filename.c_str(), "w");
      if (f == NULL) {
        fprintf(stderr, "Cannot create file: %s!\n", filename.c_str());
        return 1;
      }
      for(auto cinstance: cluster.datapoints) {
        fprintf(f, "%s\n", cinstance.c_str());
      }
      fclose(f);
    }
  }

  return 0;
}
