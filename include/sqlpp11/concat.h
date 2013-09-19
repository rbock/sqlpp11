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

#ifndef SQLPP_CONCAT_H
#define SQLPP_CONCAT_H

#include <sstream>
#include <sqlpp11/text.h>
#include <sqlpp11/detail/set.h>

namespace sqlpp
{
	namespace detail
	{
		struct text;

		template<typename Text, typename... Args>
		struct concat_t: public Text::template operators<concat_t<Text, Args...>>
		{
			using _valid_args = typename detail::make_set_if_not<is_text_t, Args...>::type;
			static_assert(_valid_args::size::value == 0, "at least one non-text argument detected in concat()");

			using _value_type = Text;
			struct _name_t
			{
				static constexpr const char* _get_name() { return "CONCAT"; }
				template<typename T>
					struct _member_t
					{
						T concat;
					};
			};

			concat_t(Args&&... args):
				_args(std::move(args)...)
			{}

			concat_t(const Args&... args):
				_args(args...)
			{}

			concat_t(const concat_t&) = default;
			concat_t(concat_t&&) = default;
			concat_t& operator=(const concat_t&) = default;
			concat_t& operator=(concat_t&&) = default;
			~concat_t() = default;

			template<typename Db>
				void serialize(std::ostream& os, Db& db) const
				{
					os << "CONCAT(";
					detail::serialize_tuple(os, db, _args, ',');
					os << ")";
				}

		private:
			std::tuple<Args...> _args;
		};
	}

	template<typename... T>
	auto concat(T&&... t) -> typename detail::concat_t<detail::text, typename std::decay<T>::type...>::type
	{
		return { std::forward<T>(t)... };
	}

}

#endif
