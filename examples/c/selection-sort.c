void swap(int* a, int* b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

void sort(int* xs, int n) {
  int i, mi = 0;

  if (n <= 1)
    return;

  for (i = 0; i < n; i++)
    if (xs[i] < xs[mi])
      mi = i;

  swap(xs, xs + mi);
  sort(xs + 1, n - 1);
}

int main(void) {
  int nums[5] = {3, 1, 4, 1, 5};
  sort(nums, sizeof nums / sizeof nums[0]);
}
