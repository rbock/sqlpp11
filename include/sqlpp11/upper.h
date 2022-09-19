#ifndef SQLPP11_UPPER_H
#define SQLPP11_UPPER_H

#include <sqlpp11/char_sequence.h>
#include <sqlpp11/data_types/integral/data_type.h>
#include <sqlpp11/data_types/text/data_type.h>

namespace sqlpp
{
  struct upper_alias_t
  {
    struct _alias_t
    {
      static constexpr const char _literal[] = "upper_";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template <typename T>
      struct _member_t
      {
        T upper;
        T& operator()()
        {
          return upper;
        }
        const T& operator()() const
        {
          return upper;
        }
      };
    };
  };

  template <typename Flag, typename Expr>
  struct upper_t : public expression_operators<upper_t<Flag, Expr>, text>, public alias_operators<upper_t<Flag, Expr>>
  {
    using _traits = make_traits<text, tag::is_expression, tag::is_selectable>;

    using _nodes = detail::type_vector<Expr>;
    using _can_be_null = can_be_null_t<Expr>;

    using _auto_alias_t = upper_alias_t;

    upper_t(const Expr expr) : _expr(expr)
    {
    }

    upper_t(const upper_t&) = default;
    upper_t(upper_t&&) = default;
    upper_t& operator=(const upper_t&) = default;
    upper_t& operator=(upper_t&&) = default;
    ~upper_t() = default;

    Expr _expr;
  };

  template <typename Context, typename Flag, typename Expr>
  Context& serialize(const upper_t<Flag, Expr>& t, Context& context)
  {
    context << "UPPER(";
    serialize(t._expr, context);
    context << ")";
    return context;
  }

  template <typename T>
  auto upper(T t) -> upper_t<noop, wrap_operand_t<T>>
  {
    static_assert(is_expression_t<wrap_operand_t<T>>::value, "upper() requires an expression as argument");
    return {t};
  }

}  // namespace sqlpp

#endif
