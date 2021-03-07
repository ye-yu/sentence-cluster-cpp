#ifndef CLUSTER_H
#define CLUSTER_H

#include "chrono"
#include "vector"
#include "map"
#include "random"

typedef struct
{
  std::string sentence;
  std::vector<std::string> components;

} sentence_component;

typedef struct
{
  int id;
  std::vector<std::string> datapoints;
  bool keep;
} cluster_component;

typedef struct
{
  float score;
  cluster_component &value;
} dpscore;

typedef struct {
  std::string type;
  std::chrono::milliseconds time;
} cluster_time;

typedef struct {
  int size;
  int stride;
} gram_setting;

class Cluster
{
public:
  Cluster();

  std::vector<cluster_component> clusters;
  int rounds;
  std::vector<cluster_time> times;
  float threshold;
  bool shuffle;
  std::string strategy;
  std::vector<gram_setting> grams;
  std::vector<std::string> documents;
  void print_settings();
  void print_analytics();
  void fit();
  void add_docs(std::vector<std::string> docs);
  void stem_strategy(
    std::string strategy,
    std::vector<gram_setting> gram_setting = {gram_setting{3, 3}, gram_setting{3, 5}}
  );
};

#endif