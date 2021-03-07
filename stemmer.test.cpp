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
#include <vector>
#include <sstream>
#include "lancaster.h"
#include <algorithm>
#include <regex>
#include <iostream>

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

int main() {
  std::cout << "Stemming words" << std::endl;

  const std::string test_sentences = "Welcome to the website. If you're here, you're likely looking to find random words. Random Word Generator is the perfect tool to help you do this. While this tool isn't a word creator, it is a word generator that will generate random words for a variety of activities or uses. Even better, it allows you to adjust the parameters of the random words to best fit your needs.";
  auto words = sentence_token(test_sentences);

  for(auto to_stem: words) {
    std::cout << "Stem: " << to_stem << " -> " << lancaster_stemmer(to_stem) << std::endl; 
  }

  return 0;
}
