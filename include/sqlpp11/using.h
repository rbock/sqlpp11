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

#ifndef SQLPP_USING_H
#define SQLPP_USING_H

#include <ostream>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/detail/set.h>
#include <sqlpp11/detail/serializable_list.h>
#include <sqlpp11/detail/serialize_tuple.h>

namespace sqlpp
{
	template<typename Database, typename... Table>
		struct using_t
		{
			using _is_using = std::true_type;
			using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;
			using _parameter_tuple_t = std::tuple<Table...>;

			static_assert(_is_dynamic::value or sizeof...(Table), "at least one table argument required in using()");

			// check for duplicate arguments
			static_assert(not detail::has_duplicates<Table...>::value, "at least one duplicate argument detected in using()");

			// check for invalid arguments
			using _valid_expressions = typename detail::make_set_if<is_table_t, Table...>::type;
			static_assert(_valid_expressions::size::value == sizeof...(Table), "at least one argument is not an table in using()");


			template<typename T>
				void add(T&& table)
				{
					static_assert(is_table_t<typename std::decay<T>::type>::value, "using() arguments require to be tables");
					_dynamic_tables.emplace_back(std::forward<T>(table));
				}

			template<typename Db>
				void serialize(std::ostream& os, Db& db) const
				{
					if (sizeof...(Table) == 0 and _dynamic_tables.empty())
						return;
					os << " USING ";
					detail::serialize_tuple(os, db, _tables, ',');
					_dynamic_tables.serialize(os, db, sizeof...(Table) == 0);
				}

			size_t _set_parameter_index(size_t index)
			{
				index = set_parameter_index(_tables, index);
				return index;
			}

			_parameter_tuple_t _tables;
			detail::serializable_list<Database> _dynamic_tables;
		};

}

#endif
