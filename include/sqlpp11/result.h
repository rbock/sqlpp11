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

#ifndef SQLPP_RESULT_H
#define SQLPP_RESULT_H

#include <sqlpp11/raw_result_row.h>

#include <iostream>

namespace sqlpp
{
	template<typename Db, typename ResultRow>
		class result_t
	{
		using db_result_t = typename Db::_result;
		
		db_result_t _result;
		raw_result_row_t _raw_result_row;
		raw_result_row_t _end;

	public:
		result_t(db_result_t&& result):
			_result(std::forward<db_result_t>(result)),
			_raw_result_row(_result.next()),
			_end({})
			{}

		result_t(const result_t&) = delete;
		result_t(result_t&&) = default;
		result_t& operator=(const result_t&) = delete;
		result_t& operator=(result_t&&) = default;

		// Iterator
		class iterator
		{
		public:
			iterator(db_result_t& result, raw_result_row_t& raw_result_row):
				_result(result),
				_raw_result_row(raw_result_row),
				_result_row(_raw_result_row)
			{
				std::cerr << "result::iterator::constructor" << std::endl;
			}

			const ResultRow& operator*() const
			{
				return _result_row;
			}

			const ResultRow* operator->() const
			{
				return &_result_row;
			}

			bool operator==(const iterator& rhs) const
			{
				return _result == rhs._result and _raw_result_row == rhs._raw_result_row;
			}

			bool operator!=(const iterator& rhs) const
			{
				return not (operator==(rhs));
			}

			void operator++()
			{
				_raw_result_row = _result.next();
				_result_row = _raw_result_row;
			}

			db_result_t& _result;
			raw_result_row_t& _raw_result_row;
			ResultRow _result_row;
		};

		iterator begin()
		{
			return iterator(_result, _raw_result_row);
		}

		iterator end()
		{
			return iterator(_result, _end);
		}

		const ResultRow operator*()
		{
			return {_raw_result_row};
		}

		iterator operator->()
		{
			return begin();
		}

	};
}

#endif
