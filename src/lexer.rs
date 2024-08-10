use std::fmt::Display;
use std::ops::DerefMut;
use std::result::Result;
use std::{iter::Peekable, str::Chars};

#[derive(Debug)]
pub enum Token {
    Number(f64),
    // Str(String),
    Boolean(bool),

    // Operators and symbols
    OperatorOrSym(String),

    // Keywords
    For,
    While,
    And,
    Or,
    Func,
    Let,

    // Id
    Ident(String),

    // Types,
    Num,
    // String,
    Bool,
    Nil,

    // Comment
    Comment,

    // And, of course..
    EOF,
}

impl PartialEq for Token {
    fn eq(&self, other: &Self) -> bool {
        match (self, other) {
            (Token::Number(x), Token::Number(y)) => {
                return f64::abs(x - y) < 0.0001;
            }
            (Token::OperatorOrSym(lhs), Token::OperatorOrSym(rhs)) => {
                return lhs == rhs;
            }
            (Token::Ident(lhs), Token::Ident(rhs)) => {
                return lhs == rhs;
            }
            (Token::Boolean(lhs), Token::Boolean(rhs)) => {
                return lhs == rhs;
            }
            (rhs, lhs) => return rhs == lhs,
        }
    }
}

impl Eq for Token {}

pub struct LexError<'a> {
    pub error: &'a str,
    pub index: usize,
    pub start_index: usize,
    pub err_line: &'a str,
}

impl<'a> LexError<'a> {
    fn new(error: &'a str, err_line: &'a str) -> LexError<'a> {
        LexError {
            error,
            index: 0,
            start_index: 0,
            err_line,
        }
    }
    fn new_with_index(
        error: &'a str,
        index: usize,
        start_index: usize,
        err_line: &'a str,
    ) -> LexError<'a> {
        LexError {
            error,
            index,
            start_index,
            err_line,
        }
    }
}

impl Display for LexError<'_> {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let curr_line = self.err_line;
        let mut spaces = Vec::<char>::with_capacity(self.start_index);

        for _ in 0..=self.start_index + 2 {
            spaces.push(' ')
        }
        spaces.push('^');

        for _ in self.start_index + 1..self.index {
            spaces.push('~');
        }

        write!(
            f,
            "error: {}\n| {}\n{}",
            self.error,
            curr_line,
            spaces.iter().collect::<String>()
        )
    }
}

pub struct Lexer<'a> {
    per_line_pos: usize,
    pos: usize,
    input: &'a str,
    chars: Box<Peekable<Chars<'a>>>,
    line: usize,
    lines: Vec<&'a str>,
}

pub type LexResult<'a> = Result<Token, LexError<'a>>;

impl<'a> Lexer<'a> {
    pub fn new(src: &'a str) -> Self {
        Lexer {
            per_line_pos: 0,
            pos: 0,
            input: src,
            chars: Box::new(src.chars().peekable()),
            // lines are 0-indexed
            line: 0,
            lines: src.split('\n').collect::<Vec<_>>(),
        }
    }

    pub fn lex(&mut self) -> LexResult {
        let chars = self.chars.deref_mut();
        let mut pos = self.pos;
        let src = self.input;
        let mut plp = self.per_line_pos;

        // Skip ws
        loop {
            // Call peek in its own scope, so that chars.next() after
            // this does not displease the borrow checker.
            {
                let ch = chars.peek();
                if ch.is_none() {
                    // Encountered EOF
                    self.pos = pos;
                    return Ok(Token::EOF);
                }

                if *ch.unwrap() == '\n' {
                    plp = 0;
                    self.line += 1;
                    break;
                }

                // Otherwise, loop until a non-whitespace char is encountered
                if !ch.unwrap().is_ascii_whitespace() {
                    break;
                }
            }

            chars.next();
            pos += 1;
            plp += 1;
        }

        let start = pos;
        let pls = plp;

        // advance
        let next = chars.next();
        let nx = match next {
            None => return Ok(Token::EOF),
            Some(x) => x,
        };

        pos += 1;
        plp += 1;

        let result: Result<Token, LexError> = match nx {
            '/' => Ok(Token::OperatorOrSym('/'.to_string())),
            '*' => Ok(Token::OperatorOrSym('*'.to_string())),
            '(' => Ok(Token::OperatorOrSym('('.to_string())),
            ')' => Ok(Token::OperatorOrSym(')'.to_string())),
            ']' => Ok(Token::OperatorOrSym(']'.to_string())),
            '[' => Ok(Token::OperatorOrSym('['.to_string())),
            '}' => Ok(Token::OperatorOrSym('}'.to_string())),
            '{' => Ok(Token::OperatorOrSym('{'.to_string())),
            ',' => Ok(Token::OperatorOrSym(','.to_string())),
            '>' => {
                if self.expect_next('=') {
                    pos += 1;
                    Ok(Token::OperatorOrSym(">=".to_string()))
                } else {
                    Ok(Token::OperatorOrSym('>'.to_string()))
                }
            }
            '<' => {
                if self.expect_next('=') {
                    pos += 1;
                    Ok(Token::OperatorOrSym("<=".to_string()))
                } else {
                    Ok(Token::OperatorOrSym('<'.to_string()))
                }
            }
            '=' => {
                if self.expect_next('=') {
                    pos += 1;
                    Ok(Token::OperatorOrSym("==".to_string()))
                } else {
                    Ok(Token::OperatorOrSym('='.to_string()))
                }
            }
            '!' => {
                if self.expect_next('=') {
                    pos += 1;
                    Ok(Token::OperatorOrSym("!=".to_string()))
                } else {
                    Ok(Token::OperatorOrSym('!'.to_string()))
                }
            }
            '+' => {
                if self.expect_next('=') {
                    pos += 1;
                    Ok(Token::OperatorOrSym("+=".to_string()))
                } else {
                    Ok(Token::OperatorOrSym('+'.to_string()))
                }
            }
            '-' => {
                if self.expect_next('=') {
                    pos += 1;
                    Ok(Token::OperatorOrSym("-=".to_string()))
                } else {
                    Ok(Token::OperatorOrSym('-'.to_string()))
                }
            }
            '%' => Ok(Token::OperatorOrSym('%'.to_string())),
            ';' => Ok(Token::OperatorOrSym(';'.to_string())),
            '#' => {
                loop {
                    let ch = chars.next();
                    pos += 1;
                    plp += 1;
                    if ch.unwrap() == '\n' {
                        break;
                    }
                }
                Ok(Token::Comment)
            }

            '.' | '0'..='9' => {
                while let Some(ch) = chars.peek() {
                    if *ch != '.' && !ch.is_ascii_hexdigit() {
                        break;
                    }

                    chars.next();
                    pos += 1;
                }

                let interm = src[start..pos].trim_start().trim_end().parse();
                let ret = match interm {
                    Ok(x) => LexResult::Ok(Token::Number(x)),
                    Err(_) => Err(LexError::new_with_index(
                        "Failed to convert source slice to float",
                        plp,
                        pls,
                        self.lines.clone().get(self.line).unwrap(),
                    )),
                };
                ret
            }

            'a'..='z' | 'A'..='Z' => {
                while let Some(ch) = chars.peek() {
                    if *ch != '_' && !ch.is_alphanumeric() {
                        break;
                    }

                    chars.next();
                    pos += 1;
                }

                match &src[start..pos] {
                    "for" => Ok(Token::For),
                    "while" => Ok(Token::While),
                    "and" => Ok(Token::And),
                    "or" => Ok(Token::Or),
                    "let" => Ok(Token::Let),
                    "fn" => Ok(Token::Func),
                    "bool" => Ok(Token::Bool),
                    "true" => Ok(Token::Boolean(true)),
                    "false" => Ok(Token::Boolean(false)),
                    "nil" => Ok(Token::Nil),
                    "num" => Ok(Token::Num),
                    ident => Ok(Token::Ident(ident.to_string())),
                }
            }
            _ => Err(LexError::new_with_index(
                "illegal token in input stream",
                plp,
                pls,
                self.lines.clone().get(self.line).unwrap(),
            )),
        };
        self.pos = pos;
        result
    }

