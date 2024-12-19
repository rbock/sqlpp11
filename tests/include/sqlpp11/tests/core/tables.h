#pragma once

// generated by ./scripts/ddl2cpp -auto-id tests/core/usage/tables.sql tests/core/usage/tables test

#include <sqlpp11/core/basic/table.h>
#include <sqlpp11/core/basic/table_columns.h>
#include <sqlpp11/core/type_traits.h>
#include <sqlpp11/core/name/create_name_tag.h>
#include <sqlpp11/core/compat/string_view.h>

namespace test
{
  struct TabFoo_ : public ::sqlpp::name_tag_base
  {
    struct Id : public ::sqlpp::name_tag_base
    {
      SQLPP_CREATE_NAME_TAG_FOR_SQL_AND_CPP(id, id);
      using value_type = ::sqlpp::integral;
      using has_default = std::true_type;
    };
    struct TextNnD : public ::sqlpp::name_tag_base
    {
      SQLPP_CREATE_NAME_TAG_FOR_SQL_AND_CPP(text_nn_d, textNnD);
      using value_type = ::sqlpp::text;
      using has_default = std::true_type;
    };
    struct IntN : public ::sqlpp::name_tag_base
    {
      SQLPP_CREATE_NAME_TAG_FOR_SQL_AND_CPP(int_n, intN);
      using value_type = ::sqlpp::optional<::sqlpp::integral>;
      using has_default = std::true_type;
    };
    struct DoubleN : public ::sqlpp::name_tag_base
    {
      SQLPP_CREATE_NAME_TAG_FOR_SQL_AND_CPP(double_n, doubleN);
      using value_type = ::sqlpp::optional<::sqlpp::floating_point>;
      using has_default = std::true_type;
    };
    struct UIntN : public ::sqlpp::name_tag_base
    {
      SQLPP_CREATE_NAME_TAG_FOR_SQL_AND_CPP(u_int_n, uIntN);
      using value_type = ::sqlpp::optional<::sqlpp::unsigned_integral>;
      using has_default = std::true_type;
    };
    struct BlobN : public ::sqlpp::name_tag_base
    {
      SQLPP_CREATE_NAME_TAG_FOR_SQL_AND_CPP(blob_n, blobN);
      using value_type = ::sqlpp::optional<::sqlpp::blob>;
      using has_default = std::true_type;
    };
    struct BoolN : public ::sqlpp::name_tag_base
    {
      SQLPP_CREATE_NAME_TAG_FOR_SQL_AND_CPP(bool_n, boolN);
      using value_type = ::sqlpp::optional<::sqlpp::boolean>;
      using has_default = std::true_type;
    };
    SQLPP_CREATE_NAME_TAG_FOR_SQL_AND_CPP(tab_foo, tabFoo);
    template<typename T>
    using _table_columns = sqlpp::table_columns<T,
               Id,
               TextNnD,
               IntN,
               DoubleN,
               UIntN,
               BlobN,
               BoolN>;
    using _required_insert_columns = sqlpp::detail::type_set<>;
  };
  using TabFoo = ::sqlpp::table_t<TabFoo_>;

  struct TabBar_ : public ::sqlpp::name_tag_base
  {
    struct Id : public ::sqlpp::name_tag_base
    {
      SQLPP_CREATE_NAME_TAG_FOR_SQL_AND_CPP(id, id);
      using value_type = ::sqlpp::integral;
      using has_default = std::true_type;
    };
    struct TextN : public ::sqlpp::name_tag_base
    {
      SQLPP_CREATE_NAME_TAG_FOR_SQL_AND_CPP(text_n, textN);
      using value_type = ::sqlpp::optional<::sqlpp::text>;
      using has_default = std::true_type;
    };
    struct BoolNn : public ::sqlpp::name_tag_base
    {
      SQLPP_CREATE_NAME_TAG_FOR_SQL_AND_CPP(bool_nn, boolNn);
      using value_type = ::sqlpp::boolean;
      using has_default = std::false_type;
    };
    struct IntN : public ::sqlpp::name_tag_base
    {
      SQLPP_CREATE_NAME_TAG_FOR_SQL_AND_CPP(int_n, intN);
      using value_type = ::sqlpp::optional<::sqlpp::integral>;
      using has_default = std::true_type;
    };
    SQLPP_CREATE_NAME_TAG_FOR_SQL_AND_CPP(tab_bar, tabBar);
    template<typename T>
    using _table_columns = sqlpp::table_columns<T,
               Id,
               TextN,
               BoolNn,
               IntN>;
    using _required_insert_columns = sqlpp::detail::type_set<
               sqlpp::column_t<sqlpp::table_t<TabBar_>, BoolNn>>;
  };
  using TabBar = ::sqlpp::table_t<TabBar_>;

  struct TabDateTime_ : public ::sqlpp::name_tag_base
  {
    struct Id : public ::sqlpp::name_tag_base
    {
      SQLPP_CREATE_NAME_TAG_FOR_SQL_AND_CPP(id, id);
      using value_type = ::sqlpp::integral;
      using has_default = std::true_type;
    };
    struct DayPointN : public ::sqlpp::name_tag_base
    {
      SQLPP_CREATE_NAME_TAG_FOR_SQL_AND_CPP(day_point_n, dayPointN);
      using value_type = ::sqlpp::optional<::sqlpp::day_point>;
      using has_default = std::true_type;
    };
    struct TimePointN : public ::sqlpp::name_tag_base
    {
      SQLPP_CREATE_NAME_TAG_FOR_SQL_AND_CPP(time_point_n, timePointN);
      using value_type = ::sqlpp::optional<::sqlpp::time_point>;
      using has_default = std::true_type;
    };
    struct TimeOfDayN : public ::sqlpp::name_tag_base
    {
      SQLPP_CREATE_NAME_TAG_FOR_SQL_AND_CPP(time_of_day_n, timeOfDayN);
      using value_type = ::sqlpp::optional<::sqlpp::time_of_day>;
      using has_default = std::true_type;
    };
    SQLPP_CREATE_NAME_TAG_FOR_SQL_AND_CPP(tab_date_time, tabDateTime);
    template<typename T>
    using _table_columns = sqlpp::table_columns<T,
               Id,
               DayPointN,
               TimePointN,
               TimeOfDayN>;
    using _required_insert_columns = sqlpp::detail::type_set<>;
  };
  using TabDateTime = ::sqlpp::table_t<TabDateTime_>;

} // namespace test