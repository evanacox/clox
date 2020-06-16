#include "compiler.h"
#include "../common/memory.h"
#include "scanner.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG_PRINT_CODE
#include "../util/disassembler.h"
#endif

/** The parser instance */
static struct {
    /** The current token being parsed */
    token current;

    /** The previous token */
    token previous;

    /** Whether or not the parser had a compilation error */
    bool had_err;

    /** Whether or not the parser is in a "panic" state */
    bool panic;

    /** Pointer to the chunk being written to */
    chunk *current_chunk;
} s_parser;

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT, // =
    PREC_OR,         // or
    PREC_AND,        // and
    PREC_EQUALITY,   // == !=
    PREC_COMPARISON, // < > <= >=
    PREC_TERM,       // + -
    PREC_FACTOR,     // * /
    PREC_UNARY,      // ! -
    PREC_CALL,       // . ()
    PREC_PRIMARY
} precedence;

typedef void (*parse_fn)();

typedef struct parse_rule {
    parse_fn prefix;
    parse_fn infix;
    precedence precedence;
} parse_rule;

static void binary();
static void unary();
static void grouping();
static void number();
static void literal();

parse_rule rules[] = {
    {grouping, NULL, PREC_NONE},     // TOKEN_LEFT_PAREN
    {NULL, NULL, PREC_NONE},         // TOKEN_RIGHT_PAREN
    {NULL, NULL, PREC_NONE},         // TOKEN_LEFT_BRACE
    {NULL, NULL, PREC_NONE},         // TOKEN_RIGHT_BRACE
    {NULL, NULL, PREC_NONE},         // TOKEN_COMMA
    {NULL, NULL, PREC_NONE},         // TOKEN_DOT
    {unary, binary, PREC_TERM},      // TOKEN_MINUS
    {NULL, binary, PREC_TERM},       // TOKEN_PLUS
    {NULL, NULL, PREC_NONE},         // TOKEN_SEMICOLON
    {NULL, binary, PREC_FACTOR},     // TOKEN_SLASH
    {NULL, binary, PREC_FACTOR},     // TOKEN_STAR
    {unary, NULL, PREC_NONE},        // TOKEN_BANG
    {NULL, NULL, PREC_NONE},         // TOKEN_BANG_EQUAL
    {NULL, NULL, PREC_NONE},         // TOKEN_EQUAL
    {NULL, binary, PREC_EQUALITY},   // TOKEN_EQUAL_EQUAL
    {NULL, binary, PREC_COMPARISON}, // TOKEN_GREATER
    {NULL, binary, PREC_COMPARISON}, // TOKEN_GREATER_EQUAL
    {NULL, binary, PREC_COMPARISON}, // TOKEN_LESS
    {NULL, binary, PREC_COMPARISON}, // TOKEN_LESS_EQUAL
    {NULL, NULL, PREC_NONE},         // TOKEN_IDENTIFIER
    {NULL, NULL, PREC_NONE},         // TOKEN_STRING
    {number, NULL, PREC_NONE},       // TOKEN_NUMBER
    {NULL, NULL, PREC_NONE},         // TOKEN_AND
    {NULL, NULL, PREC_NONE},         // TOKEN_CLASS
    {NULL, NULL, PREC_NONE},         // TOKEN_ELSE
    {literal, NULL, PREC_NONE},      // TOKEN_FALSE
    {NULL, NULL, PREC_NONE},         // TOKEN_FOR
    {NULL, NULL, PREC_NONE},         // TOKEN_FUN
    {NULL, NULL, PREC_NONE},         // TOKEN_IF
    {literal, NULL, PREC_NONE},      // TOKEN_NIL
    {NULL, NULL, PREC_NONE},         // TOKEN_OR
    {NULL, NULL, PREC_NONE},         // TOKEN_PRINT
    {NULL, NULL, PREC_NONE},         // TOKEN_RETURN
    {NULL, NULL, PREC_NONE},         // TOKEN_SUPER
    {NULL, NULL, PREC_NONE},         // TOKEN_THIS
    {literal, NULL, PREC_NONE},      // TOKEN_TRUE
    {NULL, NULL, PREC_NONE},         // TOKEN_VAR
    {NULL, NULL, PREC_NONE},         // TOKEN_WHILE
    {NULL, NULL, PREC_NONE},         // TOKEN_ERROR
    {NULL, NULL, PREC_NONE},         // TOKEN_EOF
};

/**
 * Reports an error at a specific token
 * @param tok Pointer to the offending token
 * @param message The error to report
 */
static void error_at(token *tok, const char *message) {
    if (s_parser.panic) return;
    s_parser.panic = true;
    fprintf(stderr, "[line %d] Error at ", tok->line);

    if (tok->type == TOKEN_EOF) {
        fprintf(stderr, "end");
    } else if (tok->type == TOKEN_ERROR) {
        //
    } else {
        fprintf(stderr, "'%.*s'", tok->len, tok->tok_start);
    }

    fprintf(stderr, ": %s\n", message);
    fflush(stderr);
    s_parser.had_err = true;
}

/**
 * Reports an error of some sort at the current token
 * @param message The error to report
 */
