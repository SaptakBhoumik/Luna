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

    auto lexer = Luna::Lexer(source_code, filename);
    auto tokens = lexer.get_tokens();
    for(const auto& token : tokens){
        std::cout << to_string(token) << std::endl;
    }

    auto parser = Luna::Parser(tokens, filename);
    auto ast = parser.parse();
    std::cout << ast->stringify() << std::endl;
    return 0;
}