int main() {
  char i;
  char out[15] = {0xCC, 0xCC, 0xCC};
  char a = 1, b = 0;

  for (i = 0; i < 15; i++) {
    char tmp;
    out[i] = b;
    b += a;
    tmp = a;
    a = b;
    b = tmp;
  }

  return 0;
}
