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

std::vector<std::string> split_str(std::string input, const char c) {
  std::vector<std::string> output;
  int i = 0;
  int start = 0;
  bool skip_next = false;
  for(auto t: input) {
    if (skip_next) {
      skip_next = false;
      ++i;
      start = i;
      continue;
    }
    if (t == c) {
      output.push_back(input.substr(start, i - start));
      skip_next = true;
    }
    ++i;
  }
  output.push_back(input.substr(start - 1, -1));
  return output;
}

std::vector<gram_setting> parse_gram_settings(std::string settings_str) {
  std::vector<gram_setting> settings;
  for(auto gram_setting_str : split_str(settings_str, ',')) {
    std::vector gram_setting_split = split_str(gram_setting_str, ':');
    if (gram_setting_split.size() != 2) throw std::runtime_error("Invalid settings");
    int size = std::stoi(gram_setting_split[0]);
    int stride = std::stoi(gram_setting_split[1]);
    settings.push_back(gram_setting{size, stride});
  }
  return settings;
}

int main(int argc, char **argv) {
  if (argc >= 2) fileorstdinit(argv[1]);
  std::string strategy_string = "strategy";
  std::string strategy_default_string = "stem";
  std::string outdir_string = "out";
  std::string stdout_string = "stdout";
  std::string analytics_string = "analytics";
  std::string verbose_string = "verbose";
  std::string gram_string = "gram";
  
  str_arg *cluster_strategy = define_str_arg('s', strategy_string.c_str(), strategy_default_string.c_str());
  str_arg *out_directory = define_str_arg('o', outdir_string.c_str(), NULL);
  str_arg *gram_setting_arg = define_str_arg('g', gram_string.c_str(), "3:4,7:4");

  flag_arg *stdout_cluster = define_flag_arg('O', stdout_string.c_str(), false);
  flag_arg *print_analytics = define_flag_arg('a', analytics_string.c_str(), false);
  flag_arg *verbose = define_flag_arg('V', verbose_string.c_str(), false);

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
    if (buffer.empty()) continue;
    docs.push_back(buffer);
    buffer.clear();
  }

  Cluster cluster;
  cluster.threshold = 0.85;
  cluster.shuffle = true;
  cluster.add_docs(docs);

  try {
    std::vector<gram_setting> gram_settings = parse_gram_settings(gram_setting_arg->value);
    cluster.stem_strategy(cluster_strategy->value, gram_settings);
  } catch (const std::exception& e) {
    fprintf(stderr, "Exceptions in parsing gram settings! Reason: %s\n", e.what());
    fprintf(stderr, "Using default gram settings (if applicable).\n");
    cluster.stem_strategy(cluster_strategy->value);
  }

  if(verbose->value){
    cluster.print_settings();
    std::cout << std::endl;
  }

  cluster.fit();

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

  free_args();

  return 0;
}
