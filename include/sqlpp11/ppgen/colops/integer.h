/*
 * Copyright (c) 2014-2015, niXman (i dot nixman dog gmail dot com)
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

#ifndef _sqlpp__ppgen__colops__integer_h
#define _sqlpp__ppgen__colops__integer_h

#define SQLPP_DECLARE_COLUMN_GET_TRAITS_LAZY_tinyint PROC_tinyint
#define SQLPP_DECLARE_COLUMN_GEN_TRAITS_PROC_tinyint(...) ::sqlpp::tinyint

#define SQLPP_DECLARE_COLUMN_GET_TRAITS_LAZY_smallint PROC_smallint
#define SQLPP_DECLARE_COLUMN_GEN_TRAITS_PROC_smallint(...) ::sqlpp::smallint

#define SQLPP_DECLARE_COLUMN_GET_TRAITS_LAZY_int PROC_int
#define SQLPP_DECLARE_COLUMN_GEN_TRAITS_PROC_int(...) ::sqlpp::integer

#define SQLPP_DECLARE_COLUMN_GET_TRAITS_LAZY_bigint PROC_bigint
#define SQLPP_DECLARE_COLUMN_GEN_TRAITS_PROC_bigint(...) ::sqlpp::bigint

#endif  // _sqlpp__ppgen__colops__integer_h
