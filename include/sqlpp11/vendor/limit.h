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

				limit_t(Limit value):
					_value(value)
				{}

				limit_t(const limit_t&) = default;
				limit_t(limit_t&&) = default;
				limit_t& operator=(const limit_t&) = default;
				limit_t& operator=(limit_t&&) = default;
				~limit_t() = default;

				const limit_t& _limit() const { return *this; }
				Limit _value;
			};

		template<typename Database>
			struct dynamic_limit_t
			{
				using _is_limit = std::true_type;
				using _is_dynamic = std::true_type;

				dynamic_limit_t():
					_value(noop())
				{
				}

				template<typename Limit>
					dynamic_limit_t(Limit value):
						_initialized(true),
						_value(typename wrap_operand<Limit>::type(value))
				{
				}

				dynamic_limit_t(const dynamic_limit_t&) = default;
				dynamic_limit_t(dynamic_limit_t&&) = default;
				dynamic_limit_t& operator=(const dynamic_limit_t&) = default;
				dynamic_limit_t& operator=(dynamic_limit_t&&) = default;
				~dynamic_limit_t() = default;

				template<typename Limit>
					void set_limit(Limit value)
					{
						using arg_t = typename wrap_operand<Limit>::type;
						_value = arg_t{value};
						_initialized = true;
					}

				const dynamic_limit_t& _limit() const { return *this; }
				
				bool _initialized = false;
				interpretable_t<Database> _value;
			};

		struct no_limit_t
		{
			using _is_limit = std::true_type;
			const no_limit_t& _limit() const { return *this; }
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
				template<typename... Args>
					struct delayed_get_database_t
					{
						using type = get_database_t<Derived>;
					};

				template<typename Arg>
					auto limit(Arg arg)
					-> vendor::update_policies_t<Derived, no_limit_t, limit_t<typename wrap_operand<Arg>::type>>
					{
						typename wrap_operand<Arg>::type value = {arg};
						return { static_cast<Derived&>(*this), limit_t<typename wrap_operand<Arg>::type>(value) };
					}

				template<typename... Args>
				auto dynamic_limit(Args... args)
					-> vendor::update_policies_t<Derived, no_limit_t, dynamic_limit_t<typename delayed_get_database_t<Args...>::type>>
					{
						static_assert(sizeof...(Args) < 2, "dynamic_limit must be called with zero or one arguments");
						static_assert(not std::is_same<get_database_t<Derived>, void>::value, "dynamic_limit must not be called in a static statement");
						return { static_cast<Derived&>(*this), dynamic_limit_t<typename delayed_get_database_t<Args...>::type>(args...) };
					}
			};

		// Interpreters
		template<typename Context, typename Database>
			struct interpreter_t<Context, dynamic_limit_t<Database>>
			{
				using T = dynamic_limit_t<Database>;

				static Context& _(const T& t, Context& context)
				{
					if (t._initialized)
					{
						context << " LIMIT ";
						interpret(t._value, context);
					}
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
