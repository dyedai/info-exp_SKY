#include <stdio.h>

int F(int X)
{
  int result = 2 * X * X * X - 3 * X * X - 5 * X + 4;
  return result;
}

int main()
{
  int X;
  printf("Enter a value for X: ");
  scanf("%d", &X);

  int result = F(X);
  printf("F(%d) = ANSWER %d\n", X, result);

  return 0;
}
