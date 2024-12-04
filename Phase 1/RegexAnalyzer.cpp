#include "RegexAnalyzer.h"
using namespace std;

RegexAnalyzer::RegexAnalyzer() {}

RegexAnalyzer::RegexAnalyzer(const string& filePath)
{
  this->filePath = filePath;
  currentRegularExpId = 0;
  currentkeyWordId = -10;
  currentRegularDefId = 0;
  currentCharId = -128;
  regularExpTokens = vector<RegularExpToken>();
  regularDefTokens = vector<RegularDefToken>();
  charTokens = unordered_map<char,char>();
  keywords = vector<string>();
  punctuations = vector<string>();
  reservedSymbols = vector<string>();
}
// private functions
bool RegexAnalyzer::isEnglishChar(char c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

string RegexAnalyzer::cleanRHS(const std::string &input)
{
  std::string RHS = input;

  // Step 1: Replace consecutive spaces with a single space
  for (size_t i = 0; i < RHS.size(); i++)
  {
    if (RHS[i] == ' ' && i + 1 < RHS.size() && RHS[i + 1] == ' ')
    {
      RHS.erase(i, 1);
      i--; // Recheck the current position
    }
  }

  // Step 2: Remove leading and trailing spaces
  RHS.erase(0, RHS.find_first_not_of(" ")); // Remove leading spaces
  RHS.erase(RHS.find_last_not_of(" ") + 1); // Remove trailing spaces

  // Step 3: Remove spaces before or after non-English characters
  for (size_t i = 0; i < RHS.size(); i++)
  {
    if (RHS[i] == ' ')
    {
      if (i > 0 && !isEnglishChar(RHS[i - 1]))
      {
        RHS.erase(i, 1);
        i--; // Recheck the current position
      }
      else if (i + 1 < RHS.size() && !isEnglishChar(RHS[i + 1]))
      {
        RHS.erase(i, 1);
        i--; // Recheck the current position
      }
    }
  }

  return RHS;
}

RegularDefToken RegexAnalyzer::get_defToken_by_name(string name)
{
  for (RegularDefToken token : regularDefTokens)
  {
    if (token.get_name() == name)
    {
      return token;
    }
  }
  return RegularDefToken();
}

RegularDefToken &RegexAnalyzer::get_defTokenRef_by_name(const string &name)
{
  for (RegularDefToken &token : regularDefTokens)
  { // Iterate with references
    if (token.get_name() == name)
    {
      return token; // Return a reference to the found token
    }
  }
  static RegularDefToken emptyToken;
  return emptyToken;
}

void RegexAnalyzer::resolveRegularDefToken()
{
  // Queue of raw pointers to RegularDefToken
  queue<RegularDefToken *> regularDefTokenQueue;

  // Push all RegularDefTokens into the queue
  for (RegularDefToken &token : regularDefTokens)
  {
    regularDefTokenQueue.push(&token); // Store the address of each token
  }

  // Process the queue
  while (!regularDefTokenQueue.empty())
  {
    // Get the front token as a raw pointer
    RegularDefToken *token = regularDefTokenQueue.front();
    // Remove the processed token from the queue
    regularDefTokenQueue.pop();
    // Initialize token's keywords and supporting variables
    vector<char> tokenKeywords = token->get_keywords();
    vector<string> inTokenKeywords;
    string regex = token->get_regex();
    bool isTokenResolved = true;

    // Process each character in the regex string
    for (int i = 0; i < regex.size(); i++)
    {
      if (!isEnglishChar(regex[i]))
      {
        // Handle special regex characters and escape sequences
        if (regex[i] == '\\')
        {
          if (regex[i + 1] == 'L')
          {
            tokenKeywords.push_back('L');
          }
          else
          {
            char charToken = regex[i + 1];
            if (charTokens.find(charToken) != charTokens.end())
            {
              token->add_usedCharsID(charTokens[charToken]);
              tokenKeywords.push_back(charTokens[charToken]);
              reservedSymbols.push_back(string(1, charToken));
            }
            else
            {
              charTokens[charToken] = currentCharId++;
              token->add_usedCharsID(charTokens[charToken]);
              tokenKeywords.push_back(charTokens[charToken]);
              reservedSymbols.push_back(string(1, charToken));
            }
          }
          i++; // Skip the next character since it's part of an escape sequence
        }
        else if (regex[i] == '*' || regex[i] == '+' || regex[i] == '?' || regex[i] == '|' ||
                 regex[i] == '(' || regex[i] == ')' || regex[i] == '$')
        {
          // Add operators and special regex characters directly
          tokenKeywords.push_back(regex[i]);
        }
        else if (regex[i] != ' ')
        {
          // Handle character ranges (e.g., a-z)
          if (i + 1 < regex.size() && regex[i + 1] == '-')
          {
            for (char c = regex[i]; c <= regex[i + 2]; c++)
            {
              char charToken = c;
              if (charTokens.find(charToken) != charTokens.end())
              {
                token->add_usedCharsID(charTokens[charToken]);
                tokenKeywords.push_back(charTokens[charToken]);
              }
              else
              {
                charTokens[charToken] = currentCharId++;
                token->add_usedCharsID(charTokens[charToken]);
                tokenKeywords.push_back(charTokens[charToken]);
              }
              if (c != regex[i + 2])
              {
                tokenKeywords.push_back('|');
              }
            }
            i += 2; // Skip the range
          }
          else
          {
            char charToken = regex[i];
            if (charTokens.find(charToken) != charTokens.end())
            {
              token->add_usedCharsID(charTokens[charToken]);
              tokenKeywords.push_back(charTokens[charToken]);
            }
            else
            {
              charTokens[charToken] = currentCharId++;
              token->add_usedCharsID(charTokens[charToken]);
              tokenKeywords.push_back(charTokens[charToken]);
            }
          }
        }
      }
      else if (i + 1 < regex.size() && isEnglishChar(regex[i + 1]))
      {
        // Handle multi-character tokens
        inTokenKeywords.push_back(string(1, regex[i]));
      }
      else if (!inTokenKeywords.empty())
      {
        // Resolve multi-character tokens
        string charToken = string(1, regex[i]);
        inTokenKeywords.push_back(charToken);
        string inTokenName = accumulate(inTokenKeywords.begin(), inTokenKeywords.end(), string(""));
        inTokenKeywords.clear();
        RegularDefToken *inToken = &get_defTokenRef_by_name(inTokenName);
        if (inToken == nullptr) {
          throw std::runtime_error("inToken is nullptr.");
        }
        if (!inToken || inToken->get_id() == -1)
        {
          assert(("Regular Definition Token not found", false));
        }

        if (!inToken->get_resolved())
        {
          // Push back unresolved tokens for reprocessing
          regularDefTokenQueue.push(token);
          isTokenResolved = false;
          token->set_keywords(tokenKeywords);
          break;
        }
        // Add resolved token's keywords
        const auto& inTokenKeywords = inToken->get_keywords();
        tokenKeywords.push_back('(');
        tokenKeywords.insert(tokenKeywords.end(), inTokenKeywords.begin(), inTokenKeywords.end());
        tokenKeywords.push_back(')');
      }
      else if (i + 1 < regex.size() && regex[i + 1] != '-')
      {
        // Add single characters
        char charToken = regex[i];
        if (charTokens.find(charToken) != charTokens.end())
        {
          token->add_usedCharsID(charTokens[charToken]);
          tokenKeywords.push_back(charTokens[charToken]);
        }
        else
        {
          charTokens[charToken] = currentCharId++;
          token->add_usedCharsID(charTokens[charToken]);
          tokenKeywords.push_back(charTokens[charToken]);
        }
      }
      else if (i + 1 < regex.size() && regex[i + 1] == '-')
      {
        // Handle character ranges (e.g., a-z)
        for (char c = regex[i]; c <= regex[i + 2]; c++)
        {
          char charToken = c;
          if (charTokens.find(charToken) != charTokens.end())
          {
            token->add_usedCharsID(charTokens[charToken]);
            tokenKeywords.push_back(charTokens[charToken]);
          }
          else
          {
            charTokens[charToken] = currentCharId++;
            token->add_usedCharsID(charTokens[charToken]);
            tokenKeywords.push_back(charTokens[charToken]);
          }
          if (c != regex[i + 2])
          {
            tokenKeywords.push_back('|');
          }
        }
        i += 2; // Skip the range
      }
      else
      {
        // add single characters
        char charToken = regex[i];
        if (charTokens.find(charToken) != charTokens.end())
        {
          token->add_usedCharsID(charTokens[charToken]);
          tokenKeywords.push_back(charTokens[charToken]);
        }
        else
        {
          charTokens[charToken] = currentCharId++;
          token->add_usedCharsID(charTokens[charToken]);
          tokenKeywords.push_back(charTokens[charToken]);
        }
      }
    }
    if (!isTokenResolved)
    {
      continue; // Skip unresolved tokens for now
    }

    // Mark the token as resolved and update its keywords
    token->set_resolved(true);
    token->set_keywords(tokenKeywords);
  }
}

// implement resolveRegularExpToken with the same logic as resolveRegularDefToken
void RegexAnalyzer::resolveRegularExpToken()
{
  // Queue of raw pointers to RegularExpToken
  queue<RegularExpToken *> regularExpTokenQueue;
  // Push all RegularExpTokens into the queue
  for (RegularExpToken &token : regularExpTokens)
  {
    regularExpTokenQueue.push(&token); // Store the address of each token
  }

  // Process the queue
  while (!regularExpTokenQueue.empty())
  {
    // Get the front token as a raw pointer
    RegularExpToken *token = regularExpTokenQueue.front();
    // Remove the processed token from the queue
    regularExpTokenQueue.pop();
    // Initialize token's keywords and supporting variables
    vector<char> tokenKeywords = token->get_keywords();
    vector<string> inTokenKeywords;
    string regex = token->get_regex();
    bool isTokenResolved = true;
    //if the name of the token is a keyword or a punctuation, add ignore it
    unordered_set<string> keywordsSet(keywords.begin(), keywords.end());
    unordered_set<string> punctuationsSet(punctuations.begin(), punctuations.end());
    if (keywordsSet.find(token->get_name()) != keywordsSet.end() ||
        punctuationsSet.find(token->get_name()) != punctuationsSet.end()) {
        continue;
    }
    // Process each character in the regex string
    for (int i = 0; i < regex.size(); i++)
    {
      if (!isEnglishChar(regex[i]))
      {
        // Handle special regex characters and escape sequences
        if (regex[i] == '\\')
        {
          if (regex[i + 1] == 'L')
          {
            tokenKeywords.push_back('L');
          }
          else
          {
            char charToken = regex[i + 1];
            if (charTokens.find(charToken) != charTokens.end())
            {
              token->add_usedCharsID(charTokens[charToken]);
              tokenKeywords.push_back(charTokens[charToken]);
              reservedSymbols.push_back(string(1, charToken));
            }
            else
            {
              charTokens[charToken] = currentCharId++;
              token->add_usedCharsID(charTokens[charToken]);
              tokenKeywords.push_back(charTokens[charToken]);
              reservedSymbols.push_back(string(1, charToken));
            }
          }
          i++; // Skip the next character since it's part of an escape sequence
        }
        else if (regex[i] == '*' || regex[i] == '+' || regex[i] == '?' || regex[i] == '|' ||
                 regex[i] == '(' || regex[i] == ')' || regex[i] == '$')
        {
          // Add operators and special regex characters directly
          tokenKeywords.push_back(regex[i]);
        }
        else if (regex[i] != ' ')
        {
          // Handle character ranges (e.g., a-z)
          if (i + 1 < regex.size() && regex[i + 1] == '-')
          {
            for (char c = regex[i]; c <= regex[i + 2]; c++)
            {
              char charToken = c;
              tokenKeywords.push_back('(');
              if (charTokens.find(charToken) != charTokens.end())
              {
                token->add_usedCharsID(charTokens[charToken]);
                tokenKeywords.push_back(charTokens[charToken]);
              }
              else
              {
                charTokens[charToken] = currentCharId++;
                token->add_usedCharsID(charTokens[charToken]);
                tokenKeywords.push_back(charTokens[charToken]);
              }
              if (c != regex[i + 2])
              {
                tokenKeywords.push_back('|');
              }
            }
            tokenKeywords.push_back(')');
            i += 2; // Skip the range
          }
          else
          {
            char charToken = regex[i];
            if (charTokens.find(charToken) != charTokens.end())
            {
              token->add_usedCharsID(charTokens[charToken]);
              tokenKeywords.push_back(charTokens[charToken]);
            }
            else
            {
              charTokens[charToken] = currentCharId++;
              token->add_usedCharsID(charTokens[charToken]);
              tokenKeywords.push_back(charTokens[charToken]);
            }
          }
        }
      }
      else if (i + 1 < regex.size() && isEnglishChar(regex[i + 1]))
      {
        // Handle multi-character tokens
        inTokenKeywords.push_back(string(1, regex[i]));
      }
      else if (!inTokenKeywords.empty())
      {
        // Resolve multi-character tokens
        string charToken = string(1, regex[i]);
        inTokenKeywords.push_back(charToken);
        string inTokenName = accumulate(inTokenKeywords.begin(), inTokenKeywords.end(), string(""));
        inTokenKeywords.clear();
        RegularDefToken *inToken = &get_defTokenRef_by_name(inTokenName);
        if (inToken == nullptr) {
          throw std::runtime_error("inToken is nullptr.");
        }
        if (!inToken || inToken->get_id() == -1)
        {
          assert(("Regular Definition Token not found", false));
        }

        if (!inToken->get_resolved())
        {
          // Push back unresolved tokens for reprocessing
          regularExpTokenQueue.push(token);
          isTokenResolved = false;
          token->set_keywords(tokenKeywords);
          break;
        }
        // Add resolved token's keywords
        const auto& inTokenKeywords = inToken->get_keywords();
        tokenKeywords.push_back('(');
        tokenKeywords.insert(tokenKeywords.end(), inTokenKeywords.begin(), inTokenKeywords.end());
        tokenKeywords.push_back(')');
      }
      else if (i + 1 < regex.size() && regex[i + 1] != '-')
      {
        // Add single characters
        char charToken = regex[i];
        if (charTokens.find(charToken) != charTokens.end())
        {
          token->add_usedCharsID(charTokens[charToken]);
          tokenKeywords.push_back(charTokens[charToken]);
        }
        else
        {
          charTokens[charToken] = currentCharId++;
          token->add_usedCharsID(charTokens[charToken]);
          tokenKeywords.push_back(charTokens[charToken]);
        }
      }
      else if (i + 1 < regex.size() && regex[i + 1] == '-')
      {
        // Handle character ranges (e.g., a-z)
        tokenKeywords.push_back('(');
        for (char c = regex[i]; c <= regex[i + 2]; c++)
        {
          char charToken = c;
          if (charTokens.find(charToken) != charTokens.end())
          {
            token->add_usedCharsID(charTokens[charToken]);
            tokenKeywords.push_back(charTokens[charToken]);
          }
          else
          {
            charTokens[charToken] = currentCharId++;
            token->add_usedCharsID(charTokens[charToken]);
            tokenKeywords.push_back(charTokens[charToken]);
          }
          if (c != regex[i + 2])
          {
            tokenKeywords.push_back('|');
          }
        }
        tokenKeywords.push_back(')');
        i += 2; // Skip the range
      }
      else
      {
        // add single characters
        char charToken = regex[i];
        if (charTokens.find(charToken) != charTokens.end())
        {
          token->add_usedCharsID(charTokens[charToken]);
          tokenKeywords.push_back(charTokens[charToken]);
        }
        else
        {
          charTokens[charToken] = currentCharId++;
          token->add_usedCharsID(charTokens[charToken]);
          tokenKeywords.push_back(charTokens[charToken]);
        }
      }
    }
    if (!isTokenResolved)
    {
      continue; // Skip unresolved tokens for now
    }

    // Mark the token as resolved and update its keywords
    token->set_resolved(true);
    token->set_keywords(tokenKeywords);
  }
}

// public functions
vector<RegularExpToken> RegexAnalyzer::get_tokens() const
{
  return regularExpTokens;
}

RegularExpToken RegexAnalyzer::get_token(string name) const
{
  for (RegularExpToken token : regularExpTokens)
  {
    if (token.get_name() == name)
    {
      return token;
    }
  }
  return RegularExpToken();
}

RegularExpToken RegexAnalyzer::get_token(int id) const
{
  for (RegularExpToken token : regularExpTokens)
  {
    if (token.get_id() == id)
    {
      return token;
    }
  }
  return RegularExpToken();
}

void RegexAnalyzer::parseLexicalRules()
{
  ifstream file(this->filePath);
  if (!file.is_open())
  {
    cout << "Error opening rules file" << endl;
    return;
  }
  string line;
  while (getline(file, line))
  {
    if (line[0] == '{')
    {
      string originalKeywords = line.substr(1, line.size() - 2); // Extract and preserve the original string
      int wordSize = 0;
      vector<char> keywordChars;
      for (int i = 0; i < originalKeywords.size(); i++)
      {
        if (originalKeywords[i] == ' ' && wordSize > 0)
        { // Check wordSize > 0 to ensure processing
          // Create and store a RegularExpToken
          string keywordstring = string(keywordChars.begin(), keywordChars.end());
          for (char & keywordChar : keywordChars)
          {
            keywordChar = charTokens[keywordChar];
          }
          RegularExpToken regToken = RegularExpToken(currentkeyWordId--, keywordstring, keywordstring, keywordChars);
          keywords.push_back(keywordstring);
          regularExpTokens.push_back(regToken);
          keywordChars.clear();
          wordSize = 0;
          continue;
        }
        else if (originalKeywords[i] == ' ')
        {
          continue;
        }
        keywordChars.push_back(originalKeywords[i]); // Store characters
        //check if the char exists in the charTokens map before adding it to the keywordChars
        if (charTokens.find(originalKeywords[i]) == charTokens.end())
        {
          charTokens[originalKeywords[i]] = currentCharId++;
        }
        wordSize++;
        if (i == originalKeywords.size() - 1)
        { // Process the last keyword
          string keywordstring = string(keywordChars.begin(), keywordChars.end());
          // replace evrey char in the keyword with its id
          for (char & keywordChar : keywordChars)
          {
            keywordChar = charTokens[keywordChar];
          }
          RegularExpToken regToken = RegularExpToken(currentkeyWordId--, keywordstring, keywordstring, keywordChars);
          keywords.push_back(keywordstring);
          regularExpTokens.push_back(regToken);
          keywordChars.clear();
        }
      }
    }
    else if (line[0] == '[')
    {
      for (int i = 1; i < line.size() - 1; i++)
      {
        if (line[i] == ' ' || line[i] == '\\' || line[i] == '\t')
        {
          continue;
        }
        string charToken = string(1, line[i]);
        charTokens[charToken[0]] = currentCharId++;
        punctuations.push_back(charToken);
        // add to regula expression
        RegularExpToken regToken = RegularExpToken(currentkeyWordId--, charToken, charToken, vector<char>({charTokens[charToken[0]]}));
        regularExpTokens.push_back(regToken);
        if (i > 1 && line[i - 1] == '\\')
        {
          reservedSymbols.push_back(charToken);
        }
      }
    }
    else if (isEnglishChar(line[0]))
    {
      /* Regular definitions are lines in the form LHS = RHS
        Regular expressions are lines in the form LHS: RHS
      */
      vector<char> prefix;
      bool isRegularExp = false;
      int i = 0;
      for (i = 0; i < line.size(); i++)
      {
        if (isEnglishChar(line[i]))
        {
          prefix.push_back(line[i]);
        }
        else if (line[i] == ' ')
        {
          continue;
        }
        else if (line[i] == '=')
        {
          isRegularExp = false;
          break;
        }
        else if (line[i] == ':')
        {
          isRegularExp = true;
          break;
        }
        else
        {
          assert(("Invalid line format detected", false));
        }
      }
      string LHS = string(prefix.begin(), prefix.end());
      string RHS = line.substr(i + 1);
      // replace any consecutive spaces with a single space for the RHS
      // remove any leading or trailing spaces from the RHS
      // remove any spaces that are before or after non English characters in the RHS
      RHS = cleanRHS(RHS);
      if (isRegularExp)
      {
        RegularExpToken regToken = RegularExpToken(currentRegularExpId++, LHS, RHS, vector<char>());
        regularExpTokens.push_back(regToken);
      }
      else
      {
        RegularDefToken regToken = RegularDefToken(currentRegularDefId++, LHS, RHS, vector<char>());
        regularDefTokens.push_back(regToken);
      }
    }
    else
    {
      assert(("Invalid line format detected", false));
    }
  }
  file.close();
}

void RegexAnalyzer::printRegularExpTokens()
{
  cout << "Regular Expression Tokens:" << endl;
  // print the regular expression tokens in the form of id, name, regex and keywords
  for (RegularExpToken token : regularExpTokens)
  {
    cout << token.get_id() << " " << token.get_name() << " " << token.get_regex() << endl;
    vector<char> keywords = token.get_keywords();
    cout << "Keywords for " << token.get_name() << ": ";
    for (char keyword : keywords)
    {
      cout << keyword << " ";
    }
    cout << endl;
  }
}

void RegexAnalyzer::printRegularDefTokens()
{
  cout << "Regular Definition Tokens:" << endl;
  // print the regular expression tokens in the form of id, name, regex and keywords
  for (RegularDefToken token : regularDefTokens)
  {
    cout << token.get_id() << " " << token.get_name() << " " << token.get_regex() << endl;
    vector<char> keywords = token.get_keywords();
    cout << "Keywords for " << token.get_name() << ": ";
    for (char keyword : keywords)
    {
      cout << keyword << " ";
    }
    cout << endl;
  }
}

void RegexAnalyzer::printCharTokens()
{
  cout << "Char Tokens:" << endl;
  for (auto const &pair : charTokens)
  {
    cout << pair.first << "->" << pair.second << endl;
  }
}

void RegexAnalyzer::printKeywords()
{
  cout << "Keywords:" << endl;
  for (string keyword : keywords)
  {
    cout << keyword << endl;
  }
}

void RegexAnalyzer::printPunctuations()
{
  cout << "Punctuations:" << endl;
  for (string punctuation : punctuations)
  {
    cout << punctuation << endl;
  }
}

void RegexAnalyzer::printReservedSymbols()
{
  cout << "Reserved Symbols:" << endl;
  for (string reservedSymbol : reservedSymbols)
  {
    cout << reservedSymbol << endl;
  }
}

NFA RegexAnalyzer::RegexToNFA()
{
    parseLexicalRules();
  resolveRegularDefToken();
  resolveRegularExpToken();
  printAll();
  NFA nfa = generateNFA();
  //nfa.print_nfa();
  return nfa;
}

NFA RegexAnalyzer::generateNFA()
{
  vector<NFA> nfas;
  stateCounter = 1;
  for (RegularExpToken token : regularExpTokens)
  {
    nfas.push_back(RegularExpTokenToNFA(token));
  }
  NFA nfa = NFA::union_nfa(nfas, 0);
  
  return nfa;
}

NFA RegexAnalyzer::RegularExpTokenToNFA(RegularExpToken token)
{
  vector<char> keywords = token.get_keywords();
  vector<NFA> baseNFAs;
  for (size_t i = 0; i < keywords.size(); i++)
  {
    if (keywords[i] < 0)
    {
      int intialState = stateCounter++;
      int acceptingState = stateCounter++;
      NFA defNFA = NFA();
      defNFA.add_state(intialState);
      defNFA.add_state(acceptingState);
      defNFA.make_initial(intialState);
      defNFA.make_accepting(acceptingState, 0);
      defNFA.add_transition(intialState, keywords[i], acceptingState);
      baseNFAs.push_back(defNFA);
      keywords[i] = 'n';
    }
    else if (keywords[i] == 'L')
    {
      int intialState = stateCounter++;
      int acceptingState = stateCounter++;
      NFA defNFA = NFA();
      defNFA.add_state(intialState);
      defNFA.add_state(acceptingState);
      defNFA.make_initial(intialState);
      defNFA.make_accepting(acceptingState, 0);
      defNFA.add_transition(intialState, '\0', acceptingState);
      baseNFAs.push_back(defNFA);
      keywords[i] = 'n';
    }
  }
  // add a '(' at the start of the keywords and a ')' at the end of the keywords
  keywords.insert(keywords.begin(), '(');
  keywords.push_back(')');
  int baseNFAIndex = 0, bracketStart = 0, bracketEnd = 0;
  while (keywords.size() > 1) // O(number of keywords * number of brackets)
  {
    baseNFAIndex = 0;
    bracketStart = 0;
    bracketEnd = 0;
    for (size_t i = 0; i < keywords.size(); i++)
    {
      if (keywords[i] == '(')
      {
        bracketStart = i;
      }
      else if (keywords[i] == ')')
      {
        bracketEnd = i;
        for (size_t j = 0; j < bracketStart; j++)
        {
          if (keywords[j] == 'n')
          {
            baseNFAIndex++;
          }
        }
        // 1. handle the case of a character followed by one of the operators *, +, ?
        //print the size of the keywords between the brackets
        int nfaIter = baseNFAIndex;
        for (size_t j = bracketStart+1; j < bracketEnd; j++)
        {
          if (keywords[j] == 'n' && j+1<bracketEnd && (keywords[j+1] == '*' | keywords[j+1] == '+'))
          {
            if (keywords[j+1] == '*')
              {
                baseNFAs[nfaIter] = baseNFAs[nfaIter].simple_repeat(true, stateCounter++, 0);
                //remove element with index j+1 from keywords
                keywords.erase(keywords.begin() + j+1);
                --j;
                bracketEnd--;
              }
              else if (keywords[j+1] == '+')
              {
                baseNFAs[nfaIter] = baseNFAs[nfaIter].simple_repeat(false, stateCounter++, 0);
                //remove element with index j+1 from keywords
                keywords.erase(keywords.begin() + j+1);
                --j;
                bracketEnd--;
              }
          }
          else if(keywords[j]=='n'){
            nfaIter++;
          }
        }
        // 2. handle the case of a character followed by a character (concatenation)
        nfaIter = baseNFAIndex;
        for(size_t j = bracketStart+1; j < bracketEnd; j++)
        {
          if (keywords[j] == 'n' && j+1<bracketEnd && keywords[j+1] == 'n')
          {
            baseNFAs[nfaIter] = baseNFAs[nfaIter].simple_concat(baseNFAs[nfaIter+1], 0);
            //remove element with index j+1 from keywords
            keywords.erase(keywords.begin() + j+1);
            //remove the baseNFA with index nfaIter+1
            baseNFAs.erase(baseNFAs.begin() + nfaIter+1);
            --j;
            bracketEnd--;
          }
          else if(keywords[j]=='n'){
            nfaIter++;
          }
        }
        // 3. handle the case of a character followed by '|' and then a character (union)
        vector <NFA> tempNFAs;
        for(size_t j = bracketStart+1; j < bracketEnd; j++)
        {
          if (keywords[j] == 'n' && j+1<bracketEnd && keywords[j+1] == '|')
          {
            tempNFAs.push_back(baseNFAs[baseNFAIndex+1]);
            //remove elements with index j+1 and j+2 from keywords
            keywords.erase(keywords.begin() + j+1);
            keywords.erase(keywords.begin() + j+1);
            //remove the baseNFA with index baseNFAIndex+1
            baseNFAs.erase(baseNFAs.begin() + baseNFAIndex+1);
            --j;
            bracketEnd -= 2;
          }
        }
        //push baseNFAIndex to the beggining of the vector
        tempNFAs.insert(tempNFAs.begin(), baseNFAs[baseNFAIndex]);
        baseNFAs[baseNFAIndex] = NFA::union_nfa(tempNFAs, stateCounter++, true, stateCounter++, 0);
        if (bracketEnd - bracketStart == 2)
        {
          keywords.erase(keywords.begin() + bracketStart);
          keywords.erase(keywords.begin() + bracketEnd-1);
        }
        else
        {
          cout << "bracketStart: " << bracketStart << " bracketEnd: " << bracketEnd << endl;
          cout << "keywords in the bracket: ";
          for (size_t j = bracketStart; j <= bracketEnd; j++)
          {
            cout << keywords[j] << " ";
          }
          cout << endl;
          //invalid regular expression
          assert(("Invalid regular expression", false));
        }
        break;
      }
    }
  }
  NFA FinalNFA = baseNFAs[0];
  FinalNFA.make_accepting(FinalNFA.get_single_accepting(), token.get_id());
  return FinalNFA;
}

unordered_map<int, std::string> RegexAnalyzer::getTokensIdNameMap()
{
  unordered_map<int, string> tokensIdNameMap;
  for (RegularExpToken token : regularExpTokens)
  {
    tokensIdNameMap[token.get_id()] = token.get_name();
  }
  return tokensIdNameMap;
}

unordered_map<char, char> RegexAnalyzer::getCharTokensMap()
{
  return charTokens;
}

void RegexAnalyzer::printAll()
{
  printRegularExpTokens();
  cout << "--------------------------------" << endl;
  printRegularDefTokens();
  cout << "--------------------------------" << endl;
  printCharTokens();
  cout << "--------------------------------" << endl;
  printKeywords();
  cout << "--------------------------------" << endl;
  printPunctuations();
  cout << "--------------------------------" << endl;
  printReservedSymbols();
}
