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

#ifndef SQLPP_INSERT_LIST_H
#define SQLPP_INSERT_LIST_H

#include <sqlpp11/detail/set.h>
#include <sqlpp11/detail/serialize_tuple.h>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
	namespace detail
	{
		template<typename Column>
			struct insert_column
			{
				template<typename Db>
					void serialize(std::ostream& os, Db& db) const
					{
						_column.serialize_name(os, db);
					}

				Column _column;
			};
	}
	template<template<typename> class ProhibitPredicate, typename... Assignment>
		struct insert_list_t
		{
			// check for at least one order expression
			static_assert(sizeof...(Assignment), "at least one select expression required in set()");

			// check for duplicate assignments
			static_assert(not detail::has_duplicates<Assignment...>::value, "at least one duplicate argument detected in set()");

			// check for invalid assignments
			using _assignment_set = typename detail::make_set_if<is_assignment_t, Assignment...>::type;
			static_assert(_assignment_set::size::value == sizeof...(Assignment), "at least one argument is not an assignment in set()");

			// check for prohibited assignments
			using _prohibited_assignment_set = typename detail::make_set_if<ProhibitPredicate, typename Assignment::column_type...>::type;
			static_assert(_prohibited_assignment_set::size::value == 0, "at least one assignment is prohibited by its column definition in set()");

			using _is_insert_list = tag_yes;

			insert_list_t(Assignment... assignment):
				_columns({assignment._lhs}...),
				_values(assignment._rhs...)
			{}

			insert_list_t(const insert_list_t&) = default;
			insert_list_t(insert_list_t&&) = default;
			insert_list_t& operator=(const insert_list_t&) = default;
			insert_list_t& operator=(insert_list_t&&) = default;
			~insert_list_t() = default;

			template<typename Db>
				void serialize(std::ostream& os, Db& db) const
				{
					os << " (";
					detail::serialize_tuple(os, db, _columns, ',');
					os << ") VALUES (";
					detail::serialize_tuple(os, db, _values, ',');
					os << ")";
				}

			std::tuple<detail::insert_column<typename Assignment::column_type>...> _columns;
			std::tuple<typename Assignment::value_type...> _values;
		};

}

#endif
