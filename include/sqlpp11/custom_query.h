/*
 * Copyright (c) 2013-2014, Roland Bock
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

#ifndef SQLPP_CUSTOM_QUERY_H
#define SQLPP_CUSTOM_QUERY_H

#include <sqlpp11/connection.h>

namespace sqlpp
{
	template<typename Database, typename... Parts>
	struct custom_query_t/*:
		public FirstPart::_result_type_provider::template _result_methods_t<custom_query_t>*/
	{
			custom_query_t(Parts... parts):
				_parts(parts...)
			{}

			custom_query_t(const custom_query_t&) = default;
			custom_query_t(custom_query_t&&) = default;
			custom_query_t& operator=(const custom_query_t&) = default;
			custom_query_t& operator=(custom_query_t&&) = default;
			~custom_query_t() = default;

		std::tuple<Parts...> _parts;
	};

	template<typename Context, typename Database, typename... Parts>
		struct serializer_t<Context, custom_query_t<Database, Parts...>>
		{
			using T = custom_query_t<Database, Parts...>;

			static Context& _(const T& t, Context& context)
			{
				interpret_tuple_without_braces(t._parts, " ", context);
				return context;
			}
		};

	template<typename... Parts>
		auto custom_query(Parts... parts)
		-> custom_query_t<void, Parts...>
		{
			return custom_query_t<void, Parts...>(parts...);
		}

	template<typename Database, typename... Parts>
		auto dynamic_custom_query(const Database&, Parts...)
		-> custom_query_t<Database, Parts...>
		{
			static_assert(std::is_base_of<connection, Database>::value, "Invalid database parameter");
			return { };
		}
}
#endif
