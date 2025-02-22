#include <sqlpp23/select.h>
#include <sqlpp23/alias_provider.h>

int main()
{
  select(sqlpp::value(false).as(sqlpp::alias::a));
  return 0;
}