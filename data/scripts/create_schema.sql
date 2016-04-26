CREATE TABLE IF NOT EXISTS SCHEMA_INFO(name TEXT PRIMARY KEY NOT NULL,
                                       value TEXT);

CREATE TABLE IF NOT EXISTS ENGINE_STATE(engine_id INTEGER PRIMARY KEY,
                                        state     INTEGER NOT NULL);

CREATE TABLE IF NOT EXISTS SCAN_REQUEST(dir TEXT PRIMARY KEY,
                                        last_scan INTEGER NOT NULL,
                                        data_version TEXT NOT NULL);

CREATE TABLE IF NOT EXISTS DETECTED_MALWARE_FILE(path TEXT PRIMARY KEY NOT NULL,
                                                 data_version TEXT NOT NULL,
                                                 severity_level INTEGER NOT NULL,
                                                 threat_type INTEGER NOT NULL,
                                                 malware_name TEXT NOT NULL,
                                                 detailed_url TEXT,
                                                 detected_time INTEGER NOT NULL,
                                                 modified_time INTEGER NOT NULL,
                                                 ignored INTEGER NOT NULL);
