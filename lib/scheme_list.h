#include "node.h"

#include <unordered_map>
#include <functional>
#include <regex>

struct BinaryFunction {
  std::string name;

  std::function<int(int, int)> function;
  bool is_bool = false;

  int operator()(int a, int b) {
    return function(a, b);
  }
};

class Func;

class SchemeList {
public:
  SchemeList(const std::shared_ptr<Object> &root) : root_(root) {
    CountSize();
  }
  SchemeList(const std::shared_ptr<Object> &root,
             std::unordered_map<std::string, std::string> *scope)
    : root_(root), scope_(scope) {
    CountSize();
  }
  SchemeList(const std::shared_ptr<Object> &root,
             std::unordered_map<std::string, std::string> *scope,
             std::unordered_map<std::string, std::vector<Func>> *func)
    : root_(root), scope_(scope), func_(func) {
    CountSize();
  }
  SchemeList() = default;

  void CountSize() {
    while ((*this)[size_]) {
      size_++;
    }
  }

  int Size() {
    return size_;
  }

  bool IsPoint() {
    return point_;
  }

  std::shared_ptr<Object> operator[](size_t ind) {
    auto tmp = root_;
    while (ind--) {
      if (!Is<Cell>(tmp)) {
        return tmp;
      }
      auto cell_tmp = As<Cell>(tmp);
      tmp = cell_tmp->GetSecond();
      if (Is<Number>(tmp)) {
        point_ = true;
        if (ind) {
          return nullptr;
        }
        return tmp;
      }
    }
    return Is<Cell>(tmp) ? As<Cell>(tmp)->GetFirst() : nullptr;
  }

  std::string IntEval(std::string operation);
  std::string BoolEval(std::string operation);
  std::string FuncEval(std::string operation);
  std::string PairEval(std::string operation);
  std::string ListEval(std::string operation);
  std::string ListOperEval(std::string operation);

  std::string QuoteEval();
  std::string LambdaEval();
  std::string AnotherLambdaEval();
  std::string SymbolEval();
  std::string ControlEval();
  std::string RawQuoteEval();
  std::string Evaluate();

private:
  std::shared_ptr<Object> root_;
  size_t size_ = 0;
  bool point_ = false;
  std::unordered_map<std::string, std::string> *scope_ = nullptr;
  std::unordered_map<std::string, std::vector<Func>> *func_ = nullptr;
};

class Func {
public:
  Func() = default;

  Func(std::shared_ptr<Cell> func) : func_(func) {
  }

  Func(std::shared_ptr<Cell> func, std::vector<std::string> &arg_names)
    : func_(func), arg_names_(arg_names) {
  }

  std::string operator()(std::unordered_map<std::string, std::string> &dict) {
    auto list = SchemeList(func_, &dict);
    return list.Evaluate();
  }

  std::string operator()(std::unordered_map<std::string, std::string> &dict,
                         std::unordered_map<std::string, std::vector<Func>> *func) {
    auto list = SchemeList(func_, &dict, func);
    return list.Evaluate();
  }

  std::shared_ptr<Object> func_;
  std::vector<std::string> arg_names_;
  std::unordered_map<std::string, std::string> dict_;

};