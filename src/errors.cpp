#include "../include/errors.h"

const char* Error::what() const noexcept { return what_.c_str(); }
