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
    virtual bool visit(const IdentifierLiteral& node) { return false; };
    virtual bool visit(const ListLiteral& node) { return false; };
    virtual bool visit(const DictLiteral& node) { return false; };
    virtual bool visit(const EmptyDictOrListLiteral& node) { return false; };
    virtual bool visit(const TupleLiteral& node) { return false; };
    virtual bool visit(const AssignTupleLiteral& node) { return false; };
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
    virtual bool visit(const InterfaceTypeExpr& node) { return false; };
    //Expression/Operator nodes
    virtual bool visit(const BinOp& node) { return false; };
    virtual bool visit(const PrefixOp& node) { return false; };
    virtual bool visit(const PostfixOp& node) { return false; };
    virtual bool visit(const CoalescingOP& node) { return false; };
    virtual bool visit(const RangeExpr& node) { return false; };
    virtual bool visit(const IndexExpr& node) { return false; };
    virtual bool visit(const DotExpr& node) { return false; };
    virtual bool visit(const ArrowExpr& node) { return false; };
    virtual bool visit(const FuncCall& node) { return false; };
    virtual bool visit(const TernaryIf& node) { return false; };
    virtual bool visit(const CompTimeExpr& node) { return false; };
    virtual bool visit(const LambdaExpr& node) { return false; };
    virtual bool visit(const FormattedStr& node) { return false; };
    virtual bool visit(const ThreadOrTaskExpr& node) { return false; };
    //Simple Statement nodes
    virtual bool visit(const DeferStmt& node) { return false; };
    virtual bool visit(const ScopeStmt& node) { return false; };
    virtual bool visit(const BreakStmt& node) { return false; };
    virtual bool visit(const ContinueStmt& node) { return false; };
    virtual bool visit(const ReturnStmt& node) { return false; };
    virtual bool visit(const GiveStmt& node) { return false; };
    virtual bool visit(const LockStmt& node) { return false; };
    //Module statement nodes
    virtual bool visit(const ImportStmt& node) { return false; };
    virtual bool visit(const UsingStmt& node) { return false; };
    //Branch statement nodes
    virtual bool visit(const WhenStmt& node) { return false; };
    virtual bool visit(const LoopStmt& node) { return false; };
    virtual bool visit(const SelectStmt& node) { return false; };
    //Defination/Assignment statement nodes
    virtual bool visit(const TypeDefStmt& node) { return false; };
    virtual bool visit(const VarStmt& node) { return false; };
    virtual bool visit(const AugAssignStmt& node) { return false; };
    virtual bool visit(const FuncDefStmt& node) { return false; };
    virtual bool visit(const MethodDefStmt& node) { return false; };
};
}