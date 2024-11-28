#include <iostream>
#include <sstream>

#include <scheme_list.h>
#include <error.h>
#include <tokenizer.h>
#include <parser.h>

int GetValue(const std::shared_ptr<Object> &obj, std::unordered_map<std::string, std::string> *scope) {
  int ans;
  if (auto temp = As<Number>(obj)) {
    ans = temp->GetValue();
  }
  if (auto temp = As<Symbol>(obj)) {
    if (temp->GetName() == "#t" or temp->GetName() == "#function") {
      throw RuntimeError{"Not integer arg"};
    }
    if (!temp->IsQuoted()) {
      ans = std::stoi((*scope)[temp->GetName()]);
    } else {
      throw NameError{"NameError"};
    }
  }
  return ans;
}
bool IsNumber(const std::string &str) {
  if (str.empty()) {
    return false;
  }
  for (size_t ind = 0; ind < str.size(); ++ind) {
    if (ind != 0) {
      if (str[ind] < '0') {
        return false;
      }
      if (str[ind] > '9') {
        return false;
      }
    } else {
      if (str[ind] == '+') {
        continue;
      }
      if (str[ind] == '-') {
        continue;
      }
      if (str[ind] < '0') {
        return false;
      }
      if (str[ind] > '9') {
        return false;
      }
    }
  }
  return true;
}

bool IsPair(const std::string &str) {
  std::stringstream s{str};
  Tokenizer tokenizer{&s};
  auto root = Read(&tokenizer);
  if (auto temp = As<Cell>(root)) {
    if (!temp->GetFirst()) {
      return false;
    }
    return true;
  }
  return false;
}

std::shared_ptr<Cell> AsPair(const std::string &str) {
  std::stringstream s{str};
  Tokenizer tokenizer{&s};
  auto root = Read(&tokenizer);
  return As<Cell>(root);
}

bool IsList(std::string str) {
  std::stringstream s{str};
  Tokenizer tokenizer{&s};
  auto root = Read(&tokenizer);
  if (auto temp = As<Cell>(root)) {
    auto list = SchemeList(temp);
    if (list.IsPoint()) {
      return false;
    }
    return true;
  }
  return false;
}

SchemeList AsList(const std::string &str) {
  std::stringstream s{str};
  Tokenizer tokenizer{&s};
  auto root = Read(&tokenizer);
  if (auto temp = As<Cell>(root)) {
    auto list = SchemeList(temp);
    if (list.IsPoint()) {
      return SchemeList();
    }
    return list;
  }
  return SchemeList();
}

std::vector<Func> GetFunc(const std::shared_ptr<Cell> &root) {
  std::vector<std::string> args;

  auto list = SchemeList(root);
  auto args_ptr = As<Cell>(list[1]);
  auto arg_list = SchemeList(args_ptr);

  for (int ind = 0; ind < arg_list.Size(); ind++) {
    auto arg = As<Symbol>(arg_list[ind]);
    if (!arg) {
      throw SyntaxError{"bad args def"};
    }
    args.push_back(arg->GetName());
  }
  std::vector<Func> ans;
  for (int ind = 2; ind < list.Size(); ind++) {
    auto temp = As<Cell>(list[ind]);
    if (!temp) {
      throw RuntimeError{"gg"};
    }
    ans.emplace_back(Func(temp, args));
  }
  return ans;
}

