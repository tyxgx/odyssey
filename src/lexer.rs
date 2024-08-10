use std::ops::DerefMut;
use std::result::Result;
use std::str::Split;
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

pub struct Lexer<'a> {
    per_line_pos: usize,
    pos: usize,
    input: &'a str,
    chars: Box<Peekable<Chars<'a>>>,
    line: isize,
    lines: Split<'a, char>,
}

pub type LexResult = Result<Token, String>;

impl<'a> Lexer<'a> {
    pub fn new(src: &'a str) -> Self {
        Lexer {
            per_line_pos: 0,
            pos: 0,
            input: src,
            chars: Box::new(src.chars().peekable()),
            // lines are 0-indexed
            line: 0,
            lines: src.split('\n'),
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

        // advance
        let next = chars.next();
        let nx = match next {
            None => return Ok(Token::EOF),
            Some(x) => x,
        };

        pos += 1;
        plp += 1;

        let result: Result<Token, String> = match nx {
            '/' => Ok(Token::OperatorOrSym('/'.to_string())),
            '*' => Ok(Token::OperatorOrSym('*'.to_string())),
            '(' => Ok(Token::OperatorOrSym('('.to_string())),
            ']' => Ok(Token::OperatorOrSym(']'.to_string())),
            '[' => Ok(Token::OperatorOrSym('['.to_string())),
            '}' => Ok(Token::OperatorOrSym('}'.to_string())),
            '{' => Ok(Token::OperatorOrSym('{'.to_string())),
            ',' => Ok(Token::OperatorOrSym(','.to_string())),
            '>' => {
                if self.expect_next('=') {
                    Ok(Token::OperatorOrSym(">=".to_string()))
                } else {
                    Ok(Token::OperatorOrSym('>'.to_string()))
                }
            }
            '<' => {
                if self.expect_next('=') {
                    Ok(Token::OperatorOrSym("<=".to_string()))
                } else {
                    Ok(Token::OperatorOrSym('<'.to_string()))
                }
            }
            '=' => {
                if self.expect_next('=') {
                    Ok(Token::OperatorOrSym("==".to_string()))
                } else {
                    Ok(Token::OperatorOrSym('='.to_string()))
                }
            }
            '!' => {
                if self.expect_next('=') {
                    Ok(Token::OperatorOrSym("!=".to_string()))
                } else {
                    Ok(Token::OperatorOrSym('!'.to_string()))
                }
            }
            '+' => {
                if self.expect_next('=') {
                    Ok(Token::OperatorOrSym("+=".to_string()))
                } else {
                    Ok(Token::OperatorOrSym('+'.to_string()))
                }
            }
            '-' => {
                if self.expect_next('=') {
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
                Ok(Token::Number(src[start..pos].parse().unwrap()))
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
            _ => Err("illegal token in input".to_string()),
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
            Err(_) | Ok(Token::EOF) => None,
            Ok(token) => Some(token),
        }
    }
}
