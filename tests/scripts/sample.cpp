#include <sample.h>

int main()
{
  test::TabFoo tab_foo;
  tab_foo.delta = "delta";
  tab_foo.Epsilon = 42;
  tab_foo.omega = 3.14;

  test::TabBar tab_bar;
  tab_bar.alpha = 42;
  tab_bar.beta = "beta";
  tab_bar.gamma = true;
  tab_bar.delta = 42;
}
