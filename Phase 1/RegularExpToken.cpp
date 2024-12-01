#include "RegularExpToken.h"
using namespace std;

RegularExpToken :: RegularExpToken() {
  this->id = -1;
  this->name = "";
  this->keywords = vector<char>();
  this->regex = "";
  this->resolved = false;
  this->usedCharsIDs = vector<int>();
}

RegularExpToken :: RegularExpToken(int id, string name, string regex, vector<char> keywords) {
  this->id = id;
  this->name = name;
  this->keywords = keywords;
  this->regex = regex;
  this->resolved = false;
  this->usedCharsIDs = vector<int>();
}

int RegularExpToken :: get_id() const {
  return id;
}

string RegularExpToken :: get_name() const {
  return name;
}

vector<char> RegularExpToken :: get_keywords() const {
  return keywords;
}

string RegularExpToken :: get_regex() const {
  return regex;
}

vector<int> RegularExpToken :: get_usedCharsIDs() const {
  return usedCharsIDs;
}

void RegularExpToken :: add_usedCharsID(int id) {
  usedCharsIDs.push_back(id);
}

bool RegularExpToken :: get_resolved() const {
  return resolved;
}

void RegularExpToken :: set_resolved(bool resolved) {
  this->resolved = resolved;
}

void RegularExpToken :: set_keywords(vector<char> keywords) {
  this->keywords = keywords;
}


