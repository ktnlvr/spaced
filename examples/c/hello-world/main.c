int main() {
  int out[15] = {}, tmp;
  int a = 1, b = 1, i = 0;
  
  for (i = 0; i < 15; i++) {
    out[i] = a;
    b += a;
    a = b;
    b = tmp;
  }

  return 0;
}
