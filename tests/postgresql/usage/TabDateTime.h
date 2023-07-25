#pragma once

#include <sqlpp11/table.h>
#include <sqlpp11/char_sequence.h>
#include <sqlpp11/column_types.h>

namespace model
{
  namespace TabDateTime_
  {
    struct C_timepoint
    {
      struct _alias_t
      {
        static constexpr const char _literal[] = "c_timepoint";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T>
        struct _member_t
        {
          T c_timepoint;
          T& operator()()
          {
            return c_timepoint;
          }
          const T& operator()() const
          {
            return c_timepoint;
          }
        };
      };

      using _traits = ::sqlpp::make_traits<::sqlpp::time_point, sqlpp::tag::can_be_null>;
    };

    struct C_day
    {
      struct _alias_t
      {
        static constexpr const char _literal[] = "c_day";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T>
        struct _member_t
        {
          T c_day;
          T& operator()()
          {
            return c_day;
          }
          const T& operator()() const
          {
            return c_day;
          }
        };
      };

      using _traits = ::sqlpp::make_traits<::sqlpp::day_point, sqlpp::tag::can_be_null>;
    };

    struct C_time
    {
      struct _alias_t
      {
        static constexpr const char _literal[] = "c_time";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T>
        struct _member_t
        {
          T c_time;
          T& operator()()
          {
            return c_time;
          }
          const T& operator()() const
          {
            return c_time;
          }
        };
      };
      using _traits = sqlpp::make_traits<sqlpp::time_of_day, sqlpp::tag::can_be_null>;
    };

  }  // namespace TabDatetime_

  struct TabDateTime : sqlpp::table_t<TabDateTime,
                                      TabDateTime_::C_time,
                                      TabDateTime_::C_day,
                                      TabDateTime_::C_timepoint>
  {
    using _value_type = sqlpp::no_value_t;
    struct _alias_t
    {
      static constexpr const char _literal[] = "TabDatetime";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template <typename T>
      struct _member_t
      {
        T TabDateTime;
        T& operator()()
        {
          return TabDateTime;
        }
        const T& operator()() const
        {
          return TabDateTime;
        }
      };
    };
  };
}
