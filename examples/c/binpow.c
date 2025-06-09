int *ptr = (int *)0x0BEE;

int binpow(int a, int x) {
  int result = 1;
  while (x > 0) {
    if (x & 1)
      result *= a;
    a *= a;
    x >>= 1;
  }
  return result;
}

int main() {
  int a = 3;
  int x = 10;

  *ptr = 0xCCCC;
  *ptr = binpow(a, x);

  return *ptr;
}
