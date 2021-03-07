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
#include "string"
#include "iostream"
#include "lancaster.h"
#include "similarity.h"
#include "vector"
#include "algorithm"
#include "regex"
#include <chrono>
#include "random"
#include "cluster.h"
#include <stdlib.h>

void init_progress(std::string progress_message = "") {
  static std::chrono::milliseconds time_start = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    );

  static std::string _progress_msg = "";

  if (progress_message.empty()) {
    const auto start = time_start;
    time_start = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    );
    std::cout << "\rFinished: " << _progress_msg << " - " << (time_start - start).count() << " ms" << std::endl;
  } else {
    _progress_msg = progress_message;
  }
}

void update_progress(int progress, int total, std::string progress_message) {
  std::cout << "\r" << progress << "/" << total << " - " << progress_message << "             ";
}

std::vector<std::string> sentence_token(const std::string& s) {
  std::vector<std::string> tokens;
  std::string token;

  std::regex rgx("[^a-zA-Z']");
  std::sregex_token_iterator iter(s.begin(),
    s.end(),
    rgx,
    -1);
  std::sregex_token_iterator end;
  for ( ; iter != end; ++iter) {
    token = *iter;
    if (token.length() == 0) continue;
    std::transform(token.begin(), token.end(), token.begin(), ::tolower);
    tokens.push_back(token);
  }
  return tokens;
}

void Cluster::add_docs(std::vector<std::string> docs) {
  std::transform(docs.begin(), docs.end(), std::back_inserter(this -> documents), [](auto a){ return a;});
}

void Cluster::stem_strategy(std::string strategy, std::vector<gram_setting> gram_settings) {
  if (strategy != "char-gram" && strategy != "stem") {
    fprintf(stderr, "The choosen strategy is neither 'char-gram' nor 'stem'. It is now defaulted to 'char-gram'.\n");
    this->strategy = "char-gram";
  } else {
    this->strategy = strategy;
  }

  if (gram_settings.empty()) {
    fprintf(stderr, "Gram settings cannot be empty. Defaulting to {3:3,5:3}.\n");
  } else {
    this->grams.clear();
    std::transform(gram_settings.begin(), gram_settings.end(), std::back_inserter(this->grams), [](auto a){return a;});
  }
}

