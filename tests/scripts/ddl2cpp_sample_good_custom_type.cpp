
#include <sqlpp11/chrono.h>
#include <ddl2cpp_sample_good_custom_type.h>

int main()
{
  test::TabFoo tab_foo;
  tab_foo.myBoolean = true;
  tab_foo.myInteger = 5;
  tab_foo.mySerial = 10;
  tab_foo.myFloatingPoint = 12.34;
  tab_foo.myText = "test";
  tab_foo.myBlob = "blob";
  tab_foo.myDate = sqlpp::chrono::day_point{};
  tab_foo.myDateTime = std::chrono::system_clock::now();
  tab_foo.myTime = std::chrono::seconds{10};
}
