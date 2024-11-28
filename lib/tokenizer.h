#pragma once

#include <variant>
#include <istream>
#include <string>
#include <string>
#include <iostream>

struct SymbolToken {
  std::string name;

  bool operator==(const SymbolToken& other) const {
    return name == other.name;
  };
};

struct QuoteToken {
  bool operator==(const QuoteToken&) const {
    return true;
  }
};

struct DotToken {
  bool operator==(const DotToken&) const {
    return true;
  }
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
  int value;

  bool operator==(const ConstantToken& other) const {
    return value == other.value;
  };
};

using Token = std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken>;

// Интерфейс, позволяющий читать токены по одному из потока.
class Tokenizer {
public:
  static char ToChar(int val) {
    return static_cast<char>(val);
  }

  static auto OnInterval(auto point, auto left, auto right) {
    return left <= point && point <= right;
  }

  static bool IsOneOf(int symbol, auto... symbols) {
    return ((symbol == symbols) || ...);
  }

  // Может начинаться с этого символа
  bool CanStartWith(int symbol) {
    return OnInterval(symbol, 'a', 'z') || OnInterval(symbol, 'A', 'Z') ||
           IsOneOf(symbol, '<', '=', '>', '*', '#');
  }

  // Может относиться к следующему за валидным символом CanStartWith слову
  bool ValidSymbol(int symbol) {
    return CanStartWith(symbol) || OnInterval(symbol, '0', '9') ||
           IsOneOf(symbol, '!', '?', '-');
  }

  // Пропускаем такие символы (например '_', '\t')
  bool ToSkip(int symbol) {
    return not(ValidSymbol(symbol) || IsOneOf(symbol, '(', ')', '.', '\'', '+', '-', EOF));
  };

  explicit Tokenizer(std::istream* in) : in_(in) {
    Next();
  }

  bool IsEnd() const {
    return is_end_;
  }

  void Next() {
    while (ToSkip(in_->peek())) {
      in_->get();
    }

    auto ch = in_->get();

    if (ch == EOF) {
      is_end_ = true;
      return;
    }

    auto c = ToChar(ch);

    if (c == '(') {
      token_ = BracketToken::OPEN;
      return;
    }

    if (c == ')') {
      token_ = BracketToken::CLOSE;
      return;
    }

    if (c == '\'') {
      token_ = QuoteToken{};
      return;
    }

    if (c == '.') {
      token_ = DotToken{};
      return;
    }

    if (c == '/') {
      token_ = SymbolToken{"/"};
      return;
    }

    if (std::isdigit(c)) {
      auto buff = ReadInteger();
      buff.insert(buff.begin(), c);

      token_ = ConstantToken{std::stoi(buff)};
      return;
    }

    if (c == '-' || c == '+') {
      auto option_integer = ReadInteger();
      if (option_integer.empty()) {
        token_ = SymbolToken{std::string{c}};
      } else {
        int coff = c == '-' ? -1 : 1;
        token_ = ConstantToken{coff * std::stoi(option_integer)};
      }
      return;
    }

    if (CanStartWith(c)) {
      std::string ans{c};
      while (ValidSymbol(in_->peek())) {
        ans.push_back(ToChar(in_->get()));
      }
      token_ = SymbolToken{ans};
      return;
    }
  }

  std::string ReadInteger() {
    std::string buffer{};
    while (std::isdigit(in_->peek())) {
      buffer.push_back(ToChar(in_->get()));
    }
    return buffer;
  }

  Token GetToken() {
    return token_;
  }

  ssize_t& State() {
    return tokenizer_state_;
  }

  void ClearState() {
    tokenizer_state_ = 0;
  }

private:
  bool is_end_{false};
  Token token_{};
  std::istream* in_;

  ssize_t tokenizer_state_{0};
};