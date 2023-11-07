// Lexer

#include "lexer.h"

#include <iostream>

#include "errors.h"

namespace helper {
bool isdigit(char c) { return '0' <= c && c <= '9'; }

bool isalpha(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == '_');
}

bool isalnum(char c) { return isalpha(c) || isdigit(c); }
}  // namespace helper

std::string Token::dump() {
    std::string dump = " [line " + std::to_string(line) + "] from col " +
                       std::to_string(starts_at) + " to col " +
                       std::to_string(ends_at) + " ";
    /* std::string dump_at_eof = " [line " + std::to_string(line - 1) +  "] to
     * the end of source code"; */
    /* if (tt != TOKEN_EOF) */
    return dump;
    /* return dump_at_eof; */
}

std::vector<struct Token> Lexer::lex() {
    while (!_at_end()) {
        _start_pos = _current_pos;
        _pos_line_start = _pos_line_current;
        _scan_token();
    }

    /* if(_error_in_lexer) { */
    /* 	/1* std::string error = "ERROR: " + _token_list.back().dump() + ": " +
     * msg; *1/ */
    /* 	LexerError err = LexerError("error"); */
    /* 	throw err; */
    /* } */

    struct Token tok_eof = {.tt = TOKEN_EOF,
                            .len = 0,
                            .starts_at = _source.size(),
                            .ends_at = _source.size(),
                            .line = _line,
                            .content = "EOF"};
    _token_list.push_back(tok_eof);
    return _token_list;
}

Lexer::Lexer(std::string source) {
    _current_pos = 0;
    _start_pos = 0;

    // position on the current line, not the source
    _pos_line_current = 0;
    _pos_line_start = 0;
    _line = 1;
    _source = source;

    _token_map["func"] = TOKEN_FUNC;
    _token_map["let"] = TOKEN_LET;
    _token_map["print"] = TOKEN_PRINT;
    _token_map["if"] = TOKEN_IF;
    _token_map["else"] = TOKEN_ELSE;
    _token_map["elif"] = TOKEN_ELIF;
    _token_map["for"] = TOKEN_FOR;
    _token_map["while"] = TOKEN_WHILE;
    _token_map["int"] = TOKEN_INT;
    _token_map["double"] = TOKEN_DOUBLE;
    _token_map["void"] = TOKEN_VOID;
    _token_map["bool"] = TOKEN_BOOL;
    _token_map["nil"] = TOKEN_NIL;
    _token_map["true"] = TOKEN_TRUE;
    _token_map["false"] = TOKEN_FALSE;
    _token_map["string"] = TOKEN_STRING;
}

char Lexer::_advance() {
    _pos_line_current++;
    _current_pos++;
    return _source[_current_pos - 1];
}

char Lexer::_peek() {
    if (_at_end()) {
        return '\0';
    }
    return _source[_current_pos];
}

char Lexer::_peek_next() {
    if (_current_pos + 1 >= _source.size()) return '\0';
    return _source[_current_pos + 1];
}

void Lexer::_add_token(enum TokenKind tt) {
    std::string content = _source.substr(_start_pos, _current_pos - _start_pos);

    struct Token token = {
        .tt = tt,
        .len = _current_pos - _start_pos,
        .starts_at = _pos_line_start,
        .ends_at = _pos_line_current,
        .line = _line,
        .content = content,
    };
    _token_list.push_back(token);
}

void Lexer::_skip_whitespaces() {
    for (;;) {
        switch (_peek()) {
            case ' ':
            case '\t':
            case '\r':
                _advance();
                break;
            case '\n':
                _pos_line_start = 0;
                _pos_line_current = 0;
                _line++;
                _advance();
                break;
            case '#':
                while (_peek() != '\n' && !_at_end()) _advance();
                break;
            default:
                return;
        }
    }
}

void Lexer::_number() {
    bool decimal = false;

    while (helper::isdigit(_peek())) _advance();

    if (_peek() == '.' && helper::isdigit(_peek_next())) {
        decimal = true;
        _advance();  // consume the '.'
        while (helper::isdigit(_peek())) _advance();
    }

    if (decimal)
        _add_token(TOKEN_DOUBLE);
    else
        _add_token(TOKEN_INT);
}

