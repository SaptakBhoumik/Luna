#include "ast/ast.hpp"
#include "ast/visitor.hpp"
namespace Luna{
// TODO: have a default behaviour in case the visitor does not provide a visit
// method for the node
void Program::accept(AstVisitor& visitor) const {
    if (!visitor.visit(*this)){
        for (auto& stmt : this->statements){
            stmt->accept(visitor);
        }
    }
}

void NoLiteral::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}

}