//********************************************************* 
//	Link_Dir_File.cpp - Link Direction File Input/Output
//*********************************************************

#include "Link_Dir_File.hpp"

#include "Utility.hpp"

#include <string.h>

//-----------------------------------------------------------
//	Link_Direction_File constructors
//-----------------------------------------------------------

Link_Direction_File::Link_Direction_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format), Time_Range (1, CLOCK24)
{
	Setup ();
}

Link_Direction_File::Link_Direction_File (char *filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format), Time_Range (1, CLOCK24)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Link_Direction_File destructor
//-----------------------------------------------------------

Link_Direction_File::~Link_Direction_File (void)
{
	Clear_Fields ();
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Link_Direction_File::Setup (void)
{
	File_Type ("Link Direction File");
	File_ID ("LinkDir");

	decimal = 0;
	link_dir = link = dir = 0;
	data = data2 = diff = NULL;
	Ignore_Periods (false);
	Join_Flag (false);
	Difference_Flag (false);
	Direction_Index (false);

	Custom_Header (true);
}

//-----------------------------------------------------------
//	Read_Header
//-----------------------------------------------------------

bool Link_Direction_File::Read_Header (bool stat)
{
	int i;
	char buffer [FIELD_BUFFER];

	Db_Field *fld;

	Db_Header::Read_Header (stat);

	Join_Flag (false);
	Difference_Flag (false);

	if (!Ignore_Periods ()) {
		for (i=1; i <= Num_Fields (); i++) {
			fld = Field (i);
			str_cpy (buffer, sizeof (buffer), fld->Name ());

			if (buffer [0] == 'P' && buffer [1] >= '0' && buffer [1] <= '9') {
				if (!Add_Label (buffer+1)) return (false);

				//---- check for decimal points ----

				if (fld->Decimal () > decimal) {
					decimal = fld->Decimal ();
				}
			} else if (buffer [0] == 'B' && buffer [1] >= '0' && buffer [1] <= '9') {
				Join_Flag (true);
			} else if (buffer [0] == 'D' && buffer [1] >= '0' && buffer [1] <= '9') {
				Difference_Flag (true);
			}
		}
	}
	return (Set_Field_Numbers ());
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Link_Direction_File::Create_Fields (void) 
{
	int i;
	char buffer [40];

	if (Direction_Index ()) {
		Add_Field ("LINKDIR", INTEGER, 10);
	}
	Add_Field ("LINK", INTEGER, 10);
	Add_Field ("DIR", INTEGER, 1);

	for (i=1; i <= Num_Ranges (); i++) {
		str_fmt (buffer, sizeof (buffer), "P%s", Range_Label (i));
		if (decimal == 0) {
			Add_Field (buffer, INTEGER, 10);
		} else {
			Add_Field (buffer, DOUBLE, 10, decimal);
		}
	}
	if (Join_Flag ()) {
		for (i=1; i <= Num_Ranges (); i++) {
			str_fmt (buffer, sizeof (buffer), "B%s", Range_Label (i));
			if (decimal == 0) {
				Add_Field (buffer, INTEGER, 10);
			} else {
				Add_Field (buffer, DOUBLE, 10, decimal);
			}
		}
	}
	if (Difference_Flag ()) {
		for (i=1; i <= Num_Ranges (); i++) {
			str_fmt (buffer, sizeof (buffer), "D%s", Range_Label (i));
			if (decimal == 0) {
				Add_Field (buffer, INTEGER, 10);
			} else {
				Add_Field (buffer, DOUBLE, 10, decimal);
			}
		}
	}
	return (Set_Field_Numbers ());
}

bool Link_Direction_File::Create_Fields (int low, int high, int increment)
{
	if (low < 0 || low > high) return (false);

	if (!Add_Range (low, high-1, increment)) return (false);

	return (Create_Fields ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Link_Direction_File::Set_Field_Numbers (void)
{
	int i;
	char buffer [40];

	Clear_Fields ();

	link = Required_Field ("LINK", "ID");

	if (link == 0) return (false);

	link_dir = Optional_Field ("LINKDIR", "LINK_DIR", "INDEX");
	dir = Optional_Field ("DIR", "NODE");

	Direction_Index (link_dir > 0);

	//---- allocate field number space ----

	if (!Ignore_Periods ()) {
		i = Num_Ranges ();
		if (i != 0) {
			data = new int [++i];

			if (data == NULL) return (false);

			memset (data, '\0', i * sizeof (int));

			//---- identify the field number for each range ----

			for (i=1; i <= Num_Ranges (); i++) {
				str_fmt (buffer, sizeof (buffer), "P%s", Range_Label (i));
				data [i] = Required_Field (buffer);
			}
		}
	}

	//---- add the join fields ----

	if (Join_Flag ()) {
		i = Num_Ranges ();
		if (i++ == 0) return (false);

		data2 = new int [i];

		if (data2 == NULL) return (false);

		memset (data2, '\0', i * sizeof (int));

		//---- identify the field number for each range ----

		for (i=1; i <= Num_Ranges (); i++) {
			str_fmt (buffer, sizeof (buffer), "B%s", Range_Label (i));
			data2 [i] = Required_Field (buffer);
		}
	} 

	//---- difference fields ----

	if (Difference_Flag ()) {
		i = Num_Ranges ();
		if (i++ == 0) return (false);

		diff = new int [i];

		if (diff == NULL) return (false);

		memset (diff, '\0', i * sizeof (int));

		//---- identify the field number for each range ----

		for (i=1; i <= Num_Ranges (); i++) {
			str_fmt (buffer, sizeof (buffer), "D%s", Range_Label (i));
			diff [i] = Required_Field (buffer);
		}
	}
	return (true);
}

//-----------------------------------------------------------
//	Zero_Fields
//-----------------------------------------------------------

void Link_Direction_File::Zero_Fields (void)
{
	for (int i=1; i <= Num_Fields (); i++) {
		Put_Field (i, 0);
	}
}

//-----------------------------------------------------------
//	Difference
//-----------------------------------------------------------

void Link_Direction_File::Difference (void)
{
	if (Difference_Flag () && Join_Flag ()) {
		double difference;

		for (int i=1; i <= Num_Fields (); i++) {
			difference = Data (i) - Data2 (i);
			Diff (i, difference);
		}
	}
}

//-----------------------------------------------------------
//	Clear_Fields
//-----------------------------------------------------------

void Link_Direction_File::Clear_Fields (void)
{
	link_dir = link = dir = 0;

	if (data != NULL) {
		delete [] data;
		data = NULL;
	}
	if (data2 != NULL) {
		delete [] data2;
		data2 = NULL;
	}
	if (diff != NULL) {
		delete [] diff;
		diff = NULL;
	}
}
