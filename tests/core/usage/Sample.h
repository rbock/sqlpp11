#pragma once

// generated by ./scripts/ddl2cpp tests/core/usage/sample.sql tests/core/usage/Sample test

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace test
{
  namespace TabFoo_
  {
    struct Id
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "id";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T id;
            T& operator()() { return id; }
            const T& operator()() const { return id; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update>;
    };
    struct TextNnD
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "text_nn_d";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T textNnD;
            T& operator()() { return textNnD; }
            const T& operator()() const { return textNnD; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text>;
    };
    struct IntN
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "int_n";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T intN;
            T& operator()() { return intN; }
            const T& operator()() const { return intN; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct DoubleN
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "double_n";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T doubleN;
            T& operator()() { return doubleN; }
            const T& operator()() const { return doubleN; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::floating_point, sqlpp::tag::can_be_null>;
    };
    struct UIntN
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "u_int_n";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T uIntN;
            T& operator()() { return uIntN; }
            const T& operator()() const { return uIntN; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer_unsigned, sqlpp::tag::can_be_null>;
    };
    struct BlobN
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "blob_n";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T blobN;
            T& operator()() { return blobN; }
            const T& operator()() const { return blobN; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::blob, sqlpp::tag::can_be_null>;
    };
  } // namespace TabFoo_

  struct TabFoo: sqlpp::table_t<TabFoo,
               TabFoo_::Id,
               TabFoo_::TextNnD,
               TabFoo_::IntN,
               TabFoo_::DoubleN,
               TabFoo_::UIntN,
               TabFoo_::BlobN>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "tab_foo";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T tabFoo;
        T& operator()() { return tabFoo; }
        const T& operator()() const { return tabFoo; }
      };
    };
  };
  namespace TabBar_
  {
    struct Id
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "id";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T id;
            T& operator()() { return id; }
            const T& operator()() const { return id; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update>;
    };
    struct TextN
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "text_n";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T textN;
            T& operator()() { return textN; }
            const T& operator()() const { return textN; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
    struct BoolNn
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "bool_nn";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T boolNn;
            T& operator()() { return boolNn; }
            const T& operator()() const { return boolNn; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::boolean, sqlpp::tag::require_insert>;
    };
    struct IntN
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "int_n";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T intN;
            T& operator()() { return intN; }
            const T& operator()() const { return intN; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
  } // namespace TabBar_

  struct TabBar: sqlpp::table_t<TabBar,
               TabBar_::Id,
               TabBar_::TextN,
               TabBar_::BoolNn,
               TabBar_::IntN>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "tab_bar";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T tabBar;
        T& operator()() { return tabBar; }
        const T& operator()() const { return tabBar; }
      };
    };
  };
  namespace TabDateTime_
  {
    struct Id
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "id";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T id;
            T& operator()() { return id; }
            const T& operator()() const { return id; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update>;
    };
    struct DayPointN
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "day_point_n";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T dayPointN;
            T& operator()() { return dayPointN; }
            const T& operator()() const { return dayPointN; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::day_point, sqlpp::tag::can_be_null>;
    };
    struct TimePointN
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "time_point_n";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T timePointN;
            T& operator()() { return timePointN; }
            const T& operator()() const { return timePointN; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::time_point, sqlpp::tag::can_be_null>;
    };
    struct TimeOfDayN
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "time_of_day_n";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T timeOfDayN;
            T& operator()() { return timeOfDayN; }
            const T& operator()() const { return timeOfDayN; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::time_of_day, sqlpp::tag::can_be_null>;
    };
  } // namespace TabDateTime_

  struct TabDateTime: sqlpp::table_t<TabDateTime,
               TabDateTime_::Id,
               TabDateTime_::DayPointN,
               TabDateTime_::TimePointN,
               TabDateTime_::TimeOfDayN>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "tab_date_time";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T tabDateTime;
        T& operator()() { return tabDateTime; }
        const T& operator()() const { return tabDateTime; }
      };
    };
  };
} // namespace test
