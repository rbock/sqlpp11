#include <sqlpp11/select.h>
#include <sqlpp11/alias_provider.h>

int main()
{
  select(sqlpp::value(false).as(sqlpp::alias::a));
  return 0;
}