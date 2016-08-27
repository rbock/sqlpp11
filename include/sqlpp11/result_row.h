/*
 * Copyright (c) 2013-2016, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SQLPP_RESULT_ROW_H
#define SQLPP_RESULT_ROW_H

#include <map>
#include <sqlpp11/data_types/text.h>
#include <sqlpp11/detail/field_index_sequence.h>
#include <sqlpp11/dynamic_select_column_list.h>
#include <sqlpp11/field_spec.h>
#include <sqlpp11/no_name.h>
#include <sqlpp11/result_row_fwd.h>

namespace sqlpp
{
  namespace detail
  {
    template <typename Db, typename IndexSequence, typename... FieldSpecs>
    struct result_row_impl;

    template <typename Db, std::size_t index, typename FieldSpec>
    struct result_field : public member_t<FieldSpec, result_field_t<Db, FieldSpec>>
    {
      using _field = member_t<FieldSpec, result_field_t<Db, FieldSpec>>;

      result_field() = default;

      void _validate()
      {
        _field::operator()()._validate();
      }

      void _invalidate()
      {
        _field::operator()()._invalidate();
      }

      template <typename Target>
      void _bind(Target& target)
      {
        _field::operator()()._bind(target, index);
      }

      template <typename Target>
      void _post_bind(Target& target)
      {
        _field::operator()()._post_bind(target, index);
      }

      template <typename Callable>
      void _apply(Callable& callable) const
      {
        callable(_field::operator()());
      }

      template <typename Callable>
      void _apply(const Callable& callable) const
      {
        callable(_field::operator()());
      }
    };

    template <std::size_t index, typename AliasProvider, typename Db, typename... FieldSpecs>
    struct result_field<Db, index, multi_field_spec_t<AliasProvider, std::tuple<FieldSpecs...>>>
        : public member_t<AliasProvider,
                          result_row_impl<Db, detail::make_field_index_sequence<index, FieldSpecs...>, FieldSpecs...>>
    {
      using _multi_field =
          member_t<AliasProvider,
                   result_row_impl<Db, detail::make_field_index_sequence<index, FieldSpecs...>, FieldSpecs...>>;

      result_field() = default;

      void _validate()
      {
        _multi_field::operator()()._validate();
      }

      void _invalidate()
      {
        _multi_field::operator()()._invalidate();
      }

      template <typename Target>
      void _bind(Target& target)
      {
        _multi_field::operator()()._bind(target);
      }

      template <typename Target>
      void _post_bind(Target& target)
      {
        _multi_field::operator()()._post_bind(target);
      }

      template <typename Callable>
      void _apply(Callable& callable) const
      {
        _multi_field::operator()()._apply(callable);
      }

      template <typename Callable>
      void _apply(const Callable& callable) const
      {
        _multi_field::operator()()._apply(callable);
      }
    };

    template <typename Db, std::size_t NextIndex, std::size_t... Is, typename... FieldSpecs>
    struct result_row_impl<Db, detail::field_index_sequence<NextIndex, Is...>, FieldSpecs...>
        : public result_field<Db, Is, FieldSpecs>...
    {
      result_row_impl() = default;

      void _validate()
      {
        using swallow = int[];
        (void)swallow{(result_field<Db, Is, FieldSpecs>::_validate(), 0)...};
      }

      void _invalidate()
      {
        using swallow = int[];
        (void)swallow{(result_field<Db, Is, FieldSpecs>::_invalidate(), 0)...};
      }

      template <typename Target>
      void _bind(Target& target)
      {
        using swallow = int[];
        (void)swallow{(result_field<Db, Is, FieldSpecs>::_bind(target), 0)...};
      }

      template <typename Target>
      void _post_bind(Target& target)
      {
        using swallow = int[];
        (void)swallow{(result_field<Db, Is, FieldSpecs>::_post_bind(target), 0)...};
      }

      template <typename Callable>
      void _apply(Callable& callable) const
      {
        using swallow = int[];
        (void)swallow{(result_field<Db, Is, FieldSpecs>::_apply(callable), 0)...};
      }

      template <typename Callable>
      void _apply(const Callable& callable) const
      {
        using swallow = int[];
        (void)swallow{(result_field<Db, Is, FieldSpecs>::_apply(callable), 0)...};
      }
    };
  }

  template <typename Db, typename... FieldSpecs>
  struct result_row_t
      : public detail::result_row_impl<Db, detail::make_field_index_sequence<0, FieldSpecs...>, FieldSpecs...>
  {
    using _field_index_sequence = detail::make_field_index_sequence<0, FieldSpecs...>;
    using _impl = detail::result_row_impl<Db, _field_index_sequence, FieldSpecs...>;
    bool _is_valid;

    result_row_t() : _impl(), _is_valid(false)
    {
    }

    result_row_t(const typename dynamic_select_column_list<void>::_names_t&) : _impl(), _is_valid(false)
    {
    }

    result_row_t(const result_row_t&) = delete;
    result_row_t(result_row_t&&) = default;
    result_row_t& operator=(const result_row_t&) = delete;
    result_row_t& operator=(result_row_t&&) = default;

    void _validate()
    {
      _impl::_validate();
      _is_valid = true;
    }

    void _invalidate()
    {
      _impl::_invalidate();
      _is_valid = false;
    }

    bool operator==(const result_row_t& rhs) const
    {
      return _is_valid == rhs._is_valid;
    }

    explicit operator bool() const
    {
      return _is_valid;
    }

    static constexpr size_t static_size()
    {
      return _field_index_sequence::_next_index;
    }

    template <typename Target>
    void _bind(Target& target)
    {
      _impl::_bind(target);
    }

    template <typename Target>
    void _post_bind(Target& target)
    {
      _impl::_post_bind(target);
    }

    template <typename Callable>
    void _apply(Callable& callable) const
    {
      _impl::_apply(callable);
    }

    template <typename Callable>
    void _apply(const Callable& callable) const
    {
      _impl::_apply(callable);
    }
  };

  template <typename Db, typename... FieldSpecs>
  struct dynamic_result_row_t
      : public detail::result_row_impl<Db, detail::make_field_index_sequence<0, FieldSpecs...>, FieldSpecs...>
  {
    using _field_index_sequence = detail::make_field_index_sequence<0, FieldSpecs...>;
    using _impl = detail::result_row_impl<Db, _field_index_sequence, FieldSpecs...>;
    using _field_type = result_field_t<Db, field_spec_t<no_name_t, text, true, true>>;

    bool _is_valid;
    std::vector<std::string> _dynamic_field_names;
    std::map<std::string, _field_type> _dynamic_fields;

    dynamic_result_row_t() : _impl(), _is_valid(false)
    {
    }

    dynamic_result_row_t(const std::vector<std::string>& dynamic_field_names)
        : _impl(), _is_valid(false), _dynamic_field_names(dynamic_field_names)
    {
      for (auto field_name : _dynamic_field_names)
      {
        _dynamic_fields.insert({field_name, _field_type{}});
      }
    }

    dynamic_result_row_t(const dynamic_result_row_t&) = delete;
    dynamic_result_row_t(dynamic_result_row_t&&) = default;
    dynamic_result_row_t& operator=(const dynamic_result_row_t&) = delete;
    dynamic_result_row_t& operator=(dynamic_result_row_t&&) = default;

    void _validate()
    {
      _impl::_validate();
      _is_valid = true;
      for (auto& field : _dynamic_fields)
      {
        field.second._validate();
      }
    }

    void _invalidate()
    {
      _impl::_invalidate();
      _is_valid = false;
      for (auto& field : _dynamic_fields)
      {
        field.second._invalidate();
      }
    }

    bool operator==(const dynamic_result_row_t& rhs) const
    {
      return _is_valid == rhs._is_valid;
    }

    const _field_type& at(const std::string& field_name) const
    {
      return _dynamic_fields.at(field_name);
    }

    explicit operator bool() const
    {
      return _is_valid;
    }

    template <typename Target>
    void _bind(Target& target)
    {
      _impl::_bind(target);

      std::size_t index = _field_index_sequence::_next_index;
      for (const auto& field_name : _dynamic_field_names)
      {
        _dynamic_fields.at(field_name)._bind(target, index);
        ++index;
      }
    }

    template <typename Target>
    void _post_bind(Target& target)
    {
      _impl::_post_bind(target);

      std::size_t index = _field_index_sequence::_next_index;
      for (const auto& field_name : _dynamic_field_names)
      {
        _dynamic_fields.at(field_name)._post_bind(target, index);
        ++index;
      }
    }

    template <typename Callable>
    void _apply(Callable& callable) const
    {
      _impl::_apply(callable);

      std::size_t index = _field_index_sequence::_next_index;
      for (const auto& field_name : _dynamic_field_names)
      {
        _dynamic_fields.at(field_name)._apply(callable);
        ++index;
      }
    }

    template <typename Callable>
    void _apply(const Callable& callable) const
    {
      _impl::_apply(callable);

      std::size_t index = _field_index_sequence::_next_index;
      for (const auto& field_name : _dynamic_field_names)
      {
        _dynamic_fields.at(field_name)._apply(callable);
        ++index;
      }
    }
  };

  template <typename T>
  struct is_static_result_row_impl
  {
    using type = std::false_type;
  };

  template <typename Db, typename... FieldSpecs>
  struct is_static_result_row_impl<result_row_t<Db, FieldSpecs...>>
  {
    using type = std::true_type;
  };

  template <typename T>
  using is_static_result_row_t = typename is_static_result_row_impl<T>::type;

  template <typename Row, typename Callable>
  void for_each_field(const Row& row, const Callable& callable)
  {
    row._apply(callable);
  }

  template <typename Row, typename Callable>
  void for_each_field(const Row& row, Callable& callable)
  {
    row._apply(callable);
  }
}

#endif
