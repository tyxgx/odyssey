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
    Operator(String),

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

        // Skip ws
        loop {
            // Call peek in its own scope, so that chars.next() after this does not displease the
            // borrow checker
            {
                let ch = chars.peek();
                if ch.is_none() {
                    // Encountered EOF
                    self.pos = pos;
                    return Ok(Token::EOF);
                }

                if *ch.unwrap() == '\n' {
                    self.pos = 0;
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
        }

        let start = pos;

        // advance
        let next = match chars.next() {
            None => return Ok(Token::EOF),
            Some(_) => chars.next(),
        };

        pos += 1;

        let _result: Result<Token, String> = match next.unwrap() {
            '/' => Ok(Token::Operator('/'.to_string())),
            '*' => Ok(Token::Operator('*'.to_string())),
            '(' => Ok(Token::Operator('('.to_string())),
            ']' => Ok(Token::Operator(']'.to_string())),
            '[' => Ok(Token::Operator('['.to_string())),
            '}' => Ok(Token::Operator('}'.to_string())),
            '{' => Ok(Token::Operator('{'.to_string())),
            ',' => Ok(Token::Operator(','.to_string())),
            '>' => {
                if self.expect_next('=') {
                    Ok(Token::Operator(">=".to_string()))
                } else {
                    Ok(Token::Operator('>'.to_string()))
                }
            }
            '<' => {
                if self.expect_next('=') {
                    Ok(Token::Operator("<=".to_string()))
                } else {
                    Ok(Token::Operator('<'.to_string()))
                }
            }
            '=' => {
                if self.expect_next('=') {
                    Ok(Token::Operator("==".to_string()))
                } else {
                    Ok(Token::Operator('='.to_string()))
                }
            }
            '!' => {
                if self.expect_next('=') {
                    Ok(Token::Operator("!=".to_string()))
                } else {
                    Ok(Token::Operator('!'.to_string()))
                }
            }
            '+' => {
                if self.expect_next('=') {
                    Ok(Token::Operator("+=".to_string()))
                } else {
                    Ok(Token::Operator('+'.to_string()))
                }
            }
            '-' => {
                if self.expect_next('=') {
                    Ok(Token::Operator("-=".to_string()))
                } else {
                    Ok(Token::Operator('-'.to_string()))
                }
            }
            '%' => Ok(Token::Operator('%'.to_string())),
            ';' => Ok(Token::Operator(';'.to_string())),
            '#' => {
                loop {
                    let ch = chars.next();
                    pos += 1;
                    if ch.unwrap() == '\n' {
                        break;
                    }
                }
                Ok(Token::Comment)
            }

            '.' | '0'..='9' => {
                loop {
                    let ch = match chars.peek() {
                        Some(x) => *x,
                        None => return Ok(Token::EOF),
                    };

                    if ch != '.' && !ch.is_ascii_hexdigit() {
                        break;
                    }

                    chars.next();
                    pos += 1;
                }
                Ok(Token::Number(src[start..pos].parse().unwrap()))
            }

            'a'..='z' | 'A'..='Z' => {
                loop {
                    let ch = match chars.peek() {
                        Some(x) => *x,
                        None => return Ok(Token::EOF),
                    };

                    if ch != '_' && !ch.is_alphanumeric() {
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
            _ => todo!(),
        };

        Err("SHIT HAPPENS".to_string())
    }

    fn expect_next(&mut self, expected: char) -> bool {
        let c = self.chars.deref_mut();
        match c.peek() {
            None => return false,
            Some(x) => return *x == expected,
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
