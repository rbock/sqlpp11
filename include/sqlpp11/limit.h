/*
 * Copyright (c) 2013, Roland Bock
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

#ifndef SQLPP_LIMIT_H
#define SQLPP_LIMIT_H

#include <ostream>
#include <sqlpp11/select_fwd.h>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
	struct limit_t
	{
		using _is_limit = std::true_type;

		template<typename Db>
			void serialize(std::ostream& os, Db& db) const
			{
				static_assert(Db::_supports_limit, "limit not supported by current database");
				os << " LIMIT " << _limit;
			}

		std::size_t _limit;
	};

	struct dynamic_limit_t
	{
		using _is_limit = std::true_type;
		using _is_dynamic = std::true_type;

		dynamic_limit_t():
			_limit(0)
		{}

		dynamic_limit_t(const dynamic_limit_t&) = default;
		dynamic_limit_t(dynamic_limit_t&&) = default;
		dynamic_limit_t& operator=(const dynamic_limit_t&) = default;
		dynamic_limit_t& operator=(dynamic_limit_t&&) = default;
		~dynamic_limit_t() = default;

		void set(std::size_t limit)
		{
			_limit = limit;
		}

		template<typename Db>
			void serialize(std::ostream& os, Db& db) const
			{
				static_assert(Db::_supports_limit, "limit not supported by current database");
				if (_limit > 0)
					os << " LIMIT " << _limit;
			}

		std::size_t _limit = 0;
	};
}

#endif
