#pragma once

#include <sqlpp11/char_sequence.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/table.h>

namespace IntegralSample_
{
  struct SignedValue
  {
    struct _alias_t
    {
      static constexpr const char _literal[] = "signed_value";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template <typename T>
      struct _member_t
      {
        T signedValue;
        T& operator()()
        {
          return signedValue;
        }
        const T& operator()() const
        {
          return signedValue;
        }
      };
    };
    using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
  };
  struct UnsignedValue
  {
    struct _alias_t
    {
      static constexpr const char _literal[] = "unsigned_value";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template <typename T>
      struct _member_t
      {
        T unsignedValue;
        T& operator()()
        {
          return unsignedValue;
        }
        const T& operator()() const
        {
          return unsignedValue;
        }
      };
    };
    using _traits = sqlpp::make_traits<sqlpp::integer_unsigned, sqlpp::tag::can_be_null>;
  };

}  // namespace IntegralSample_

struct IntegralSample : sqlpp::table_t<IntegralSample, IntegralSample_::SignedValue, IntegralSample_::UnsignedValue>
{
  struct _alias_t
  {
    static constexpr const char _literal[] = "integral_sample";
    using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
    template <typename T>
    struct _member_t
    {
      T fpSample;
      T& operator()()
      {
        return fpSample;
      }
      const T& operator()() const
      {
        return fpSample;
      }
    };
  };
};