    fn expect_next(&mut self, expected: char) -> bool {
        let match_occurred: bool;
        {
            let c = self.chars.deref_mut();
            match_occurred = match c.peek() {
                None => false,
                Some(x) => *x == expected,
            }
        }

        if match_occurred {
            self.chars.next();
            match_occurred
        } else {
            match_occurred
        }
    }
}

impl<'a> Iterator for Lexer<'a> {
    type Item = Token;
    fn next(&mut self) -> Option<<Self as Iterator>::Item> {
        match self.lex() {
            Err(err) => {
                eprintln! {"{}", err}
                None
            }
            Ok(Token::EOF) => None,
            Ok(token) => Some(token),
        }
    }
}

#[cfg(test)]
mod tests {
    use super::Lexer;
    use super::Token::*;
    use super::*;

    #[test]
    fn single_char_oper() {
        let mut result = Lexer::new("1 + 1").collect::<Vec<Token>>();
        assert_eq!(
            result,
            [Number(1.0), OperatorOrSym("+".to_string()), Number(1.0)]
        );
        result = Lexer::new("1 / 1").collect::<Vec<Token>>();
        assert_eq!(
            result,
            [Number(1.0), OperatorOrSym("/".to_string()), Number(1.0)]
        );
        result = Lexer::new("1 + (1 + 1) / 2;").collect::<Vec<Token>>();
        assert_eq!(
            result,
            [
                Number(1.0),
                OperatorOrSym("+".to_string()),
                OperatorOrSym("(".to_string()),
                Number(1.0),
                OperatorOrSym("+".to_string()),
                Number(1.0),
                OperatorOrSym(")".to_string()),
                OperatorOrSym("/".to_string()),
                Number(2.0),
                OperatorOrSym(";".to_string())
            ]
        );
    }

    #[test]
    fn two_char_oper() {
        let mut result = Lexer::new("1 += 1").collect::<Vec<Token>>();
        assert_eq!(
            result,
            [Number(1.0), OperatorOrSym("+=".to_string()), Number(1.0)]
        );
        result = Lexer::new("1 == 1").collect::<Vec<Token>>();
        assert_eq!(
            result,
            [Number(1.0), OperatorOrSym("==".to_string()), Number(1.0)]
        );
        result = Lexer::new("1 >= 1").collect::<Vec<Token>>();
        assert_eq!(
            result,
            [Number(1.0), OperatorOrSym(">=".to_string()), Number(1.0)]
        )
    }

    #[test]
    fn float_num() {
        let result = Lexer::new("1 += 1.1111").collect::<Vec<Token>>();
        assert_eq!(
            result,
            [Number(1.0), OperatorOrSym("+=".to_string()), Number(1.1111)]
        )
    }

    #[test]
    fn misc() {
        let result = Lexer::new("1 += some_string").collect::<Vec<Token>>();
        assert_eq!(
            result,
            [
                Number(1.0),
                OperatorOrSym("+=".to_string()),
                Ident("some_string".to_string())
            ]
        );
        let result = Lexer::new("1 == some_string").collect::<Vec<Token>>();
        assert_eq!(
            result,
            [
                Number(1.0),
                OperatorOrSym("==".to_string()),
                Ident("some_string".to_string())
            ]
        );
    }
}
