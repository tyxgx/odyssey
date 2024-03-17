#include "lib.hpp"

auto main() -> int
{
  auto const lib = library {};

  return lib.name == "odyssey" ? 0 : 1;
}
