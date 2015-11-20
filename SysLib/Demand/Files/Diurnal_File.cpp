//********************************************************* 
//	Diurnal_File.cpp - Diurnal File Input/Output
//*********************************************************

#include "Diurnal_File.hpp"

//-----------------------------------------------------------
//	Diurnal_File constructors
//-----------------------------------------------------------

Diurnal_File::Diurnal_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Diurnal_File::Diurnal_File (char *filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Diurnal_File destructor
//-----------------------------------------------------------

Diurnal_File::~Diurnal_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Diurnal_File::Setup (void)
{
	File_Type ("Diurnal Distribution File");
	File_ID ("Diurnal");

	start = end = share = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Diurnal_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Header_Lines (0);
	}
	Add_Field ("START", STRING, TIME_BUFFER);
	Add_Field ("END", STRING, TIME_BUFFER);
	Add_Field ("SHARE", DOUBLE, 16, 6);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Diurnal_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	start = Required_Field ("START", "START_TIME", "STARTTIME", "TIME1");
	end = Required_Field ("END", "END_TIME", "ENDTIME", "TIME2");

	if (!start || !end) return (false);

	if (share == 0) {
		share = Optional_Field ("SHARE", "PERCENT", "COUNT", "VOLUME");

		if (share == 0) {
			if (Num_Fields () > 2 && start == 1 && end == 2) {
				share = 3;
			} else {
				Required_Field ("SHARE", "PERCENT", "COUNT", "VOLUME");
				return (false);
			}
		}
	}

	//---- optional fields ----

	return (true);
}

//-----------------------------------------------------------
//	Default_Definition
//-----------------------------------------------------------

bool Diurnal_File::Default_Definition (void)
{
	if (Dbase_Format () == VERSION3) {
		Create_Fields ();

		return (Write_Def_Header (NULL));
	} else {
		return (false);
	}
}
