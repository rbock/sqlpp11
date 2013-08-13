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

#ifndef SQLPP_DETAIL_SERIALIZE_TUPLE_H
#define SQLPP_DETAIL_SERIALIZE_TUPLE_H

#include <tuple>
#include <ostream>
#include <sqlpp11/type_traits.h>

namespace sqlpp
{
	namespace detail
	{
		template<std::size_t begin, std::size_t index, std::size_t end>
			struct tuple_serializer_impl
			{
				template<typename Db, typename Tuple>
					static void serialize(std::ostream& os, Db& db, const Tuple& flags_and_columns, char separator)
					{
						if (index > begin)
							os << separator;
						const auto& entry = std::get<index>(flags_and_columns);
						using entry_type = typename std::tuple_element<index, Tuple>::type;
						if (requires_braces_t<entry_type>::value)
							os << "(";
						entry.serialize(os, db);
						if (requires_braces_t<entry_type>::value)
							os << ")";
						tuple_serializer_impl<begin, index + 1, end>::serialize(os, db, flags_and_columns, separator);
					}
			};
		template<std::size_t begin, std::size_t end>
			struct tuple_serializer_impl<begin, end, end>
			{
				template<typename Db, typename Tuple>
					static void serialize(std::ostream& os, Db& db, const Tuple& flags_and_columns, char separator)
					{
					}
			};

		template<typename Db, typename Tuple>
			static void serialize_tuple(std::ostream& os, Db& db, const Tuple& flags_and_columns, char separator)
			{
				tuple_serializer_impl<0, 0, std::tuple_size<Tuple>::value>::serialize(os, db, flags_and_columns, separator);
			}
	}
}

#endif
