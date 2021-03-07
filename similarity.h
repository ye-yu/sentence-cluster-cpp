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
#ifndef SIMILARITY_H
#define SIMILARITY_H

#include <string>
#include <vector>
extern std::vector<std::string> char_gram(std::string input, const int maxgram = 3, const int stride = -1);
extern float similarity(std::vector<std::string> s1grammed, std::vector<std::string> s2grammed);
extern float similarity(std::string s1, std::string s2, const int maxgram);
#endif