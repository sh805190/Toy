#pragma once

#include "interpreter.hpp"

#include <string>

void run(std::string source, Interpreter* interpreter, bool moduleFlag = false);
