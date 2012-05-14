CREATE TABLE files (
        id uuid PRIMARY KEY,
        filename text NOT NULL,
        path text NOT NULL,
        size integer,
        site varchar(3),
        facility varchar(3),
        stream text,
        time_start timestamp with time zone NOT NULL,
        time_stop timestamp with time zone NOT NULL,
        time_duration integer,
        purge boolean default false
);

grant all on files to armuser;
