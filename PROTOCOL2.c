#include <stdio.h>

int F(int X, int Y)
{
  int result = X * X + 2 * X * Y - Y * Y;
  return result;
}

int main()
{
  int X, Y;
  printf("Enter a value for X: ");
  scanf("%d", &X);
  printf("Enter a value for Y: ");
  scanf("%d", &Y);

  int result = F(X, Y);
  printf("F(%d,%d) = ANSWER %d\n", X, Y, result);

  return 0;
}
