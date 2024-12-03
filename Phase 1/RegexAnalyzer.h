#ifndef RegexAnalyzer_H
#define RegexAnalyzer_H
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <cassert>
#include <numeric>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include "RegularExpToken.h"
#include "RegularDefToken.h"
#include "NFA.h"

class RegexAnalyzer {
  private:
    /** The file path of the lexical rules. */
    std::string filePath;
    /** The Current Regular Expression ID */
    int currentRegularExpId;
    /** The Current Keyword ID */
    int currentkeyWordId;
    /** The Current Regular Definition ID */
    int currentRegularDefId;
    /** The Current char ID */
    char currentCharId;
    /** The Current state ID */
    int stateCounter;
    /** The Regular Expressions of the regex. */
    std::vector<RegularExpToken> regularExpTokens;
    /** The Regular Definitions of the regex. */
    std::vector<RegularDefToken> regularDefTokens;
    /** The characters of the regex. */
    std::unordered_map<char, char> charTokens;
    /** The Keywords of the regex. */
    std::vector<std::string> keywords;
    /** The Punctuations of the regex. */
    std::vector<std::string> punctuations;
    /** The Reserved Symbols of the regex. */
    std::vector<std::string> reservedSymbols;
    /** check if the char is an english char */
    bool isEnglishChar(char c);
    /** Parses the lexical rules from a file. */
    void parsLexicalRules();
    /** resolve the regular definition token 
     * by replacing tokens and expressions from the regex
     * with their corresponding char id
     */
    void resolveRegularDefToken();
    /** resolve the regular expression token 
     * by replacing tokens and expressions from the regex
     * with their corresponding char id
     */
    void resolveRegularExpToken();
    /** generate the NFA from the nfas representing the regular expressions tokens */
    NFA generateNFA();
    /** convert a single regular expression token to NFA */
    NFA RegularExpTokenToNFA(RegularExpToken token);
    /** get the regular definition token by its name */
    RegularDefToken get_defToken_by_name(std::string name);
    /**get the regular definition ref name */
    RegularDefToken& get_defTokenRef_by_name(const std::string& name);
    /** clean the right hand side of the regular definition 
     * replacing consecutive spaces with a single space
     * removing extra spaces and spaces before and after non-english characters
    */
    std::string cleanRHS(const std::string& input);



  public:
    /** Default constructor */
    RegexAnalyzer();

    /** DI constructor */
    RegexAnalyzer(const std::string& filePath);
    /** Returns the tokens of the regex. */
    std::vector<RegularExpToken> get_tokens() const;
    /** Returns a token by its name. */
    RegularExpToken get_token(std::string name) const;
    /** Returns a token by its id. */
    RegularExpToken get_token(int id) const;
    /** Converts the regular expressions to NFA. */
    NFA RegexToNFA();
    /** Returns the tokens map id -> name */
    std::unordered_map<int, std::string> getTokensIdNameMap();
    /** Returns the char tokens map char -> id */
    std::unordered_map<char, char> getCharTokensMap();
    void printRegularExpTokens();
    void printRegularDefTokens();
    void printCharTokens();
    void printKeywords();
    void printPunctuations();
    void printReservedSymbols();
    void printAll();
    
};

#endif