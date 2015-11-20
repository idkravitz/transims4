//*********************************************************
//	User_Program.cpp - Processes User Programs
//*********************************************************

#include "User_Program.hpp"

#include <string.h>
#include <stdarg.h>
#include <time.h>

//----------------------------------------------------------
//	User_Program constructor - No Files
//----------------------------------------------------------

User_Program::User_Program (void) : Program_Code ()
{
	Initialize (0);
}

//----------------------------------------------------------
//	User_Program constructor - Db_Base list
//----------------------------------------------------------

User_Program::User_Program (int count, Db_Base *file1, ...) : Program_Code ()
{
	if (!Initialize (count)) return;

	if (num_files == 0) return;
	
	va_list	args;
		
	va_start (args, file1);

	for (int i=0; i < num_files; i++) {
		
		file [i] = (i == 0) ? file1 : va_arg (args, Db_Base *);

		if (file [i] != NULL) {
			file_type [i] = file [i]->Record_Format ();
			read_only [i] = (file [i]->File_Access () == Db_Code::READ);
		}
	}
	va_end (args);
}

//----------------------------------------------------------
//	User_Program constructor - Db_Base array
//----------------------------------------------------------

User_Program::User_Program (int count, Db_Base **files) : Program_Code ()
{
	Set_Files (count, files);
}

//----------------------------------------------------------
//	Set_Files - Db_Base list
//----------------------------------------------------------

bool User_Program::Set_Files (int count, Db_Base *file1, ...)
{
	if (!Initialize (count)) return (false);

	if (num_files == 0) return (true);
	
	va_list	args;
		
	va_start (args, file1);

	for (int i=0; i < num_files; i++) {
		
		file [i] = (i == 0) ? file1 : va_arg (args, Db_Base *);

		if (file [i] != NULL) {
			file_type [i] = file [i]->Record_Format ();
			read_only [i] = (file [i]->File_Access () == Db_Code::READ);
		}
	}
	va_end (args);
	return (true);
}

//----------------------------------------------------------
//	Set_Files - Db_Base array
//----------------------------------------------------------

bool User_Program::Set_Files (int count, Db_Base **files)
{
	if (!Initialize (count)) return (false);

	if (num_files == 0) return (true);

	for (int i=0; i < num_files; i++) {
		file [i] = files [i];

		if (file [i] != NULL) {
			file_type [i] = file [i]->Record_Format ();
			read_only [i] = (file [i]->File_Access () == Db_Code::READ);
		}
	}
	return (true);
}

//-----------------------------------------------------------
//	Initialize
//-----------------------------------------------------------

bool User_Program::Initialize (int count)
{
	table_flag = declare_flag = ascii_write = open_par = false;
	level = 1;
	record = line_num = assign_set = if_level = 0;
	last_value = nest = loop = first_read = -1;

	token_level.Num_Records (MAX_LEVEL);

	Random_Seed ();

	//---- file data ----

	if (count <= 0) {
		num_files = 0;
		return (true);
	}
	file = new Db_Base * [count];
	file_type = new int [count];
	file_stat = new int [count];
	read_only = new int [count];
	out_flag = new int [count];

	if (file == NULL || file_type == NULL || file_stat == NULL ||
		read_only == NULL || out_flag == NULL) return (false);

	num_files = count;

	memset (file, '\0', num_files * sizeof (Db_Base *));
	memset (file_type, '\0', num_files * sizeof (int));
	memset (file_stat, '\0', num_files * sizeof (int));
	memset (read_only, '\0', num_files * sizeof (int));
	memset (out_flag, '\0', num_files * sizeof (int));

	return (true);
}

//----------------------------------------------------------
//	Clear
//----------------------------------------------------------

void User_Program::Clear (void)
{
	int i, j;

	//---- compile data ----

	token_level.Clear ();
	stack.Clear ();
	command.Clear ();
	lvalue.Clear ();
	fvalue.Clear ();
	svalue.Clear ();	

	//---- file data ----

	if (num_files) {
		if (file != NULL) {
			delete [] file;
		}
		if (file_type != NULL) {
			delete [] file_type;
		}
		if (file_stat != NULL) {
			delete [] file_stat;
		}
		if (out_flag != NULL) {
			delete [] out_flag;
		}
		if (read_only != NULL) {
			delete [] read_only;
		}
	}

	//---- table data ----

	for (i=0; i < 3; i++) {
		for (j=0; j < MAX_TABLE; j++) {
			table [i] [j].Clear ();
		}
	}
}
