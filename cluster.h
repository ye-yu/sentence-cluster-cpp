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
} gram_settings;

class Cluster
{
public:
  Cluster(
    std::vector<std::string> docs,
    const float threshold = 0.85,
    const bool shuffle = true,
    const std::string strategy = "char-gram",
    const std::vector<gram_settings> grams = {gram_settings{3, 3}, gram_settings{5, 3}}
);

  std::vector<cluster_component> clusters;
  int rounds;
  std::vector<cluster_time> times;
  float threshold;
  bool shuffle;
  std::string strategy;
  std::vector<gram_settings> grams;
};

#endif