#pragma once

#include "enum/HttpStatus.hpp"
#include "Logger.hpp"
#include <string>
#include <stdio.h>
#include <dirent.h>

std::string getSentenceResponseHttpStatus(HttpStatus::Code status);
void replaceAll(std::string& str, const std::string& from, const std::string& to);
const std::string& generateAutoindexPage(std::string path);