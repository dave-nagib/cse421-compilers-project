cmake_minimum_required(VERSION 3.26)
project(cse421_compilers_project)

set(CMAKE_CXX_STANDARD 14)

include_directories("Phase 1")

add_executable(cse421_compilers_project
        "Phase 1/DFA.cpp"
        "Phase 1/DFA.h"
        "Phase 1/DFAMinimizer.cpp"
        "Phase 1/DFAMinimizer.h"
        "Phase 1/LexicalAnalyzer.cpp"
        "Phase 1/LexicalAnalyzer.h"
        "Phase 1/Main.cpp"
        "Phase 1/NFA.cpp"
        "Phase 1/NFA.h"
        "Phase 1/NFA2DFA.cpp"
        "Phase 1/NFA2DFA.h"
        "Phase 1/RegexAnalyzer.cpp"
        "Phase 1/RegexAnalyzer.h"
        "Phase 1/RegularDefToken.cpp"
        "Phase 1/RegularDefToken.h"
        "Phase 1/RegularExpToken.cpp"
        "Phase 1/RegularExpToken.h"
)

set_target_properties(cse421_compilers_project PROPERTIES OUTPUT_NAME "lexical_analyzer_generator")