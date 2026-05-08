#include "ast.hpp"

namespace Luna {

class AstVisitor {
  public:
    virtual ~AstVisitor() = default;

    virtual bool visit(const Program& node) { return false; };
    virtual bool visit(const NoLiteral& node) { return false; };
};
}