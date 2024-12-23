#ifndef DFA_CPP_PARSINGDATASTRUCTS_H
#define DFA_CPP_PARSINGDATASTRUCTS_H

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

using Grammar = std::unordered_map<std::string, std::vector<std::vector<std::string>>>;
using SymbolSet = std::unordered_set<std::string>;

#endif //DFA_CPP_PARSINGDATASTRUCTS_H
