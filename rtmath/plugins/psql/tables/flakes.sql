--- These tables reference flakeTypes, found in dda_run_tables.sql

create table flake (
	hashLower varchar(80) primary key,
	hashUpper varchar(80) unique,
	flakeType_id uuid references flakeTypes(id),
	tags text[],
	standardD real,
	description varchar(1023),
	flake_references varchar(80) references flake(hashLower)
);
create index flake_group on flake(flakeType_id);
create index flake_upperhash on flake(hashUpper);

grant select on flake TO public;
grant insert ON flake TO ddastatus_updater;
grant update ON flake TO ddastatus_updater;
