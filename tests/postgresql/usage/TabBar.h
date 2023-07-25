#pragma once

#include <sqlpp11/table.h>
#include <sqlpp11/char_sequence.h>
#include <sqlpp11/column_types.h>

namespace model
{
  namespace TabBar_
  {
    struct C_int
    {
      struct _alias_t
      {
        static constexpr const char _literal[] = "c_int";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T>
        struct _member_t
        {
          T c_int;
          T& operator()()
          {
            return c_int;
          }
          const T& operator()() const
          {
            return c_int;
          }
        };
      };

      using _traits = ::sqlpp::make_traits<::sqlpp::bigint, sqlpp::tag::can_be_null>;
    };
  }

  struct TabBar : sqlpp::table_t<TabBar, TabBar_::C_int>
  {
    using _value_type = sqlpp::no_value_t;
    struct _alias_t
    {
      static constexpr const char _literal[] = "TabBar";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template <typename T>
      struct _member_t
      {
        T TabBar;
        T& operator()()
        {
          return TabBar;
        }
        const T& operator()() const
        {
          return TabBar;
        }
      };
    };
  };
}
