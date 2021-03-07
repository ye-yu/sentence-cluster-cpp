/**
  * YeYu Simple English Sentence Clustering Tool
  * Copyright (C) 2020 Ye-Yu rafolwen98@gmail.com
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
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