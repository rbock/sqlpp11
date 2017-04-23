/*
* Copyright (c) 2013 - 2017, Roland Bock, Frank Park, Aaron Bishop
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

#include <experimental/coroutine>

struct resumable_function
{
	struct promise_type
	{
		resumable_function get_return_object()
		{
			return resumable_function(std::experimental::coroutine_handle<promise_type>::from_promise(*this));
		}

		auto initial_suspend()
		{
			return std::experimental::suspend_never{};
		}

		auto final_suspend()
		{
			return std::experimental::suspend_always{};
		}

		auto return_void()
		{
		}
	};

	std::experimental::coroutine_handle<promise_type> _coroutine = nullptr;

	explicit resumable_function(std::experimental::coroutine_handle<promise_type> coroutine)
		: _coroutine(coroutine)
	{
	}

	~resumable_function()
	{
		if (_coroutine)
		{
			_coroutine.destroy();
		}
	}

	void resume()
	{
		_coroutine.resume();
	}

	resumable_function() = default;
	resumable_function(resumable_function const&) = delete;
	resumable_function& operator= (resumable_function const&) = delete;

	resumable_function(resumable_function && other)
		: _coroutine(std::move(other._coroutine))
	{
	}

	resumable_function& operator=(resumable_function&& other)
	{
		_coroutine = std::move(other._coroutine);
	}
};