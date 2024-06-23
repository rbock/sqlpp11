#pragma once

/*
 * Copyright (c) 2024, Roland Bock
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

#ifdef _MSVC_LANG
#define CXX_STD_VER _MSVC_LANG
#else
#define CXX_STD_VER __cplusplus
#endif

#if CXX_STD_VER >= 201703L
#include <optional>
namespace sqlpp
{
  namespace compat
  {
  template <class T>
  using optional = std::optional<T>;

  using std::nullopt_t;
  using std::nullopt;

  using std::bad_optional_access;
  using std::make_optional;

  }  // namespace compat
}  // namespace sqlpp

#else // incomplete backport of std::optional

#include <utility>
#include <stdexcept>

namespace sqlpp
{
  namespace compat
  {
    class nullopt_t
    {
    };
    constexpr nullopt_t nullopt;

    class bad_optional_access : public std::exception
    {
    public:
      ~bad_optional_access() override = default;
      const char* what() const noexcept override
      {
        return "bad optional access";
      }
    };

    template <class T>
    class optional
    {
      // Unnamed union, injecting members into scope.
      union
      {
        char _nothing;
        T _value;
      };
      bool _active = false;

      // Placement new
      template <typename... Args>
      void create(Args&&... args)
      {
        new ((void*)std::addressof(_value)) T(std::forward<Args>(args)...);
        _active = true;
      }

      void destroy()
      {
        if (_active)
        {
          _value.~T();
        }
      }

    public:
      optional() noexcept : _nothing(), _active(false)
      {
      }
      optional(T t) : _active(true)
      {
        create(std::move(t));
      }

      optional(const optional&) = default;
      optional(optional&&) = default;
      optional(const nullopt_t&) noexcept
      {
      }

      optional& operator=(const optional&) = default;
      optional& operator=(optional&&) = default;
      optional& operator=(const nullopt_t&) noexcept
      {
        destroy();
      }

      ~optional()
      {
        destroy();
      }

      bool has_value() const
      {
        return _active;
      }

      explicit operator bool() const
      {
        return _active;
      }

      T& operator*()
      {
        return _value;
      }

      const T& operator*() const
      {
        return _value;
      }

      const T& operator->() const
      {
        return _value;
      }

      template <typename... Args>
      optional& emplace(Args&&... args)
      {
        create(std::forward<Args>(args)...);
      }

      T& value()
      {
        if (_active)
          return _value;
        throw bad_optional_access();
      }

      template <typename U>
      T value_or(U&& u)
      {
        if (_active)
          return _value;
        return std::forward<U>(u);
      }

      const T& value() const
      {
        if (_active)
          return _value;
        throw bad_optional_access();
      }

      void reset()
      {
        destroy();
      }
    };

    template <class L, class R>
    bool operator==(const optional<L>& left, const optional<R>& right)
    {
      if (static_cast<bool>(left) != static_cast<bool>(right))
        return false;
      if (!static_cast<bool>(left))
        return true;
      return *left == *right;
    }

    template <class L, class R>
    bool operator==(const optional<L>& left, const R& right)
    {
      if (!static_cast<bool>(left))
        return false;
      return *left == right;
    }

    template <class L, class R>
    bool operator==(const L& left, const optional<R>& right)
    {
      if (!static_cast<bool>(right))
        return false;
      return left == *right;
    }

    template <class L, class R>
    bool operator!=(const optional<L>& left, const optional<R>& right)
    {
      if (static_cast<bool>(left) != static_cast<bool>(right))
        return true;
      if (!static_cast<bool>(left))
        return false;
      return *left != *right;
    }

    template <class L, class R>
    bool operator!=(const optional<L>& left, const R& right)
    {
      if (!static_cast<bool>(left))
        return true;
      return *left != right;
    }

    template <class L, class R>
    bool operator!=(const L& left, const optional<R>& right)
    {
      if (!static_cast<bool>(right))
        return true;
      return left != *right;
    }

    template <class T>
    bool operator==(const optional<T>& left, const nullopt_t&)
    {
      return !left;
    }

    template <class T>
    bool operator==(const nullopt_t& n, const optional<T>& right)
    {
      return !right;
    }

    template <typename T>
    constexpr optional<typename std::decay<T>::type> make_optional(T&& value)
    {
      return optional<typename std::decay<T>::type>(std::forward<T>(value));
    }

  }  // namespace compat
}  // namespace sqlpp

#endif
