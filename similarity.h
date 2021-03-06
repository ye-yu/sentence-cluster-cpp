#ifndef SIMILARITY_H
#define SIMILARITY_H

#include <string>
#include <vector>
extern std::vector<std::string> char_gram(std::string input, const int maxgram);
extern float similarity(std::vector<std::string> s1grammed, std::vector<std::string> s2grammed);
extern float similarity(std::string s1, std::string s2, const int maxgram);
#endif