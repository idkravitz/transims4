//********************************************************* 
//	Db_Header.cpp - general database interface
//*********************************************************

#include "Db_Header.hpp"

#include "Utility.hpp"
#include "Date_Time.hpp"

char Db_Header::aecom_header [] = "AECOM HEADER";
char Db_Header::nested_flag [] = "NESTED";

//-----------------------------------------------------------
//	Db_Header constructors
//-----------------------------------------------------------

Db_Header::Db_Header (Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Base (access, format)
{
	header_record = NULL;

	Header_Lines (1);
	Scan_Lines ();
	Custom_Header (false);
	Notes_Field (0);
	Notes_Flag (notes_flag);
	LinkDir_Type (LINK_NODE);
}

Db_Header::Db_Header (char *filename, Access_Type access, Format_Type format, bool notes_flag) : 
	Db_Base (access, format)
{
	header_record = NULL;

	Header_Lines (1);
	Scan_Lines ();
	Custom_Header (false);
	Notes_Field (0);
	Notes_Flag (notes_flag);
	LinkDir_Type (LINK_NODE);

	Open (filename);
}

//-----------------------------------------------------------
//	Db_Header destructor
//-----------------------------------------------------------

Db_Header::~Db_Header (void)
{
	if (header_record != NULL) {
		free (header_record);
		header_record = NULL;
	}
}

//-----------------------------------------------------------
//	Open
//-----------------------------------------------------------

bool Db_Header::Open (char *filename)
{
	bool stat;
	Format_Type format;

	if (filename != NULL) {
		Filename (filename);
	}

	//---- read the definition file if it exists ----

	filename = Filename ();

	if (*filename != '\0' && File_Access () != CREATE) {
		stat = Read_Def_Header ();
	} else {
		stat = false;
	}

	//---- open the file ----

	if (Db_File::Open ()) {

		if (File_Access () != CREATE) {

			//---- process the header ----

			if (!stat && Header_Lines () == 0 && Num_Fields () != 0) {
				stat = Write_Def_Header (NULL);
			}
			if (!Read_Header (stat)) {
				return (Status (OPEN_DEF));
			}
			format = Dbase_Format ();

			if (format != Default_Format () && Send_Messages ()) {
				exe->Print (1, "%s Format = %s", File_Type (), Format_Code (format));
			}

		} else {
			if (Dbase_Format () != Default_Format () && Send_Messages ()) {
				exe->Print (1, "%s Format = %s", File_Type (), Format_Code (Dbase_Format ()));
			}
			if (!Custom_Header ()) {
				if (Num_Fields () == 0) Create_Fields ();
				if (Num_Fields () > 0 && (Header_Lines () <= 1 ||
					(Nested () && Header_Lines () == 2))) {
					Write_Header ();
				}
			}
		}
		return (true);
	}
	return (false);
}

//-----------------------------------------------------------
//	Close
//-----------------------------------------------------------

bool Db_Header::Close (void)
{
	if (!Is_Open ()) return (Db_File::Close ());

	if (Dbase_Format () == DBASE && File_Access () != READ) {
		int records = Max_Record_Number ();
		if (records < Num_Records ()) {
			records = Num_Records ();
		}
		if (records > 0) {
			dBase_Header header;
			
			if (!Read (&header, sizeof (header), 0L)) {
				return (Status (FILE_HEADER));
			}
			
			//---- update the database header ----

			if (header.num_records < records) {
				header.num_records = records;
		
				if (!Write (&header, sizeof (header), 0L)) {
					return (Status (HEADER_WRITE));
				}
			}
		}
	}
	if (File_Access () == READ && File_Format () == CSV_DELIMITED) {
		Db_Field *fld;
		bool def_flag = false;

		for (int i=1; i <= Num_Fields (); i++) {
			fld = Field (i);
			if (fld->Resized ()) def_flag = true;
		}
		if (def_flag) {
			Write_Def_Header (Header_Record ());
		}
	}
	return (Db_File::Close ());
}

//-----------------------------------------------------------
//	Header_Record
//-----------------------------------------------------------

bool Db_Header::Header_Record (char *header)
{
	if (header == NULL) return (false);

	int len = (int) strlen (header) + 1;

	header_record = (char *) realloc (header_record, len);

	if (header_record == NULL) return (false);

	str_cpy (header_record, len, header);
	return (true);
}

//-----------------------------------------------------------
//	Read_Header
//-----------------------------------------------------------

bool Db_Header::Read_Header (bool stat)
{
	Offset (0L);

	if (File_Format () == CSV_DELIMITED) {
		if (!Read_CSV_Header (stat)) {
			return (Status (FILE_HEADER));
		}
	} else if (Dbase_Format () == DBASE) {
		if (!Read_dBase_Header (stat)) return (false);
	} else if (File_Format () == SQLITE3) {
		if (!Read_SQLite3_Header (stat)) return (false);
	} else if (!stat) {
		return (Status (OPEN_DEF));
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Write_Header
//-----------------------------------------------------------

bool Db_Header::Write_Header (char *user_lines) 
{
	Offset (0L);

	if (user_lines == NULL) {
		user_lines = Header_Record ();
	}
	if (File_Format () == CSV_DELIMITED) {
		if (!Write_CSV_Header (user_lines)) return (false);
	} else if (Dbase_Format () == DBASE) {
		if (!Write_dBase_Header ()) return (false);
	} else if (File_Format () == SQLITE3) {
		if (!Write_SQLite3_Header ()) return (false);
	}
	if (!Write_Def_Header (user_lines)) return (false);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Read_CSV_Header
//-----------------------------------------------------------

bool Db_Header::Read_CSV_Header (bool stat)
{
	int i, j, k, len, nfield, decimal, exponent;
	char *ptr, *p, buffer [FIELD_BUFFER];
	Db_Field *fld_ptr;
	Format_Type type;

	if (Header_Lines () == 0) return (stat);

	type = Dbase_Format ();
	File_Format (UNFORMATED);
	Record_Format (type);

	//---- read the database header ----

	len = 0;

	if (Nested ()) {
		for (i=2; i < Header_Lines (); i++) {
			if (!Read (false)) return (false);
			Header_Record (Record ());
			len += (int) strlen (Record ());
		}
		if (!Read (false)) return (false);
		len += (int) strlen (Record ());

		if (!Read (true)) return (false);
		len += (int) strlen (Record ());

		Nest (false);
	} else {
		for (i=1; i <= Header_Lines (); i++) {
			if (!Read ()) return (false);
			len += (int) strlen (Record ());

			if (i < Header_Lines ()) {
				Header_Record (Record ());
			}
		}
	}
	First_Offset (len);

	//---- validate the definition file ----

	if (stat) {
		Db_Field *fld_ptr;

		nfield = 0;

		for (i=0; i < 2; i++) {
			ptr = Clean_Record ();

			//---- process the header record ----

			while (ptr != NULL) {

				ptr = Parse_String (ptr, buffer, sizeof (buffer));

				if (*buffer == '\0') break;

				fld_ptr = Field (++nfield);

				if (fld_ptr == NULL || str_cmp (fld_ptr->Name (), buffer)) {
					exe->Write (1, "field %d does not match %s vs %s", nfield, ((fld_ptr != NULL) ? fld_ptr->Name () : "NULL"), buffer);
					nfield = 0;
					break;
				}
			}
			if (!Nested () || nfield == 0) break;
			Nest (true);
		}
		if (nfield != Num_Fields ()) {
			Clear_Fields ();
			stat = false;
		}
	}

	//---- if a definition file was read, skip the field scan ----

	if (!stat) {
		if (Nested ()) {
			return (Status (DEF_NEST));
		}
		ptr = Clean_Record ();

		//---- refine the delimiter type ----

		if (Record_Format () == CSV_DELIMITED) {
			
			//---- scan for tabs ----

			i = 0;
			p = ptr;

			while (p != NULL) {
				p = strchr (p, '\t');
				if (p != NULL) {
					i++;
					p++;
				}
			}

			//---- scan for commas ----

			j = 0;
			p = ptr;

			while (p != NULL) {
				p = strchr (p, ',');
				if (p != NULL) {
					j++;
					p++;
				}
			}

			//---- reset the delimiter ----

			if (i == 0 && j == 0) {
				Record_Format (type = SPACE_DELIMITED);
			} else if (i == 0 || j > i * 2) {
				Record_Format (type = COMMA_DELIMITED);
			} else if (j == 0 || i > j * 2) {
				Record_Format (type = TAB_DELIMITED);
			}
		}

		//---- process the header record ----

		while (ptr != NULL) {

			ptr = Parse_String (ptr, buffer, sizeof (buffer));

			if (*buffer == '\0') break;

			if ((buffer [0] >= 'a' && buffer [0] <= 'z') || 
				(buffer [0] >= 'A' && buffer [0] <= 'Z') || buffer [0] == '_') {

				if (str_cmp (buffer, "SQLite format 3") == 0) {
					exe->Error ("Opened an SQLITE3 Database as a Text File");
				}
				if (!Add_Field (buffer, INTEGER, 1)) return (false);
			} else {
				File_Format (type);
				return (false);
			}
		}
		nfield = Num_Fields ();

		if (nfield == 0) return (Status (NO_FIELD));

		//---- scan the file for field information ----

		for (k=0; k < scan_lines || scan_lines == SCAN_ALL_RECORDS; k++) {
			if (!Read ()) break;

			//---- read the data fields ----

			ptr = Clean_Record ();

			for (i=1; i <= nfield && ptr != NULL; i++) {
				ptr = Parse_String (ptr, buffer, sizeof (buffer));

				if (*buffer == '\0') continue;

				//---- change NA to zero ----

				if (str_cmp (buffer, "NA") == 0) {
					buffer [0] = '0';
					buffer [1] = '\0';
				}
				fld_ptr = Field (i);
				if (fld_ptr == NULL) return (Status (NO_FIELD));

				//---- check the field size ----

				len = (int) strlen (buffer);
				if (scan_lines != SCAN_ALL_RECORDS && len < 10) {
					if (fld_ptr->Size () < 10) {
						fld_ptr->Size (10);
					}
				} else {
					if (fld_ptr->Size () < len) fld_ptr->Size (len);
				}

				//---- check for non-numbers and decimal points ----

				decimal = exponent = 0;

				for (j=0; j < len; j++) {
					if (buffer [j] < '0' || buffer [j] > '9') {
						if (buffer [j] == '.' && decimal == 0) {
							decimal = len - j - 1;
							continue;
						}
						if (buffer [j] == '-') {
							if (j == 0) continue;
							if (exponent > 0 && j == exponent + 1) continue;
						}
						if (buffer [j] == 'E' && exponent == 0 && decimal != 0) {
							exponent = j;
							continue;
						}
						break;
					}
				}
				if (j < len) {
					fld_ptr->Type (STRING);
				} else if (decimal) {
					fld_ptr->Type (DOUBLE);
					if (decimal > fld_ptr->Decimal ()) fld_ptr->Decimal (decimal);
				}
			}
		}

		//---- set the number of records ----

		if (scan_lines == SCAN_ALL_RECORDS) {
			Num_Records (k);
		}
	}

	//---- check the header alignment ----

	if (scan_lines != 0) {
		Rewind ();
	}

	//---- restore the file format ----

	File_Format (type);

	if (!stat) {
		Write_Def_Header (Header_Record ());
	}
	return (Record_Number (-1));
}

//-----------------------------------------------------------
//	Write_CSV_Header
//-----------------------------------------------------------

bool Db_Header::Write_CSV_Header (char *user_lines) 
{
	int i, len, nfield, lines;
	char *name, *ptr, *nptr, delimiter;
	bool first, first_nest;
	Db_Field *fld_ptr;
	FILE *fh;

	if (Header_Lines () <= 0) return (true);

	fh = File ();
	if (fh == NULL) return (Status (NOT_OPEN));

	//---- write extra header lines ----

	if (Nested ()) {
		lines = 2;
	} else {
		lines = 1;
	}

	if (Header_Lines () > lines) {
		ptr = user_lines;

		for (i=lines; i < Header_Lines (); i++) {
			if (ptr != NULL) {
				nptr = strchr (ptr, '\n');

				if (nptr != NULL) {
					len = (int) (nptr - ptr);
					fprintf (fh, "%*.*s\n", len, len, ptr);
					ptr = nptr + 1;
				} else {
					fprintf (fh, "%s\n", ptr);
					ptr = NULL;
				}
			} else {
				fprintf (fh, "\n");
			}
		}
	}
	nfield = Num_Fields ();
	if (nfield == 0) return (Status (NO_FIELD));	

	//---- setup the output record ----

	delimiter = *(Delimiters ());

	//--- write each field name ----

	first = first_nest = true;

	for (i=1; i <= nfield; i++) {
        fld_ptr = Field (i);

		if (fld_ptr->Nested () && first_nest) {
			fprintf (fh, "\n");
			first_nest = false;
			first = true;
		}
		if (!first) {
			fprintf (fh, "%c", delimiter);
		} else {
			first = false;
		}
		name = fld_ptr->Name ();

		if (name != NULL) {
			if (strstr (name, Delimiters ())) {
				fprintf (fh, "\"%s\"", name);
			} else {
				fprintf (fh, "%s", name);
			}
		}
	}

	//---- end the header line ----

	fprintf (fh, "\n");

	First_Offset (Offset ());

	return (Record_Number (-1));
}

//-----------------------------------------------------------
//	Read_dBase_Header
//-----------------------------------------------------------

bool Db_Header::Read_dBase_Header (bool stat)
{
	int i, nfield, offset;
	char marker, name [16];
	Field_Type type;

	dBase_Header header;
	dBase_Field field;

	//---- read the database header ----

	if (!Read (&header, sizeof (header), 0L)) {
		return (Status (FILE_HEADER));
	}
	nfield = (header.header_size - sizeof (header) - 1) / sizeof (field);
	offset = 1;		//---- delete marker ----

	//---- validate the definition file ----

	if (stat) {
		if (Num_Fields () == nfield && Num_Fields () > 0) {
			Db_Field *fld_ptr;

			for (i=1; i <= nfield; i++) {
				if (!Read (&field, sizeof (field))) {
					return (Status (FIELD_LIST));
				}
				fld_ptr = Field (i);
				str_cpy (name, sizeof (name), field.name);
				name [11] = '\0';

				if (fld_ptr == NULL || str_cmp (name, fld_ptr->Name ())) {
					stat = false;
					break;
				}
			}
			if (!stat) {
				Read (&header, sizeof (header), 0L);
				Clear_Fields ();
			}
		} else {
			stat = false;
			Clear_Fields ();
		}
	}

	//---- read each field ----

	if (!stat) {
		for (i=1; i <= nfield; i++) {
			if (!Read (&field, sizeof (field))) {
				return (Status (FIELD_LIST));
			}
			marker = field.type;
			if (marker > 90) marker -= 32;

			switch (marker) {
				case 'C':
					type = STRING;
					break;
				case 'D':
					type = DATE;
					break;
				case 'L':
					type = CHAR;
					break;
				case 'N':
					if (field.decimal) {
						type = FIXED;
					} else {
						type = INTEGER;
					}
					break;
				case 'F':
					type = DOUBLE;
					break;
				default:
					offset += field.width;
					continue;
			}
			str_cpy (name, sizeof (name), field.name);
			name [11] = '\0';

			if (!Add_Field (name, type, (int) field.width, (int) field.decimal, offset)) {
				return (false);
			}
			offset += field.width;
		}
	}

	//---- read the end of header marker ----

	Read (&marker, sizeof (char));

	if (marker != 13) {
		return (Status (RECORD_SYNTAX));
	}

	//---- set the record information ----

	Record_Size (0);
	Record_Size (header.record_size);
	First_Offset (header.header_size);
	Num_Records (header.num_records);

	if (!stat) {
		Write_Def_Header (NULL);
	}
	return (Record_Number (-1));
}

//-----------------------------------------------------------
//	Write_dBase_Header
//-----------------------------------------------------------

bool Db_Header::Write_dBase_Header (void) 
{
	char marker, *name;
	int i, j, nfield, size, max_size, num_field;
	time_t ltime;
	struct tm *dt;
	Db_Field *fld;

	dBase_Header header;
	dBase_Field field;

	nfield = Num_Fields ();

	if (nfield == 0) return (Status (NO_FIELD));

	//---- get the record size ----

	max_size = num_field = 0;

	for (i=1; i <= nfield; i++) {
		fld = Field (i);
		if (fld == NULL) continue;

		size = fld->Offset () + fld->Size ();

		if (size > max_size) {
			max_size = size;
		}
		num_field++;
	}

	//---- write the header ----

	time (&ltime);
	dt = local_time (&ltime);

	memset (&header, '\0', sizeof (header));

	header.date [0] = (char) dt->tm_year;
	header.date [1] = (char) (dt->tm_mon + 1);
	header.date [2] = (char) dt->tm_mday;

	header.version = 3;
	header.num_records = Num_Records ();
	header.header_size = (short) (sizeof (header) + num_field * sizeof (dBase_Field) + 1);
	header.record_size = (short) max_size;
	
	if (!Write (&header, sizeof (header), 0L)) {
		return (Status (HEADER_WRITE));
	}

	//---- write each field ----

	for (int i=1; i <= nfield; i++) {
        fld = Field (i);
		if (fld == NULL) continue;

		memset (&field, '\0', sizeof (field));

		switch (fld->Type ()) {
			case DATE:
				field.type = 'D';
				break;
			case STRING:
			case CHAR:
			case TIME:
			case DATE_TIME:
			case DAY_TIME:
				field.type = 'C';
				break;
			case DOUBLE:
				field.type = 'F';
				break;
			case INTEGER:
			case UNSIGNED:
			case FIXED:
				field.type = 'N';
				break;
		}
		name = fld->Name ();
		size = (int) strlen (name);
		
		if (size > sizeof (field.name)) {
			size = (int) sizeof (field.name);
		}
		for (j=0; j < size; j++, name++) {
			field.name [j] = *name;
		}
		field.width = (char) (fld->Size () & 0xff);	
		field.decimal = (char) (fld->Decimal () & 0xff);

		if (!Write (&field, sizeof (field))) {
			return (Status (HEADER_WRITE));
		}
	}
	marker = 13;

	if (!Write (&marker, sizeof (char))) {
		return (Status (HEADER_WRITE));
	}
	Record_Size (0);
	Record_Size (header.record_size);
	First_Offset (header.header_size);

	return (Record_Number (-1));
}

//-----------------------------------------------------------
//	Read_SQLite3_Header
//-----------------------------------------------------------

bool Db_Header::Read_SQLite3_Header (bool stat)
{
	int i, nfield, nnest, num_rows, length;
	char sql [4096];
	const char *name;
	Field_Type type;

	//---- get the number of rows ----

	First_Offset (0);
	num_rows = Num_Records ();

	//---- create the read statements ----

	str_fmt (sql, sizeof (sql), "SELECT * FROM %s WHERE ROWID=?;", File_ID ());

	if (sqlite3_prepare_v2 (db_file, sql, -1, &read_stmt, NULL) != SQLITE_OK) {
		exe->Warning ("Preparing Read Statement: %s", sqlite3_errmsg (db_file));
	}
	nfield = sqlite3_column_count (read_stmt);

	if (Nested ()) {
		str_fmt (sql, sizeof (sql), "SELECT * FROM %s_Nest WHERE PARENTID=?;", File_ID ());

		if (sqlite3_prepare_v2 (db_file, sql, -1, &read_nest, NULL) != SQLITE_OK) {
			exe->Warning ("Preparing Read Statement: %s", sqlite3_errmsg (db_file));
		}
		nnest = sqlite3_column_count (read_nest) - 1;
	} else {
		nnest = 0;
	}

	//---- validate the definition file ----

	if (stat) {
		if (Num_Fields () == (nfield + nnest) && Num_Fields () > 0) {
			for (i=0; i < nfield; i++) {
				name = sqlite3_column_name (read_stmt, i);
				if (Field_Number ((char *) name) != (i + 1)) {
					stat = false;
					break;
				}
			}
			for (i=1; i <= nnest; i++) {
				name = sqlite3_column_name (read_nest, i);
				if (Field_Number ((char *) name) != (nfield + i)) {
					stat = false;
					break;
				}
			}
			if (!stat) {
				Clear_Fields ();
			}
		} else {
			stat = false;
			Clear_Fields ();
		}
	}

	//---- read each field ----

	if (!stat) {
		int result, sqlite_type, width, decimal;

		sqlite3_bind_int (read_stmt, 1, num_rows);
		result = sqlite3_step (read_stmt);

		if (result != SQLITE_ROW && result != SQLITE_DONE) {
			exe->Warning ("SQLITE3 Database Table %s: %s", File_ID (), sqlite3_errmsg (db_file));
			return (false);
		}
		length = 0;
		Record_Size (0);

		for (i=0; i < nfield; i++) {
			sqlite_type = sqlite3_column_type (read_stmt, i);

			switch (sqlite_type) {
				case SQLITE_INTEGER:
					type = INTEGER;
					width = 4;
					decimal = 0;
					break;
				case SQLITE_FLOAT:
					type = DOUBLE;
					width = 8;
					decimal = 2;
					break;
				case SQLITE_TEXT:
				case SQLITE_BLOB:
				case SQLITE_NULL:
				default:
					type = STRING;
					width = FIELD_BUFFER;
					decimal = 0;
					break;
			}
			name = sqlite3_column_name (read_stmt, i);

			if (!Add_Field ((char *) name, type, width, decimal, length, true, false)) {
				return (false);
			}
			length += width;
		}
		sqlite3_reset (read_stmt);

		//---- nested fields ----

		if (Nested ()) {
			sqlite3_bind_int (read_nest, 1, num_rows);
			result = sqlite3_step (read_nest);

			if (result != SQLITE_ROW && result != SQLITE_DONE) {
				exe->Warning ("SQLITE3 Database Table %s_Nest: %s", File_ID (), sqlite3_errmsg (db_file));
				return (false);
			}
			length = 0;
			Record_Size (0);

			for (i=1; i <= nnest; i++) {
				sqlite_type = sqlite3_column_type (read_nest, i);

				switch (sqlite_type) {
					case SQLITE_INTEGER:
						type = INTEGER;
						width = 4;
						decimal = 0;
						break;
					case SQLITE_FLOAT:
						type = DOUBLE;
						width = 8;
						decimal = 2;
						break;
					case SQLITE_TEXT:
					case SQLITE_BLOB:
					case SQLITE_NULL:
					default:
						type = STRING;
						width = FIELD_BUFFER;
						decimal = 0;
						break;
				}
				name = sqlite3_column_name (read_nest, i);

				if (!Add_Field ((char *) name, type, width, decimal, length, true, true)) {
					return (false);
				}
				length += width;
			}
			sqlite3_reset (read_nest);
		}
	}

	//---- set the record information ----

	if (!stat) {
		Write_Def_Header (NULL);
	}
	return (Record_Number (-1));
}

//-----------------------------------------------------------
//	Write_SQLite3_Header
//-----------------------------------------------------------

bool Db_Header::Write_SQLite3_Header (void) 
{
	char *name, buffer [STRING_BUFFER], sql [4096];
	int i, j, nfield;
	Db_Field *fld;

	nfield = Num_Fields ();

	if (nfield == 0) return (Status (NO_FIELD));

	//---- drop the table if it currently exists ----

	str_fmt (sql, sizeof (sql), "DROP TABLE %s;", File_ID ());
	sqlite3_exec (db_file, sql, NULL, NULL, NULL);

	//---- create the database table ----

	str_fmt (sql, sizeof (sql), "CREATE TABLE %s (", File_ID ());

	for (i=j=1; i <= nfield; i++) {
		fld = Field (i);
		if (!fld->Nested ()) {
			if (fld->Type () == INTEGER) {
				name = "INTEGER";
			} else if (fld->Type () == DOUBLE) {
				name = "FLOAT";
			} else {
				name = "TEXT";
			}
			str_fmt (buffer, sizeof (buffer), "%s%s %s", ((j++ == 1) ? " " : ", "), fld->Name (), name);
			str_cat (sql, sizeof (sql), buffer);
		}
	}
	str_cat (sql, sizeof (sql), " );");

	if (sqlite3_exec (db_file, sql, NULL, NULL, NULL) != SQLITE_OK) {
		exe->Warning ("Creating Database Table: %s", sqlite3_errmsg (db_file));
	}

	//---- create the read statements ----

	str_fmt (sql, sizeof (sql), "SELECT * FROM %s WHERE ROWID=?;", File_ID ());

	if (sqlite3_prepare_v2 (db_file, sql, -1, &read_stmt, NULL) != SQLITE_OK) {
		exe->Warning ("Preparing Read Statement: %s", sqlite3_errmsg (db_file));
	}

	//---- create the insert statements ----

	str_fmt (sql, sizeof (sql), "INSERT INTO %s VALUES (", File_ID ());

	for (i=j=1; i <= nfield; i++) {
		fld = Field (i);
		if (!fld->Nested ()) {
			str_cat (sql, sizeof (sql), (char *) ((j++ == 1) ? "?" : ",?"));
		}
	}
	str_cat (sql, sizeof (sql), " );");

	if (sqlite3_prepare_v2 (db_file, sql, -1, &insert_stmt, NULL) != SQLITE_OK) {
		exe->Warning ("Preparing Insert Statement: %s", sqlite3_errmsg (db_file));
	}

	//---- create the nested table ----
	
	if (Nested ()) {

		//---- drop the table if it currently exists ----

		str_fmt (sql, sizeof (sql), "DROP TABLE %s_Nest;", File_ID ());
		sqlite3_exec (db_file, sql, NULL, NULL, NULL);

		//---- create the database table ----

		str_fmt (sql, sizeof (sql), "CREATE TABLE %s_Nest (PARENTID INTEGER", File_ID ());

		for (i=1; i <= nfield; i++) {
			fld = Field (i);
			if (fld->Nested ()) {
				if (fld->Type () == INTEGER) {
					name = "INTEGER";
				} else if (fld->Type () == DOUBLE) {
					name = "FLOAT";
				} else {
					name = "TEXT";
				}
				str_fmt (buffer, sizeof (buffer), ", %s %s", fld->Name (), name);
				str_cat (sql, sizeof (sql), buffer);
			}
		}
		str_cat (sql, sizeof (sql), " );");

		if (sqlite3_exec (db_file, sql, NULL, NULL, NULL) != SQLITE_OK) {
			exe->Warning ("Creating Database Table: %s", sqlite3_errmsg (db_file));
		}

		//---- create the read statements ----

		str_fmt (sql, sizeof (sql), "SELECT * FROM %s_Nest WHERE PARENTID=?;", File_ID ());

		if (sqlite3_prepare_v2 (db_file, sql, -1, &read_nest, NULL) != SQLITE_OK) {
			exe->Warning ("Preparing Read Statement: %s", sqlite3_errmsg (db_file));
		}

		//---- create the insert statements ----

		str_fmt (sql, sizeof (sql), "INSERT INTO %s_Nest VALUES (?", File_ID ());

		for (i=1; i <= nfield; i++) {
			fld = Field (i);
			if (fld->Nested ()) {
				str_cat (sql, sizeof (sql), (char *) ",?");
			}
		}
		str_cat (sql, sizeof (sql), " );");

		if (sqlite3_prepare_v2 (db_file, sql, -1, &insert_nest, NULL) != SQLITE_OK) {
			exe->Warning ("Preparing Insert Statement: %s", sqlite3_errmsg (db_file));
		}
	}

	//---- inialized the processing block ----

	sqlite3_exec (db_file, "BEGIN", NULL, NULL, NULL);

	return (Record_Number (-1));
}

//-----------------------------------------------------------
//	Read_Def_Header
//-----------------------------------------------------------

bool Db_Header::Read_Def_Header (void) 
{
	bool binary, nested;
	int size, decimal, offset, lines;
	Field_Type type;
	char *str_ptr, filename [FILE_NAME_SIZE], buffer [FIELD_BUFFER], name [FIELD_BUFFER];
	FILE *def;

	//---- construct the definition filename ----

	str_cpy (filename, sizeof (filename)-5, Filename ());
	str_cat (filename, sizeof (filename), ".def");

	//---- open the definition file ----

	def = f_open (filename, "rt");

	if (def == NULL) return (Default_Definition ());

	//---- get the header line ----

	if (fgets (filename, sizeof (filename), def) == NULL) goto error;

	size = (int) strlen (aecom_header);

	if (str_cmp (filename, aecom_header, size) != 0) goto error;

	str_ptr = filename + size;

	str_ptr = Get_Token (str_ptr, buffer);

	Dbase_Format (buffer);

	binary = (Dbase_Format () == BINARY || Dbase_Format () == SQLITE3);

	str_ptr = Get_Token (str_ptr, buffer);

	Header_Lines (atoi (buffer));

	str_ptr = Get_Token (str_ptr, buffer);

	if (buffer [0] != '\0') {
		if (str_cmp (buffer, nested_flag) != 0) goto error;
		nested = true;
		lines = 2;
	} else {
		nested = false;
		lines = 1;
	}
	Nested (nested);

	if (binary || Dbase_Format () == FIXED_COLUMN) {
		Header_Lines (Header_Lines () + lines);
	}

	for (int i=lines; i < Header_Lines (); i++) {
		if (fgets (filename, sizeof (filename), def) == NULL) {
			exe->Error ("Reading the Header Lines");
		}
		Header_Record (filename);
	}

	//---- read the fields ----

	while (fgets (filename, sizeof (filename), def) != NULL) {
		str_ptr = Get_Token (filename, name);
		if (*name == '\0') continue;

		str_ptr = Get_Token (str_ptr, buffer);
		type = Field_Code (buffer);

		str_ptr = Get_Token (str_ptr, buffer);
		offset = atoi (buffer);

		str_ptr = Get_Token (str_ptr, buffer);
		size = atoi (buffer);

		str_ptr = Get_Token (str_ptr, buffer);
		decimal = atoi (buffer);

		str_ptr = Get_Token (str_ptr, buffer);

		if (buffer [0] != '\0') {
			if (str_cmp (buffer, nested_flag) != 0) goto error;
			nested = true;
		} else {
			nested = false;
		}
		if (!Add_Field (name, type, size, decimal, offset, binary, nested)) goto error;
	}
	fclose (def);
	return (true);

error:
	return (Status (DEF_FORMAT));
}

//-----------------------------------------------------------
//	Write_Def_Header
//-----------------------------------------------------------

bool Db_Header::Write_Def_Header (char *user_lines) 
{
	int i, num_lines, lines, len;
	char filename [FILE_NAME_SIZE], *ptr, *nptr;
	FILE *def;
	Db_Field *fld;

	//---- construct the definition filename ----

	str_cpy (filename, sizeof (filename)-5, Filename ());
	str_cat (filename, sizeof (filename), ".def");

	//---- create the definition file ----

	def = f_open (filename, "wt");

	if (def == NULL) return (Status (OPEN_DEF));

	if (Nested ()) {
		lines = 2;
	} else {
		lines = 1;
	}
	num_lines = Header_Lines ();

	if (Dbase_Format () == BINARY || Dbase_Format () == FIXED_COLUMN || Dbase_Format () == SQLITE3) {
		num_lines -= lines;
	}

	//---- write the header line ----

	if (Nested ()) {
		if (fprintf (def, "%s, %s, %d, %s\n", aecom_header, Format_Code (Dbase_Format ()), 
			num_lines, nested_flag) < 0) goto error;
	} else {
		if (fprintf (def, "%s, %s, %d\n", aecom_header, Format_Code (Dbase_Format ()), num_lines) < 0) goto error;
	}

	//---- write extra header lines ----

	if (Header_Lines () > lines) {
		ptr = user_lines;

		for (i=lines; i < Header_Lines (); i++) {
			if (ptr != NULL) {
				nptr = strchr (ptr, '\n');

				if (nptr != NULL) {
					len = (int) (nptr - ptr);
					fprintf (def, "%*.*s\n", len, len, ptr);
					ptr = nptr + 1;
				} else {
					fprintf (def, "%s\n", ptr);
					ptr = NULL;
				}
			} else {
				fprintf (def, "\n");
			}
		}
	}

	//---- write the fields ----

	for (i=1; i <= Num_Fields (); i++) {
		fld = Field (i);

		if (fld->Nested ()) {
			if (fprintf (def, "%s, %s, %d, %d, %d, %s\n", fld->Name (), Field_Code (fld->Type ()), 
				fld->Offset (), fld->Size (), fld->Decimal (), nested_flag) < 0) goto error;
		} else if (fld->Decimal () > 0) {
			if (fprintf (def, "%s, %s, %d, %d, %d\n", fld->Name (), Field_Code (fld->Type ()), 
				fld->Offset (), fld->Size (), fld->Decimal ()) < 0) goto error;
		} else {
			if (fprintf (def, "%s, %s, %d, %d\n", fld->Name (), Field_Code (fld->Type ()), 
				fld->Offset (), fld->Size ()) < 0) goto error;
		}
	}
	fclose (def);
	return (true);

error:
	return (Status (DEF_FORMAT));
}
