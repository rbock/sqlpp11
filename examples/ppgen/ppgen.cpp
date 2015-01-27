/*
 * Copyright (c) 2014-2015, niXman (i dot nixman dog gmail dot com)
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

#include <sqlpp11/ppgen.h>

#if 0
SQLPP_DECLARE_TABLE(
	(table, ENGINE("InnoDB"), CHARACTER_SET("utf-8"))
	,
	(id, int, NOT_NULL, PRIMARY_KEY, AUTO_INCREMENT)
	(name, varchar(64), NOT_NULL, INDEX("name_index"), DEFAULT("any name"))
	(age, int, NOT_NULL, INDEX("age_index"), UNIQUE, COMMENT("some comments"))
)
#endif

SQLPP_DECLARE_TABLE(
	(table)
	,
	(id, int, NOT_NULL)
	(c0, blob, NULL)
	(c1, bool, NOT_NULL)
	(c2, float, NULL)
	(c3, tinyint, NULL)
	(c4, smallint, NOT_NULL)
	(c5, int, NULL)
	(c6, bigint, NOT_NULL)
	(c7, text, NULL)
	(c8, varchar(16), NOT_NULL)
	(c9, varchar(32), NULL)
)
