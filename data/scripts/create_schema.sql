CREATE TABLE IF NOT EXISTS SCHEMA_INFO(
	name TEXT PRIMARY KEY NOT NULL,
	value TEXT
);

CREATE TABLE IF NOT EXISTS ENGINE_STATE(
	id INTEGER PRIMARY KEY,
	state INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS SCAN_REQUEST(
	dir TEXT PRIMARY KEY,
	last_scan INTEGER NOT NULL,
	data_version TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS NAMES(
	name TEXT NOT NULL,
	is_ignored INTEGER NOT NULL DEFAULT 0,

	PRIMARY KEY(name)
);

CREATE TABLE IF NOT EXISTS DETECTED_MALWARE(
	file_path TEXT NOT NULL,
	name TEXT NOT NULL,
	data_version TEXT NOT NULL,
	malware_name TEXT NOT NULL,
	detailed_url TEXT NOT NULL,
	severity INTEGER NOT NULL,
	detected_time INTEGER NOT NULL,

	PRIMARY KEY(file_path),
	FOREIGN KEY(name) REFERENCES NAMES(name) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS PACKAGE_INFO(
	pkg_id TEXT NOT NULL,
	name TEXT NOT NULL,
	worst TEXT NOT NULL,

	PRIMARY KEY(pkg_id),
	FOREIGN KEY(worst) REFERENCES DETECTED_MALWARE(file_path) ON DELETE CASCADE,
	FOREIGN KEY(name) REFERENCES NAMES(name) ON DELETE CASCADE
);

CREATE VIEW IF NOT EXISTS [join_p_d] AS
	SELECT P.name, D.file_path, D.data_version, D.malware_name, D.detailed_url,
			D.severity, D.detected_time, P.pkg_id
		FROM PACKAGE_INFO AS P INNER JOIN DETECTED_MALWARE AS D ON P.worst = D.file_path;

CREATE VIEW IF NOT EXISTS [join_detecteds_by_name] AS
	SELECT J.*, N.is_ignored
		FROM
			(SELECT D.name, D.file_path, D.data_version, D.malware_name, D.detailed_url,
					D.severity, D.detected_time, P.pkg_id
				FROM DETECTED_MALWARE AS D LEFT JOIN PACKAGE_INFO AS P ON D.name = P.name
				WHERE NOT EXISTS (SELECT * FROM join_p_d WHERE name = D.name)
			UNION
			SELECT name, file_path, data_version, malware_name, detailed_url,
					severity, detected_time, pkg_id
				FROM join_p_d)
			AS J INNER JOIN NAMES AS N ON J.name = N.name;

CREATE VIEW IF NOT EXISTS [join_detecteds_by_file_path] AS
	SELECT D.file_path, D.data_version, D.malware_name, D.detailed_url, D.severity,
			D.detected_time, P.pkg_id, N.is_ignored
		FROM (DETECTED_MALWARE AS D LEFT JOIN PACKAGE_INFO AS P ON D.name = P.name)
			AS J INNER JOIN NAMES AS N ON J.name = N.name;
