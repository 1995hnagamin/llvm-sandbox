class A {
  explicit A() = default;
};

#pragma dead "aaa"

int main() {
  int sum = 0;
  for (int i = 1; i <= 100; ++i) {
    sum += i;
  }
  return 0;
}
