#pragma once

#include "enum/HttpStatus.hpp"
#include <string>

std::string getSentenceResponseHttpStatus(HttpStatus::Code status);
void replaceAll(std::string& str, const std::string& from, const std::string& to);