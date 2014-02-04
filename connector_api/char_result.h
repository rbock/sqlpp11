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


#ifndef SQLPP_DATABASE_CHAR_RESULT_H
#define SQLPP_DATABASE_CHAR_RESULT_H

#include <sqlpp11/vendor/char_result_row.h>

namespace sqlpp
{
	namespace database
	{
		/*
		 * char_result_t yields results as 
		 * sqlpp11::vendor::char_result_row_t
		 */
		class char_result_t
		{
			::sqlpp11::vendor::char_result_row_t char_result_row;
		public:
			char_result_t(); // default constructor for a result that will not yield a valid row
			char_result_t(...);
			char_result_t(const char_result_t&) = delete;
			char_result_t(char_result_t&& rhs);
			char_result_t& operator=(const char_result_t&) = delete;
			char_result_t& operator=(char_result_t&&);
			~char_result_t();

			bool operator==(const char_result_t& rhs) const;

			template<typename ResultRow>
			void next(ResultRow& result_row);

			// Something like
			/*
			{
				next_impl();
				if (_char_result_row.data)
					result_row = _char_result_row;
				else
					result_row.invalidate();
			};
			*/
	}
}
#endif
