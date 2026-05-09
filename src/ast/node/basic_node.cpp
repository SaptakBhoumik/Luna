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
    for (const auto& stmt : this->statements) {
        result += stmt->stringify() + "\n";
    }
    return result;
};



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