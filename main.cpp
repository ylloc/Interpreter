#include <iostream>
#include <concepts>
#include <set>

#include "scheme.h"

int main() {
  Scheme scheme;
  scheme.Evaluate(R"EOF((define fib
  (lambda (x)
    (if (= x 0)
        0
        (if (= x 1)
            1
            (+ (fib (- x 1)) (fib (- x 2)))))))
  )EOF");
  std::cout << scheme.Evaluate("(fib 20)") << std::endl;
}