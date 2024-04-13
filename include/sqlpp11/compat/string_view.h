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
#include <string_view>
namespace sqlpp
{
  using string_view = std::string_view;
}  // namespace sqlpp

#else // incomplete backport of std::string_view

#include <utility>
#include <string>
#include <iosfwd>

namespace sqlpp
{
  class string_view
  {
    const char* _data = nullptr;
    size_t _size = 0u;
  public:
    constexpr string_view() = default;
    string_view(const std::string& source) : _data(source.data()), _size(source.size())
    {
    }

    constexpr string_view(const char* data, size_t size) : _data(data), _size(size)
    {
    }

    string_view(const char* data) : _data(data), _size(std::char_traits<char>::length(data))
    {
    }

    const char* data() const
    {
      return _data;
    }

    size_t size() const
    {
      return _size;
    }

    operator std::string() const
    {
      return std::string(_data, _size);
    }
  };

  inline bool operator==(const string_view& left, const string_view& right)
  {
    if (left.size() != right.size())
      return false;
    return std::char_traits<char>::compare(left.data(), right.data(), left.size()) == 0;
  }

  inline bool operator!=(const string_view& left, const string_view& right)
  {
    if (left.size() != right.size())
      return true;
    return std::char_traits<char>::compare(left.data(), right.data(), left.size()) != 0;
  }

  inline std::ostream& operator<<(std::ostream& os, const string_view& sv)
  {
    return os << std::string(sv);
  }


}  // namespace sqlpp

#endif
