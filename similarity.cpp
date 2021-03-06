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
#include "lancaster.h"
#include "vector"
#include "algorithm"
#include "map"
#include "math.h"
#include "similarity.h"

std::vector<std::string> char_gram(std::string input, const int maxgram, const int stride)
{
  std::vector<std::string> grams;
  if (input.length() <= maxgram)
  {
    grams.push_back(input);
    return grams;
  }

  const auto _stride = stride == -1 ? 0 : stride;
  const auto should_stride = !!_stride;
  auto stride_count = -1;

  for (size_t i = 0; i < input.length() - maxgram + 1; i++)
  {
    if (should_stride) {
      stride_count = ++stride_count % _stride;
      if (stride_count) continue;
    }
    grams.push_back(input.substr(i, maxgram));
  }

  return grams;
}

float cosine_similarity_vectors(std::vector<int> A, std::vector<int> B)
{
  float mul = 0.0;
  float d_a = 0.0;
  float d_b = 0.0;

  if (A.size() != B.size())
  {
    return -1;
  }

  // Prevent Division by zero
  if (A.size() < 1)
  {
    return -1;
  }

  std::vector<int>::iterator B_iter = B.begin();
  std::vector<int>::iterator A_iter = A.begin();
  for (; A_iter != A.end(); A_iter++, B_iter++)
  {
    mul += *A_iter * *B_iter;
    d_a += *A_iter * *A_iter;
    d_b += *B_iter * *B_iter;
  }

  if (d_a == 0.0f || d_b == 0.0f)
  {
    return -1;
  }

  return mul / (sqrt(d_a) * sqrt(d_b));
}

float similarity(std::vector<std::string> s1grammed, std::vector<std::string> s2grammed)
{
  std::map<std::string, int> hmap;
  std::map<std::string, int> s1map;
  std::map<std::string, int> s2map;
  std::vector<int> s1vector;
  std::vector<int> s2vector;
  for (auto tok : s1grammed)
  {
    hmap[tok] = 1;
  }

  for (auto tok : s2grammed)
  {
    hmap[tok] = 1;
  }

  // initializes all components to zero
  for (auto const &entry : hmap)
  {
    s1map[entry.first] = 0;
    s2map[entry.first] = 0;
  }

  // reiterate map
  for (auto tok : s1grammed)
  {
    s1map[tok] += 1;
  }

  for (auto tok : s2grammed)
  {
    s2map[tok] += 1;
  }

  // fill in the vector
  for (auto const &entry : hmap)
  {
    s1vector.push_back(s1map[entry.first]);
    s2vector.push_back(s2map[entry.first]);
  }

  return cosine_similarity_vectors(s1vector, s2vector);
}

float similarity(std::string s1, std::string s2, const int maxgram = 3)
{
  std::vector<std::string> s1grammed = char_gram(s1, maxgram);
  std::vector<std::string> s2grammed = char_gram(s2, maxgram);
  return similarity(s1grammed, s2grammed);
}
