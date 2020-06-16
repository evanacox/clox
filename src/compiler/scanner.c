#include "scanner.h"
#include "../common/common.h"
#include <stdio.h>
#include <string.h>

typedef struct scanner {
    /** Starting point for one token */
    const char *start;

    /** The current character the scanner is on */
    const char *current;

    /** The current line the scanner is on */
    size_t line;
} scanner;

static const char *current;
static scanner s_scanner;

/**
 * Consumes and returns a character
 * @return The consumed character
 */
static inline char advance() {
    return *s_scanner.current++;
}

/**
 * "Peeks" at the current character
 * @return The current character
 */
static inline char peek() {
    return *s_scanner.current;
}

/**
 * Returns whether or not the scanner is at the end of the file
 * @return Whether the current character is EOF
 */
static inline bool is_end() {
    return peek() == '\0';
}

/**
 * Returns the next character
 * @return The character after the current one
 */
static inline char peek_next() {
    if (is_end()) return '\0';

    return s_scanner.current[1];
}

/**
 * Checks if a character is a digit
 * @param c The char to check
 * @return Whether the char is 0-9
 */
static inline bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

/**
 * Checks if the current char is equal to the one passed
 * @param c The character to match against
 * @return Whether the current character is equal
 */
static inline bool match(char c) {
    if (peek() != c) { return false; }

    ++s_scanner.current;
    return true;
}

/**
 * Returns whether or not the character is alphabetical
 * @param c The character to check
 * @return Whether it matches [a-zA-Z_]
 */
static inline bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}

/**
 * Makes a token from the type passed
 * @param type The type to make the token as
 * @return A token object
 */
static inline token make_tok(token_type type) {
    token tok = {
        .type = type,
        .line = s_scanner.line,
        .tok_start = s_scanner.start,
        .len = (int)(s_scanner.current - s_scanner.start),
    };

    return tok;
}

/**
 * Makes an "error" token using the message passed
 * @param message The error message
 * @return A token object
 */
static inline token error_token(const char *message) {
    token tok = {
        .type = TOKEN_ERROR,
        .line = s_scanner.line,
        .tok_start = message,
        .len = (int)(strlen(message)),
    };

    return tok;
}

/**
 * Checks if the rest of an identifier is equal to a keyword
 * @param start Position of second character
 * @param len Length of the identifier
 * @param rest Pointer to the string to compare to
 * @param type Type to return if they are equal
 * @return type if the identifier IS the keyword, TOKEN_IDENTIFIER otherwise
 */
static inline token_type check_keyword(int start, int len, const char *rest, token_type type) {
    return s_scanner.current - s_scanner.start == start + len &&
                   memcmp(s_scanner.start + start, rest, len) == 0
               ? type
               : TOKEN_IDENTIFIER;
}

/**
 * Determines if an identifier is a keyword or not
 * @return The correct token type for an identifier
 */
static inline token_type identifier_type() {
    switch (s_scanner.start[0]) {
        case 'a': return check_keyword(1, 2, "nd", TOKEN_AND);
        case 'c': return check_keyword(1, 4, "lass", TOKEN_CLASS);
        case 'e': return check_keyword(1, 3, "lse", TOKEN_ELSE);
        case 'i': return check_keyword(1, 1, "f", TOKEN_IF);
        case 'n': return check_keyword(1, 2, "il", TOKEN_NIL);
        case 'o': return check_keyword(1, 1, "r", TOKEN_OR);
        case 'p': return check_keyword(1, 4, "rint", TOKEN_PRINT);
        case 'r': return check_keyword(1, 5, "eturn", TOKEN_RETURN);
        case 's': return check_keyword(1, 4, "uper", TOKEN_SUPER);
        case 'v': return check_keyword(1, 2, "ar", TOKEN_VAR);
        case 'w': return check_keyword(1, 4, "hile", TOKEN_WHILE);
        case 'f': {
            if (s_scanner.current - s_scanner.start > 1) {
                switch (s_scanner.start[1]) {
                    case 'a': return check_keyword(2, 3, "lse", TOKEN_FALSE);
                    case 'o': return check_keyword(2, 1, "r", TOKEN_FOR);
                    case 'u': return check_keyword(2, 1, "n", TOKEN_FUN);
                }
            }
            break;
        }
        case 't': {
            if (s_scanner.current - s_scanner.start > 1) {
                switch (s_scanner.start[1]) {
                    case 'h': return check_keyword(2, 2, "is", TOKEN_THIS);
                    case 'r': return check_keyword(2, 2, "ue", TOKEN_TRUE);
                }
            }
            break;
        }
    }

    return TOKEN_IDENTIFIER;
}

/**
 * Consumes all whitespace up until useful characters for each token
 */
static void consume_whitespace() {
    while (true) {
        switch (peek()) {
            case ' ':
            case '\t':
            case '\r': advance(); break;
            case '\n':
                ++s_scanner.line;
                advance();
                break;
            case '/': {
                if (peek_next() != '/') { return; }
                while (peek() != '\n' && !is_end()) {
                    advance();
                }
                break;
            }
            default: return;
        }
    }
}

/**
 * Consumes a string and returns it as a token
 * @return The string token
 */
static token string() {
    while (peek() != '"' && !is_end()) {
        if (peek() == '\n') { ++s_scanner.line; }

        advance();
    }

    if (is_end()) { return error_token("Unterminated string."); }

    advance();
    return make_tok(TOKEN_STRING);
}

/**
 * Consumes a number literal and returns a token for it
 * @return The number token
 */
static token number() {
    while (is_digit(peek())) {
        advance();
    }

    if (peek() == '.' && is_digit(peek_next())) {
        advance();

        while (is_digit(peek())) {
            advance();
        }
    }

    return make_tok(TOKEN_NUMBER);
}

/**
 * Consumes an identifier and returns a token for it
 * @return The identifier token
 */
static token identifier() {
    while (is_alpha(peek()) || is_digit(peek())) {
        advance();
    }

    return make_tok(identifier_type());
}

void init_scanner(const char *source) {
    s_scanner.start = source;
    s_scanner.current = s_scanner.start;
    s_scanner.line = 1;
}

token scan() {
    consume_whitespace();

    s_scanner.start = s_scanner.current;

    if (is_end()) return make_tok(TOKEN_EOF);

    char c = advance();

    if (is_digit(c)) return number();
    if (is_alpha(c)) return identifier();

    switch (c) {
        case '(': return make_tok(TOKEN_LEFT_PAREN);
        case ')': return make_tok(TOKEN_RIGHT_PAREN);
        case '{': return make_tok(TOKEN_LEFT_BRACE);
        case '}': return make_tok(TOKEN_RIGHT_BRACE);
        case ';': return make_tok(TOKEN_SEMICOLON);
        case ',': return make_tok(TOKEN_COMMA);
        case '.': return make_tok(TOKEN_DOT);
        case '-': return make_tok(TOKEN_MINUS);
        case '+': return make_tok(TOKEN_PLUS);
        case '/': return make_tok(TOKEN_SLASH);
        case '*': return make_tok(TOKEN_STAR);
        case '!': return make_tok(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=': return make_tok(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '<': return make_tok(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>': return make_tok(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '"': return string();
        default: break;
    }

    return error_token("Unexpected character.");
}