void Lexer::_string() {
    // starting '"' has alraedy been matched
    while (_peek() != '"' && !_at_end()) {
        _advance();
        if (_peek() == '\n') {
            _line++;
            /* _pos_line_current = 0; */
            /* _pos_line_start = 0; */
        }
    }
    if (_at_end()) {
        _error("Unterminated string");
        return;
    }
    _advance();  // consume the closing '"'
    _add_token(TOKEN_STRING);
}

void Lexer::_identifier() {
    while (helper::isalnum(_peek())) _advance();

    _identifier_type();
}

void Lexer::_identifier_type() {
    std::string identifier =
        _source.substr(_start_pos, _current_pos - _start_pos);

    if (_token_map.find(identifier) == _token_map.end()) {
        _add_token(TOKEN_ID);
    } else
        _add_token(_token_map[identifier]);
}

bool Lexer::_match(char expected) {
    /* std::cout << "\t" << expected << " " << int(expected ==
     * _source[_current_pos]) << std::endl; */
    if (_at_end()) return false;
    if (_source[_current_pos] != expected) {
        /* if (_source[_current_pos] == ' ') */
        /* std::cout << "NO MATCH" << " " <<  _source[_current_pos] <<
         * std::endl; */
        return false;
    }

    _pos_line_current++;
    _current_pos++;
    return true;
}

bool Lexer::_at_end() { return _current_pos >= _source.size(); }

void Lexer::_error(std::string msg) {
    _error_in_lexer = true;

    if (!_at_end()) {
        std::string error = "[line " + std::to_string(_line) + "] from col " +
                            std::to_string(_pos_line_start + 1) + " to col " +
                            std::to_string(_pos_line_current + 1) + ": " + msg;
        struct Diagnostic diagnostic = {
            .starts_at = _pos_line_start,
            .ends_at = _pos_line_current,
            .line = _line,
            .message = error,
        };
        _diagnostics.push_back(diagnostic);

    } else {
        std::string error = "[line " + std::to_string(_line) + "] from col " +
                            std::to_string(_pos_line_start + 1) +
                            " till the end of file" + ": " + msg;
        struct Diagnostic diagnostic = {
            .starts_at = _pos_line_start,
            .ends_at = _pos_line_current,
            .line = _line,
            .message = error,
        };
        _diagnostics.push_back(diagnostic);
    }
}

void Lexer::_scan_token() {
    /* _skip_whitespaces(); */
    char c = _advance();
    switch (c) {
        case '+':
            _add_token(TOKEN_PLUS);
            break;
        case '-':
            _add_token(TOKEN_MINUS);
            break;
        case '/':
            _add_token(TOKEN_SLASH);
            break;
        case '*':
            _add_token(TOKEN_STAR);
            break;
        case '!':
            _match('=') ? _add_token(TOKEN_BANG_EQUAL) : _add_token(TOKEN_BANG);
            break;
        case '=':
            _match('=') ? _add_token(TOKEN_EQUAL_EQUAL)
                        : _add_token(TOKEN_EQUAL);
            break;
        case '>':
            _match('=') ? _add_token(TOKEN_GREATER_EQUAL)
                        : _add_token(TOKEN_GREATER);
            break;
        case '<':
            _match('=') ? _add_token(TOKEN_LESS_EQUAL) : _add_token(TOKEN_LESS);
            break;
        case ':':
            _add_token(TOKEN_COLON);
            break;
        case ';':
            _add_token(TOKEN_SEMICOLON);
            break;
        case '"':
            _string();
            break;
        case '(':
            _add_token(TOKEN_LEFT_PAREN);
            break;
        case ')':
            _add_token(TOKEN_RIGHT_PAREN);
            break;
        case '{':
            _add_token(TOKEN_LEFT_CURLY);
            break;
        case '}':
            _add_token(TOKEN_RIGHT_CURLY);
            break;
        case ',':
            _add_token(TOKEN_COMMA);
            break;
        case ' ':
        case '\t':
        case '\r':
            /* _advance(); */
            break;
        case '\n':
            _pos_line_start = 0;
            _pos_line_current = 0;
            _line++;
            /* _advance(); */
            break;
        case '#':
            while (_peek() != '\n' && !_at_end()) _advance();
            break;
        default:
            if (helper::isdigit(c))
                _number();
            else if (helper::isalpha(c))
                _identifier();
            else {
                std::string err = "Unrecognised token in input: ";
                if (c == ' ')
                    err += "whitespace";
                else
                    err += c;
                _error(err);
            }
            break;
    }
}
