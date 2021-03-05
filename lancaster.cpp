#include <map>
#include <algorithm>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include "lancaster.h"

typedef enum _stemmer_type
{
  STEM_STOP,
  STEM_INTACT,
  STEM_CONT,
  STEM_PROTECT,
} stemmer_type;

typedef struct _rule_instance
{
  std::string match;
  std::string replacement;
  stemmer_type type;
} rule_instance;

static std::string vowelgroup = "aeiouy";

static std::map<char, std::vector<rule_instance>> lancaster{
    {
        'a',
        std::vector<rule_instance>({rule_instance{"ia", "", STEM_INTACT},
                                    rule_instance{"a", "", STEM_INTACT}}),
    },
    {
        'b',
        std::vector<rule_instance>({rule_instance{"bb", "b", STEM_STOP}}),
    },
    {
        'c',
        std::vector<rule_instance>({rule_instance{"ytic", "ys", STEM_STOP},
                                    rule_instance{"ic", "", STEM_CONT},
                                    rule_instance{"nc", "nt", STEM_CONT}}),
    },
    {
        'd',
        std::vector<rule_instance>({rule_instance{"dd", "d", STEM_STOP},
                                    rule_instance{"ied", "y", STEM_CONT},
                                    rule_instance{"ceed", "cess", STEM_STOP},
                                    rule_instance{"eed", "ee", STEM_STOP},
                                    rule_instance{"ed", "", STEM_CONT},
                                    rule_instance{"hood", "", STEM_CONT}}),
    },
    {
        'e',
        std::vector<rule_instance>({rule_instance{"e", "", STEM_CONT}}),
    },
    {
        'f',
        std::vector<rule_instance>({rule_instance{"lief", "liev", STEM_STOP},
                                    rule_instance{"if", "", STEM_CONT}}),
    },
    {
        'g',
        std::vector<rule_instance>({rule_instance{"ing", "", STEM_CONT},
                                    rule_instance{"iag", "y", STEM_STOP},
                                    rule_instance{"ag", "", STEM_CONT},
                                    rule_instance{"gg", "g", STEM_STOP}}),
    },
    {
        'h',
        std::vector<rule_instance>({rule_instance{"th", "", STEM_INTACT},
                                    rule_instance{"guish", "ct", STEM_STOP},
                                    rule_instance{"ish", "", STEM_CONT}}),
    },
    {
        'i',
        std::vector<rule_instance>({rule_instance{"i", "", STEM_INTACT},
                                    rule_instance{"i", "y", STEM_CONT}}),
    },
    {
        'j',
        std::vector<rule_instance>({rule_instance{"ij", "id", STEM_STOP},
                                    rule_instance{"fuj", "fus", STEM_STOP},
                                    rule_instance{"uj", "ud", STEM_STOP},
                                    rule_instance{"oj", "od", STEM_STOP},
                                    rule_instance{"hej", "her", STEM_STOP},
                                    rule_instance{"verj", "vert", STEM_STOP},
                                    rule_instance{"misj", "mit", STEM_STOP},
                                    rule_instance{"nj", "nd", STEM_STOP},
                                    rule_instance{"j", "s", STEM_STOP}}),
    },
    {
        'l',
        std::vector<rule_instance>({rule_instance{"ifiabl", "", STEM_STOP},
                                    rule_instance{"iabl", "y", STEM_STOP},
                                    rule_instance{"abl", "", STEM_CONT},
                                    rule_instance{"ibl", "", STEM_STOP},
                                    rule_instance{"bil", "bl", STEM_CONT},
                                    rule_instance{"cl", "c", STEM_STOP},
                                    rule_instance{"iful", "y", STEM_STOP},
                                    rule_instance{"ful", "", STEM_CONT},
                                    rule_instance{"ul", "", STEM_STOP},
                                    rule_instance{"ial", "", STEM_CONT},
                                    rule_instance{"ual", "", STEM_CONT},
                                    rule_instance{"al", "", STEM_CONT},
                                    rule_instance{"ll", "l", STEM_STOP}}),
    },
    {
        'm',
        std::vector<rule_instance>({rule_instance{"ium", "", STEM_STOP},
                                    rule_instance{"um", "", STEM_INTACT},
                                    rule_instance{"ism", "", STEM_CONT},
                                    rule_instance{"mm", "m", STEM_STOP}}),
    },
    {
        'n',
        std::vector<rule_instance>({rule_instance{"sion", "j", STEM_CONT},
                                    rule_instance{"xion", "ct", STEM_STOP},
                                    rule_instance{"ion", "", STEM_CONT},
                                    rule_instance{"ian", "", STEM_CONT},
                                    rule_instance{"an", "", STEM_CONT},
                                    rule_instance{"een", "", STEM_PROTECT},
                                    rule_instance{"en", "", STEM_CONT},
                                    rule_instance{"nn", "n", STEM_STOP}}),
    },
    {
        'p',
        std::vector<rule_instance>({rule_instance{"ship", "", STEM_CONT},
                                    rule_instance{"pp", "p", STEM_STOP}}),
    },
    {
        'r',
        std::vector<rule_instance>({rule_instance{"er", "", STEM_CONT},
                                    rule_instance{"ear", "", STEM_PROTECT},
                                    rule_instance{"ar", "", STEM_STOP},
                                    rule_instance{"ior", "", STEM_CONT},
                                    rule_instance{"or", "", STEM_CONT},
                                    rule_instance{"ur", "", STEM_CONT},
                                    rule_instance{"rr", "r", STEM_STOP},
                                    rule_instance{"tr", "t", STEM_CONT},
                                    rule_instance{"ier", "y", STEM_CONT}}),
    },
    {
        's',
        std::vector<rule_instance>({rule_instance{"ies", "y", STEM_CONT},
                                    rule_instance{"sis", "s", STEM_STOP},
                                    rule_instance{"is", "", STEM_CONT},
                                    rule_instance{"ness", "", STEM_CONT},
                                    rule_instance{"ss", "", STEM_PROTECT},
                                    rule_instance{"ous", "", STEM_CONT},
                                    rule_instance{"us", "", STEM_INTACT},
                                    rule_instance{"s", "", STEM_CONT},
                                    rule_instance{"s", "", STEM_STOP}}),
    },
    {
        't',
        std::vector<rule_instance>({rule_instance{"plicat", "ply", STEM_STOP},
                                    rule_instance{"at", "", STEM_CONT},
                                    rule_instance{"ment", "", STEM_CONT},
                                    rule_instance{"ent", "", STEM_CONT},
                                    rule_instance{"ant", "", STEM_CONT},
                                    rule_instance{"ript", "rib", STEM_STOP},
                                    rule_instance{"orpt", "orb", STEM_STOP},
                                    rule_instance{"duct", "duc", STEM_STOP},
                                    rule_instance{"sumpt", "sum", STEM_STOP},
                                    rule_instance{"cept", "ceiv", STEM_STOP},
                                    rule_instance{"olut", "olv", STEM_STOP},
                                    rule_instance{"sist", "", STEM_PROTECT},
                                    rule_instance{"ist", "", STEM_CONT},
                                    rule_instance{"tt", "t", STEM_STOP}}),
    },
    {
        'u',
        std::vector<rule_instance>({rule_instance{"iqu", "", STEM_STOP},
                                    rule_instance{"ogu", "og", STEM_STOP}}),
    },
    {
        'v',
        std::vector<rule_instance>({rule_instance{"siv", "j", STEM_CONT},
                                    rule_instance{"eiv", "", STEM_PROTECT},
                                    rule_instance{"iv", "", STEM_CONT}}),
    },
    {
        'y',
        std::vector<rule_instance>({rule_instance{"bly", "bl", STEM_CONT},
                                    rule_instance{"ily", "y", STEM_CONT},
                                    rule_instance{"ply", "", STEM_PROTECT},
                                    rule_instance{"ly", "", STEM_CONT},
                                    rule_instance{"ogy", "og", STEM_STOP},
                                    rule_instance{"phy", "ph", STEM_STOP},
                                    rule_instance{"omy", "om", STEM_STOP},
                                    rule_instance{"opy", "op", STEM_STOP},
                                    rule_instance{"ity", "", STEM_CONT},
                                    rule_instance{"ety", "", STEM_CONT},
                                    rule_instance{"lty", "l", STEM_STOP},
                                    rule_instance{"istry", "", STEM_STOP},
                                    rule_instance{"ary", "", STEM_CONT},
                                    rule_instance{"ory", "", STEM_CONT},
                                    rule_instance{"ify", "", STEM_STOP},
                                    rule_instance{"ncy", "nt", STEM_CONT},
                                    rule_instance{"acy", "", STEM_CONT}}),
    },
    {
        'z',
        std::vector<rule_instance>({rule_instance{"iz", "", STEM_CONT},
                                    rule_instance{"yz", "ys", STEM_STOP}}),
    },

};

