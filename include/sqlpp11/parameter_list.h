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

#ifndef SQLPP_PARAMETER_H
#define SQLPP_PARAMETER_H

#include <tuple>

namespace sqlpp
{
	template<typename... Parameter>
		struct named_parameter_list_t: public Parameter::_member_t...
	{
		named_parameter_list_t():
			Parameter::_member_t()...,
			_parameter_tuple(static_cast<typename Parameter::_member_t&>(*this)()...)
		{}

		named_parameter_list_t(const named_parameter_list_t& rhs):
			Parameter::_member_t(static_cast<const typename Parameter::_member_t&>(rhs))...,
			_parameter_tuple(static_cast<typename Parameter::_member_t&>(*this)()...)
		{}

		named_parameter_list_t(named_parameter_list_t&& rhs):
			Parameter::_member_t(std::move(static_cast<typename Parameter::_member_t&>(rhs)))...,
			_parameter_tuple(static_cast<typename Parameter::_member_t&>(*this)()...)
		{}

		named_parameter_list_t& operator=(const named_parameter_list_t&) = delete;
		named_parameter_list_t& operator=(named_parameter_list_t&&) = delete;
		~named_parameter_list_t() = default;

		std::tuple<const typename Parameter::type&...> _parameter_tuple;
	};

}

#endif
