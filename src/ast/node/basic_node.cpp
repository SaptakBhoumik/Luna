#include "ast/ast.hpp"

namespace Luna{
Program::Program(std::vector<AstNodePtr> statements){
    this->statements = statements;
}

std::vector<AstNodePtr> Program::get_statements() const{
    return this->statements;
}

Token Program::token() const{
    return Token{};
}
AstKind Program::kind() const{
    return AstKind::Program;
}

std::string Program::stringify() const{
    std::string result;
    for (size_t i = 0; i < this->statements.size(); i++) {
        result += this->statements[i]->stringify();
        if (i != this->statements.size() - 1) {
            result += "\n";
        }
    }
    return result;
};


Block::Block(Token tok, std::vector<AstNodePtr> statements){
    this->tok = tok;
    this->statements = statements;
}

std::vector<AstNodePtr> Block::get_statements() const {
    return this->statements;
}

Token Block::token() const {
    return this->tok;
}

AstKind Block::kind() const {
    return AstKind::Block;
}

std::string Block::stringify() const {
    std::string result;
    for (size_t i = 0; i < this->statements.size(); i++) {
        result += "\t" + this->statements[i]->stringify();
        if (i != this->statements.size() - 1) {
            result += "\n";
        }
    }
    return result;
}

Token NoLiteral::token() const{
    return Token{};
}
AstKind NoLiteral::kind() const{
    return AstKind::NoLiteral;
}
std::string NoLiteral::stringify() const{
    return "<No literal>";
}
}