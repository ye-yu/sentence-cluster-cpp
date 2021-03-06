#include "string"
#include "iostream"
#include "lancaster.h"
#include "similarity.h"
#include "vector"
#include "algorithm"
#include "regex"
#include <chrono>
#include "random"

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

class Cluster
{
public:
  Cluster(
    std::vector<std::string> docs,
    const float threshold = 0.85,
    const bool shuffle = true,
    const std::string strategy = "char-gram",
    const std::vector<int> grams = {3, 5}
);

  std::vector<cluster_component> clusters;
  int rounds;
  std::vector<cluster_time> times;
  float threshold;
  bool shuffle;
  std::string strategy;
  std::vector<int> grams;
};

Cluster::Cluster(
  std::vector<std::string> docs,
  const float threshold,
  const bool shuffle,
  const std::string strategy,
  const std::vector<int> grams
)
{
  if (strategy != "char-gram" && strategy != "stem") {
    perror("The choosen strategy is neither 'char-gram' nor 'stem'. It is now defaulted to 'char-gram'.");
    this->strategy = "char-gram";
  } else {
    this->strategy = strategy;
  }

  this->threshold = threshold;
  this->shuffle = shuffle;
  this->grams = grams;

  std::vector<sentence_component> stemmed_docs;
  std::map<std::string, std::map<std::string, float>> similarity_context;
  const auto start = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()
  );

  for(auto d: docs) {
    std::vector<std::string> tokens = sentence_token(d);
    if (this->strategy == "char-gram") {
      std::string s;
      for(std::string token: tokens) {
        std::string stemmed = lancaster_stemmer(token);
        s.append(stemmed);
      }

      std::vector<std::string> component;
      for(auto gram: this -> grams) {
        std::vector<std::string> grammed = char_gram(s, gram);
        std::transform(grammed.begin(), grammed.end(), std::back_inserter(component), [](auto a) { return a; });
      }
      stemmed_docs.push_back(sentence_component{d, component});
    } else {
      std::transform(tokens.begin(), tokens.end(), tokens.begin(), ::lancaster_stemmer);
      stemmed_docs.push_back(sentence_component{d, tokens});
    }
  }

  const auto stem_end = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()
  );

  this->times.push_back(cluster_time{
    "stemming",
    stem_end - start
  });

  for(auto stemData: stemmed_docs) {
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

  if (shuffle) {
    std::shuffle(clusters.begin(), clusters.end(), std::default_random_engine());
  }


  while(true) {
    ++this->rounds;
    auto repeat = false;
    std::vector<dpscore> matches;
    for(auto &cluster: clusters) {
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

        if (scores/dps < 0.8) continue; 
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

int main()
{
  std::vector<std::string> documents{
      "He waited for the stop sign to turn to a go sign.",
      "He looked behind the door and didn't like what he saw.",
      "If I don’t like something, I’ll stay away from it.",
      "25 years later, she still regretted that specific moment.",
      "The clock within this blog and the clock on my laptop are 1 hour different from each other.",
      "The fish listened intently to what the frogs had to say.",
      "I really want to go to work, but I am too sick to drive.",
      "It didn't take long for Gary to detect the robbers were amateurs.",
      "The random sentence generator generated a random sentence about a random sentence.",
      "Joe made the sugar cookies; Susan decorated them.",
      "hE wAITED fOr tHE StoP sigN To TUrn To a go sIgN.",
      "hE LOoKEd beHiNd the DOOR aNd DIdn'T LIKE WHaT hE saW.",
      "iF I dOn’T liKe SoMeThIng, I’LL STaY AwAY froM it.",
      "25 yEaRs LaTer, SHe sTILl rEgretTed That sPECiFIC MOmENT.",
      "The CLoCK wITHin thIs BLoG aNd THe cLOck oN my LAPTOP ARe 1 HOur dIFFerEnt FROm EaCH othER.",
      "the FiSH LISTEnEd inteNtLY to whaT the froGs had to saY.",
      "i REally WanT TO gO to woRK, bUt i aM ToO sICK To DRive.",
      "IT Didn't tAkE loNG FOR garY TO Detect ThE rObBERs WeRE AMaTEuRs.",
      "tHe RaNdOm SenTeNcE gENeRatoR gENeraTed A RAnDom SEntEnCe aBOUT a rANDom SeNteNce.",
      "joE maDe THE sUgAr cOOKieS; susAn DecOrAtEd thEM.",
      "He waited until the stop sign turned into a go sign.",
      "He investigated behind the door and hated what he saw.",
      "I’ll not go near a thing if I don’t like it.",
      "She still feel unhappy about that specific moment 25 years later.",
      "The difference of the clock within this blog and the clock on my laptop is 1 hour from each other.",
      "The fish listened while giving a full attention to what the frogs had to say.",
      "I am too sick to drive even though I really want to go to work.",
      "Gary took a short time detect that the robbers were amateurs.",
      "A random sentence about a random sentence can be generated by the random sentence.",
      "Susan decorated the sugar cookies the Joe made.",
      "He waited for the cessation sign to turn to a go sign.",
      "He looked abaft the door and didn't relish what he optically discerned.",
      "If I don’t prefer something, I’ll avoid it.",
      "25 years later, she still regretted that categorical moment.",
      "This blog has a clock that has an hour difference with my laptop clock",
      "The fish heedfully aurally perceived to what the frogs had to verbalize.",
      "I authentically want to go to work, but I am feeling too unwell to drive.",
      "It took momentarily for Gary to ascertain the larcenists were tyros.",
      "The arbitrary sentence generator engendered an arbitrary sentence about an arbitrary sentence.",
      "Joe crafted the sugar cookies; Susan embellished them.",
      "Lets all be unique together until we realise we are all the same.",
      "The virus had powers none of us knew existed.",
      "Choosing to do nothing is still a choice, after all.",
      "Beach-combing replaced wine tasting as his new obsession.",
      "There should have been a time and a place, but this wasn't it.",
      "For oil spots on the floor, nothing beats parking a motorbike in the lounge.",
      "They throw cabbage that turns your brain into emotional baggage.",
      "It didn't make sense unless you had the power to eat colors.",
      "Whenever he saw a red flag warning at the beach he grabbed his surfboard."};
  
  Cluster cluster(documents, 0.85, true, "stem");

  std::string strategy_string = cluster.strategy;

  if (strategy_string == "char-gram") {
    auto gram = cluster.grams.begin();
    strategy_string.append(" {");
    strategy_string.append(std::to_string(*gram++));
    for(; gram != cluster.grams.end(); gram++) {
      strategy_string.append(" ,");
      strategy_string.append(std::to_string(*gram));
    }
    strategy_string.append("}");
  }

  std::cout << "Clustering " << documents.size() << " documents" << std::endl
            << std::endl
            << "Settings:" << std::endl
            << "  - strategy: " << strategy_string << std::endl
            << "  - threshold: " << cluster.threshold << std::endl
            << "  - shuffle: " << (cluster.shuffle ? "yes" : "no") << std::endl
            << std::endl
            << "Analytics:" << std::endl;

  for(auto time: cluster.times) {
    std::cout << "  - " << time.type << ": " << time.time.count() << " ms" << std::endl;
  }

  
  
  std::cout << "  - rounds: " << cluster.rounds << " rounds" << std::endl
            << "  - clusters: " << cluster.clusters.size() << " (" << cluster.clusters.size() * 100 / documents.size() << "%)" << std::endl
            << std::endl 
            << "Clusters:" << std::endl << std::endl;

  for(auto instance: cluster.clusters) {
    std::cout << "id: " << instance.id << std::endl;

    for(auto sentence: instance.datapoints) {
      std::cout << "  - " << sentence << std::endl;
    }

    std::cout << std::endl;
  }
}
