/*
 * Copyright (c) 2013-2014, Roland Bock
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

#ifndef SQLPP_RESULT_FIELD_H
#define SQLPP_RESULT_FIELD_H

#include <sqlpp11/wrong.h>
#include <sqlpp11/result_field_methods.h>

namespace sqlpp
{
	template<typename ValueType, typename Db, typename FieldSpec>
		struct result_field_t
		{
			static_assert(wrong_t<result_field_t>::value, "Missing specialization for result_field_t");
		};

	template<typename Context, typename ValueType, typename Db, typename FieldSpec>
		struct serializer_t<Context, result_field_t<ValueType, Db, FieldSpec>>
		{
			using T = result_field_t<ValueType, Db, FieldSpec>;

			static Context& _(const T& t, Context& context)
			{
				if (t.is_null() and not null_is_trivial_value_t<T>::value)
				{
					context << "NULL";
				}
				else
				{
					context << t.value();
				}
				return context;
			}
		};


}
#endif
