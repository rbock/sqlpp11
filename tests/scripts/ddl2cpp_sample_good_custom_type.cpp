
#include <ddl2cpp_sample_good_custom_type.h>
#include <sqlpp11/core/chrono.h>

int main() {
  test::TabFoo tab_foo;
  tab_foo.myBoolean = true;
  tab_foo.myInteger = 5;
  tab_foo.mySerial = 10;
  tab_foo.myFloatingPoint = 12.34;
  tab_foo.myText = "test";
  tab_foo.myBlob = std::vector<uint8_t>{'b', 'l', 'o', 'b'};
  tab_foo.myDate = sqlpp::chrono::day_point{};
  tab_foo.myDateTime = std::chrono::system_clock::now();
  tab_foo.myTime = std::chrono::seconds{10};
  // Special cases
  tab_foo.mySecondText = "another text";
  tab_foo.myTypeWithSpaces = 20;
  // Capitalisation
  tab_foo.capBoolean = false;
  // Build in types
  tab_foo.builtinBoolean = true;
  tab_foo.builtinInteger = 5;
  tab_foo.builtinSerial = 10;
  tab_foo.builtinFloatingPoint = 12.34;
  tab_foo.builtinText = "test";
  tab_foo.builtinBlob = std::vector<uint8_t>{'b', 'l', 'o', 'b'};
  tab_foo.builtinDate = sqlpp::chrono::day_point{};
  tab_foo.builtinDateTime = std::chrono::system_clock::now();
  tab_foo.builtinTime = std::chrono::seconds{10};
}
