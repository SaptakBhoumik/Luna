#include "ast/ast.hpp"
#include "ast/visitor.hpp"
namespace Luna{
//Basic nodes  
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

//Literal nodes
void IntegerLiteral::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void DecimalLiteral::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void StringLiteral::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void BoolLiteral::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void NoneLiteral::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void IdentifierExpression::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
//Type expression nodes
void TypeExpr::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void ListTypeExpr::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void PtrTypeExpr::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void OptionalTypeExpr::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void ErrorTypeExpr::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void FuncTypeExpr::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void TupleTypeExpr::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void SimdTypeExpr::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void SumTypeExpr::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void EnumTypeExpr::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void StructTypeExpr::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
}