#ifndef TEST_SAMPLE_H
#define TEST_SAMPLE_H

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
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
      using _traits = sqlpp::make_traits<sqlpp::varchar>;
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
    struct Psi
    {
      struct _alias_t
      {
        static constexpr const char _literal[] = "psi";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T>
        struct _member_t
        {
          T psi;
          T& operator()()
          {
            return psi;
          }
          const T& operator()() const
          {
            return psi;
          }
        };
      };
      using _traits = sqlpp::make_traits<sqlpp::bigint_unsigned, sqlpp::tag::can_be_null>;
    };
  }

  struct TabFoo : sqlpp::table_t<TabFoo, TabFoo_::Delta, TabFoo_::Epsilon, TabFoo_::Omega, TabFoo_::Psi>
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
    struct ColDayPoint
    {
      struct _alias_t
      {
        static constexpr const char _literal[] = "col_day_point";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T>
        struct _member_t
        {
          T colDayPoint;
          T& operator()()
          {
            return colDayPoint;
          }
          const T& operator()() const
          {
            return colDayPoint;
          }
        };
      };
      using _traits = sqlpp::make_traits<sqlpp::day_point, sqlpp::tag::can_be_null>;
    };
    struct ColTimePoint
    {
      struct _alias_t
      {
        static constexpr const char _literal[] = "col_time_point";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T>
        struct _member_t
        {
          T colTimePoint;
          T& operator()()
          {
            return colTimePoint;
          }
          const T& operator()() const
          {
            return colTimePoint;
          }
        };
      };
      using _traits = sqlpp::make_traits<sqlpp::time_point, sqlpp::tag::can_be_null>;
    };

    struct ColTimeOfDay
    {
      struct _alias_t
      {
        static constexpr const char _literal[] = "col_time_of_day";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T>
        struct _member_t
        {
          T colTimeOfDay;
          T& operator()()
          {
            return colTimeOfDay;
          }
          const T& operator()() const
          {
            return colTimeOfDay;
          }
        };
      };
      using _traits = sqlpp::make_traits<sqlpp::time_of_day, sqlpp::tag::can_be_null>;
    };
  }

  struct TabDateTime
      : sqlpp::table_t<TabDateTime, TabDateTime_::ColDayPoint, TabDateTime_::ColTimePoint, TabDateTime_::ColTimeOfDay>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] = "tab_time_point";
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
