//********************************************************* 
//	Db_File.hpp - database file
//*********************************************************

#ifndef DB_FILE_HPP
#define DB_FILE_HPP

#include "Db_Record.hpp"

#include "sqlite3.h"

#include <stdio.h>
#include <sys/types.h>

//---------------------------------------------------------
//	Db_File Class definition
//---------------------------------------------------------

class  Db_File : public Db_Record
{
public:
	Db_File (Access_Type access = READ, Format_Type format = UNFORMATED);
	Db_File (char *filename, Access_Type access = READ, Format_Type format = UNFORMATED);
	virtual ~Db_File (void);

	void Initialize (Access_Type access = READ, Format_Type format = UNFORMATED);

	virtual bool Filename (char *path);
	char * Filename (void)							{ return ((filename != NULL) ? filename : (char *) ""); }

	bool Create (char *path = NULL)					{ File_Access (CREATE); return (Open (path)); }
	virtual bool Open (char *path = NULL);

	FILE * File (void)								{ return (file); }
	bool Check_File (void)							{ return (Is_Open () ? true : Status (NOT_OPEN)); }
	bool Is_Open (void)								{ return ((File_Format () == SQLITE3) ? (db_file != NULL) : (file != NULL)); }

	virtual bool  Read_Record (int number = 0);

	virtual bool  Read (void *record, int size);
	bool  Read (bool nest)							{ Nest (nest); return (Read ()); }
	bool  Read (void *record)						{ return (Read (record, Max_Size ())); }
	bool  Read (void)								{ return (Read (Record (), Max_Size ())); }
	bool  Read (void *record, int size, off_t offset);

	virtual bool  Write_Record (int number = 0);

	virtual bool  Write (void *record, int size);
	bool  Write (bool nest)							{ Nest (nest); return (Write ()); }
	bool  Write (const char *record)				{ return (Write ((void *) record, Max_Size ())); }
	bool  Write (void *record)						{ return (Write (record, Max_Size ())); }
	bool  Write (void)								{ return (Write (Record (), Max_Size ())); }
	bool  Write (void *record, int size, off_t offset);

	virtual bool  Close (void);
	virtual bool  Rewind (void);
	virtual off_t File_Size (void);

	off_t Offset (void);
	bool  Offset (off_t offset);

	void  First_Offset (off_t offset)				{ first_offset = offset; }
	off_t First_Offset (void)						{ return (first_offset); }

	void  File_Access (Access_Type access)			{ file_access = access; }
	Access_Type  File_Access (void)					{ return (file_access); }
	
	Format_Type  File_Format (void)					{ return (file_format); }
	void  File_Format (Format_Type format);
	void File_Format (char *text)					{ File_Format (Format_Code (text)); }

	int  Num_Records (void);
	void Num_Records (int _num_records)				{ num_records = _num_records; }

	virtual int Estimate_Records (void);

	int  Record_Number (void)						{ return (record_num); }
	virtual bool Record_Number (int number);

	int  Max_Record_Number (void)					{ return (max_record_num); }

	void Flush (void)								{ fflush (file); }

protected:
	virtual bool Input_Record (char *record, int size)		{ return (Record (record, size)); }
	virtual char * Output_Record (char *record)		{ return (record); }

	bool new_nest_flag;
	int parent_id;
	sqlite3 *db_file;
	sqlite3_stmt *insert_stmt, *insert_nest;
	sqlite3_stmt *read_stmt, *read_nest;

private:
	FILE *file;

	Access_Type file_access;
	Format_Type file_format;

	int   num_records, record_num, max_record_num;
	off_t first_offset;

	char *filename;
};
#endif
