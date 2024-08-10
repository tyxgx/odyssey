mod lexer;

fn main() {
    let mut first_arg = true;
    for arg in std::env::args() {
        if first_arg {
            first_arg = false;
            continue;
        }
        match arg.as_str() {
            "--help" => {
                eprintln! {"Welcome to the Odyssey compiler, V0.0.1."}
                eprintln! {"Usage: ./odc <file_name> [args]*"}
                return
            }
            _ => {
                eprintln! {"Invalid arguments to the compiler, please use '--help' to get usage"}
                return
            }
        }
    }

   let token_list = lexer::Lexer::new("1 / str").collect::<Vec<lexer::Token>>();
   eprintln!{"{:?}", token_list}
}
