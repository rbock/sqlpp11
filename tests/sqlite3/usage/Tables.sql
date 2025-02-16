CREATE TABLE tab_sample (
  id INTEGER PRIMARY KEY,
  alpha bigint(20) DEFAULT NULL,
  beta varchar(255) DEFAULT NULL,
  gamma boolean
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE tab_foo (
  omega bigint(20) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE integral_sample (
  signed_value INTEGER,
  unsigned_value INTEGER UNSIGNED
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE blob_sample (
  id INTEGER PRIMARY KEY,
  data blob
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE fp_sample (
  id INTEGER PRIMARY KEY,
  fp DOUBLE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE tab_date_time
(
  id INTEGER PRIMARY KEY,
  day_point_n date,
  time_point_n datetime,
  time_of_day_n time
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

