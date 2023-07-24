#pragma once

#include <sqlpp11/table.h>
#include <sqlpp11/char_sequence.h>
#include <sqlpp11/column_types.h>

namespace model
{
  namespace TabFoo_
  {
    struct Alpha
    {
      struct _alias_t
      {
        static constexpr const char _literal[] = "alpha";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T>
        struct _member_t
        {
          T alpha;
          T& operator()()
          {
            return alpha;
          }
          const T& operator()() const
          {
            return alpha;
          }
        };
      };

      using _traits = ::sqlpp::make_traits<::sqlpp::bigint, sqlpp::tag::can_be_null>;
    };

    struct Beta
    {
      struct _alias_t
      {
        static constexpr const char _literal[] = "beta";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T>
        struct _member_t
        {
          T beta;
          T& operator()()
          {
            return beta;
          }
          const T& operator()() const
          {
            return beta;
          }
        };
      };

      using _traits = ::sqlpp::make_traits<::sqlpp::smallint, sqlpp::tag::can_be_null>;
    };

    struct Gamma
    {
      struct _alias_t
      {
        static constexpr const char _literal[] = "gamma";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T>
        struct _member_t
        {
          T gamma;
          T& operator()()
          {
            return gamma;
          }
          const T& operator()() const
          {
            return gamma;
          }
        };
      };

      using _traits = ::sqlpp::make_traits<::sqlpp::text, sqlpp::tag::can_be_null>;
    };

    struct C_bool
    {
      struct _alias_t
      {
        static constexpr const char _literal[] = "c_bool";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T>
        struct _member_t
        {
          T c_bool;
          T& operator()()
          {
            return c_bool;
          }
          const T& operator()() const
          {
            return c_bool;
          }
        };
      };

      using _traits = ::sqlpp::make_traits<::sqlpp::boolean, sqlpp::tag::can_be_null>;
    };

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
  }

  struct TabFoo : sqlpp::table_t<TabFoo,
                                 TabFoo_::Alpha,
                                 TabFoo_::Beta,
                                 TabFoo_::Gamma,
                                 TabFoo_::C_bool,
                                 TabFoo_::C_timepoint,
                                 TabFoo_::C_day>
  {
    using _value_type = sqlpp::no_value_t;
    struct _alias_t
    {
      static constexpr const char _literal[] = "TabFoo";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template <typename T>
      struct _member_t
      {
        T TabFoo;
        T& operator()()
        {
          return TabFoo;
        }
        const T& operator()() const
        {
          return TabFoo;
        }
      };
    };
  };
}
