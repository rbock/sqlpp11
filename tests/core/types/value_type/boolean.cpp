/*
 * Copyright (c) 2024, Roland Bock
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

#include <sqlpp11/sqlpp11.h>

template <typename T> void test_boolean() {
  static_assert(std::is_same<sqlpp::value_type_of_t<T>, sqlpp::boolean>::value,
                "");
  static_assert(sqlpp::is_boolean<T>::value, "");
  static_assert(sqlpp::is_numeric<T>::value, "");

  static_assert(not sqlpp::is_integral<T>::value, "");
  static_assert(not sqlpp::is_unsigned_integral<T>::value, "");
  static_assert(not sqlpp::is_floating_point<T>::value, "");
  static_assert(not sqlpp::is_text<T>::value, "");
  static_assert(not sqlpp::is_blob<T>::value, "");
  static_assert(not sqlpp::is_time_point<T>::value, "");
  static_assert(not sqlpp::is_day_point<T>::value, "");
  static_assert(not sqlpp::is_time_of_day<T>::value, "");
}

int main() {
  test_boolean<bool>();
  test_boolean<sqlpp::boolean>();
}
