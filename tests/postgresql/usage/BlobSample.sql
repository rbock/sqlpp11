DROP TABLE IF EXISTS blob_sample;
CREATE TABLE blob_sample (
  id bigserial PRIMARY KEY,
  data bytea
);
