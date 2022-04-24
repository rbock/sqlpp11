/*
 * Copyright (c) 2013-2015, Roland Bock
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

CREATE TABLE tab_foo
(
	delta varchar(255),
	_epsilon bigint,
	`omega` double,
        some_number NUMERIC(314,15),
        CONSTRAINT uc_delta UNIQUE (delta, _epsilon),
	
	-- explicit column definition for System-Versioned table
	rowStart TIMESTAMP(6) GENERATED ALWAYS AS ROW START,
	rowEnd TIMESTAMP(6) GENERATED ALWAYS AS ROW END,
	PERIOD FOR SYSTEM_TIME(rowStart, rowEnd)
) WITH SYSTEM VERSIONING; -- enable System-Versioning for this table

CREATE OR REPLACE TABLE tab_bar
(
	alpha bigint AUTO_INCREMENT RRIMARY_KEY,
	beta varchar(255) NULL DEFAULT "",
	gamma bool NOT NULL,
	delta int
	
	-- implicit column definition for System-Versioned table
)WITH SYSTEM VERSIONING; -- enable System-Versioning  for this table
