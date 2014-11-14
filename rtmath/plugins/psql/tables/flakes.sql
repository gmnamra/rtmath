--- These tables reference flakeTypes, found in dda_run_tables.sql

create table flake (
	hashLower varchar(80) primary key,
	hashUpper varchar(80) unique,
	flakeType_id uuid references flakeTypes(id),
	tags text[],
	standardD real,
	numDipoles int,
	description varchar(1023),
	flake_references varchar(80) references flake(hashLower)
);
create index flake_group on flake(flakeType_id);
create index flake_upperhash on flake(hashUpper);
create index flake_dipoles on flake(numDipoles);

grant select on flake TO public;
grant insert ON flake TO ddastatus_updater;
grant update ON flake TO ddastatus_updater;

create view flake_summary as 
 select flake.hashLower,
 flakeTypes.name,
 flake.tags,
 flake.numDipoles,
 flake.standardD,
 flake.flake_references
 from flake, flakeTypes
 where flake.flakeType_id = flakeTypes.id
 order by flakeTypes.name, flake.numDipoles
 ;
grant select on flake_summary to public;

create view flake_counts as 
 select name, count(*) 
 from flake_summary 
 group by name order by name
 ;
grant select on flake_counts to public;

create table flakeResult (
	hashLower varchar(80) references flake(hashLower) not null,
	runid uuid references flakeRuns(id) not null,
	aeff real not null,
	path text,
	tsAdded timestamp,
	Qsca_iso real,
	Qbk_iso real,
	Qext_iso real,
	Qabs_iso real,
	g_iso real,
	---frequency real,
	---temperature	real,
	---polarization varchar(9),
	---nBetas int,
	---nThetas int,
	---nPhis int,
	---nOris int,
	---ddaVersion uuid references ddaVersion(id),
	---host uuid references host(id),
	primary key (hashLower, runid)
);

create index flakeres_hash on flakeResult(hashLower);
create index flakeres_run on flakeResult(runid);
create index flakeres_aeff on flakeResult(aeff);

grant select on flakeResult to public;
grant insert on flakeResult to ddastatus_updater;
grant update on flakeResult to ddastatus_updater;
grant insert on flakeResult to rtmath_test;
grant update on flakeResult to rtmath_test;

create view flakeResult_unified as
 select
 flakeTypes.name as fType,
 flakeRuns.frequency,
 flakeRuns.temperature,
 flakeResult.aeff,
 6.28318 * flakeResult.aeff / (299790 / flakeRuns.frequency ) as size_parameter,
 flake.numDipoles,
 flakeRuns.nBetas,
 flakeRuns.nThetas,
 flakeRuns.nPhis,
 flakeRuns.polarization,
 flakeResult.Qsca_iso,
 flakeResult.Qbk_iso,
 flakeResult.Qext_iso,
 flakeResult.Qabs_iso,
 flakeResult.g_iso,
 flakeResult.path
 from flakeTypes, flakeResult, flake, flakeRuns
 where flake.hashLower = flakeResult.hashLower
 and flake.flakeType_id = flakeTypes.id
 and flakeResult.runid = flakeRuns.id
 order by fType, frequency, temperature, aeff, nBetas
 ;

 grant select on flakeResult_unified to public;


create view run_v as 
SELECT flakeResult.hashLower, flakeResult.runid, flakeResult.aeff, 
	flakeType_id, flakeTypes.name, flake.numDipoles, flake.standardD, 
	flakeRuns.frequency, flakeRuns.temperature, flakeRuns.nBetas, flakeRuns.nThetas, 
	flakeRuns.nPhis, flakeRuns.polarization, flakeResult.path FROM flakeResult, flakeRuns, flakeTypes, flake 
	where flake.hashLower = flakeResult.hashLower 
	and flake.flakeType_id = flakeTypes.id 
	and flakeResult.runid = flakeRuns.id
	order by flakeRuns.frequency, flakeRuns.temperature, flakeResult.aeff
	;
