#pragma once
#include "lexer/token.hpp"
#include <memory>
#include <map>
#include <vector>
namespace Luna{
class AstNode;

using AstNodePtr = std::shared_ptr<AstNode>;

// ---- Attribute: #[name] or #[name(args...)] stored directly on owning nodes ----

struct Attribute {
    Token name;
    std::vector<AstNodePtr> args = {}; // empty if no argument list
    std::vector<std::pair<Token, AstNodePtr>> named_args = {}; // empty if no named arguments
};

std::string to_string(const Attribute& attr);

// ---- Decorator: @name or @name(args...) stored directly on owning nodes ----
struct Decorator{
    /*
    @func(args)
    The decorator field can only contain the callee of the FuncCall i.e it is just the ast of the identifier func
    Parsing tips:-U use the parse_expr to parse it. It may return a FuncCall. If FuncCall then just deconstruct to this form and done.
                 If not FuncCall then just use the returned ast as the decorator and args will be empty. 
                 This way we can support complex expressions as decorators like @decorator_factory(arg1)(arg2)
    */
    AstNodePtr decorator; // An IdentifierLiteral or some expression
    std::vector<AstNodePtr> args = {}; // empty if no argument list. 
    std::vector<std::pair<Token, AstNodePtr>> named_args = {}; // empty if no named arguments
};

std::string to_string(const Decorator& decorator);

// ---- Annotation: A decorator or a attribute used on a function
struct Annotation {
    Decorator decorator;
    Attribute attributes;
    bool is_decorator; // true if this annotation is a decorator, false if its an attribute
};

std::string to_string(const Annotation& annotation);

// ---- Parameter ----

enum class ParamKind {
    Normal,
    CVariadic, // ...
    VarArg, // *args
    CompileTimeVarArg, // *$args 
    KwVararg, // **kwargs
    CompileTimeKwVararg, // **$kwargs
    CompileTime,// $param
};

struct Parameter {
    AstNodePtr type;//No literal if type is inferred. 
    Token name;//"" for CVariadic
    AstNodePtr default_value; // NoLiteral if absent
    bool is_mut = false;
    ParamKind kind = ParamKind::Normal;
};
std::string to_string(const Parameter& param);

// ---- Lambda capture ----

enum class CaptureKind {
    None,    // []
    AllCopy, // [=]
    AllRef,  // [&]
    List,    // [a, &b, ...]
};

struct CaptureEntry {
    bool by_ref;
    AstNodePtr name;
};

struct CaptureClause {
    CaptureKind kind = CaptureKind::None;
    std::vector<CaptureEntry> entries = {}; // populated when kind == List
};

std::string to_string(const CaptureClause& capture);

struct LambdaFuncSignature {
    std::vector<Parameter> parameters;
    CaptureClause capture;
    AstNodePtr return_type; // NoLiteral if no return type annotation
};
// ---- Struct field (for struct definitions) ----

// A single field inside a struct definition
struct StructField {
    Token name;
    AstNodePtr type;
    AstNodePtr default_value; // NoLiteral when absent
    bool is_pub = false;
    bool is_mut = false;
    std::vector<Attribute> attributes;
};

std::string to_string(const StructField& field);

// ---- Loop and select arms ----
// Unified loop node - all forms use the single `loop` keyword.
enum class LoopKind {
    Infinite,    // loop { }
    WhileStyle,  // loop cond { }
    IteratorFor, // loop v :iter  or  loop (k, v) :iter
};

enum class SelectArmKind {
    Recv,    // pattern <-- channel { body }
    Send,    // value --> channel { body }
    Default, // ? { body }
};

// A single arm inside a select block.
// For Default arms, pattern and channel are NoLiteral.
struct SelectArm {
    SelectArmKind arm_kind;
    std::vector<std::pair<AstNodePtr, bool>> value; // receive variables or literal patterns for value-matching recv. IT is pair of (variable,is mutable)
                                                    // For send it is just the value to be sent and false on mutable. For default its just empty.
    std::vector<AstNodePtr> channel;
};

std::string to_string(const SelectArm& arm);

// ----Types of variable----
enum class VarKind {
    Normal,
    ThreadLocal,
    TaskLocal,
};
}