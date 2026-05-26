error_src = [
    'error/error.cpp' 
]
lexer_src = [
    'lexer/error.cpp',
    'lexer/lexer.cpp',
    'lexer/token.cpp',
    'lexer/utils.cpp'
]

ast_src = [
    'ast/node/basic_node.cpp',
    'ast/node/branch_stmt_node.cpp',
    'ast/node/def_stmt_node.cpp',
    'ast/node/expr_node.cpp',
    'ast/node/literal_node.cpp',
    'ast/node/module_stmt_node.cpp',
    'ast/node/simple_stmt_node.cpp',
    'ast/node/type_expr_node.cpp',

    'ast/ast_utils.cpp',
    'ast/visitor.cpp'
]

parser_src = [
    'parser/parse_branch_stmt.cpp',
    'parser/parse_def_stmt.cpp',
    'parser/parse_expr_node.cpp',
    'parser/parse_literal.cpp',
    # 'parser/parse_module_stmt.cpp',
    'parser/parse_simple_stmt.cpp',
    'parser/parse_type_expr.cpp',
    'parser/parse_utils.cpp',
    'parser/parser.cpp',
    'parser/utils.cpp',
]

include = include_directories('../include/')
error = static_library('error', sources: error_src, include_directories: include)
lexer = static_library('lexer', sources: lexer_src, include_directories: include)
ast = static_library('ast', sources: ast_src, include_directories: include)
parser = static_library('parser', sources: parser_src, include_directories: include)