std::string SchemeList::IntEval(std::string oper) {
  BinaryFunction op;
  int ans = 1;
  size_t ind = 1;
  int prev = 0;

  if (oper == "+") {
    op = {oper, [](int a, int b) { return a + b; }};
    ans = 0;
  }

  if (oper == "-") {
    op = {oper, [](int a, int b) { return a - b; }};
    if (size_ <= 2) {
      throw RuntimeError{"RuntimeError"};
    }
    ans = GetValue((*this)[1], scope_);
    ind = 2;
  }

  if (oper == "*") {
    op = {oper, [](int a, int b) { return a * b; }};
    ans = 1;
  }

  if (oper == "/") {
    op = {oper, [](int a, int b) { return a / b; }};
    if (size_ <= 2) {
      throw RuntimeError{"RuntimeError"};
    }
    ans = GetValue((*this)[1], scope_);
    ind = 2;
  }

  if (oper == "max") {
    op = {oper, [](int a, int b) { return std::max(a, b); }};
    ans = INT32_MIN;
    if (size_ <= 1) {
      throw RuntimeError{"RuntimeError"};
    }
  }

  if (oper == "min") {
    op = {oper, [](int a, int b) { return std::min(a, b); }};
    ans = INT32_MAX;
    if (size_ <= 1) {
      throw RuntimeError{"RuntimeError"};
    }
  }

  if (oper == "abs") {
    op = {oper, [](int a, int b) { return std::abs(b + a * 0); }};
    if (size_ != 2) {
      throw RuntimeError{"RuntimeError"};
    }
  }

  if (oper == "=") {
    if (size_ <= 2) {
      return "#t";
    }
    prev = GetValue((*this)[1], scope_);
    op = {oper, [](int a, int b) { return a == b ? 1 : 0; }, true};
    ind = 2;
  }

  if (oper == "<") {
    if (size_ <= 2) {
      return "#t";
    }
    prev = GetValue((*this)[1], scope_);
    op = {oper, [](int a, int b) { return a < b ? 1 : 0; }, true};
    ind = 2;
  }

  if (oper == ">") {
    if (size_ <= 2) {
      return "#t";
    }
    prev = GetValue((*this)[1], scope_);
    op = {oper, [](int a, int b) { return a > b ? 1 : 0; }, true};
    ind = 2;
  }

  if (oper == "<=") {
    if (size_ <= 2) {
      return "#t";
    }
    prev = GetValue((*this)[1], scope_);
    op = {oper, [](int a, int b) { return a <= b ? 1 : 0; }, true};
    ind = 2;
  }

  if (oper == ">=") {
    if (size_ <= 2) {
      return "#t";
    }
    prev = GetValue((*this)[1], scope_);
    op = {oper, [](int a, int b) { return a >= b ? 1 : 0; }, true};
    ind = 2;
  }

  if (oper == "number?") {
    ind = 1;
    while ((*this)[ind]) {
      if (auto temp = As<Number>((*this)[ind])) {
        ind++;
        continue;
      }

      if (auto temp = As<Cell>((*this)[ind])) {
        auto new_list = SchemeList(temp, scope_, func_);
        if (!std::regex_match(new_list.Evaluate(), std::regex("[(-|+)|][0-9]+"))) {
          return "#function";
        }
        ind++;
        continue;
      }

      return "#function";
    }
    return "#t";
  }

  if (!op.is_bool) {
    while ((*this)[ind]) {
      if (auto temp = As<Number>((*this)[ind])) {
        ans = op(ans, temp->GetValue());
        ind++;
        continue;
      }

      if (auto temp = As<Cell>((*this)[ind])) {
        auto new_list = SchemeList(temp, scope_, func_);
        ans = op(ans, std::stoi(new_list.Evaluate()));
        ind++;
        continue;
      }

      if (auto temp = As<Symbol>((*this)[ind])) {
        if (temp->GetName() == "#t" or temp->GetName() == "#function") {
          throw RuntimeError{"Not integer arg"};
        }
        if (scope_->find(temp->GetName()) == scope_->end()) {
          throw NameError{"Unknown variable " + temp->GetName()};
        }
        ans = op(ans, std::stoi((*scope_)[temp->GetName()]));
        ind++;
        continue;
      }
      throw RuntimeError{"RuntimeError"};
    }
    return std::to_string(ans);
  } else {
    while ((*this)[ind]) {
      if (auto temp = As<Number>((*this)[ind])) {
        ans &= op(prev, temp->GetValue());
        prev = temp->GetValue();
        ind++;
        continue;
      }

      if (auto temp = As<Cell>((*this)[ind])) {
        auto new_list = SchemeList(temp, scope_, func_);
        ans &= op(prev, std::stoi(new_list.Evaluate()));
        prev = std::stoi(new_list.Evaluate());
        ind++;
        continue;
      }

      if (auto temp = As<Symbol>((*this)[ind])) {
        if (temp->GetName() == "#t" or temp->GetName() == "#function") {
          throw RuntimeError{"Not integer arg"};
        }
        if (scope_->find(temp->GetName()) == scope_->end()) {
          throw NameError{"Unknown variable " + temp->GetName()};
        }

        ans &= op(ans, std::stoi((*scope_)[temp->GetName()]));
        prev = std::stoi((*scope_)[temp->GetName()]);
        ind++;
        continue;
      }

      throw RuntimeError{"RuntimeError"};
    }
    return ans == 1 ? "#t" : "#function";
  }
}

