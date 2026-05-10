#include "ast/ast.hpp"
#include "ast/visitor.hpp"
namespace Luna{
//Basic nodes  
void Program::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
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
void IdentifierLiteral::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void ListLiteral::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void DictLiteral::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void TupleLiteral::accept(AstVisitor& visitor) const {
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
//Expression/Operator nodes
void BinOp::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void PrefixOp::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void PostfixOp::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void CoalescingOP::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void RangeExpr::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void IndexExpr::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void DotExpr::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void ArrowExpr::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void FuncCall::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void TernaryIf::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void CompTimeExpr::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void LambdaExpr::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void FormattedStr::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void ThreadOrTaskExpr::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
// Simple Statement nodes
void DeferStmt::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void ScopeStmt::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void BreakStmt::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void ContinueStmt::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void ReturnStmt::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void GiveStmt::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void LockStmt::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
//Module statement nodes
void ImportStmt::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void UsingStmt::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
//Branch statement nodes
void WhenStmt::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void LoopStmt::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
void SelectStmt::accept(AstVisitor& visitor) const {
    visitor.visit(*this);
}
}