static inline void error_at_current(const char *message) {
    error_at(&s_parser.current, message);
}

/**
 * Reports an error of some sort at the previous token
 * @param message The error to report
 */
static inline void error(const char *message) {
    error_at(&s_parser.previous, message);
}

/**
 * Writes a byte to the currently compiling chunk
 * @param byte The byte to emit
 */
static inline void emit_byte(uint8_t byte) {
    write_byte(s_parser.current_chunk, byte, s_parser.previous.line);
}

/**
 * Emits `count` bytes
 * @param count The number of bytes to emit
 * @param ... List of the bytes to emit
 */
static inline void emit_bytes(size_t count, ...) {
    va_list bytes;
    va_start(bytes, count);

    for (; count != 0; --count) {
        uint8_t byte = (uint8_t)va_arg(bytes, int);

        emit_byte(byte);
    }

    va_end(bytes);
}

/**
 * Writes a constant value to the chunk
 * @param constant The constant to write
 */
static inline void emit_constant(value constant) {
    write_constant(s_parser.current_chunk, constant, s_parser.previous.line);
}

/**
 * Returns the correct rule for a token_type
 * @param type The token_type to get the rule for
 * @return A parse_rule pointer
 */
static inline parse_rule *get_rule(token_type type) {
    return &rules[type];
}

/**
 * "Advances" the parser by consuming another token
 */
static void advance() {
    s_parser.previous = s_parser.current;

    while (true) {
        s_parser.current = scan();

        if (s_parser.current.type == TOKEN_ERROR) error_at_current(s_parser.current.tok_start);

        break;
    }
}

/**
 * Checks if the next token is of `type`, and if it isn't, returns
 * an error with `err` as the message.
 * @param type The type to match against
 * @param err The error to return if the next token is not of `type`
 */
static void consume(token_type type, const char *err) {
    if (s_parser.current.type != type) error_at_current(err);

    return advance();
}

static void parse_with_precedence(precedence p) {
    advance();

    parse_fn prefix_rule = get_rule(s_parser.previous.type)->prefix;

    if (prefix_rule == NULL) {
        error("Expected an expression.");
        return;
    }

    prefix_rule();

    while (p <= get_rule(s_parser.current.type)->precedence) {
        advance();

        get_rule(s_parser.previous.type)->infix();
    }
}

/**
 * Reads an expression and emits it as bytecode
 */
static void expression() {
    parse_with_precedence(PREC_ASSIGNMENT);
}

/**
 * Reads a number and emits it as a constant
 */
static void number() {
    double val = strtod(s_parser.previous.tok_start, NULL);
    emit_constant(number_value(val));
}

static void literal() {
    switch (s_parser.previous.type) {
        case TOKEN_NIL: emit_byte(OP_NIL); break;
        case TOKEN_TRUE: emit_byte(OP_TRUE); break;
        case TOKEN_FALSE: emit_byte(OP_FALSE); break;
        default: return;
    }
}

/**
 * Parses a unary expression
 */
static void unary() {
    token_type prev_type = s_parser.previous.type;

    parse_with_precedence(PREC_UNARY);

    switch (prev_type) {
        case TOKEN_MINUS: emit_byte(OP_NEGATE); break;
        case TOKEN_BANG: emit_byte(OP_NOT); break;
        default: return;
    }
}

/**
 * Parses a binary expression
 */
static void binary() {
    token_type op_type = s_parser.previous.type;

    parse_rule *r = get_rule(op_type);
    parse_with_precedence((precedence)(r->precedence + 1));

    switch (op_type) {
        case TOKEN_PLUS: emit_byte(OP_ADD); break;
        case TOKEN_MINUS: emit_byte(OP_SUBTRACT); break;
        case TOKEN_STAR: emit_byte(OP_MULTIPLY); break;
        case TOKEN_SLASH: emit_byte(OP_DIVIDE); break;
        case TOKEN_EQUAL_EQUAL: emit_byte(OP_EQUAL); break;
        case TOKEN_BANG_EQUAL: emit_bytes(2, OP_EQUAL, OP_NOT); break;
        case TOKEN_GREATER: emit_byte(OP_GREATER); break;
        case TOKEN_GREATER_EQUAL: emit_bytes(2, OP_LESS, OP_NOT); break;
        case TOKEN_LESS: emit_byte(OP_LESS); break;
        case TOKEN_LESS_EQUAL: emit_bytes(2, OP_GREATER, OP_NOT); break;
        default: break;
    }
}

/**
 * Parses a "grouping"
 */
static void grouping() {
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expected ')' after expression.");
}

bool compile(const char *source, chunk *c) {
    init_scanner(source);

    s_parser.had_err = false;
    s_parser.panic = false;
    s_parser.current_chunk = c;

    advance();
    expression();
    consume(TOKEN_EOF, "Expected end of expression");

    emit_byte(OP_RETURN);

#ifdef DEBUG_PRINT_CODE
    if (!s_parser.had_err) {
        disassemble_chunk(s_parser.current_chunk, "current state of chunk being scanned into");
    }
#endif

    return !s_parser.had_err;
}