std::string SchemeList::BoolEval(std::string oper) {
  if (oper == "boolean?") {
    if (size_ != 2) {
      throw RuntimeError{"Wrong arg number for boolean?"};
    }
    auto val = (*this)[1];
    if (auto temp = As<Symbol>(val)) {
      if (temp->IsQuoted()) {
        return "#function";
      }
      if (temp->GetName() == "#function" or temp->GetName() == "#t") {
        return "#t";
      }
    }
    if (auto temp = As<Cell>(val)) {
      auto new_list = SchemeList(temp, scope_, func_);
      auto new_val = new_list.Evaluate();
      if (new_val == "#function" or new_val == "#t") {
        return "#t";
      }
    }
    return "#function";
  }

  if (oper == "not") {
    if (size_ != 2) {
      throw RuntimeError{"Wrong arg number for not"};
    }
    auto val = (*this)[1];
    if (auto temp = As<Symbol>(val)) {
      if (temp->IsQuoted()) {
        return "#function";
      }
      if (temp->GetName() == "#function") {
        return "#t";
      }
    }

    if (auto temp = As<Cell>(val)) {
      auto new_list = SchemeList(temp, scope_, func_);
      auto new_val = new_list.Evaluate();
      if (new_val == "#function") {
        return "t";
      }
    }
    return "#function";
  }

  if (oper == "and") {
    if (size_ == 1) {
      return "#t";
    }
    int ind = 1;
    std::string last;
    while ((*this)[ind]) {
      if (auto temp = As<Symbol>((*this)[ind])) {
        if (temp->IsQuoted() || temp->GetName() != "#function") {
          last = temp->GetName();
        } else {
          return "#function";
        }
      }

      if (auto temp = As<Cell>((*this)[ind])) {
        auto list = SchemeList(temp, scope_, func_);
        auto val = list.Evaluate();
        if (val == "#function") {
          return "#function";
        } else {
          last = val;
        }
      }
      ind++;
    }
    return last;
  }

  if (oper == "or") {
    if (size_ == 1) {
      return "#function";
    }
    int ind = 1;
    std::string last;
    while ((*this)[ind]) {
      if (auto temp = As<Symbol>((*this)[ind])) {
        if (!temp->IsQuoted() && temp->GetName() == "#t") {
          return "#t";
        } else {
          last = temp->GetName();
        }
      }

      if (auto temp = As<Cell>((*this)[ind])) {
        auto list = SchemeList(temp, scope_, func_);
        auto val = list.Evaluate();
        if (val == "#t") {
          return "#t";
        } else {
          last = val;
        }
      }

      if (auto temp = As<Number>((*this)[ind])) {
        last = std::to_string(temp->GetValue());
      }
      ind++;
    }
    return last;
  }

  return " ";
}

std::string SchemeList::QuoteEval() {
  std::string ans;
  size_t ind = 0;
  if (size_ != 2) {
    throw SyntaxError("Wrong arg number for quote");
  }
  auto obj = (*this)[1];
  if (auto tmp = As<Symbol>(obj)) {
    return tmp->GetName();
  }
  if (auto tmp = As<Symbol>(obj)) {
    return tmp->GetName();
  }
  if (auto tmp = As<Cell>(obj)) {
    auto new_list = SchemeList(tmp, scope_, func_);
    size_t new_ind = 0;
    while (new_list[new_ind]) {
      if (auto temp = As<Symbol>(new_list[new_ind])) {
        ans += temp->GetName();
      }
      if (auto temp = As<Number>(new_list[new_ind])) {
        ans += std::to_string(temp->GetValue());
      }
      if (auto temp = As<Cell>(new_list[new_ind])) {
        ans += SchemeList(temp, scope_, func_).RawQuoteEval();
      }
      if (new_list[++new_ind]) {
        if (!new_list[new_ind + 1] && new_list.point_) {
          ans += " .";
        }
        ans += " ";
      }
    }
  }
  return "(" + ans + ")";
}