void Cluster::fit() {
  if (this->documents.empty()) {
    fprintf(stderr, "No documents to cluster yet!");
    return;
  }
  if (!this->clusters.empty()) return;
  std::vector<sentence_component> stemmed_docs;
  std::map<std::string, std::map<std::string, float>> similarity_context;
  const auto start = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()
  );

  init_progress("Stemming documents");
  static int docs_progress;
  docs_progress = 0;
  for(auto d: this->documents) {
    update_progress(++docs_progress, this->documents.size(), "Stemming documents");
    std::vector<std::string> tokens = sentence_token(d);
    if (this->strategy == "char-gram") {
      std::string s;
      for(std::string token: tokens) {
        std::string stemmed = lancaster_stemmer(token);
        s.append(stemmed);
      }

      std::vector<std::string> component;
      for(auto gram: this -> grams) {
        std::vector<std::string> grammed = char_gram(s, gram.size, gram.stride);
        std::transform(grammed.begin(), grammed.end(), std::back_inserter(component), [](auto a) { return a; });
      }
      stemmed_docs.push_back(sentence_component{d, component});
    } else {
      std::transform(tokens.begin(), tokens.end(), tokens.begin(), ::lancaster_stemmer);
      stemmed_docs.push_back(sentence_component{d, tokens});
    }
  }
  init_progress();

  const auto stem_end = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()
  );

  this->times.push_back(cluster_time{
    "stemming",
    stem_end - start
  });

  init_progress("Calculating similarity");
  static int sim_progress;
  sim_progress = 0;
  for(auto stemData: stemmed_docs) {
    update_progress(++sim_progress, stemmed_docs.size(), "Calculating similarity");
    if (similarity_context.find(stemData.sentence) == similarity_context.end()) {
      similarity_context[stemData.sentence] = std::map<std::string, float>();
    }

    for(auto simTarget: stemmed_docs) {
      if (similarity_context.find(simTarget.sentence) == similarity_context.end()) {
        similarity_context[simTarget.sentence] = std::map<std::string, float>();
      }

      if (!simTarget.sentence.compare(stemData.sentence)) continue;

      {
        auto test_candidate = similarity_context[stemData.sentence];
        if (test_candidate.find(simTarget.sentence) != test_candidate.end()) continue;
      }

      {
        auto test_candidate = similarity_context[simTarget.sentence];
        if (test_candidate.find(stemData.sentence) != test_candidate.end()) continue;
      }

      const auto similarity_score = similarity(stemData.components, simTarget.components);
      similarity_context[stemData.sentence][simTarget.sentence] = similarity_score;
      similarity_context[simTarget.sentence][stemData.sentence] = similarity_score;
    }
  }
  init_progress();

  const auto sim_end = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()
  );

  this->times.push_back(cluster_time{
    "similarity",
    sim_end - stem_end
  });

  std::vector<cluster_component> clusters;

  int id = 0;
  for(auto entry: similarity_context) {
    const auto key = entry.first;
    clusters.push_back(cluster_component{
      ++id,
      std::vector<std::string>{key},
      true
    });
  }

  if (this->shuffle) {
    std::shuffle(clusters.begin(), clusters.end(), std::default_random_engine());
  }

  this->rounds = 0;
  while(true) {
    ++this->rounds;
    auto repeat = false;
    std::vector<dpscore> matches;

    static int cluster_progress;
    cluster_progress = 0;
    std::string cluster_progress_message = "Clustering round " + std::to_string(this->rounds);
    init_progress(cluster_progress_message);
    for(auto &cluster: clusters) {
      update_progress(++cluster_progress, clusters.size(), cluster_progress_message);
      if (!cluster.keep) continue;

      for(auto &test: clusters) {
        if (test.id == cluster.id) continue;
        if (!test.keep) continue;

        float scores = 0;
        int dps = 0;
        for(const auto test_dp: test.datapoints) {
          for(const auto clust_dp: cluster.datapoints) {
            auto smap = similarity_context[test_dp];
            if (smap.find(clust_dp) == smap.end()) continue;
            scores += smap[clust_dp];
            dps++;
          }
        }

        if (!dps) continue;

        if (scores/dps < this->threshold) continue; 
        matches.push_back(dpscore{
          scores/dps,
          test
        });
      }

      if (matches.empty()) continue;
      repeat = true;
      int highest_index = 0;

      for(int i = 1; i < matches.size(); i++) {
        if (matches[highest_index].score > matches[i].score) continue;
        highest_index = i;
      }

      dpscore &highest = matches[highest_index];
      highest.value.keep = false;
      for(auto c: highest.value.datapoints) {
        cluster.datapoints.push_back(c);
      }

      matches.clear();
    }
    init_progress("");

    if(!repeat) break;
  }

  const auto cluster_end = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()
  );

  this->times.push_back(cluster_time{
    "cluster",
    cluster_end - sim_end
  });

  this->times.push_back(cluster_time{
    "total",
    cluster_end - start
  });

  for(auto cluster: clusters) {
    if (!cluster.keep) continue;
    this->clusters.push_back(cluster);
  }
}

void Cluster::print_settings() {
  std::string strategy_string = this->strategy;

  if (strategy_string == "char-gram") {
    auto gram = this->grams.begin();
    strategy_string.append(" {");
    strategy_string.append(std::to_string(gram->size));
    strategy_string.append(":");
    strategy_string.append(std::to_string(gram->stride));
    ++gram;
    for(; gram != this->grams.end(); gram++) {
      strategy_string.append(", ");
      strategy_string.append(std::to_string(gram->size));
      strategy_string.append(":");
      strategy_string.append(std::to_string(gram->stride));
    }
    strategy_string.append("}");
  }

  std::cout << "Settings:" << std::endl
            << "  - strategy: " << strategy_string << std::endl
            << "  - threshold: " << this->threshold << std::endl
            << "  - shuffle: " << (this->shuffle ? "yes" : "no") << std::endl;
}

void Cluster::print_analytics() {
  if (this->clusters.empty()) {
    fprintf(stderr, "No clustering has been performed yet!\n");
    return;
  }

  std::cout << "Analytics:" << std::endl;

  for(auto time: this->times) std::cout << "  - " << time.type << ": " << time.time.count() << " ms" << std::endl;

  std::cout << "  - rounds: " << this->rounds << " rounds" << std::endl
            << "  - clusters: " << this->clusters.size() << " (" << this->clusters.size() * 100 / this->documents.size() << "%)" << std::endl
            << std::endl;
}

Cluster::Cluster() {
  this->threshold=0.85;
  this->shuffle-true;
  this->strategy="char-gram";
  this->grams.push_back(gram_setting{3, 3});
  this->grams.push_back(gram_setting{5, 3});
}
