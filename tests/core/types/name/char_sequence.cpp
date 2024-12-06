/*
 * Copyright (c) 2024, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Sample.h"
#include <sqlpp11/sqlpp11.h>
#include "../../../include/test_helpers.h"

namespace A {
SQLPP_CREATE_NAME_TAG(cheese);
SQLPP_CREATE_QUOTED_NAME_TAG(cake); // Quoted
}

namespace B {
SQLPP_CREATE_NAME_TAG(cheese);
SQLPP_CREATE_NAME_TAG(cake); // Unquoted
}

int main()
{
  using ACheese = sqlpp::make_char_sequence_t<decltype(A::cheese)>;
  using ACake = sqlpp::make_char_sequence_t<decltype(A::cake)>;
  using BCheese = sqlpp::make_char_sequence_t<decltype(B::cheese)>;
  using BCake = sqlpp::make_char_sequence_t<decltype(B::cake)>;

  static_assert(std::is_same<ACheese, BCheese>::value, "");
  static_assert(std::is_same<ACake, BCake>::value, "");

  static_assert(not std::is_same<ACheese, ACake>::value, "");
  static_assert(not std::is_same<ACheese, BCake>::value, "");

  static_assert(not std::is_same<BCheese, ACake>::value, "");
  static_assert(not std::is_same<BCheese, BCake>::value, "");
}


