#include <scheme.h>

#include <string>

std::string Scheme::Evaluate(const std::string &expression) {
  std::stringstream s{expression};
  Tokenizer tokenizer{&s};

  root = Read(&tokenizer);

  if (!tokenizer.IsEnd()) {
    throw SyntaxError{"list syntax error"};
  }

  if (As<Cell>(root)) {
    auto list = SchemeList(root, &scope, &functions);
    return list.Evaluate();
  }

  if (auto temp = As<Number>(root)) {
    return std::to_string(temp->GetValue());
  }

  if (auto temp = As<Symbol>(root)) {
    if (temp->GetName() == "#t" or temp->GetName() == "#function") {
      return temp->GetName();
    }

    if (temp->IsQuoted()) {
      return temp->GetName();
    }

    if (scope.find(temp->GetName()) == scope.end()) {
      throw NameError{temp->GetName() + " was never defined"};
    }
    return scope[temp->GetName()];
  }

  return {" "};
}
