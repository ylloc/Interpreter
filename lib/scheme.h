#pragma once

#include <string>
#include <sstream>
#include "parser.h"
#include "scheme_list.h"

class Scheme {
public:
  std::string Evaluate(const std::string& expression);

private:
  std::unordered_map<std::string, std::vector<Func>> functions;
  std::shared_ptr<Object> root;
  std::unordered_map<std::string, std::string> scope;
};
