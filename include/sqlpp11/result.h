#pragma once

/*
 * Copyright (c) 2013-2017, Roland Bock, Aaron Bishop
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

#include <functional>
#include <iterator>
#include <utility>

namespace sqlpp
{
  template <typename>
  struct iterator_category
  {
    using type = std::input_iterator_tag;
  };

  namespace detail
  {
    template<class DbResult, class = void>
    struct result_has_size : std::false_type {};

    template<class DbResult>
    struct result_has_size<DbResult, void_t<decltype(std::declval<DbResult>().size())>>
      : std::true_type {};

    template<class DbResult, class = void>
    struct result_size_type { using type = void; };

    template<class DbResult>
    struct result_size_type<DbResult, void_t<decltype(std::declval<DbResult>().size())>>
    {
      using type = decltype(std::declval<DbResult>().size());
    };
  }

  template <typename DbResult, typename ResultRow>
  class result_t
  {
    using db_result_t = DbResult;
    using result_row_t = ResultRow;

    db_result_t _result;
    result_row_t _result_row;
    db_result_t _end;
    result_row_t _end_row;

  public:
    result_t() = default;

    template <typename DynamicNames>
    result_t(db_result_t&& result, const DynamicNames& dynamic_names)
        : _result(std::move(result)), _result_row(dynamic_names)
    {
      _result.next(_result_row);
    }

    result_t(const result_t&) = delete;
    result_t(result_t&&) = default;
    result_t& operator=(const result_t&) = delete;
    result_t& operator=(result_t&&) = default;

    // Iterator
    class iterator
    {
    public:
      using iterator_category = typename iterator_category<DbResult>::type;
      using value_type = result_row_t;
      using pointer = const result_row_t*;
      using reference = const result_row_t&;
      using difference_type = std::ptrdiff_t;

      iterator(std::reference_wrapper<db_result_t> result,
               std::reference_wrapper<result_row_t> result_row) :
          _result(std::move(result)), _result_row(std::move(result_row))
      {
      }

      reference operator*() const
      {
        return _result_row;
      }

      pointer operator->() const
      {
        return &_result_row.get();
      }

      bool operator==(const iterator& rhs) const
      {
        return _result_row.get() == rhs._result_row.get();
      }

      bool operator!=(const iterator& rhs) const
      {
        return not(operator==(rhs));
      }

      iterator& operator++()
      {
        _result.get().next(_result_row.get());
        return *this;
      }

      iterator operator++(int)
      {
        auto previous_it = *this;
        _result.next(_result_row.get());
        return previous_it;
      }

      std::reference_wrapper<db_result_t> _result;
      std::reference_wrapper<result_row_t> _result_row;
    };

    iterator begin()
    {
      return iterator(std::ref(_result), std::ref(_result_row));
    }

    iterator end()
    {
      return iterator(std::ref(_end), std::ref(_end_row));
    }

    const result_row_t& front() const
    {
      return _result_row;
    }

    bool empty() const
    {
      return _result_row == _end_row;
    }

    void pop_front()
    {
      _result.next(_result_row);
    }

    template<class Size = typename detail::result_size_type<DbResult>::type>
    Size size() const
    {
      static_assert(detail::result_has_size<DbResult>::value, "Underlying connector does not support size()");
      return _result.size();
    }
  };
}  // namespace sqlpp
