#include <node.h>
#include <parser.h>
#include <tokenizer.h>
#include <scheme_list.h>

#include <memory>

static int counter = 0;

std::shared_ptr<Object> Read(Tokenizer *tokenizer) {
  if (tokenizer->IsEnd()) {
    counter = 0;
    throw SyntaxError{"Syntax error"};
  }

  Token token = tokenizer->GetToken();
  tokenizer->Next();

  if (auto num = std::get_if<ConstantToken>(&token)) {
    return std::make_shared<Number>(num->value);
  }

  if (auto symbol = std::get_if<SymbolToken>(&token)) {
    if (counter <= 0) {
      if (symbol->name[0] == '-') {
        counter = 0;
        throw SyntaxError{"Syntax error"};
      }
    }

    return std::make_shared<Symbol>(symbol->name);
  }

  if (std::get_if<DotToken>(&token)) {
    if (counter <= 0) {
      counter = 0;
      throw SyntaxError{"Syntax error"};
    }
    return std::make_shared<Dot>();
  }

  if (std::get_if<QuoteToken>(&token)) {
    bool q = false;
    auto next_token = tokenizer->GetToken();
    if (std::get_if<QuoteToken>(&next_token)) {
      q = true;
    }
    auto next = Read(tokenizer);
    if (As<Cell>(next)) {
      auto list = SchemeList(next);
      return std::make_shared<Symbol>(list.RawQuoteEval(), true);
    }

    if (auto temp = As<Symbol>(next)) {
      if (q) {
        return std::make_shared<Symbol>("'" + temp->GetName(), true);
      }
      return std::make_shared<Symbol>(temp->GetName(), true);
    }
    if (auto temp = As<Number>(next)) {
      return std::make_shared<Symbol>(std::to_string(temp->GetValue()), true);
    }
    throw RuntimeError("RuntimeError");
  }

  if (auto cell = std::get_if<BracketToken>(&token)) {
    if (*cell == BracketToken::OPEN) {
      return ReadList(tokenizer);
    } else {
      if (counter <= 0) {
        counter = 0;
        throw SyntaxError{"Syntax error"};
      }
      return std::make_shared<Object>(true);
    }
  }

  return std::make_shared<Object>();
}

std::shared_ptr<Object> ReadList(Tokenizer *tokenizer) {
  std::vector<std::shared_ptr<Object>> objects;
  counter++;
  while (true) {
    auto temp = Read(tokenizer);
    if (temp && temp->IsTrash()) {
      break;
    }
    objects.push_back(temp);
  }
  counter--;
  if (objects.empty()) {
    return std::make_shared<Cell>();
  }
  std::shared_ptr<Object> prev = nullptr;
  std::shared_ptr<Cell> root = nullptr;

  bool has_dot = false;

  for (size_t ind = 0; ind < objects.size(); ++ind) {
    auto object = objects[ind];
    if (!Is<Dot>(object)) {
      if (!has_dot) {
        std::shared_ptr<Cell> tmp = std::make_shared<Cell>();
        if (!root) {
          root = tmp;
        }
        tmp->SetFirst(object);
        if (Is<Cell>(prev)) {
          As<Cell>(prev)->SetSecond(tmp);
        }
        prev = tmp;
      } else {
        if (Is<Cell>(prev)) {
          has_dot = false;
          As<Cell>(prev)->SetSecond(object);
        }
      }
    } else {
      if (ind + 2 != objects.size() || ind == 0) {
        counter = 0;
        throw SyntaxError{"Syntax error"};
      }

      has_dot = true;
    }
  }

  return root;
}