std::string SchemeList::RawQuoteEval() {
  std::string ans;
  size_t ind = 0;
  auto obj = (*this)[0];

  while ((*this)[ind]) {
    if (auto temp = As<Symbol>((*this)[ind])) {
      ans += temp->GetName();
    }
    if (auto temp = As<Number>((*this)[ind])) {
      ans += std::to_string(temp->GetValue());
    }
    if (auto temp = As<Cell>((*this)[ind])) {
      ans += SchemeList(temp, scope_, func_).RawQuoteEval();
    }
    if ((*this)[++ind]) {
      if (!(*this)[ind + 1] && (*this).point_) {
        ans += " .";
      }
      ans += " ";
    }
  }

  return "(" + ans + ")";
}

std::string SchemeList::SymbolEval() {
  if (size_ != 2) {
    throw SyntaxError{"Wrong arg number for symbol?"};
  }

  if (auto temp = As<Symbol>((*this)[1])) {
    if (temp->IsQuoted() && temp->GetName()[0] != '\'') {
      return "#t";
    }
  }
  return "#function";
}

std::string SchemeList::FuncEval(std::string oper) {
  if (oper == "define") {
    if (auto var = As<Symbol>((*this)[1])) {
      if (size_ != 3) {
        throw SyntaxError{"Wrong arg number for define"};
      }
      if (auto val = As<Number>((*this)[2])) {
        (*scope_)[var->GetName()] = std::to_string(val->GetValue());
        return "";
      }
      if (auto val = As<Symbol>((*this)[2])) {
        if (IsNumber(val->GetName()) || val->IsQuoted()) {
          (*scope_)[var->GetName()] = val->GetName();
          return "";
        }
      }
      if (auto val = As<Cell>((*this)[2])) {
        auto list = SchemeList(val, scope_, func_);
        if (As<Symbol>(list[0])->GetName() == "lambda") {
          (*func_)[var->GetName()] = GetFunc(val);
          return "";
        }
        if (func_->find(As<Symbol>(list[0])->GetName()) != func_->end()) {
          auto funcs = (*func_)[As<Symbol>(list[0])->GetName()];

          auto args = funcs[0].arg_names_;
          auto dict = funcs[0].dict_;
          for (int ind = 0; ind < args.size(); ++ind) {
            dict[args[ind]] = std::to_string(As<Number>((list[ind + 1]))->GetValue());
          }
          for (auto &func: funcs) {
            func.dict_ = dict;
          }
          (*func_)[var->GetName()] = funcs;
        }
        auto new_val = list.Evaluate();
        if (IsNumber(new_val)) {
          (*scope_)[var->GetName()] = new_val;
          return "";
        }
      }
    }
    if (auto temp = As<Cell>((*this)[1])) {
      auto list = SchemeList(temp, scope_, func_);
      std::string name = As<Symbol>(list[0])->GetName();
      std::vector<std::string> args;
      for (int ind = 1; ind < list.size_; ind++) {
        args.push_back(As<Symbol>(list[ind])->GetName());
      }
      std::vector<Func> funcs;
      for (int ind = 2; ind < size_; ind++) {
        if (auto root = As<Cell>((*this)[ind])) {
          funcs.push_back(Func(root, args));
        }
        if (auto tmp = As<Number>((*this)[ind])) {
          auto root = std::make_shared<Cell>();
          auto plus = std::make_shared<Symbol>("+");
          auto x = std::make_shared<Number>(As<Number>((*this)[ind])->GetValue());
          auto zero = std::make_shared<Number>(0);
          auto tmp_root = std::make_shared<Cell>();
          tmp_root->SetFirst(x);
          tmp_root->SetSecond(zero);
          root->SetFirst(plus);
          root->SetSecond(tmp_root);
          funcs.push_back(Func(root, args));
        }
      }
      (*func_)[name] = funcs;
      return "";
    }
    throw SyntaxError{"Wrong define end"};
  }

  if (oper == "set!") {
    if (size_ != 3) {
      throw SyntaxError{"Wrong arg number for set!"};
    }
    if (!As<Symbol>((*this)[1])) {
      throw RuntimeError{"Wrong arg set!"};
    }
    auto name = As<Symbol>((*this)[1])->GetName();
    if (scope_->find(name) == scope_->end()) {
      throw NameError{"Arg for set! was never defined"};
    }
    if (auto val = As<Number>((*this)[2])) {
      (*scope_)[name] = std::to_string(val->GetValue());
      return "";
    }
    if (auto val = As<Symbol>((*this)[2])) {
      if (IsNumber(val->GetName()) || val->IsQuoted()) {
        (*scope_)[name] = val->GetName();
        return "";
      }
    }
    if (auto val = As<Cell>((*this)[2])) {
      auto list = SchemeList(val, scope_, func_);
      auto new_val = list.Evaluate();
      if (IsNumber(new_val)) {
        (*scope_)[name] = new_val;
        return "";
      }
    }
    throw RuntimeError("Wrong set!");
  }
  return " ";
}

