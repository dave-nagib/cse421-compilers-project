#include "FirstFollowGenerator.h"
using namespace std;

// Compute FIRST set for a single symbol
SymbolSet FirstFollowGenerator::computeFirst(const string& symbol) {
    // If we've already computed FIRST for this symbol, return it
    if (firstSets.find(symbol) != firstSets.end()) {
        return firstSets[symbol];
    }

    // If symbol is a terminal or epsilon, FIRST is just the symbol itself
    if (terminals.find(symbol) != terminals.end() || symbol == EPSILON) {
        return (firstSets[symbol] = {symbol});
    }

    // For non-terminals, add the First set of all productions
    SymbolSet first;
    for (const auto& production : grammar.at(symbol)) {
        const auto& prodFirst = computeFirstOfSequence(production);
        first.insert(prodFirst.begin(), prodFirst.end());
    }

    firstSets[symbol] = first;
    return first;
}

// Compute FIRST set for a sequence of symbols
SymbolSet FirstFollowGenerator::computeFirstOfSequence(const vector<string>& sequence) {
    SymbolSet result;
    bool allCanBeEmpty = true;

    for (const auto& symbol : sequence) {
        SymbolSet symFirst = computeFirst(symbol);

        // Add all non-epsilon symbols from FIRST(symbol)
        for (const auto& f : symFirst) {
            if (f != EPSILON) result.insert(f);
        }

        // If this symbol can't produce epsilon, stop here
        if (symFirst.find(EPSILON) == symFirst.end()) {
            allCanBeEmpty = false;
            break;
        }
    }

    //
    if (allCanBeEmpty && !sequence.empty()) {
        result.insert(EPSILON);
    }

    return result;
}

// Compute FOLLOW sets for all non-terminals
void FirstFollowGenerator::computeFollow() {
    // Initialize FOLLOW sets
    for (const auto& entry : grammar) {
        followSets[entry.first] = SymbolSet();
    }

    // Add $ to FOLLOW(StartSymbol)
    followSets[startSymbol].insert("$");

    // Repeat until no changes are made
    bool changed;
    do {
        changed = false;
        // For each production rule
        for (const auto& symbolAndProductions : grammar) {
            for (const auto& production : symbolAndProductions.second) {
                // For each symbol in the production, update its FOLLOW set
                changed = updateFollow(symbolAndProductions.first, production);
            }
        }
    } while (changed);
}

bool FirstFollowGenerator::updateFollow(const string &lhs, const vector<string> &rhs) {
    bool updated;
    for (long i = 0; i < rhs.size(); i++) {
        // Only compute FOLLOW for non-terminals
        if (terminals.find(rhs[i]) != terminals.end()) {
            continue;
        }

        SymbolSet toAdd;

        // If this is the last symbol
        if (i == rhs.size()-1) {
            // Add FOLLOW(LHS) to FOLLOW(last symbol)
            toAdd = followSets[lhs];
        } else {
            // Compute FIRST(remaining sequence)
            vector<string> remaining(rhs.begin()+i+1, rhs.end());
            SymbolSet firstOfRemaining = computeFirstOfSequence(remaining);

            // Add all non-epsilon symbols from FIRST of remaining sequence
            for (const auto& f : firstOfRemaining) {
                if (f != EPSILON) {
                    toAdd.insert(f);
                }
            }

            // If remaining sequence can derive epsilon, add FOLLOW(LHS) to FOLLOW(current symbol)
            if (firstOfRemaining.find(EPSILON) != firstOfRemaining.end()) {
                const auto& lhsFollow = followSets[lhs];
                toAdd.insert(lhsFollow.begin(), lhsFollow.end());
            }
        }

        // Add new symbols to FOLLOW set
        size_t originalSize = followSets[rhs[i]].size();
        followSets[rhs[i]].insert(toAdd.begin(), toAdd.end());
        // If the size has changed, keep iterating
        if (followSets[rhs[i]].size() > originalSize) {
            updated = true;
        }
    }
    return updated;
}

// Getters for the computed sets
const unordered_map<string, SymbolSet>& FirstFollowGenerator::getFirstSets() {
    // Compute FIRST sets for all symbols if not already computed
    for (const auto& entry : grammar) {
        computeFirst(entry.first);
    }
    return firstSets;
}

const unordered_map<string, SymbolSet>& FirstFollowGenerator::getFollowSets() {
    if (followSets.empty()) {
        computeFollow();
    }
    return followSets;
}