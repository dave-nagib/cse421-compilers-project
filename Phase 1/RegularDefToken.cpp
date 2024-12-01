#include "RegularDefToken.h"
using namespace std;

RegularDefToken :: RegularDefToken() {
  this->id = -1;
  this->name = "";
  this->keywords = vector<char>();
  this->regex = "";
  this->resolved = false;
  this->usedCharsIDs = vector<int>();
}

RegularDefToken :: RegularDefToken(int id, string name, string regex, vector<char> keywords) {
  this->id = id;
  this->name = name;
  this->keywords = keywords;
  this->regex = regex;
  this->resolved = false;
  this->usedCharsIDs = vector<int>();
}

int RegularDefToken :: get_id() const {
  return id;
}

string RegularDefToken :: get_name() const {
  return name;
}

vector<char> RegularDefToken :: get_keywords() const {
  return keywords;
}

string RegularDefToken :: get_regex() const {
  return regex;
}

vector<int> RegularDefToken :: get_usedCharsIDs() const {
  return usedCharsIDs;
}

void RegularDefToken :: add_usedCharsID(int id) {
  usedCharsIDs.push_back(id);
}

bool RegularDefToken :: get_resolved() const {
  return resolved;
}

void RegularDefToken :: set_resolved(bool resolved) {
  this->resolved = resolved;
}

void RegularDefToken :: set_keywords(vector<char> keywords) {
  this->keywords = keywords;
}
