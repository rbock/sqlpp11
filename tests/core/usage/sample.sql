--
-- Copyright (c) 2013-2016, Roland Bock, Aaron Bishop
-- All rights reserved.
--
-- Redistribution and use in source and binary forms, with or without modification,
-- are permitted provided that the following conditions are met:
--
--   Redistributions of source code must retain the above copyright notice, this
--   list of conditions and the following disclaimer.
--
--   Redistributions in binary form must reproduce the above copyright notice, this
--   list of conditions and the following disclaimer in the documentation and/or
--   other materials provided with the distribution.
--
-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
-- ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
-- WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
-- DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
-- ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
-- (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
-- LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
-- ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
-- (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
-- SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
--

DROP TABLE IF EXISTS tab_foo;

CREATE TABLE tab_foo
(
	id bigint AUTO_INCREMENT PRIMARY KEY,
	text_nn_d varchar(255) NOT NULL DEFAULT "",
	int_n bigint,
	double_n double,
	u_int_n bigint UNSIGNED,
	blob_n BLOB
);

DROP TABLE IF EXISTS tab_bar;

CREATE TABLE tab_bar
(
	id bigint AUTO_INCREMENT PRIMARY KEY,
	text_n varchar(255) NULL DEFAULT "",
	bool_nn bool NOT NULL,
	int_n int
);

CREATE TABLE tab_date_time
(
	id bigint AUTO_INCREMENT PRIMARY KEY,
	day_point_n date,
	time_point_n datetime,
	time_of_day_n time
);