bool has_vowel(const std::string value)
{
  for (size_t i = 0; i < value.length(); i++)
  {
    const bool is_vowel = value[i] == 'a' || value[i] == 'e' || value[i] == 'i' || value[i] == 'o' || value[i] == 'u' || value[i] == 'y';
    if (is_vowel)
      return true;
  }
  return false;
}

bool should_stem(std::string value)
{
  if (value.length() == 0)
    return false;

  const bool first_is_vowel = value[0] == 'a' || value[0] == 'e' || value[0] == 'i' || value[0] == 'o' || value[0] == 'u' || value[0] == 'y';
  return first_is_vowel
             ? value.length() > 1
             : value.length() > 2 && has_vowel(value);
}

std::string lancaster_stemmer(std::string value, bool is_intact)
{
  auto rules = lancaster[value[value.length() - 1]];
  for (size_t i = 0; i < rules.size(); ++i)
  {
    auto rule = rules[i];
    if (rule.type == STEM_INTACT && !is_intact)
      continue;
    const int breakpoint = value.length() - rule.match.length();
    if (breakpoint < 0 || value.compare(breakpoint, value.length() - breakpoint, rule.match))
    {
      continue;
    }
    if (rule.type == STEM_PROTECT)
      return value;

    std::string next = value.substr(0, breakpoint).append(rule.replacement);

    if (!should_stem(next))
      continue;

    if (rule.type == STEM_CONT)
      return lancaster_stemmer(next, false);

    return next;
  }
  return value;
}

std::string lancaster_stemmer(std::string value) {
  return lancaster_stemmer(value, true);
}

void lancaster_stemmer_c(char *dest, const char *source) {
  std::string result = lancaster_stemmer(source);
  for(auto c: result) {
    (*dest) = c;
    dest++;
  }
  (*dest) = '\0';
}
