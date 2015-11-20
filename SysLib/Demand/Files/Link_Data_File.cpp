//********************************************************* 
//	Link_Data_File.cpp - Link Data File Input/Output
//*********************************************************

#include "Link_Data_File.hpp"

#include "Utility.hpp"

#include <string.h>

//-----------------------------------------------------------
//	Link_Data_File constructors
//-----------------------------------------------------------

Link_Data_File::Link_Data_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format), Time_Range (1, CLOCK24)
{
	Setup ();
}

Link_Data_File::Link_Data_File (char *filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format), Time_Range (1, CLOCK24)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Link_Data_File destructor
//-----------------------------------------------------------

Link_Data_File::~Link_Data_File (void)
{
	Clear_Fields ();
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Link_Data_File::Setup (void)
{
	File_Type ("Link Data File");
	File_ID ("LinkData");

	decimal = 0;
	link = anode = bnode = 0;
	ab = ba = NULL;

	Custom_Header (true);
}

//-----------------------------------------------------------
//	Read_Header
//-----------------------------------------------------------

bool Link_Data_File::Read_Header (bool stat)
{
	int i;
	char buffer [FIELD_BUFFER];

	Db_Field *fld;

	Db_Header::Read_Header (stat);

	for (i=1; i <= Num_Fields (); i++) {
		fld = Field (i);
		str_cpy (buffer, sizeof (buffer), fld->Name ());
			
		if (str_cmp (buffer, "AB_", 3) != 0) continue;

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

bool Link_Data_File::Create_Fields (void) 
{
	int i;
	char buffer [FIELD_BUFFER];

	Add_Field ("LINK", INTEGER, 10);
	Add_Field ("ANODE", INTEGER, 10);
	Add_Field ("BNODE", INTEGER, 10);

	for (i=1; i <= Num_Ranges (); i++) {
		str_fmt (buffer, sizeof (buffer), "AB_%s", Range_Label (i));

		if (decimal == 0) {
			Add_Field (buffer, INTEGER, 10);
		} else {
			Add_Field (buffer, DOUBLE, 10, decimal);
		}
	}
	for (i=1; i <= Num_Ranges (); i++) {
		str_fmt (buffer, sizeof (buffer), "BA_%s", Range_Label (i));

		if (decimal == 0) {
			Add_Field (buffer, INTEGER, 10);
		} else {
			Add_Field (buffer, DOUBLE, 10, decimal);
		}
	}
	return (Set_Field_Numbers ());
}

bool Link_Data_File::Create_Fields (int low, int high, int increment)
{
	if (low < 0 || low > high) return (false);

	if (!Add_Range (low, high-1, increment)) return (false);

	return (Create_Fields ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Link_Data_File::Set_Field_Numbers (void)
{
	int i;
	char buffer [FIELD_BUFFER];

	Clear_Fields ();

	link = Required_Field ("LINK", "ID");

	if (link == 0) return (false);

	anode = Optional_Field ("ANODE", "NODEA", "A");
	bnode = Optional_Field ("BNODE", "NODEB", "B");

	//---- allocate field number space ----

	i = Num_Ranges ();
	if (i++ != 0) {
		ab = new int [i];
		ba = new int [i];

		if (ab == NULL || ba == NULL) return (false);

		memset (ab, '\0', i * sizeof (int));
		memset (ba, '\0', i * sizeof (int));

		//---- identify the field number for each range ----

		for (i=1; i <= Num_Ranges (); i++) {
			str_fmt (buffer, sizeof (buffer), "AB_%s", Range_Format (i));

			ab [i] = Optional_Field (buffer);

			if (ab [i] == 0) {
				str_fmt (buffer, sizeof (buffer), "AB_%s", Range_Label (i));

				ab [i] = Required_Field (buffer);
			}
			buffer [0] = 'B';
			buffer [1] = 'A';

			ba [i] = Required_Field (buffer);
		}
	}
	return (true);
}

//-----------------------------------------------------------
//	Zero_Fields
//-----------------------------------------------------------

void Link_Data_File::Zero_Fields (void)
{
	for (int i=1; i <= Num_Fields (); i++) {
		Put_Field (i, 0);
	}
}

//-----------------------------------------------------------
//	Clear_Fields
//-----------------------------------------------------------

void Link_Data_File::Clear_Fields (void)
{
	link = anode = bnode = 0;

	if (ab != NULL) {
		delete [] ab;
		ab = NULL;
	}
	if (ba != NULL) {
		delete [] ba;
		ba = NULL;
	}
}
