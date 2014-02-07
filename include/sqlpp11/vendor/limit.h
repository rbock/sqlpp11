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

#include <sqlpp11/type_traits.h>
#include <sqlpp11/vendor/policy_update.h>
#include <sqlpp11/vendor/crtp_wrapper.h>

namespace sqlpp
{
	namespace vendor
	{
		// LIMIT
		template<typename Limit>
			struct limit_t
			{
				using _is_limit = std::true_type;
				static_assert(is_integral_t<Limit>::value, "limit requires an integral value or integral parameter");

				limit_t(size_t value):
					_value(value)
				{}

				limit_t(const limit_t&) = default;
				limit_t(limit_t&&) = default;
				limit_t& operator=(const limit_t&) = default;
				limit_t& operator=(limit_t&&) = default;
				~limit_t() = default;

				limit_t& _limit = *this;
				Limit _value;
			};

		template<typename Database>
			struct dynamic_limit_t
			{
				using _is_limit = std::true_type;
				using _is_dynamic = std::true_type;

				dynamic_limit_t(size_t value):
					_value(value)
				{}

				dynamic_limit_t(const dynamic_limit_t&) = default;
				dynamic_limit_t(dynamic_limit_t&&) = default;
				dynamic_limit_t& operator=(const dynamic_limit_t&) = default;
				dynamic_limit_t& operator=(dynamic_limit_t&&) = default;
				~dynamic_limit_t() = default;

				void set_limit(std::size_t limit)
				{
					_value = limit;
				}

				dynamic_limit_t& _limit = *this;
				std::size_t _value; // FIXME: This should be a serializable!
			};

		struct no_limit_t
		{
			using _is_limit = std::true_type;
			no_limit_t& _limit = *this;
		};

		// CRTP Wrappers
		template<typename Derived, typename Limit>
			struct crtp_wrapper_t<Derived, limit_t<Limit>>
			{
			};

		template<typename Derived, typename Database>
			struct crtp_wrapper_t<Derived, dynamic_limit_t<Database>>
			{
			};

		template<typename Derived>
			struct crtp_wrapper_t<Derived, no_limit_t>
			{
				template<typename Arg>
					struct delayed_get_database_t
					{
						using type = get_database_t<Derived>;
					};

				template<typename Arg>
					auto limit(Arg arg)
					-> vendor::update_policies_t<Derived, no_limit_t, limit_t<Arg>>
					{
						return { static_cast<Derived&>(*this), limit_t<Arg>(arg) };
					}

				template<typename Arg>
				auto dynamic_limit(Arg arg)
					-> vendor::update_policies_t<Derived, no_limit_t, dynamic_limit_t<typename delayed_get_database_t<Arg>::type>>
					{
						static_assert(not std::is_same<get_database_t<Derived>, void>::value, "dynamic_limit must not be called in a static statement");
						return { static_cast<Derived&>(*this), dynamic_limit_t<typename delayed_get_database_t<Arg>::type>(arg) };
					}
			};

		// Interpreters
		template<typename Context, typename Database>
			struct interpreter_t<Context, dynamic_limit_t<Database>>
			{
				using T = dynamic_limit_t<Database>;

				static Context& _(const T& t, Context& context)
				{
					if (t._value > 0)
						context << " LIMIT " << t._limit;
					return context;
				}
			};

		template<typename Context, typename Limit>
			struct interpreter_t<Context, limit_t<Limit>>
			{
				using T = limit_t<Limit>;

				static Context& _(const T& t, Context& context)
				{
					context << " LIMIT ";
					interpret(t._value, context);
					return context;
				}
			};

		template<typename Context>
			struct interpreter_t<Context, no_limit_t>
			{
				using T = no_limit_t;

				static Context& _(const T& t, Context& context)
				{
					return context;
				}
			};

	}
}

#endif
