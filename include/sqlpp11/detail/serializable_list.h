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

#ifndef SQLPP_SERIALIZABLE_LIST_H
#define SQLPP_SERIALIZABLE_LIST_H

#include <vector>
#include <sqlpp11/detail/serializable.h>

namespace sqlpp
{
	namespace detail
	{
		template<typename Db>
			struct serializable_list
			{
				std::vector<detail::serializable_t<Db>> _serializables;

				std::size_t size() const
				{
					return _serializables.size();
				}

				bool empty() const
				{
					return _serializables.empty();
				}

				template<typename Expr>
				void emplace_back(Expr&& expr)
				{
					_serializables.emplace_back(std::forward<Expr>(expr));
				}

				void serialize(std::ostream& os, Db& db, bool first) const
				{
					for (const auto entry : _serializables)
					{
						if (not first)
							os << ',';
						entry.serialize(os, db);
						first = false;
					}
				}

				template<typename Separator>
				void serialize(std::ostream& os, Db& db, const Separator& separator, bool first) const
				{
					for (const auto entry : _serializables)
					{
						if (not first)
							os << separator;
						entry.serialize(os, db);
						first = false;
					}
				}

			};

		template<>
			struct serializable_list<void>
			{
				template<typename T>
					void emplace_back(const T&) {}

				constexpr std::size_t size() const
				{
					return 0;
				}

				constexpr bool empty() const
				{
					return true;
				}

				template<typename Db>
					void serialize(std::ostream&, Db&, bool) const
					{
					}

				template<typename Db, typename Separator>
				void serialize(std::ostream& os, Db& db, const Separator& separator, bool first) const
				{
				}

			};

	}
}

#endif
