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

				offset_t(Offset value):
					_value(value)
				{}

				offset_t(const offset_t&) = default;
				offset_t(offset_t&&) = default;
				offset_t& operator=(const offset_t&) = default;
				offset_t& operator=(offset_t&&) = default;
				~offset_t() = default;

				Offset _value;
			};

		template<typename Database>
			struct dynamic_offset_t
			{
				using _is_offset = std::true_type;
				using _is_dynamic = std::true_type;

				dynamic_offset_t():
					_value(noop())
				{
				}

				template<typename Offset>
					dynamic_offset_t(Offset value):
						_initialized(true),
						_value(typename wrap_operand<Offset>::type(value))
				{
				}

				dynamic_offset_t(const dynamic_offset_t&) = default;
				dynamic_offset_t(dynamic_offset_t&&) = default;
				dynamic_offset_t& operator=(const dynamic_offset_t&) = default;
				dynamic_offset_t& operator=(dynamic_offset_t&&) = default;
				~dynamic_offset_t() = default;

				template<typename Offset>
					void set_offset(Offset value)
					{
						using arg_t = typename wrap_operand<Offset>::type;
						_value = arg_t(value);
						_initialized = true;
					}

				bool _initialized = false;
				interpretable_t<Database> _value;
			};

		struct no_offset_t
		{
			using _is_noop = std::true_type;
		};

		// Interpreters
		template<typename Context, typename Offset>
			struct serializer_t<Context, offset_t<Offset>>
			{
				using T = offset_t<Offset>;

				static Context& _(const T& t, Context& context)
				{
					context << " OFFSET ";
					serialize(t._value, context);
					return context;
				}
			};

		template<typename Context, typename Database>
			struct serializer_t<Context, dynamic_offset_t<Database>>
			{
				using T = dynamic_offset_t<Database>;

				static Context& _(const T& t, Context& context)
				{
					if (t._initialized)
					{
						context << " OFFSET ";
						serialize(t._value, context);
					}
					return context;
				}
			};

		template<typename Context>
			struct serializer_t<Context, no_offset_t>
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
