#pragma once

#include "lexer/token.hpp"
#include "ast_utils.hpp"
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string>
#include <vector>
namespace Luna {

enum class AstKind {
    //Basic nodes
    Program,
    NoLiteral,

    // Literal nodes
    Integer,
    Decimal,
    String,
    Bool,
    None,
    Identifier,

    // Type nodes
    TypeExpr,
    ListTypeExpr,
    PtrTypeExpr,
    OptionalTypeExpr,
    ErrorTypeExpr,
    FuncTypeExpr,
    TupleTypeExpr,
    SimdTypeExpr,
    SumTypeExpr,
    EnumTypeExpr,
    StructTypeExpr,

    // Expression nodes
    List,
    Dict,
    TupleExpr,
    BinaryOp,
    PrefixExpr,
    PostfixExpr,
    IndexExpr,
    DotExpr,
    ArrowExpr,
    FunctionCall,
    GenericCall,
    TernaryIf,
    TernaryFor,
    CastExpr,
    CompileTimeExpr,
    Lambda,
    FormattedStr,
    DefaultArg,
    ThreadExpr,
    TaskExpr,

    // Statement nodes
    ImportStmt,
    UsingStmt,
    VariableStmt,
    MultipleAssign,
    AugAssign,
    BlockStmt,
    FunctionDef,
    MethodDef,
    ExternFuncDef,
    ReturnStmt,
    GiveStmt,
    DeferStmt,
    IfStmt,
    LoopStmt,
    BreakStmt,
    ContinueStmt,
    MatchStmt,
    TypeDefinition,
    DecoratorStmt,
    PubDef,
    InlineAsm,
    LockStmt,
    SelectStmt,
    CompileTimeIf,
    CompileTimeLoop,
    CompileTimeMatch,
};

// ---- Forward declarations ----

class AstVisitor;

class AstNode {
public:
    virtual ~AstNode() = default;

    virtual Token token() const = 0;
    virtual AstKind kind() const = 0;
    virtual std::string stringify() const = 0;

    virtual void accept(AstVisitor& visitor) const = 0;
};

using AstNodePtr = std::shared_ptr<AstNode>;

// ============================
//  Basic nodes
// ============================
class Program : public AstNode {
    std::vector<AstNodePtr> statements;
public:
    Program(std::vector<AstNodePtr> statements);

    std::vector<AstNodePtr> get_statements() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// For absent optional AST fields
class NoLiteral : public AstNode {
public:
    NoLiteral() = default;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// ============================
//  Literal nodes
// ============================

class IntegerLiteral : public AstNode {
    Token tok;
public:
    IntegerLiteral(Token tok);

