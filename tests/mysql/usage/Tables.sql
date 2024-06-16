CREATE TABLE tab_sample (
  id bigint(20) AUTO_INCREMENT PRIMARY KEY,
  int_n bigint(20) DEFAULT NULL,
  text_n varchar(255) DEFAULT NULL,
  bool_n bool DEFAULT NULL
) ENGINE=InnSampleoDB DEFAULT CHARSET=latin1;

CREATE TABLE tab_date_time (
  id bigint(20) AUTO_INCREMENT PRIMARY KEY,
  day_point_n date,
  time_point_n datetime(3),
  date_time_point_n_d datetime DEFAULT CURRENT_TIMESTAMP,
  time_of_day_n time(3)
)

CREATE TABLE tab_foo (
  id bigint(20) AUTO_INCREMENT PRIMARY KEY,
  int_n bigint(20) DEFAULT NULL
)

CREATE TABLE tab_json (
  id bigint(20) AUTO_INCREMENT PRIMARY KEY,
  data JSON NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
