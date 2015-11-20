//*********************************************************
//	Db_File.cpp - standard database interface
//*********************************************************

#include "Db_File.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Db_File constructors
//---------------------------------------------------------

Db_File::Db_File (Access_Type access, Format_Type format) : Db_Record ()
{
	Initialize (access, format);
}

Db_File::Db_File (char *_filename, Access_Type access, Format_Type format) : Db_Record ()
{
	Initialize (access, format);

	Open (_filename);
}

//---------------------------------------------------------
//	Db_File destructor
//---------------------------------------------------------

Db_File::~Db_File (void)
{
	Close ();
	if (filename != NULL) {
		delete [] filename;
		filename = NULL;
	}
	if (read_stmt != NULL) {
		sqlite3_finalize (read_stmt);
		read_stmt = NULL;
	}
	if (read_nest != NULL) {
		sqlite3_finalize (read_nest);
		read_nest = NULL;
	}
	if (insert_stmt != NULL) {
		sqlite3_finalize (insert_stmt);
		insert_stmt = NULL;
	}
	if (insert_nest != NULL) {
		sqlite3_finalize (insert_nest);
		insert_nest = NULL;
	}
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Db_File::Initialize (Access_Type access, Format_Type format) 
{
	filename = NULL;
	file = NULL;
	db_file = NULL;
	insert_stmt = insert_nest = NULL;
	read_stmt = read_nest = NULL;
	first_offset = 0;
	num_records = record_num = max_record_num = parent_id = 0;
	new_nest_flag = true;

	File_Access (access);
	File_Format (format);

	switch (format) {
		default:
			File_Type ("Text File");
			break;
		case BINARY:
			File_Type ("Binary File");
			break;
		case FIXED_COLUMN:
			File_Type ("Fixed Column File");
			break;
		case COMMA_DELIMITED:
			File_Type ("Comma Delimited File");
			break;
		case SPACE_DELIMITED:
			File_Type ("Space Delimited File");
			break;
		case VERSION3:
		case TAB_DELIMITED:
			File_Type ("Tab Delimited File");
			break;
		case CSV_DELIMITED:
			File_Type ("CSV Delimited File");
			break;
		case DBASE:
			File_Type ("dBase File");
			break;
		case SQLITE3:
			File_Type ("SQLite3 Database");
			break;
	}
}

//---------------------------------------------------------
//	File_Format
//---------------------------------------------------------

void Db_File::File_Format (Format_Type format)
{
	if (format == DEFAULT_FORMAT) {
		format = (Format_Type) Default_Format ();
	}
	Record_Format (format);

	if (format == COMMA_DELIMITED || format == SPACE_DELIMITED || 
		format == TAB_DELIMITED || format == VERSION3) {
		file_format = CSV_DELIMITED;
	} else if (format == DBASE) {
		file_format = BINARY;
	} else {
		file_format = format;
	}
}

//---------------------------------------------------------
//	Filename
//---------------------------------------------------------

bool Db_File::Filename (char *path)
{
	if (filename != NULL) {
		delete [] filename;
		filename = NULL;
	}
	if (path != NULL) {
		int len = (int) strlen (path) + 1;
		filename = new char [len];
		if (filename == NULL) return (Status (MEMORY));
		str_cpy (filename, len, path);
		return (true);
	}
	return (false);
}

//---------------------------------------------------------
//	Open
//---------------------------------------------------------

bool Db_File::Open (char *path)
{
	if (path != NULL) {
		Filename (path);
	} else if (filename == NULL) {
		return (false);
	}
	if (Is_Open ()) {
		Close ();
	}
	char *code = 0;
	int  sqlite_code = 0;
	bool result = false;
	bool create_flag = false;
	bool binary_flag = (File_Format () == BINARY);
	bool sqlite_flag = (File_Format () == SQLITE3);

	//if (sqlite_flag && Nested ()) {
	//	exe->File_Error ("SQLITE3 does not support Nested Files", Filename ());
	//	return (Status (NOT_OPEN));
	//}

	switch (File_Access ()) {
		default:
			return (Status (ERROR));
			break;
		case READ:
			if (sqlite_flag) {
				sqlite_code = SQLITE_OPEN_READONLY;
			} else {
				code = (char *) ((binary_flag) ? "rb" : "rt");
			}
			break;
		case MODIFY:
			if (sqlite_flag) {
				sqlite_code = SQLITE_OPEN_READWRITE;
			} else {
				code = (char *) ((binary_flag) ? "r+b" : "r+t");
			}
			break;
		case APPEND:
			if (sqlite_flag) {
				sqlite_code = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
			} else {
				code = (char *) ((binary_flag) ? "a+b" : "a+t");
			}
			break;
		case CREATE:
			if (sqlite_flag) {
				remove (Filename ());
				sqlite_code = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
			} else {
				code = (char *) ((binary_flag) ? "w+b" : "w+t");
			}
			create_flag = true;
			num_records = record_num = max_record_num = 0;
			break;
	}
	if (sqlite_flag) {
		result = (sqlite3_open_v2 (Filename (), &db_file, sqlite_code, NULL) == SQLITE_OK);
	} else {
		file = f_open (Filename (), code);
		result = (file != NULL);
	}
	if (!result) {
		if (Send_Messages ()) {
			char buffer [STRING_BUFFER];

			if (create_flag) {
				str_cpy (buffer, sizeof (buffer), "Creating ");
			} else {
				str_cpy (buffer, sizeof (buffer), "Opening ");
			}
			str_cat (buffer, sizeof (buffer), File_Type ());
			exe->File_Error (buffer, Filename ());
		}
		return (Status (NOT_OPEN));
	} else {
		if (create_flag && str_cmp (File_Type (), "New", 3) && str_cmp (File_Type (), "Output", 6)) {
			char buffer [STRING_BUFFER];

			str_fmt (buffer, sizeof (buffer), "New %s", File_Type ());
			File_Type (buffer);

			if (!sqlite_flag) {
				str_fmt (buffer, sizeof (buffer), "New%s", File_ID ());
				File_ID (buffer);
			}
		}
		if (Send_Messages ()) {
			exe->Print_Filename (1, File_Type (), Filename ());
		}
		if (sqlite_flag) {
			if (sqlite3_exec (db_file, "PRAGMA synchronous = OFF", NULL, NULL, NULL) != SQLITE_OK) {
				return (Status (NOT_OPEN));
			}
		}
		return (Status (OK));
	}
}

//---------------------------------------------------------
//	Read
//---------------------------------------------------------

bool Db_File::Read (void *record, int size)
{
	//---- check the file status ----

	if (!Check_File ()) return (false);
	if (record == NULL) return (Status (NULL_POINTER));
	if (size <= 0) return (Status (RECORD_SIZE));
	
	//---- read the record ----

	switch (File_Format ()) {
		case BINARY:
			if (fread (record, size, 1, file) != 1) {
				memset (record, '\0', size);
				return (feof (File ()) ? false : Status (FILE_IO));
			}
			break;
		case FIXED_COLUMN:
			memset (record, ' ', size);
			if (fgets ((char *) record, size+2, file) == NULL) {
				return (feof (File ()) ? false : Status (FILE_IO));
			}
			((char *) record) [size] = '\0';
			break;
		case CSV_DELIMITED:
			if (fgets ((char *) record, size+2, file) == NULL) {
				return (feof (File ()) ? false : Status (FILE_IO));
			}
			((char *) record) [size] = '\0';
			size = (int) strlen ((char *) record);
			return (Input_Record ((char *) record, size));
			break;
		case SQLITE3:
			return (Input_Record ((char *) record, size));
			break;
		default:
			if (fgets ((char *) record, size+2, file) == NULL) {
				return (feof (File ()) ? false : Status (FILE_IO));
			}
			((char *) record) [size] = '\0';
			size = (int) strlen ((char *) record);
			break;
	}
	return (Record (record, size));
}

bool Db_File::Read (void *record, int size, off_t offset)
{
	if (!Offset (offset)) return (false);

	return (Read (record, size));
}

bool Db_File::Read_Record (int number) 
{
	if (!Record_Number (number)) return (false);

	return (Read ());
}

//---------------------------------------------------------
//	Write
//---------------------------------------------------------

bool Db_File::Write (void *record, int size)
{
	//---- check the file status ----

	if (!Check_File ()) return (false);
	if (record == NULL) return (Status (NULL_POINTER));
	if (size <= 0) return (Status (RECORD_SIZE));
	
	//---- write the record ----

	switch (File_Format ()) {
		case BINARY:
			if (fwrite (record, size, 1, file) != 1) {
				return (Status (FILE_IO));
			}
			break;
		case FIXED_COLUMN:
			if (fprintf (file, "%-*.*s\n", size, size, ((char *) record)) < 0) {
				return (Status (FILE_IO));
			}
			break;
		case CSV_DELIMITED:
			if (fprintf (file, "%.*s\n", size, Output_Record ((char *) record)) < 0) {
				return (Status (FILE_IO));
			}
			break;
		case SQLITE3:
			Output_Record ((char *) record);
			break;
		default:
			if (fprintf (file, "%s\n", ((char *) record)) < 0) {
				return (Status (FILE_IO));
			}
			break;
	}
	return (true);
}

bool Db_File::Write (void *record, int size, off_t offset)
{
	if (!Offset (offset)) return (false);

	return (Write (record, size));
}

bool Db_File::Write_Record (int number) 
{
	if (!Record_Number (number)) return (false);

	return (Write ());
}

//---------------------------------------------------------
//	Close
//---------------------------------------------------------

bool Db_File::Close (void)
{
	bool stat = true;

	if (Is_Open ()) {
		if (File_Format () == SQLITE3) {
			if (read_stmt != NULL) {
				sqlite3_finalize (read_stmt);
				read_stmt = NULL;
			}
			if (read_nest != NULL) {
				sqlite3_finalize (read_nest);
				read_nest = NULL;
			}
			if (insert_stmt != NULL) {
				sqlite3_finalize (insert_stmt);
				insert_stmt = NULL;
			}
			if (insert_nest != NULL) {
				sqlite3_finalize (insert_nest);
				insert_nest = NULL;
			}
			if (db_file != NULL) {
				sqlite3_exec (db_file, "COMMIT", NULL, NULL, NULL);
				sqlite3_close (db_file);
				db_file = NULL;
			}
		} else {
			if (fclose (file)) {
				stat = Status (CLOSING);
			}
			file = NULL;
		}
	}
	return (stat);
}

//---------------------------------------------------------
//	Rewind
//---------------------------------------------------------

bool Db_File::Rewind (void)
{
	Record_Number (-1); 

	if (File_Format () == BINARY) {
		return (Offset (First_Offset ()));
	} else if (File_Format () != SQLITE3) {
		rewind (file);

		if (First_Offset () > 0) {
			int len, size;
			char *record;

			len = 0;
			record = Record ();
			size = Max_Size ();

			while (fgets (record, size+2, file) != NULL) {
				record [size] = '\0';
				len += (int) strlen (record);

				if (len >= First_Offset ()) {
					return (true);
				}
			}
			return (Status (FILE_IO));
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Offset
//---------------------------------------------------------

bool Db_File::Offset (off_t offset)
{
	if (Check_File ()) {
		if (File_Format () == SQLITE3) {
			if (offset == 0) {
				Record_Number (-1);
			} else if (Max_Size () > 0) {
				Record_Number ((int) ((offset / Max_Size ()) + 1));
			} else {
				exe->Warning ("Db_File::Offset on a SQLITE3 Database offset=%d", offset);
				return (Status (POSITIONING));
			}
		} else if (offset < 0 || fseek (file, offset, SEEK_SET)) {
			return (Status (POSITIONING));
		}
		return (true);
	}
	return (false);
}

off_t Db_File::Offset (void)
{
	off_t offset = -1;

	if (Check_File ()) {
		if (File_Format () == SQLITE3) {
			offset = 0;
			exe->Warning ("Db_File::Offset on a SQLITE3 Database");
		} else {
			offset = ftell (file);

			if (offset < 0) {
				if (File_Format () == BINARY) {
					Status (POSITIONING);
				} else {
					char *rec = Record ();

					if (rec != NULL) {
						offset = (int) strlen (rec) + 1;
					} else {
						offset = 0;
					}
				}
			}
		}
	}
	return (offset);
}

//-----------------------------------------------------------
//	File_Size
//-----------------------------------------------------------

off_t Db_File::File_Size (void)
{
	off_t offset = -1;

	if (Check_File ()) {
		if (File_Format () == SQLITE3) {
			exe->Warning ("Db_File::File_Size on a SQLITE3 Database");
			return (Status (POSITIONING));
		} else {
			off_t current = ftell (file);

			if (!fseek (file, 0, SEEK_END)) {
				offset = ftell (file);
			}
			if (offset < 0) Status (POSITIONING);
			if (current >= 0) {
				fseek (file, current, SEEK_SET);
			}
		}
	}
	return (offset);
}

//-----------------------------------------------------------
//	Num_Records
//-----------------------------------------------------------

int Db_File::Num_Records (void)
{
	if (!num_records && Check_File ()) {
		if (File_Format () == SQLITE3) {
			char sql [FIELD_BUFFER];
			sqlite3_stmt *record_stmt;

			//---- get the number of rows ----

			str_fmt (sql, sizeof (sql), "SELECT COUNT(*) FROM %s;", File_ID ());

			if (sqlite3_prepare_v2 (db_file, sql, -1, &record_stmt, NULL) != SQLITE_OK) {
				exe->Warning ("Preparing Record Statement: %s", sqlite3_errmsg (db_file));
			}
			sqlite3_step (record_stmt);
			num_records = sqlite3_column_int (record_stmt, 0);

			sqlite3_finalize (record_stmt);
		} else if (File_Format () == BINARY || File_Format () == FIXED_COLUMN) {
			if (Max_Size () > 0) {
				num_records = (int) (File_Size () - First_Offset () + Max_Size () - 1) / Max_Size ();
			}
		} else {
			FILE *temp = f_open (Filename (), "rt");

			if (temp != NULL) {
				char buffer [4080];

				while (fgets (buffer, sizeof (buffer), temp) != NULL) {
					num_records++;
				}
				fclose (temp);
			}
		}
	}
	return (num_records);
}

//-----------------------------------------------------------
//	Estimate_Records
//-----------------------------------------------------------

int Db_File::Estimate_Records (void)
{
	int num = 0;

	if (Check_File ()) {
		if (File_Format () == SQLITE3) {
			num = Num_Records ();
		} else if (File_Format () == BINARY || File_Format () == FIXED_COLUMN) {
			if (Max_Size () > 0) {
				num = (File_Size () - First_Offset () + Max_Size () - 1) / Max_Size ();
			}
		} else {
			FILE *temp = f_open (Filename (), "rt");

			if (temp != NULL) {
				int cum_size = 0;
				off_t offset;
				char buffer [4080];

				while (fgets (buffer, sizeof (buffer), temp) != NULL) {
					cum_size += (int) strlen (buffer);
					if (++num == 100) break;
				}
				if (cum_size > 0) {
					if (!fseek (temp, 0, SEEK_END)) {
						offset = ftell (temp);
						num *= (int) (offset / cum_size) + 1;
					}
				}
				fclose (temp);
			}
		}
	}
	return (num);
}

//-----------------------------------------------------------
//	Record_Number
//-----------------------------------------------------------

bool Db_File::Record_Number (int number)
{
	if (number == 0) {
		record_num++;
	} else if (number < 0) {
		record_num = 0;
	} else {
		if (number != record_num + 1 && (File_Format () == BINARY || File_Format () == FIXED_COLUMN)) {
			off_t offset = First_Offset () + (number - 1) * Record_Size ();
			if (!Offset (offset)) return (false);
		}
		record_num = number;
	}
	if (record_num > max_record_num) max_record_num = record_num;
	return (true);
}