std::string SchemeList::ControlEval() {
  if (size_ > 4 or size_ < 3) {
    throw SyntaxError{"Wrong arg number for if block"};
  }

  auto temp = (*this)[1];
  bool if_val = false;
  if (auto statement = As<Symbol>(temp)) {
    if (statement->IsQuoted()) {
      throw SyntaxError{"Not boolean statement for if block"};
    }
    if (statement->GetName() == "#t") {
      if_val = true;
    } else if (statement->GetName() == "#function") {
      if_val = false;
    } else {
      throw SyntaxError{"Not boolean statement for if block"};
    }
  }

  if (auto statement = As<Cell>(temp)) {
    auto list = SchemeList(statement, scope_, func_);
    std::string ans = list.Evaluate();
    if (ans == "#t") {
      if_val = true;
    } else if (ans == "#function") {
      if_val = false;
    } else {
      throw SyntaxError{"Not boolean statement for if block"};
    }
  }

  int ind = if_val ? 2 : 3;
  if (ind == size_) {
    return "()";
  }
  auto temp_val = (*this)[ind];

  if (auto val = As<Number>(temp_val)) {
    return std::to_string(val->GetValue());
  }

  if (auto val = As<Symbol>(temp_val)) {
    if (val->IsQuoted()) {
      return val->GetName();
    }
    if (scope_->find(val->GetName()) == scope_->end()) {
      throw NameError{val->GetName() + " was never declared"};
    }
    return (*scope_)[val->GetName()];
  }

  if (auto val = As<Cell>(temp_val)) {
    auto list = SchemeList(val, scope_, func_);
    std::string ans = list.Evaluate();
    return ans;
  }

  return " ";
}

std::string SchemeList::ListEval(std::string oper) {
  if (oper == "pair?") {
    if (size_ != 2) {
      throw SyntaxError{"Wrong arg number for pair?"};
    }

    if (!As<Symbol>((*this)[1])) {
      return "#function";
    }

    auto arg = As<Symbol>((*this)[1]);
    std::string temp = " ";
    if (!arg->IsQuoted()) {
      if (arg->GetName() == "#t" or arg->GetName() == "#function") {
        return "#function";
      }
      if (scope_->find(arg->GetName()) == scope_->end()) {
        throw NameError{arg->GetName() + " was never declared"};
      }
      temp = (*scope_)[arg->GetName()];
    } else {
      temp = arg->GetName();
    }
    return IsPair(temp) ? "#t" : "#function";
  }

  if (oper == "null?") {
    if (size_ != 2) {
      throw SyntaxError{"Wrong arg number for null?"};
    }
    if (!As<Symbol>((*this)[1])) {
      return "#function";
    }

    auto arg = As<Symbol>((*this)[1]);
    std::string temp = " ";
    if (!arg->IsQuoted()) {
      if (arg->GetName() == "#t" or arg->GetName() == "#function") {
        return "#function";
      }
      if (scope_->find(arg->GetName()) == scope_->end()) {
        throw NameError{arg->GetName() + " was never declared"};
      }
      temp = (*scope_)[arg->GetName()];
    } else {
      temp = arg->GetName();
    }
    return temp == "()" ? "#t" : "#function";
  }

  if (oper == "list?") {
    if (size_ != 2) {
      throw SyntaxError{"Wrong arg number for pair?"};
    }

    if (!As<Symbol>((*this)[1])) {
      return "#function";
    }

    auto arg = As<Symbol>((*this)[1]);
    std::string temp = " ";
    if (!arg->IsQuoted()) {
      if (arg->GetName() == "#t" or arg->GetName() == "#function") {
        return "#function";
      }
      if (scope_->find(arg->GetName()) == scope_->end()) {
        throw NameError{arg->GetName() + " was never declared"};
      }
      temp = (*scope_)[arg->GetName()];
    } else {
      temp = arg->GetName();
    }
    return IsList(temp) ? "#t" : "#function";
  }

  return " ";
}

