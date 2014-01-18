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

namespace sqlpp
{
	namespace vendor
	{
		template<typename Offset>
			struct offset_t
			{
				using _is_offset = std::true_type;
				static_assert(is_integral_t<Offset>::value, "offset requires an integral value or integral parameter");

				Offset _offset;
			};

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

		struct dynamic_offset_t
		{
			using _is_offset = std::true_type;
			using _is_dynamic = std::true_type;

			void set(std::size_t offset)
			{
				_offset = offset;
			}

			std::size_t _offset;
		};

		template<typename Context>
			struct interpreter_t<Context, dynamic_offset_t>
			{
				using T = dynamic_offset_t;

				static Context& _(const T& t, Context& context)
				{
					if (t._offset > 0)
						context << " OFFSET " << t._offset;
					return context;
				}
			};

	}
}

#endif