    std::string get_value() const;// Why std::string here? Because the literal might be too big to fit in built-in integer types, so we keep it as a string and 
                              // parse it into a big integer type later when needed.
    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class DecimalLiteral : public AstNode {
    Token tok;
public:
    DecimalLiteral(Token tok);

    std::string get_value() const;//Same reason for string as IntegerLiteral

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class StringLiteral : public AstNode {
    Token tok;
    bool raw;
public:
    StringLiteral(Token tok,bool raw = false);
    std::string get_value() const;
    bool is_raw() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class BoolLiteral : public AstNode {
    Token tok;
public:
    BoolLiteral(Token tok);
    bool get_value() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class NoneLiteral : public AstNode {
    Token tok;
public:
    NoneLiteral(Token tok);

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class IdentifierExpression : public AstNode {
    Token tok;
    std::vector<std::string> path;//The path. Like A::B::C will be ["A", "B", "C"]
    std::vector<AstNodePtr> generic_args; // populated if this identifier is something like func{generic_arg1, generic_arg2} but we dont call the funciton yet. 
    // This is just the identifier with generic args, the actual function call will be a separate FunctionCall node with this as the callee.
public:
    IdentifierExpression(Token tok, std::vector<std::string> path, std::vector<AstNodePtr> generic_args = {});

    std::vector<std::string> get_path() const;
    std::vector<AstNodePtr> get_generic_args() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;
    
    void accept(AstVisitor& visitor) const;
};

// ============================
//  Type expression nodes
// ============================

// Named type, optionally generic: i32, Stack{T}, Map{K,V}
class TypeExpr : public AstNode {
    Token tok;
    AstNodePtr value;//Will be a identifier expression node representing the type name and its generic args if any. 
    AstNodePtr idx; //When we do stuff like T[0] where T is a tuple type. T[0] represents the first type of T. And so on, NoLiteral if not applicable.
                    //It can also be a string as the idx. Like T["field"] for accessing the field type of a struct
                    //Note:-Only one itm as idx. Unlike regular indexing, a type expr can have only one idx
public:
    TypeExpr(Token tok, AstNodePtr value, AstNodePtr idx);

    AstNodePtr get_value() const;
    AstNodePtr get_idx() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;
    
    void accept(AstVisitor& visitor) const;
};

// [T, N] — fixed-size array; size is NoLiteral for unsized [T]
class ListTypeExpr : public AstNode {
    Token tok;
    AstNodePtr elem_type;
    AstNodePtr size;
public:
    ListTypeExpr(Token tok, AstNodePtr elem_type, AstNodePtr size);

    AstNodePtr get_elem_type() const;
    AstNodePtr get_size() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;
    
    void accept(AstVisitor& visitor) const;
};

// *T
class PtrTypeExpr : public AstNode {
    Token tok;
    AstNodePtr base_type;
public:
    PtrTypeExpr(Token tok, AstNodePtr base_type);

    AstNodePtr get_base_type() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// ?T
class OptionalTypeExpr : public AstNode {
    Token tok;
    AstNodePtr base_type;
public:
    OptionalTypeExpr(Token tok, AstNodePtr base_type);

    AstNodePtr get_base_type() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// !T — error-or-value type
class ErrorTypeExpr : public AstNode {
    Token tok;
    AstNodePtr base_type;
public:
    ErrorTypeExpr(Token tok, AstNodePtr base_type);

    AstNodePtr get_base_type() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// fn(T1, T2) -> T; return_type is NoLiteral for void
class FuncTypeExpr : public AstNode {
    Token tok;
    std::vector<AstNodePtr> param_types = {};
    bool c_variadic = false; // true if ... is present at the end of the parameter list
    AstNodePtr return_type;
public:
    FuncTypeExpr(Token tok, std::vector<AstNodePtr> param_types, bool c_variadic, AstNodePtr return_type);

    std::vector<AstNodePtr> get_param_types() const;
    AstNodePtr get_return_type() const;
    bool has_c_variadic() const;


    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// (T1, T2, T3) — tuple type
class TupleTypeExpr : public AstNode {
    Token tok;
    std::vector<AstNodePtr> elem_types;
public:
    TupleTypeExpr(Token tok, std::vector<AstNodePtr> elem_types);

    std::vector<AstNodePtr> get_elem_types() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// <T, N> — SIMD vector type
class SimdTypeExpr : public AstNode {
    Token tok;

    AstNodePtr elem_type;
    AstNodePtr lanes; // integer literal/compile time for lane count
public:
    SimdTypeExpr(Token tok, AstNodePtr elem_type, AstNodePtr lanes);
    AstNodePtr get_elem_type() const;
    AstNodePtr get_lanes() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// T1 | T2 | T3 — sum / tagged-union type alias
class SumTypeExpr : public AstNode {
    Token tok;
    std::vector<AstNodePtr> variants;
public:
    SumTypeExpr(Token tok, std::vector<AstNodePtr> variants);
    std::vector<AstNodePtr> get_variants() const;
    
    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};


// enum { A = expr, B, ... }  or  type Name = enum:base_type { ... }
// variant value is NoLiteral when not explicitly set.
class EnumTypeExpr : public AstNode {
    Token tok;
    AstNodePtr base_type; // NoLiteral -> default underlying type
    std::vector<std::pair<AstNodePtr, AstNodePtr>> variants; // (name, value)
public:
    EnumTypeExpr(Token tok, AstNodePtr base_type, std::vector<std::pair<AstNodePtr, AstNodePtr>> variants);

    AstNodePtr get_base_type() const;
    std::vector<std::pair<AstNodePtr, AstNodePtr>> get_variants() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// { fields... }
class StructTypeExpr : public AstNode {
    Token tok;
    std::vector<StructField> fields;
public:
    StructTypeExpr(Token tok, std::vector<StructField> fields);

    std::vector<StructField> get_fields() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};
// ============================
//  Expression nodes
// ============================

class ListLiteral : public AstNode {
    Token m_token;
    std::vector<AstNodePtr> m_elements;
public:
    ListLiteral(Token tok, std::vector<AstNodePtr> elements = {});
    std::vector<AstNodePtr> elements() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class DictLiteral : public AstNode {
    Token m_token;
    std::vector<std::pair<AstNodePtr, AstNodePtr>> m_elements; // (key, value)
public:
    DictLiteral(Token tok,std::vector<std::pair<AstNodePtr, AstNodePtr>> elements);
    std::vector<std::pair<AstNodePtr, AstNodePtr>> elements() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// (a, b, c) — expression tuple / multiple-return value
class TupleExpression : public AstNode {
    Token m_token;
    std::vector<AstNodePtr> m_elements;
public:
    TupleExpression(Token tok, std::vector<AstNodePtr> elements);
    std::vector<AstNodePtr> elements() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class BinaryOperation : public AstNode {
    Token m_token;

    AstNodePtr m_left;
    Token m_operator;
    AstNodePtr m_right;
public:
    BinaryOperation(Token tok, AstNodePtr left, Token op, AstNodePtr right);

    AstNodePtr left() const;
    Token op() const;
    AstNodePtr right() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class PrefixExpression : public AstNode {
    Token m_token;
    Token m_prefix;
    AstNodePtr m_right;
public:
    PrefixExpression(Token tok, Token prefix, AstNodePtr right);
    Token prefix() const;
    AstNodePtr right() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// Covers ++, --, ! (error propagation)
class PostfixExpression : public AstNode {
    Token m_token;
    
    Token m_postfix;
    AstNodePtr m_left;
public:
    PostfixExpression(Token tok, Token postfix, AstNodePtr left);
    Token postfix() const;
    AstNodePtr left() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// a[i]  or  a[i, j]  (multi-index for magic __getitem__)
class IndexExpression : public AstNode {
    Token m_token;
    AstNodePtr m_container;
    std::vector<AstNodePtr> m_indices;
public:
    IndexExpression(Token tok, AstNodePtr container,std::vector<AstNodePtr> indices);
    AstNodePtr container() const;

    std::vector<AstNodePtr> indices() const;
    Token token() const;
    AstKind kind() const;
    
    std::string stringify() const;
    void accept(AstVisitor& visitor) const;
};

// a.b
class DotExpression : public AstNode {
    Token m_token;
    AstNodePtr m_owner;
    AstNodePtr m_member;
public:
    DotExpression(Token tok, AstNodePtr owner, AstNodePtr member);
    AstNodePtr owner() const;
    AstNodePtr member() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;
    void accept(AstVisitor& visitor) const;
};

// a->b  (pointer member access)
class ArrowExpression : public AstNode {
    Token m_token;
    AstNodePtr m_owner;
    AstNodePtr m_member;
public:
    ArrowExpression(Token tok, AstNodePtr owner, AstNodePtr member);
    AstNodePtr owner() const;
    AstNodePtr member() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class FunctionCall : public AstNode {
    Token m_token;
    AstNodePtr m_callee;
    std::vector<AstNodePtr> m_generic_args;
    std::vector<AstNodePtr> m_arguments;
    std::map<std::string, AstNodePtr> m_named_arguments; // for calls with named arguments
public:
    FunctionCall(Token tok, AstNodePtr callee,std::vector<AstNodePtr> arguments = {}, 
                std::map<std::string, AstNodePtr> named_arguments = {}, std::vector<AstNodePtr> generic_args = {});
    AstNodePtr callee() const;
    std::vector<AstNodePtr> arguments() const;
    std::map<std::string, AstNodePtr> namedArguments() const;
    std::vector<AstNodePtr> genericArgs() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// // f{T1, T2}  — the generic-instantiation expression; wraps the callee.
// // The surrounding FunctionCall node carries the actual argument list.
// class GenericCall : public AstNode {
//     Token m_token;
//     AstNodePtr m_callee;
//     std::vector<AstNodePtr> m_generic_args;
// public:
//     GenericCall(Token tok, AstNodePtr callee,
//                 std::vector<AstNodePtr> generic_args);
//     AstNodePtr callee() const;
//     std::vector<AstNodePtr> genericArgs() const;
//     Token token() const;
//     AstKind kind() const;
//     std::string stringify() const;
//     void accept(AstVisitor& visitor) const;
// };

// condition ? then_value : else_value
class TernaryIf : public AstNode {
    Token m_token;
    AstNodePtr m_condition;
    AstNodePtr m_then_value;
    AstNodePtr m_else_value;
public:
    TernaryIf(Token tok, AstNodePtr condition, AstNodePtr then_value,
              AstNodePtr else_value);
    AstNodePtr condition() const;
    AstNodePtr thenValue() const;
    AstNodePtr elseValue() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// $expr — any compile-time expression ($typesize, $typeid, $has_method, etc. Even for compile time functiona and stuff)
class CompileTimeExpression : public AstNode {
    Token m_token;
    AstNodePtr m_expression;
public:
    CompileTimeExpression(Token tok, AstNodePtr expression);
    AstNodePtr expression() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;
    
    void accept(AstVisitor& visitor) const;
};

// fn(params)[capture] -> ret_type { body }
class LambdaDefinition : public AstNode {
    Token m_token;
    std::vector<Parameter> m_parameters;//Note for lambda parameter:-Only regular parameters without named argument is allowed
    CaptureClause m_capture;
    AstNodePtr m_return_type; // NoLiteral if inferred to be void
    AstNodePtr m_body;
public:
    LambdaDefinition(Token tok, std::vector<Parameter> parameters, CaptureClause capture, AstNodePtr return_type, AstNodePtr body);
    std::vector<Parameter> parameters() const;
    CaptureClause capture() const;
    AstNodePtr returnType() const;
    AstNodePtr body() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;
    
    void accept(AstVisitor& visitor) const;
};

// f"text {expr} more text"
// parts alternates: StringLiteral segments and interpolated expression nodes
class FormattedString : public AstNode {
    Token m_token;
    std::vector<AstNodePtr> m_parts;
public:
    FormattedString(Token tok, std::vector<AstNodePtr> parts);
    std::vector<AstNodePtr> parts() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};


// thread { body }  or  thread single_expr or task { body } or task single_expr
// restart_count / timeout_ms / restart_delay_ms are NoLiteral when absent
class ThreadOrTaskExpr : public AstNode {
    Token m_token;
    bool is_thread; // true for thread, false for task
    AstNodePtr m_body;
    AstNodePtr m_restart_count;
    AstNodePtr m_timeout_ms;
    AstNodePtr m_always_restart;
    AstNodePtr m_restart_delay_ms;
public:
    ThreadOrTaskExpr(Token tok, bool is_thread, AstNodePtr body,
                     AstNodePtr restart_count, AstNodePtr timeout_ms,
                     AstNodePtr always_restart, AstNodePtr restart_delay_ms);
    AstNodePtr body() const;
    AstNodePtr restartCount() const;
    AstNodePtr timeoutMs() const;
    AstNodePtr alwaysRestart() const;
    AstNodePtr restartDelayMs() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};



// ============================
//  Statement nodes
// ============================

// import path::sym  or  import path::{s1, s2}  
class ImportStatement : public AstNode {
    Token m_token;
    std::vector<std::string> m_module_path;
    std::vector<std::string> m_imported_symbols;
public:
    ImportStatement(Token tok, std::vector<std::string> module_path, std::vector<std::string> imported_symbols);
    std::vector<std::string> modulePath() const;
    std::vector<std::string> importedSymbols() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// using alias = path  or  using path  (alias is NoLiteral for the second form)
class UsingStatement : public AstNode {
    Token m_token;
    std::vector<std::string> m_path;
    std::optional<std::string> m_alias;
public:
    UsingStatement(Token tok, std::vector<std::string> path, std::optional<std::string> alias);
    std::vector<std::string> path() const;
    std::optional<std::string> alias() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class ExternalModuleSymbol {
    Token m_token;
    std::vector<std::string> m_path;
public:
    ExternalModuleSymbol(Token tok, std::vector<std::string> path);
    std::vector<std::string> path() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

enum class VarKind {
    Normal,
    Atomic,
    ThreadLocal,
    TaskLocal,
};

// Covers all variable introduction forms:
//   x := expr
//   x:T = expr
//   mut x:T = expr
//   atomic mut x:T = expr
//   thread_local mut x:T = expr
//   task_local mut x:T = expr
// type is NoLiteral when inferred; value is NoLiteral when uninitialized.
class VariableStatement : public AstNode {
    Token m_token;
    AstNodePtr m_name;
    AstNodePtr m_type;
    AstNodePtr m_value;
    bool m_is_mut   = false;
    bool m_is_def = false;//Means if it is an assignment or defination
    VarKind m_var_kind = VarKind::Normal;
    std::vector<Attribute> m_attributes;
public:
    VariableStatement(Token tok, AstNodePtr name, AstNodePtr type,
                      AstNodePtr value, bool is_mut, bool is_def,
                      VarKind var_kind = VarKind::Normal,
                      std::vector<Attribute> attributes = {});
    AstNodePtr name() const;
    AstNodePtr varType() const;
    AstNodePtr value() const;
    bool isMut() const;
    bool isDef() const;
    VarKind varKind() const;
    std::vector<Attribute> attributes() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};


class AugAssign : public AstNode {
    Token m_token;
    Token m_op;
    AstNodePtr m_target;
    AstNodePtr m_value;
public:
    AugAssign(Token tok, Token op, AstNodePtr target, AstNodePtr value);
    AstNodePtr target() const;
    Token op() const;
    AstNodePtr value() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class BlockStatement : public AstNode {
    Token m_token;
    std::vector<AstNodePtr> m_statements;
public:
    BlockStatement(Token tok, std::vector<AstNodePtr> statements);
    std::vector<AstNodePtr> statements() const;
    Token token() const;
    AstKind kind() const;
    std::string stringify() const;
    void accept(AstVisitor& visitor) const;
};

// fn name{generics}(params) -> return_type { body }
// body is NoLiteral for a forward declaration.
class FunctionDefinition : public AstNode {
    Token m_token;
    std::string m_name;
    std::vector<std::string> m_generics;
    std::vector<Parameter> m_parameters;
    AstNodePtr m_return_type; // NoLiteral for void / inferred
    AstNodePtr m_body;
    std::vector<Attribute> m_attributes;
public:
    FunctionDefinition(Token tok, std::string name,
                       std::vector<std::string> generics,
                       std::vector<Parameter> parameters,
                       AstNodePtr return_type, AstNodePtr body,
                       std::vector<Attribute> attributes = {});
    std::string name() const;
    std::vector<std::string> generics() const;
    std::vector<Parameter> parameters() const;
    AstNodePtr returnType() const;
    AstNodePtr body() const;
    std::vector<Attribute> attributes() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// fn (receiver) name{generics}(params) -> return_type { body }
class MethodDefinition : public AstNode {
    Token m_token;
    Parameter m_receiver;//Only normal type with no default value
    std::string m_name;
    std::vector<std::string> m_generics;
    std::vector<Parameter> m_parameters;
    AstNodePtr m_return_type;
    AstNodePtr m_body;
    std::vector<Attribute> m_attributes;
public:
    MethodDefinition(Token tok, Parameter receiver, std::string name,
                     std::vector<std::string> generics,
                     std::vector<Parameter> parameters,
                     AstNodePtr return_type, AstNodePtr body,
                     std::vector<Attribute> attributes = {});
    Parameter receiver() const;
    std::string name() const;
    std::vector<std::string> generics() const;
    std::vector<Parameter> parameters() const;
    AstNodePtr returnType() const;
    AstNodePtr body() const;
    std::vector<Attribute> attributes() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// ret expr  (expr is NoLiteral for bare `ret`)
class ReturnStatement : public AstNode {
    Token m_token;
    AstNodePtr m_value;
public:
    ReturnStatement(Token tok, AstNodePtr value);
    AstNodePtr value() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// give expr — value-yielding exit from a ?? / !! handler block
class GiveStatement : public AstNode {
    Token m_token;
    AstNodePtr m_value;
public:
    GiveStatement(Token tok, AstNodePtr value);
    AstNodePtr value() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;
    void accept(AstVisitor& visitor) const;
};

// defer { body }
class DeferStatement : public AstNode {
    Token m_token;
    AstNodePtr m_body;
public:
    DeferStatement(Token tok, AstNodePtr body);
    AstNodePtr body() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// if { cond1 { body1 }  cond2 { body2 }  ? { else_body } }
// Each branch is (condition, body); condition is NoLiteral for the ? (else) arm.
class IfStatement : public AstNode {
    Token m_token;
    std::vector<std::pair<AstNodePtr, AstNodePtr>> m_branches;
public:
    IfStatement(Token tok, std::vector<std::pair<AstNodePtr, AstNodePtr>> branches);
    std::vector<std::pair<AstNodePtr, AstNodePtr>> branches() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// Unified loop node — all forms use the single `loop` keyword.
enum class LoopKind {
    Infinite,    // loop { }
    WhileStyle,  // loop cond { }
    IteratorFor, // loop v :iter  or  loop k, v :iter
};

// Fields that are irrelevant for a given LoopKind hold NoLiteral.
class LoopStatement : public AstNode {
    Token m_token;
    LoopKind m_loop_kind;
    std::vector<AstNodePtr> m_variables; // loop variable(s); empty for Infinite/WhileStyle. Not a string cuz u can have complex stuff like tuples and all
    AstNodePtr m_value;// Iterable for `for` loop and condition for `while`, no literal for infinite loop
    AstNodePtr m_body;
    std::vector<Attribute> m_attributes; // @[parallel], @[simd], etc.
public:
    LoopStatement(Token tok, LoopKind loop_kind,
                  std::vector<AstNodePtr> variables,
                  AstNodePtr value,
                  AstNodePtr body,
                  std::vector<Attribute> attributes = {});
    LoopKind loopKind() const;
    std::vector<AstNodePtr> variables() const;
    AstNodePtr value() const;
    AstNodePtr body() const;
    std::vector<Attribute> attributes() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class RangeStatement : public AstNode {
    Token m_token;
    AstNodePtr m_start;
    AstNodePtr m_end;   
    AstNodePtr m_step;  // NoLiteral for default step of 1
public:
    RangeStatement(Token tok, AstNodePtr start, AstNodePtr end, AstNodePtr step);
    AstNodePtr start() const;
    AstNodePtr end() const;
    AstNodePtr step() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};
class BreakStatement : public AstNode {
    Token m_token;
public:
    BreakStatement(Token tok);
    
    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

class ContinueStatement : public AstNode {
    Token m_token;
public:
    ContinueStatement(Token tok);

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// match subj1, subj2 { pat1, pat2 { body }  ? { default_body } }
// default body is the one where the m_cases[i] has only one pattern which is the ?
class MatchStatement : public AstNode {
    Token m_token;
    std::vector<AstNodePtr> m_subjects;
    std::vector<std::pair<std::vector<AstNodePtr>, AstNodePtr>> m_cases;
public:
    MatchStatement(Token tok, std::vector<AstNodePtr> subjects,
                   std::vector<std::pair<std::vector<AstNodePtr>, AstNodePtr>> cases);
    std::vector<AstNodePtr> subjects() const;
    std::vector<std::pair<std::vector<AstNodePtr>, AstNodePtr>> cases() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// type Name{generics} = base_type
class TypeDefinition : public AstNode {
    Token m_token;
    AstNodePtr m_name;
    std::vector<std::string> m_generics;
    AstNodePtr m_base_type;
    bool m_is_pub = false;
    std::vector<Attribute> m_attributes; // e.g. @[align(16)]
public:
    TypeDefinition(Token tok, AstNodePtr name,
                   std::vector<std::string> generics, AstNodePtr base_type,std::vector<Attribute> attributes, bool is_pub = false);
    AstNodePtr name() const;
    std::vector<std::string> generics() const;
    AstNodePtr baseType() const;
    std::vector<Attribute> attributes() const;
    bool isPub() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};

// @decorator  or  @decorator(args)  applied to the wrapped definition
class DecoratorStatement : public AstNode {
    Token m_token;
    // Each decorator is an IdentifierExpression or a FunctionCall
    std::vector<AstNodePtr> m_decorators;
    AstNodePtr m_body;
public:
    DecoratorStatement(Token tok, std::vector<AstNodePtr> decorators,AstNodePtr body);
    std::vector<AstNodePtr> decorators() const;
    AstNodePtr body() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;
    
    void accept(AstVisitor& visitor) const;
};

// lock var { body }  or  lock (var1, var2) { body }
class LockStatement : public AstNode {
    Token m_token;
    std::vector<AstNodePtr> m_targets;
    AstNodePtr m_body;
public:
    LockStatement(Token tok, std::vector<AstNodePtr> targets,AstNodePtr body);
    std::vector<AstNodePtr> targets() const;
    AstNodePtr body() const;
    
    Token token() const;
    AstKind kind() const;
    std::string stringify() const;
    
    void accept(AstVisitor& visitor) const;
};

enum class SelectArmKind {
    Recv,    // pattern <- channel { body }
    Send,    // value -> channel { body }
    Default, // ? { body }
};

// A single arm inside a select block.
// For Default arms, pattern and channel are NoLiteral.
struct SelectArm {
    SelectArmKind arm_kind;
    AstNodePtr pattern; // receive variable or literal pattern for value-matching recv
    AstNodePtr channel;
    AstNodePtr body;
};

// select { arm1  arm2  ... }
class SelectStatement : public AstNode {
    Token m_token;
    std::vector<SelectArm> m_arms;
public:
    SelectStatement(Token tok, std::vector<SelectArm> arms);
    std::vector<SelectArm> arms() const;

    Token token() const;
    AstKind kind() const;
    std::string stringify() const;

    void accept(AstVisitor& visitor) const;
};
} // namespace ast