std::string SchemeList::PairEval(std::string oper) {
  std::string first, second;
  if (oper == "cons") {
    if (size_ != 3) {
      throw SyntaxError{"Wrong arg number for cons"};
    }
    if (auto temp = As<Number>((*this)[1])) {
      first = std::to_string(temp->GetValue());
    }
    if (auto temp = As<Number>((*this)[2])) {
      second = std::to_string(temp->GetValue());
    }
    return "(" + first + " . " + second + ")";
  }
  if (oper == "car" || oper == "cdr") {
    int ind = oper == "car" ? 0 : 1;
    if (size_ != 2) {
      throw SyntaxError{"Wrong arg number for car"};
    }
    auto pair = (*this)[1];
    if (auto temp = As<Symbol>(pair)) {
      if (temp->IsQuoted()) {
        auto ls = SchemeList(AsPair(temp->GetName()));
        if (auto ans = As<Symbol>(ls[ind])) {
          return ans->GetName();
        }
        if (auto ans = As<Number>(ls[ind])) {
          return std::to_string(ans->GetValue());
        }
      }
      if (scope_->find(temp->GetName()) == scope_->end()) {
        throw NameError{temp->GetName() + " was never declared"};
      }
      auto ls = SchemeList(AsPair((*scope_)[temp->GetName()]));
      if (auto ans = As<Symbol>(ls[ind])) {
        return ans->GetName();
      }
      if (auto ans = As<Number>(ls[ind])) {
        return std::to_string(ans->GetValue());
      }
    }
  }

  if (oper == "set-car!" or oper == "set-cdr!") {
    int ind = oper == "set-car!" ? 0 : 1;
    if (size_ != 3) {
      throw SyntaxError{"Wrong arg number for set-car!"};
    }
    auto pair = (*this)[1];

    if (auto temp = As<Symbol>(pair)) {
      if (temp->IsQuoted()) {
        throw RuntimeError{"..."};
      }
      if (scope_->find(temp->GetName()) == scope_->end()) {
        throw NameError{temp->GetName() + " was never declared"};
      }
      auto new_pair = AsPair((*scope_)[temp->GetName()]);
      if (ind == 0) {
        new_pair->SetFirst((*this)[2]);
      }
      if (ind == 1) {
        new_pair->SetSecond((*this)[2]);
      }
      auto ls = SchemeList(new_pair);
      (*scope_)[temp->GetName()] = ls.RawQuoteEval();
    }
  }
  return " ";
}

