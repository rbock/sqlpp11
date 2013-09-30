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

#include <vector>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/detail/set.h>
#include <sqlpp11/detail/serialize_tuple.h>
#include <sqlpp11/detail/serializable.h>

namespace sqlpp
{
	namespace detail
	{
		template<typename Db>
			void serialize_empty_insert_list(std::ostream& os, const Db& db)
			{

				if (connector_has_empty_list_insert_t<typename std::decay<Db>::type>::value)
					os << " () VALUES()";
				else
					os << " DEFAULT VALUES";
			}

		template<typename Db>
			struct dynamic_column_list
			{
				using type = std::vector<detail::serializable_t<Db>>;
			};

		template<>
			struct dynamic_column_list<void>
			{
				using type = std::vector<noop>;
			};

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
	template<typename Database, template<typename> class ProhibitPredicate, typename... Assignments>
		struct insert_list_t
		{
			using _is_insert_list = std::true_type;
			using _is_dynamic = typename std::conditional<std::is_same<Database, void>::value, std::false_type, std::true_type>::type;

			// check for at least one order expression
			static_assert(_is_dynamic::value or sizeof...(Assignments), "at least one select expression required in set()");

			// check for duplicate assignments
			static_assert(not detail::has_duplicates<Assignments...>::value, "at least one duplicate argument detected in set()");

			// check for invalid assignments
			using _assignment_set = typename detail::make_set_if<is_assignment_t, Assignments...>::type;
			static_assert(_assignment_set::size::value == sizeof...(Assignments), "at least one argument is not an assignment in set()");

			// check for prohibited assignments
			using _prohibited_assignment_set = typename detail::make_set_if<ProhibitPredicate, typename Assignments::column_type...>::type;
			static_assert(_prohibited_assignment_set::size::value == 0, "at least one assignment is prohibited by its column definition in set()");

			insert_list_t(Assignments... assignment):
				_columns({assignment._lhs}...),
				_values(assignment._rhs...)
			{}

			insert_list_t(const insert_list_t&) = default;
			insert_list_t(insert_list_t&&) = default;
			insert_list_t& operator=(const insert_list_t&) = default;
			insert_list_t& operator=(insert_list_t&&) = default;
			~insert_list_t() = default;

			template<typename Assignment>
				void add(Assignment&& assignment)
				{
					static_assert(is_assignment_t<typename std::decay<Assignment>::type>::value, "set() arguments require to be assigments");
					static_assert(not ProhibitPredicate<typename std::decay<Assignment>::type>::value, "set() argument must not be used in insert");
					_dynamic_columns.push_back(std::forward<typename Assignment::column_type>(assignment._lhs));
					_dynamic_values.push_back(std::forward<typename Assignment::value_type>(assignment._rhs));
				}

			template<typename Db>
				void serialize(std::ostream& os, Db& db) const
				{
					if (sizeof...(Assignments) + _dynamic_columns.size())
						detail::serialize_empty_insert_list(os, db);
					else
					{
						os << " (";
						detail::serialize_tuple(os, db, _columns, ',');
						{
							bool first = sizeof...(Assignments) == 0;
							for (const auto column : _dynamic_columns)
							{
								if (not first)
									os << ',';
								column.serialize(os, db);
								first = false;
							}
						}
						os << ") VALUES (";
						detail::serialize_tuple(os, db, _values, ',');
						{
							bool first = sizeof...(Assignments) == 0;
							for (const auto column : _dynamic_values)
							{
								if (not first)
									os << ',';
								column.serialize(os, db);
								first = false;
							}
						}
						os << ")";
					}
				}

			std::tuple<detail::insert_column<typename Assignments::column_type>...> _columns;
			std::tuple<typename Assignments::value_type...> _values;
			typename detail::dynamic_column_list<Database>::type _dynamic_columns;
			typename detail::dynamic_column_list<Database>::type _dynamic_values;
		};

}

#endif
