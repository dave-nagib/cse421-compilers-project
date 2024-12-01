#ifndef RegularDefToken_H
#define RegularDefToken_H
#include <string>
#include <vector>


class RegularDefToken {
  private:
    int id;
    bool resolved;
    std::string name;
    std::string regex;
    std::vector<char> keywords;
    std::vector<int> usedCharsIDs;

  public:
    /** Default constructor */
    RegularDefToken();
    /** DI constructor */
    RegularDefToken(int id, std::string name, std::string regex, std::vector<char> keywords);
    /** Returns the id of the token. */
    int get_id() const;
    /** Returns the name of the token. */
    std::string get_name() const;
    /** Returns the keywords of the token. */
    std::vector<char> get_keywords() const;
    /** Returns the regex of the token. */
    std::string get_regex() const;
    /** Returns the usedCharsIDs of the token. */
    std::vector<int> get_usedCharsIDs() const;
    /** Add a usedCharsID to the token. */
    void add_usedCharsID(int id);
    /** Returns the resolved of the token. */
    bool get_resolved() const;
    /** Set the resolved of the token. */
    void set_resolved(bool resolved);
    /** Set the keywords of the token. */
    void set_keywords(std::vector<char> keywords);
};

#endif