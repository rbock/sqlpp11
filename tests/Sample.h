#ifndef TEST_SAMPLE_H
#define TEST_SAMPLE_H

#include <sqlpp11/table.h>
#include <sqlpp11/column_types.h>
#include <sqlpp11/char_sequence.h>

namespace test
{
  namespace TabFoo_
  {
    struct Delta
    {
      struct _alias_t
      {
        static constexpr const char _literal[] = "delta";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T>
        struct _member_t
        {
          T delta;
          T& operator()()
          {
            return delta;
          }
          const T& operator()() const
          {
            return delta;
          }
        };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::can_be_null>;
    };
    struct Epsilon
    {
      struct _alias_t
      {
        static constexpr const char _literal[] = "epsilon";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T>
        struct _member_t
        {
          T epsilon;
          T& operator()()
          {
            return epsilon;
          }
          const T& operator()() const
          {
            return epsilon;
          }
        };
      };
      using _traits = sqlpp::make_traits<sqlpp::bigint, sqlpp::tag::can_be_null>;
    };
    struct Omega
    {
      struct _alias_t
      {
        static constexpr const char _literal[] = "omega";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T>
        struct _member_t
        {
          T omega;
          T& operator()()
          {
            return omega;
          }
          const T& operator()() const
          {
            return omega;
          }
        };
      };
      using _traits = sqlpp::make_traits<sqlpp::floating_point, sqlpp::tag::can_be_null>;
    };
  }

  struct TabFoo : sqlpp::table_t<TabFoo, TabFoo_::Delta, TabFoo_::Epsilon, TabFoo_::Omega>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] = "tab_foo";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template <typename T>
      struct _member_t
      {
        T tabFoo;
        T& operator()()
        {
          return tabFoo;
        }
        const T& operator()() const
        {
          return tabFoo;
        }
      };
    };
  };
  namespace TabBar_
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
      using _traits = sqlpp::
          make_traits<sqlpp::bigint, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update, sqlpp::tag::can_be_null>;
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
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::can_be_null>;
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
      using _traits = sqlpp::make_traits<sqlpp::boolean, sqlpp::tag::require_insert>;
    };
    struct Delta
    {
      struct _alias_t
      {
        static constexpr const char _literal[] = "delta";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T>
        struct _member_t
        {
          T delta;
          T& operator()()
          {
            return delta;
          }
          const T& operator()() const
          {
            return delta;
          }
        };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
  }

  struct TabBar : sqlpp::table_t<TabBar, TabBar_::Alpha, TabBar_::Beta, TabBar_::Gamma, TabBar_::Delta>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] = "tab_bar";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template <typename T>
      struct _member_t
      {
        T tabBar;
        T& operator()()
        {
          return tabBar;
        }
        const T& operator()() const
        {
          return tabBar;
        }
      };
    };
  };

  namespace TabDateTime_
  {
    struct ColDate
    {
      struct _alias_t
      {
        static constexpr const char _literal[] = "col_date";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T>
        struct _member_t
        {
          T colDate;
          T& operator()()
          {
            return colDate;
          }
          const T& operator()() const
          {
            return colDate;
          }
        };
      };
      using _traits = sqlpp::make_traits<sqlpp::date, sqlpp::tag::can_be_null>;
    };
    struct ColDateTime
    {
      struct _alias_t
      {
        static constexpr const char _literal[] = "col_date_time";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T>
        struct _member_t
        {
          T colDateTime;
          T& operator()()
          {
            return colDateTime;
          }
          const T& operator()() const
          {
            return colDateTime;
          }
        };
      };
      using _traits = sqlpp::make_traits<sqlpp::date_time, sqlpp::tag::can_be_null>;
    };
  }

  struct TabDateTime : sqlpp::table_t<TabDateTime, TabDateTime_::ColDate, TabDateTime_::ColDateTime>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] = "tab_date_time";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template <typename T>
      struct _member_t
      {
        T tabDateTime;
        T& operator()()
        {
          return tabDateTime;
        }
        const T& operator()() const
        {
          return tabDateTime;
        }
      };
    };
  };
}
#endif
