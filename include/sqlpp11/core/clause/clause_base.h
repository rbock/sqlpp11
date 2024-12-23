#pragma once

/*
Copyright (c) 2016 - 2018, Roland Bock
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this
   list of conditions and the following disclaimer in the documentation and/or
   other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <utility>

namespace sqlpp
{
  template <typename Clause, typename Statement>
  struct clause_base
  {
    using _data_t = Clause;

    clause_base() = default;
    template<typename OtherStatement>
    clause_base(clause_base<Clause, OtherStatement> base) : _data{std::move(base._data)}
    {
    }

    clause_base(_data_t data) : _data{std::move(data)}
    {
    }

    _data_t _data;
  };

  template <typename Clause, typename Statement>
  struct clause_data
  {
    using _data_t = Clause;

    clause_data() = default;
    template<typename OtherStatement>
    clause_data(clause_base<Clause, OtherStatement> base) : _data{std::move(base._data)}
    {
    }

    clause_data(_data_t data) : _data{std::move(data)}
    {
    }

    _data_t _data;
  };
}  // namespace sqlpp
