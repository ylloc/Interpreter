# Interpreter

Поддерживает scheme-like синтаксис, в main.cpp можно найти несколько "сложных" тестов, например 

```cpp
void TestFactorial() {
  Scheme scheme;
  scheme.Evaluate(R"EOF((define fact
  (lambda (n)
    (if (= n 0)
        1
        (* n (fact (- n 1))))))
  )EOF");
  
  assert(scheme.Evaluate("(fact 10)") == "3628800");
}
```

```cpp
void TestSum() {
  Scheme scheme;
  scheme.Evaluate(R"EOF((define sum
  (lambda (a b)
    (+ a b)))
  )EOF");

  assert(scheme.Evaluate("(sum 3 5)") == "8");
  assert(scheme.Evaluate("(sum 10 20)") == "30");
}
```

```cpp
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
```