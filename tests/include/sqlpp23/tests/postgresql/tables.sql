DROP TABLE IF EXISTS tab_foo;

CREATE TABLE tab_foo
(
  id bigserial PRIMARY KEY,
  text_nn_d varchar(255) NOT NULL DEFAULT '',
  int_n bigint,
  double_n double precision,
  bool_n bool,
  blob_n bytea
);

DROP TABLE IF EXISTS tab_bar;

CREATE TABLE tab_bar
(
  id bigserial PRIMARY KEY,
  text_n varchar(255) NULL,
  bool_nn bool NOT NULL DEFAULT false,
  int_n int
);

CREATE TABLE tab_date_time
(
  day_point_n date,
  time_point_n timestamp,
  time_of_day_n time,
  time_point_n_tz timestamp with time zone,
  time_of_day_n_tz time with time zone
);
DROP TABLE IF EXISTS blob_sample;

CREATE TABLE blob_sample (
  id bigserial PRIMARY KEY,
  data bytea
);

CREATE TABLE tab_except (
  int_small_n_u smallint UNIQUE,
  text_short_n text CHECK( length(text_short_n) < 5 )
)