std::string SchemeList::ListOperEval(std::string oper) {
  if (oper == "list") {
    std::string ans;
    int ind = 1;
    while ((*this)[ind]) {
      if (auto temp = As<Number>((*this)[ind])) {
        if (!ans.empty()) {
          ans += " " + std::to_string(temp->GetValue());
        } else {
          ans = std::to_string(temp->GetValue());
        }
      }
      if (auto temp = As<Symbol>((*this)[ind])) {
        if (temp->IsQuoted()) {
          if (!ans.empty()) {
            ans += " " + temp->GetName();
          } else {
            ans = temp->GetName();
          }
        }
        if (scope_->find(temp->GetName()) == scope_->end()) {
          throw NameError{temp->GetName() + " was never defined"};
        }
        if (!ans.empty()) {
          ans += " " + (*scope_)[temp->GetName()];
        } else {
          ans = (*scope_)[temp->GetName()];
        }
      }
      ind++;
    }
    return "(" + ans + ")";
  }

  if (oper == "list-ref") {
    if (size_ != 3) {
      throw SyntaxError{"Wrong arg count for list-ref"};
    }
    int ind = -1;
    if (auto temp = As<Number>((*this)[2])) {
      ind = temp->GetValue();
    }
    if (ind == -1) {
      throw SyntaxError{"Wrong index for list-ref"};
    }
    if (auto temp = As<Symbol>((*this)[1])) {
      std::string list_str;
      if (temp->IsQuoted()) {
        list_str = temp->GetName();
      } else {
        if (scope_->find(temp->GetName()) == scope_->end()) {
          throw NameError{temp->GetName() + " was never defined"};
        }
        list_str = (*scope_)[temp->GetName()];
      }
      auto list = AsList(list_str);

      auto ans = list[ind];
      if (auto tmp = As<Number>(ans)) {
        return std::to_string(tmp->GetValue());
      }
      if (auto tmp = As<Symbol>(ans)) {
        if (tmp->IsQuoted()) {
          return tmp->GetName();
        }
        if (scope_->find(tmp->GetName()) == scope_->end()) {
          throw NameError{temp->GetName() + " was never declared before"};
        }
        return (*scope_)[temp->GetName()];
      }
    }
  }

  if (oper == "list-tail") {
    if (size_ != 3) {
      throw SyntaxError{"Wrong arg count for list-ref"};
    }
    int ind = -1;
    if (auto temp = As<Number>((*this)[2])) {
      ind = temp->GetValue();
    }
    if (ind == -1) {
      throw SyntaxError{"Wrong index for list-ref"};
    }
    std::string ans;
    if (auto temp = As<Symbol>((*this)[1])) {
      std::string list_str;
      if (temp->IsQuoted()) {
        list_str = temp->GetName();
      } else {
        if (scope_->find(temp->GetName()) == scope_->end()) {
          throw NameError{temp->GetName() + " was never defined"};
        }
        list_str = (*scope_)[temp->GetName()];
      }

      auto list = AsList(list_str);

      while (list[ind]) {
        if (auto tmp = As<Number>(list[ind])) {
          ans += " " + std::to_string(tmp->GetValue());
        }
        if (auto tmp = As<Symbol>(list[ind])) {
          if (tmp->IsQuoted()) {
            ans += " " + tmp->GetName();
          }
        }
        ind++;
      }
    }
    if (ans.empty()) {
      return "()";
    }

    return "(" + ans.substr(1) + ")";
  }

  return " ";
}

std::string SchemeList::LambdaEval() {
  auto lambda = SchemeList(As<Cell>((*this)[0]));

  std::unordered_map<std::string, std::string> dict;
  // arg binding
  if (auto args = As<Cell>(lambda[1])) {
    auto arg_list = SchemeList(args);
    int ind = 0;
    while (arg_list[ind]) {
      if (auto argname = As<Symbol>(arg_list[ind])) {
        if (auto val = As<Number>((*this)[ind + 1])) {
          dict[argname->GetName()] = std::to_string(val->GetValue());
        }
      }
      ind++;
    }
  }
  for (int ind = 2; ind < lambda.size_ - 1; ind++) {
    auto temp = As<Cell>(lambda[ind]);
    if (!temp) {
      throw RuntimeError{"no func?"};
    }
    Func func = Func(temp);
    func(dict, func_);
  }

  if (auto temp = As<Cell>(lambda[lambda.size_ - 1])) {
    Func func = Func(temp);
    return func(dict, func_);
  }
  if (auto temp = As<Symbol>(lambda[lambda.size_ - 1])) {
    if (dict.find(temp->GetName()) == dict.end()) {
      throw NameError{temp->GetName() + " is not in lambda dict"};
    }
    return dict[temp->GetName()];
  }

}

