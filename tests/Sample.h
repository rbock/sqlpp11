#ifndef TEST_SAMPLE_H
#define TEST_SAMPLE_H

#include <sqlpp11/table.h>
#include <sqlpp11/column_types.h>

namespace test
{
  namespace TabFoo_
  {
    struct Delta
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "delta"; }
        template<typename T>
        struct _member_t
          {
            T delta;
            T& operator()() { return delta; }
            const T& operator()() const { return delta; }
          };
      };
      using _value_type = sqlpp::varchar;
      struct _column_type
      {
        using _can_be_null = std::true_type;
      };
    };
    struct Epsilon
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "epsilon"; }
        template<typename T>
        struct _member_t
          {
            T epsilon;
            T& operator()() { return epsilon; }
            const T& operator()() const { return epsilon; }
          };
      };
      using _value_type = sqlpp::bigint;
      struct _column_type
      {
        using _can_be_null = std::true_type;
      };
    };
    struct Omega
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "omega"; }
        template<typename T>
        struct _member_t
          {
            T omega;
            T& operator()() { return omega; }
            const T& operator()() const { return omega; }
          };
      };
      using _value_type = sqlpp::floating_point;
      struct _column_type
      {
        using _can_be_null = std::true_type;
      };
    };
  }

  struct TabFoo: sqlpp::table_t<TabFoo,
               TabFoo_::Delta,
               TabFoo_::Epsilon,
               TabFoo_::Omega>
  {
    using _value_type = sqlpp::no_value_t;
    struct _name_t
    {
      static constexpr const char* _get_name() { return "tab_foo"; }
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
    struct Alpha
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "alpha"; }
        template<typename T>
        struct _member_t
          {
            T alpha;
            T& operator()() { return alpha; }
            const T& operator()() const { return alpha; }
          };
      };
      using _value_type = sqlpp::bigint;
      struct _column_type
      {
        using _must_not_insert = std::true_type;
        using _must_not_update = std::true_type;
        using _can_be_null = std::true_type;
      };
    };
    struct Beta
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "beta"; }
        template<typename T>
        struct _member_t
          {
            T beta;
            T& operator()() { return beta; }
            const T& operator()() const { return beta; }
          };
      };
      using _value_type = sqlpp::varchar;
      struct _column_type
      {
        using _can_be_null = std::true_type;
      };
    };
    struct Gamma
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "gamma"; }
        template<typename T>
        struct _member_t
          {
            T gamma;
            T& operator()() { return gamma; }
            const T& operator()() const { return gamma; }
          };
      };
      using _value_type = sqlpp::boolean;
      struct _column_type
      {
        using _require_insert = std::true_type;
      };
    };
    struct Delta
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "delta"; }
        template<typename T>
        struct _member_t
          {
            T delta;
            T& operator()() { return delta; }
            const T& operator()() const { return delta; }
          };
      };
      using _value_type = sqlpp::integer;
      struct _column_type
      {
        using _can_be_null = std::true_type;
      };
    };
  }

  struct TabBar: sqlpp::table_t<TabBar,
               TabBar_::Alpha,
               TabBar_::Beta,
               TabBar_::Gamma,
               TabBar_::Delta>
  {
    using _value_type = sqlpp::no_value_t;
    struct _name_t
    {
      static constexpr const char* _get_name() { return "tab_bar"; }
      template<typename T>
      struct _member_t
      {
        T tabBar;
        T& operator()() { return tabBar; }
        const T& operator()() const { return tabBar; }
      };
    };
  };
}
#endif
