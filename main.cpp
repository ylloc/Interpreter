#include <iostream>
#include <concepts>
#include <set>
#include <cassert>

#include "scheme.h"

void TestFactorial() {
  Scheme scheme;
  scheme.Evaluate(R"EOF((define fact
  (lambda (n)
    (if (= n 0)
        1
        (* n (fact (- n 1))))))
  )EOF");

  assert(scheme.Evaluate("(fact 0)") == "1");
  assert(scheme.Evaluate("(fact 1)") == "1");
  assert(scheme.Evaluate("(fact 5)") == "120");
  assert(scheme.Evaluate("(fact 10)") == "3628800");
}

void TestSum() {
  Scheme scheme;
  scheme.Evaluate(R"EOF((define sum
  (lambda (a b)
    (+ a b)))
  )EOF");

  assert(scheme.Evaluate("(sum 3 5)") == "8");
  assert(scheme.Evaluate("(sum 10 20)") == "30");
  assert(scheme.Evaluate("(sum -5 5)") == "0");
}

void TestNestedFunctions() {
  Scheme scheme;
  scheme.Evaluate(R"EOF((define square
  (lambda (x) (* x x)))
  )EOF");

  scheme.Evaluate(R"EOF((define sum-of-squares
  (lambda (a b)
    (+ (square a) (square b))))
  )EOF");

  assert(scheme.Evaluate("(sum-of-squares 3 4)") == "25");
  assert(scheme.Evaluate("(sum-of-squares 5 12)") == "169");
}

void TestConditional() {
  Scheme scheme;
  scheme.Evaluate(R"EOF((define abs
  (lambda (x)
    (if (< x 0)
        (- 0 x)
        x)))
  )EOF");

  assert(scheme.Evaluate("(abs 5)") == "5");
  assert(scheme.Evaluate("(abs -5)") == "5");
  assert(scheme.Evaluate("(abs 0)") == "0");
}

void TestRecursion() {
  Scheme scheme;
  scheme.Evaluate(R"EOF((define count-down
  (lambda (n)
    (if (= n 0)
        0
        (count-down (- n 1)))))
  )EOF");

  assert(scheme.Evaluate("(count-down 5)") == "0");
  assert(scheme.Evaluate("(count-down 100)") == "0");
}


void TestFib() {
  Scheme scheme;
  scheme.Evaluate(R"EOF((define fib
  (lambda (x)
    (if (= x 0)
        0
        (if (= x 1)
            1
            (+ (fib (- x 1)) (fib (- x 2)))))))
  )EOF");
  assert(scheme.Evaluate("(fib 20)") == "6765");
}

int main() {
  TestFib();
  TestFactorial();
  TestSum();
  TestNestedFunctions();
  TestConditional();
  TestRecursion();
}