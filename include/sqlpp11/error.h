/*
* Copyright (c) 2013 - 2017, Roland Bock, Frank Park
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

#pragma once

#ifndef SQLPP_ERROR_H
#define SQLPP_ERROR_H

#include <system_error>

namespace sqlpp
{
	struct error : std::system_error
	{
		enum
		{
			ok = 0,
			failed,
			unknown,
			connection_error,
			query_error
		};

		error() : std::system_error(sqlpp::error::ok, std::generic_category()) {}
		error(int code) : std::system_error(code, std::generic_category()) {}
		error(int code, std::string message) : std::system_error(code, std::generic_category(), message) {}
		error(const error& other) : std::system_error(other) {}

		operator bool() const
		{
			return code().value() != 0;
		}

		bool operator!() const
		{
			return code().value() == 0;
		}

		bool operator==(const error& other) const
		{
			return code().value() == other.code().value();

		}
		bool operator!=(const error& other) const {
			return !operator==(other);
		}
	};
}

#endif
