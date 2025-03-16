/*
 * Copyright (c) 2016-2016, Roland Bock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sqlpp23/sqlpp23.h>
#include <sqlpp23/tests/core/serialize_helpers.h>
#include <sqlpp23/tests/core/tables.h>

#include <iostream>

int Insert(int, char*[]) {
  const auto bar = test::TabBar{};

  SQLPP_COMPARE(insert_into(bar).default_values(),
                "INSERT INTO tab_bar DEFAULT VALUES");
  SQLPP_COMPARE(
      insert_into(bar).set(bar.textN = "cheesecake", bar.boolNn = true),
      "INSERT INTO tab_bar (text_n, bool_nn) VALUES('cheesecake', 1)");
  SQLPP_COMPARE(
      insert_into(bar).set(bar.textN = std::nullopt, bar.boolNn = true),
      "INSERT INTO tab_bar (text_n, bool_nn) VALUES(NULL, 1)");
  std::string_view cheeseCake = "cheesecake";
  SQLPP_COMPARE(
      insert_into(bar).set(bar.textN = std::string(cheeseCake),
                           bar.boolNn = true),
      "INSERT INTO tab_bar (text_n, bool_nn) VALUES('cheesecake', 1)");
  SQLPP_COMPARE(
      insert_into(bar).set(bar.textN = std::string_view(cheeseCake),
                           bar.boolNn = true),
      "INSERT INTO tab_bar (text_n, bool_nn) VALUES('cheesecake', 1)");

  return 0;
}