std::string SchemeList::AnotherLambdaEval() {
  for (int ind = 2; ind < size_ - 1; ind++) {
    auto temp = As<Cell>((*this)[ind]);
    if (!temp) {
      throw RuntimeError{"no func?"};
    }
    Func func = Func(temp);
    func(*scope_, func_);
  }

  if (auto temp = As<Cell>((*this)[size_ - 1])) {
    Func func = Func(temp);
    return func(*scope_, func_);
  }
  if (auto temp = As<Symbol>((*this)[size_ - 1])) {
    if (scope_->find(temp->GetName()) == scope_->end()) {
      throw SyntaxError{temp->GetName() + " has no val"};
    }
    return (*scope_)[temp->GetName()];
  }
  return " ";
}

std::unordered_map<std::string, std::string> map{{"+",         "int"},
                                                 {"-",         "int"},
                                                 {"*",         "int"},
                                                 {"/",         "int"},
                                                 {"max",       "int"},
                                                 {"min",       "int"},
                                                 {"abs",       "int"},
                                                 {"=",         "int"},
                                                 {"<",         "int"},
                                                 {"<=",        "int"},
                                                 {">=",        "int"},
                                                 {">",         "int"},
                                                 {"number?",   "int"},
                                                 {"quote",     "quote"},
                                                 {"boolean?",  "bool"},
                                                 {"not",       "bool"},
                                                 {"and",       "bool"},
                                                 {"or",        "bool"},
                                                 {"",          "err"},
                                                 {"define",    "func"},
                                                 {"set!",      "func"},
                                                 {"symbol?",   "symb"},
                                                 {"if",        "control"},
                                                 {"pair?",     "list"},
                                                 {"null?",     "list"},
                                                 {"list?",     "list"},
                                                 {"cons",      "pair"},
                                                 {"car",       "pair"},
                                                 {"cdr",       "pair"},
                                                 {"set-car!",  "pair"},
                                                 {"set-cdr!",  "pair"},
                                                 {"list",      "list_oper"},
                                                 {"list-ref",  "list_oper"},
                                                 {"list-tail", "list_oper"}};

std::string SchemeList::Evaluate() {
  if (Is<Cell>((*this)[0])) {
    return LambdaEval();
  }

  std::string oper = Is<Symbol>((*this)[0]) ? As<Symbol>((*this)[0])->GetName() : "";

  if (oper == "lambda") {
    return AnotherLambdaEval();
  }

  if (map[oper] == "int") {
    return IntEval(oper);
  }

  if (map[oper] == "quote") {
    return QuoteEval();
  }

  if (map[oper] == "bool") {
    return BoolEval(oper);
  }

  if (map[oper] == "func") {
    return FuncEval(oper);
  }

  if (map[oper] == "symb") {
    return SymbolEval();
  }

  if (map[oper] == "control") {
    return ControlEval();
  }

  if (map[oper] == "list") {
    return ListEval(oper);
  }

  if (map[oper] == "pair") {
    return PairEval(oper);
  }

  if (map[oper] == "list_oper") {
    return ListOperEval(oper);
  }

  if (map[oper] == "err") {
    throw RuntimeError{"Wrong operator"};
  }

  if (!func_) {
    throw RuntimeError{"no function to call"};
  }

  if (func_->find(oper) != func_->end()) {
    auto &funcs = (*func_)[oper];
    auto args = funcs[0].arg_names_;
    std::unordered_map<std::string, std::string> dict;
    if (!funcs[0].dict_.empty()) {
      dict = funcs[0].dict_;
    }
    for (size_t ind = 0; ind < args.size(); ind++) {
      if (As<Number>((*this)[ind + 1])) {
        dict[args[ind]] = std::to_string(As<Number>((*this)[ind + 1])->GetValue());
      }
      if (auto temp = As<Cell>((*this)[ind + 1])) {
        auto list = SchemeList(temp, scope_, func_);
        dict[args[ind]] = list.Evaluate();
      }
      if (auto temp = As<Symbol>((*this)[ind + 1])) {
        dict[args[ind]] = (*scope_)[temp->GetName()];
      }
    }

    for (size_t ind = 0; ind < funcs.size() - 1; ind++) {
      funcs[ind](dict, func_);
    }

    auto ans = funcs[funcs.size() - 1](dict, func_);
    for (auto &func: funcs) {
      if (not func.dict_.empty()) {
        func.dict_ = dict;
      }
    }
    return ans;
  }
  throw SyntaxError{"Wrong operator1"};
}