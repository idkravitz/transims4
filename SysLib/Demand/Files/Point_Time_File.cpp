//********************************************************* 
//	Point_Time_File.cpp - Point Time File Input/Output
//*********************************************************

#include "Point_Time_File.hpp"

#include "Utility.hpp"

#include <string.h>

//-----------------------------------------------------------
//	Point_Time_File constructors
//-----------------------------------------------------------

Point_Time_File::Point_Time_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format), Time_Range (1, CLOCK24)
{
	Setup ();
}

Point_Time_File::Point_Time_File (char *filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format), Time_Range (1, CLOCK24)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Point_Time_File destructor
//-----------------------------------------------------------

Point_Time_File::~Point_Time_File (void)
{
	Clear_Fields ();
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Point_Time_File::Setup (void)
{
	File_Type ("Point Time File");
	File_ID ("PointTime");

	decimal = 0;
	id = 0;
	in = out = NULL;

	Custom_Header (true);
}

//-----------------------------------------------------------
//	Read_Header
//-----------------------------------------------------------

bool Point_Time_File::Read_Header (bool stat)
{
	int i;
	char buffer [FIELD_BUFFER];

	Db_Field *fld;

	Db_Header::Read_Header (stat);

	for (i=1; i <= Num_Fields (); i++) {
		fld = Field (i);
		str_cpy (buffer, sizeof (buffer), fld->Name ());
			
		if (str_cmp (buffer, "IN_", 3) != 0) continue;

		if (!Add_Label (buffer + 3)) return (false);

		//---- check for decimal points ----

		if (fld->Decimal () > decimal) {
			decimal = fld->Decimal ();
		}
	}
	return (Set_Field_Numbers ());
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Point_Time_File::Create_Fields (void) 
{
	int i;
	char buffer [FIELD_BUFFER];

	Add_Field ("ID", INTEGER, 10);

	for (i=1; i <= Num_Ranges (); i++) {
		str_fmt (buffer, sizeof (buffer), "IN_%s", Range_Label (i));

		if (decimal == 0) {
			Add_Field (buffer, INTEGER, 10);
		} else {
			Add_Field (buffer, DOUBLE, 10, decimal);
		}
	}
	for (i=1; i <= Num_Ranges (); i++) {
		str_fmt (buffer, sizeof (buffer), "OUT_%s", Range_Label (i));

		if (decimal == 0) {
			Add_Field (buffer, INTEGER, 10);
		} else {
			Add_Field (buffer, DOUBLE, 10, decimal);
		}
	}
	return (Set_Field_Numbers ());
}

bool Point_Time_File::Create_Fields (int low, int high, int increment)
{
	if (low < 0 || low > high) return (false);

	if (!Add_Range (low, high-1, increment)) return (false);

	return (Create_Fields ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Point_Time_File::Set_Field_Numbers (void)
{
	int i;
	char buffer [FIELD_BUFFER];

	Clear_Fields ();

	id = Required_Field ("ID", "LOCATION", "ZONE", "POINT");

	if (id == 0) return (false);

	//---- allocate field number space ----

	i = Num_Ranges ();
	if (i++ != 0) {
		in = new int [i];
		out = new int [i];

		if (in == NULL || out == NULL) return (false);

		memset (in, '\0', i * sizeof (int));
		memset (out, '\0', i * sizeof (int));

		//---- identify the field number for each range ----

		for (i=1; i <= Num_Ranges (); i++) {
			str_fmt (buffer, sizeof (buffer), "IN_%s", Range_Format (i));

			in [i] = Optional_Field (buffer);

			if (in [i] == 0) {
				str_fmt (buffer, sizeof (buffer), "IN_%s", Range_Label (i));

				in [i] = Required_Field (buffer);
			}
			str_fmt (buffer, sizeof (buffer), "OUT_%s", Range_Format (i));

			out [i] = Optional_Field (buffer);

			if (out [i] == 0) {
				str_fmt (buffer, sizeof (buffer), "OUT_%s", Range_Label (i));

				out [i] = Required_Field (buffer);
			}
		}
	}
	return (true);
}

//-----------------------------------------------------------
//	Zero_Fields
//-----------------------------------------------------------

void Point_Time_File::Zero_Fields (void)
{
	for (int i=1; i <= Num_Fields (); i++) {
		Put_Field (i, 0);
	}
}

//-----------------------------------------------------------
//	Clear_Fields
//-----------------------------------------------------------

void Point_Time_File::Clear_Fields (void)
{
	id = 0;

	if (in != NULL) {
		delete [] in;
		in = NULL;
	}
	if (out != NULL) {
		delete [] out;
		out = NULL;
	}
}
