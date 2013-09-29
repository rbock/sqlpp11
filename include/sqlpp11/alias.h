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

#ifndef SQLPP_ALIAS_H
#define SQLPP_ALIAS_H

#include <sqlpp11/type_traits.h>
namespace sqlpp
{
	namespace alias
	{
#define SQLPP_ALIAS_PROVIDER_GENERATOR(name) \
		struct name##_t\
		{\
			struct _name_t\
			{\
				static constexpr const char* _get_name() { return #name; }\
				template<typename T>\
				struct _member_t\
				{\
					template<typename... TT>\
					_member_t(TT&&... t): name(std::forward<TT>(t)...) {}\
					\
					template<typename TT>\
					_member_t& operator=(TT&& t) { name = std::forward<TT>(t); return *this; }\
					\
					T name;\
				};\
			};\
		};\
		constexpr name##_t name = {};

		SQLPP_ALIAS_PROVIDER_GENERATOR(a);
		SQLPP_ALIAS_PROVIDER_GENERATOR(b);
		SQLPP_ALIAS_PROVIDER_GENERATOR(c);
		SQLPP_ALIAS_PROVIDER_GENERATOR(d);
		SQLPP_ALIAS_PROVIDER_GENERATOR(e);
		SQLPP_ALIAS_PROVIDER_GENERATOR(f);
		SQLPP_ALIAS_PROVIDER_GENERATOR(g);
		SQLPP_ALIAS_PROVIDER_GENERATOR(h);
		SQLPP_ALIAS_PROVIDER_GENERATOR(i);
		SQLPP_ALIAS_PROVIDER_GENERATOR(j);
		SQLPP_ALIAS_PROVIDER_GENERATOR(k);
		SQLPP_ALIAS_PROVIDER_GENERATOR(l);
		SQLPP_ALIAS_PROVIDER_GENERATOR(m);
		SQLPP_ALIAS_PROVIDER_GENERATOR(n);
		SQLPP_ALIAS_PROVIDER_GENERATOR(o);
		SQLPP_ALIAS_PROVIDER_GENERATOR(p);
		SQLPP_ALIAS_PROVIDER_GENERATOR(q);
		SQLPP_ALIAS_PROVIDER_GENERATOR(s);
		SQLPP_ALIAS_PROVIDER_GENERATOR(t);
		SQLPP_ALIAS_PROVIDER_GENERATOR(u);
		SQLPP_ALIAS_PROVIDER_GENERATOR(v);
		SQLPP_ALIAS_PROVIDER_GENERATOR(w);
		SQLPP_ALIAS_PROVIDER_GENERATOR(x);
		SQLPP_ALIAS_PROVIDER_GENERATOR(y);
		SQLPP_ALIAS_PROVIDER_GENERATOR(z);
		SQLPP_ALIAS_PROVIDER_GENERATOR(left);
		SQLPP_ALIAS_PROVIDER_GENERATOR(right);
	};

	template<typename Expression, typename AliasProvider>
		struct expression_alias_t
	{
		struct _value_type: Expression::_value_type
		{
			using _is_expression = std::false_type;
			using _is_named_expression = std::true_type;
			using _is_alias = std::true_type;
		};

		using _name_t = typename AliasProvider::_name_t;

		template<typename Db>
			void serialize(std::ostream& os, Db& db) const
			{
				os << "("; _expression.serialize(os, db); os << ") AS " << _name_t::_get_name();
			}

		Expression _expression;
	};

}

#endif
