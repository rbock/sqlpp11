/*
 * Copyright (c) 2013, Roland Bock
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

#ifndef SQLPP_MOCK_DB_H
#define  SQLPP_MOCK_DB_H

#include <sqlpp11/connection.h>

class DbMock: public sqlpp::connection
{
public:
	struct Printer
	{
		using _database_t = DbMock;
		Printer(std::ostream& os):
			_os(os)
		{}

		template<typename T>
			std::ostream& operator<<(T t)
			{
				return _os << t;
			}

		void flush()
		{
			_os << std::endl;
		}

		std::string escape(std::string arg)
		{
			return arg;
		}

		std::ostream& _os;
	};

	// join types
	static constexpr bool _supports_inner_join = true;
	static constexpr bool _supports_outer_join = true;
	static constexpr bool _supports_left_outer_join = true;
	static constexpr bool _supports_right_outer_join = true;

	// functions
	static constexpr bool _supports_avg = true;
	static constexpr bool _supports_count = true;
	static constexpr bool _supports_exists = true;
	static constexpr bool _supports_like = true;
	static constexpr bool _supports_in = true;
	static constexpr bool _supports_max = true;
	static constexpr bool _supports_min = true;
	static constexpr bool _supports_not_in = true;
	static constexpr bool _supports_sum = true;

	// select
	static constexpr bool _supports_group_by = true;
	static constexpr bool _supports_having = true;
	static constexpr bool _supports_limit = true;
	static constexpr bool _supports_order_by = true;
	static constexpr bool _supports_select_as_table = true;

	static constexpr bool _supports_some = true;
	static constexpr bool _supports_any = true;
	static constexpr bool _use_concat_operator = true;
	static constexpr bool _use_concat_function = true;

	const std::string& escape(const std::string& text) { return text; }
};

#endif

