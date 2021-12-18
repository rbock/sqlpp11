#include <sample_identity_naming.h>

int main()
{
  test::tab_foo tab_foo;
  tab_foo.delta = "delta";
  tab_foo._epsilon = 42;
  tab_foo.omega = 3.14;

  test::tab_bar tab_bar;
  tab_bar.alpha = 42;
  tab_bar.beta = "beta";
  tab_bar.gamma = true;
  tab_bar.delta = 42;
}
