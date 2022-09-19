#ifndef SQLPP11_LOWER_H
#define SQLPP11_LOWER_H

#include <sqlpp11/char_sequence.h>
#include <sqlpp11/data_types/integral/data_type.h>
#include <sqlpp11/data_types/text/data_type.h>

namespace sqlpp
{
  struct lower_alias_t
  {
    struct _alias_t
    {
      static constexpr const char _literal[] = "lower_";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template <typename T>
      struct _member_t
      {
        T lower;
        T& operator()()
        {
          return lower;
        }
        const T& operator()() const
        {
          return lower;
        }
      };
    };
  };

  template <typename Flag, typename Expr>
  struct lower_t : public expression_operators<lower_t<Flag, Expr>, text>, public alias_operators<lower_t<Flag, Expr>>
  {
    using _traits = make_traits<text, tag::is_expression, tag::is_selectable>;

    using _nodes = detail::type_vector<Expr>;
    using _can_be_null = can_be_null_t<Expr>;

    using _auto_alias_t = lower_alias_t;

    lower_t(const Expr expr) : _expr(expr)
    {
    }

    lower_t(const lower_t&) = default;
    lower_t(lower_t&&) = default;
    lower_t& operator=(const lower_t&) = default;
    lower_t& operator=(lower_t&&) = default;
    ~lower_t() = default;

    Expr _expr;
  };

  template <typename Context, typename Flag, typename Expr>
  Context& serialize(const lower_t<Flag, Expr>& t, Context& context)
  {
    context << "LOWER(";
    serialize(t._expr, context);
    context << ")";
    return context;
  }

  template <typename T>
  auto lower(T t) -> lower_t<noop, wrap_operand_t<T>>
  {
    static_assert(is_expression_t<wrap_operand_t<T>>::value, "lower() requires an expression as argument");
    return {t};
  }

}  // namespace sqlpp

#endif
