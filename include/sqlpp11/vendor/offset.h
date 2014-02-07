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

#ifndef SQLPP_OFFSET_H
#define SQLPP_OFFSET_H

#include <sqlpp11/type_traits.h>
#include <sqlpp11/vendor/policy_update.h>
#include <sqlpp11/vendor/crtp_wrapper.h>

namespace sqlpp
{
	namespace vendor
	{
		// OFFSET
		template<typename Offset>
			struct offset_t
			{
				using _is_offset = std::true_type;
				static_assert(is_integral_t<Offset>::value, "offset requires an integral value or integral parameter");

				offset_t(size_t value):
					_value(value)
				{}

				offset_t(const offset_t&) = default;
				offset_t(offset_t&&) = default;
				offset_t& operator=(const offset_t&) = default;
				offset_t& operator=(offset_t&&) = default;
				~offset_t() = default;

				offset_t& _offset = *this;
				Offset _value;
			};

		template<typename Database>
			struct dynamic_offset_t
			{
				using _is_offset = std::true_type;
				using _is_dynamic = std::true_type;

				dynamic_offset_t(size_t value):
					_value(value)
				{}

				dynamic_offset_t(const dynamic_offset_t&) = default;
				dynamic_offset_t(dynamic_offset_t&&) = default;
				dynamic_offset_t& operator=(const dynamic_offset_t&) = default;
				dynamic_offset_t& operator=(dynamic_offset_t&&) = default;
				~dynamic_offset_t() = default;

				void set_offset(std::size_t offset)
				{
					_value = offset;
				}

				dynamic_offset_t& _offset = *this;
				std::size_t _value; // FIXME: This should be a serializable!
			};

		struct no_offset_t
		{
			using _is_offset = std::true_type;
			no_offset_t& _offset = *this;
		};

		// CRTP Wrappers
		template<typename Derived, typename Limit>
			struct crtp_wrapper_t<Derived, offset_t<Limit>>
			{
			};

		template<typename Derived, typename Database>
			struct crtp_wrapper_t<Derived, dynamic_offset_t<Database>>
			{
			};

		template<typename Derived>
			struct crtp_wrapper_t<Derived, no_offset_t>
			{
				template<typename Arg>
					struct delayed_get_database_t
					{
						using type = get_database_t<Derived>;
					};

				template<typename Arg>
					auto offset(Arg arg)
					-> vendor::update_policies_t<Derived, no_offset_t, offset_t<Arg>>
					{
						return { static_cast<Derived&>(*this), offset_t<Arg>(arg) };
					}

				template<typename Arg>
				auto dynamic_offset(Arg arg)
					-> vendor::update_policies_t<Derived, no_offset_t, dynamic_offset_t<typename delayed_get_database_t<Arg>::type>>
					{
						static_assert(not std::is_same<get_database_t<Derived>, void>::value, "dynamic_offset must not be called in a static statement");
						return { static_cast<Derived&>(*this), dynamic_offset_t<get_database_t<Derived>>(arg) };
					}
			};

		// Interpreters
		template<typename Context, typename Offset>
			struct interpreter_t<Context, offset_t<Offset>>
			{
				using T = offset_t<Offset>;

				static Context& _(const T& t, Context& context)
				{
					context << " OFFSET ";
					interpret(t._offset, context);
					return context;
				}
			};

		template<typename Context, typename Database>
			struct interpreter_t<Context, dynamic_offset_t<Database>>
			{
				using T = dynamic_offset_t<Database>;

				static Context& _(const T& t, Context& context)
				{
					if (t._value > 0)
						context << " OFFSET " << t._offset;
					return context;
				}
			};

		template<typename Context>
			struct interpreter_t<Context, no_offset_t>
			{
				using T = no_offset_t;

				static Context& _(const T& t, Context& context)
				{
					return context;
				}
			};
	}
}

#endif
