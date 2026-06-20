#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <vector>
#include <sys/stat.h>
#include <filesystem>
#include <chrono>
int main(int argc, char* argv[]) {
    if(argc < 2){
        std::cerr << "Usage: " << argv[0] << " <source_file.luna>" << std::endl;
        return 1;
    }
    std::string filename = std::filesystem::canonical(argv[1]).string();
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source_code = buffer.str();

    auto start1 = std::chrono::high_resolution_clock::now();
    auto lexer = Luna::Lexer(source_code, filename);
    auto end1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);
    auto tokens = lexer.get_tokens();
    for(const auto& token : tokens){
        std::cout << to_string(token) << std::endl;
    }

    auto start2 = std::chrono::high_resolution_clock::now();
    auto parser = Luna::Parser(tokens, filename);
    auto ast = parser.parse();
    auto end2 = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2);

    std::cout << ast->stringify() << std::endl;
    std::cout << "Time take for lexing:" << duration1 << std::endl;
    std::cout << "Time take for parsing:" << duration2 << std::endl;
    return 0;
}
