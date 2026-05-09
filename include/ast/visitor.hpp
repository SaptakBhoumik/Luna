#include "ast.hpp"

namespace Luna {

class AstVisitor {
  public:
    virtual ~AstVisitor() = default;
    //Basic nodes
    virtual bool visit(const Program& node) { return false; };
    virtual bool visit(const NoLiteral& node) { return false; };
    //Literal nodes
    virtual bool visit(const IntegerLiteral& node) { return false; };
    virtual bool visit(const DecimalLiteral& node) { return false; };
    virtual bool visit(const StringLiteral& node) { return false; };
    virtual bool visit(const BoolLiteral& node) { return false; };
    virtual bool visit(const NoneLiteral& node) { return false; };
    virtual bool visit(const IdentifierExpression& node) { return false; };
    //Type expression nodes
    virtual bool visit(const TypeExpr& node) { return false; };
    virtual bool visit(const ListTypeExpr& node) { return false; };
    virtual bool visit(const PtrTypeExpr& node) { return false; };
    virtual bool visit(const OptionalTypeExpr& node) { return false; };
    virtual bool visit(const ErrorTypeExpr& node) { return false; };
    virtual bool visit(const FuncTypeExpr& node) { return false; };
    virtual bool visit(const TupleTypeExpr& node) { return false; };
    virtual bool visit(const SimdTypeExpr& node) { return false; };
    virtual bool visit(const SumTypeExpr& node) { return false; };
    virtual bool visit(const EnumTypeExpr& node) { return false; };
    virtual bool visit(const StructTypeExpr& node) { return false; };
};
}