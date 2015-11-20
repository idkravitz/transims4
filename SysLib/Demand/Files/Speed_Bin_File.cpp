//********************************************************* 
//	Speed_Bin_File.cpp - Speed Bin File Input/Output
//*********************************************************

#include "Speed_Bin_File.hpp"

#include "Network_Service.hpp"
#include "Utility.hpp"
#include "Date_Time.hpp"

char * Speed_Bin_File::BOX_LENGTH		= "BOX_LENGTH";
char * Speed_Bin_File::CELL_LENGTH		= "CELL_LENGTH";
char * Speed_Bin_File::SAMPLE_TIME		= "SAMPLE_TIME";
char * Speed_Bin_File::INCREMENT		= "INCREMENT";
char * Speed_Bin_File::TIME_STEP		= "TIME_STEP";
char * Speed_Bin_File::VEHICLE_SUBTYPE	= "VEHICLE_SUBTYPE";
char * Speed_Bin_File::VEHICLE_TYPE		= "VEHICLE_TYPE";
char * Speed_Bin_File::NUM_BINS			= "NUM_BINS";
char * Speed_Bin_File::VELOCITY_BINS	= "VELOCITY_BINS";
char * Speed_Bin_File::VELOCITY_MAX		= "VELOCITY_MAX";

//-----------------------------------------------------------
//	Speed_Bin_File constructors
//-----------------------------------------------------------

Speed_Bin_File::Speed_Bin_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Speed_Bin_File::Speed_Bin_File (char *filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Speed_Bin_File destructor
//-----------------------------------------------------------

Speed_Bin_File::~Speed_Bin_File (void)
{
	if (bin != NULL) {
		delete [] bin;
	}
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Speed_Bin_File::Setup (void)
{
	File_Type ("Speed Bin File");
	File_ID ("Speed_Bin");

	Header_Lines (2);

	link = dir = offset = end = 0;
	bin = NULL;

	num_bins = 0;
	sample_time = 1;
	increment = 900;
	veh_type = 1;
	subtype = 0;
	box_length = 30.0;
	cell_length = 7.5;
}
//-----------------------------------------------------------
//	Read_Header
//-----------------------------------------------------------

bool Speed_Bin_File::Read_Header (bool stat)
{
	if (!Db_Header::Read_Header (stat)) return (false);

	char *str_ptr, token [FIELD_BUFFER], value [FIELD_BUFFER];

	str_ptr = Header_Record ();

	while (str_ptr) {
		str_ptr = Get_Token (str_ptr, token, sizeof (token));
		if (token [0] == '\0') break;

		str_ptr = Get_Token (str_ptr, value, sizeof (value));
		if (value [0] == '\0') {
			return (Status (FILE_HEADER));
		}

		if (str_cmp (token, BOX_LENGTH) == 0) {
			box_length = atof (value);

			if (box_length != 0.0 && box_length < 15.0) {
				if (Send_Messages ()) {
					exe->Error ("%s Box Length %s is Out of Range (>= 15.0)", File_Type (), value);
				}
				return (Status (FILE_HEADER));
			}
		} else if (str_cmp (token, CELL_LENGTH) == 0) {
			cell_length = atof (value);

			if (cell_length < 1.0) {
				if (Send_Messages ()) {
					exe->Error ("%s Cell Length %s is Out of Range (>= 1.0)", File_Type (), value);
				}
				return (Status (FILE_HEADER));
			}
		} else if (str_cmp (token, SAMPLE_TIME) == 0) {
			sample_time = atoi (value);

			if (sample_time < 1 || sample_time > 900) {
				if (Send_Messages ()) {
					exe->Error ("%s Sample Time %s is Out of Range (1..900)", File_Type (), value);
				}
				return (Status (FILE_HEADER));
			}
		} else if (str_cmp (token, TIME_STEP) == 0 || str_cmp (token, INCREMENT) == 0) {
			increment = atoi (value);

			if (increment < 60 && increment > 7200) {
				if (Send_Messages ()) {
					exe->Error ("%s Time Increment %s is Out of Range (60..7200)", File_Type (), value);
				}
				return (Status (FILE_HEADER));
			}
		} else if (str_cmp (token, VEHICLE_TYPE) == 0) {
			if (value [0] >= '0' && value [0] <= '9') {
				veh_type = atoi (value);
			} else {
				veh_type = -1;
			}
			if (veh_type < 1) {
				if (Send_Messages ()) {
					exe->Error ("%s Vehicle Type %s is Unrecognized", File_Type (), value);
				}
				return (Status (FILE_HEADER));
			}
		} else if (str_cmp (token, VEHICLE_SUBTYPE) == 0) {
			subtype = atoi (value);

			if (subtype < 0) {
				if (Send_Messages ()) {
					exe->Error ("%s Vehicle SubType %s is Out of Range (> 0)", File_Type (), value);
				}
				return (Status (FILE_HEADER));
			}
		} else if (str_cmp (token, VELOCITY_BINS) == 0 || str_cmp (token, NUM_BINS) == 0) {
			num_bins = atoi (value);

			if (num_bins < 1 || num_bins > 100) {
				if (Send_Messages ()) {
					exe->Error ("%s Number of Bins %s is Out of Range (1..100)", File_Type (), value);
				}
				return (Status (FILE_HEADER));
			}
		} else if (str_cmp (token, VELOCITY_MAX) == 0) {
			double speed = atof (value);

			if (speed / cell_length < num_bins - 1) {
				if (Send_Messages ()) {
					exe->Error ("%s Velocity Max %s is Illogical", File_Type (), value);
				}
				return (Status (FILE_HEADER));
			}
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Create_Header
//---------------------------------------------------------

bool Speed_Bin_File::Create_Header (void) 
{
	time_t ltime;
	char header [STRING_BUFFER];

	time (&ltime);

	if (Dbase_Format () == VERSION3) {
		str_fmt (header, sizeof (header), 
			"METADATA\t%s\t%s\t%g\t%s\t%g\t%s\t%d\t%s\t%d\t%s\t%d\t%s\t%d\t%s\t%d\t%s\t%g\n", 
			c_time (&ltime), BOX_LENGTH, box_length, CELL_LENGTH, cell_length, 
			SAMPLE_TIME, sample_time, TIME_STEP, increment, VEHICLE_SUBTYPE, subtype,
			VEHICLE_TYPE, veh_type, VELOCITY_BINS, num_bins, VELOCITY_MAX, ((num_bins - 1) * cell_length));
	} else {
		str_fmt (header, sizeof (header), 
			"METADATA\t%s\t%s\t%g\t%s\t%g\t%s\t%d\t%s\t%d\t%s\t%d\t%s\t%d\t%s\t%d\n", 
			c_time (&ltime), BOX_LENGTH, box_length, CELL_LENGTH, cell_length, 
			SAMPLE_TIME, sample_time, INCREMENT, increment, NUM_BINS, num_bins,
			VEHICLE_TYPE, veh_type, VEHICLE_SUBTYPE, subtype);
	}
	return (Db_Header::Write_Header (header));
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Speed_Bin_File::Create_Fields (void) 
{
	int i;
	char buffer [16];

	if (num_bins == 0) num_bins = 6;

	if (Dbase_Format () == VERSION3) {
		for (i=0; i < num_bins; i++) {
			str_fmt (buffer, sizeof (buffer), "COUNT%d", i);
			Add_Field (buffer, INTEGER, 6);
		}
		Add_Field ("DISTANCE", DOUBLE, 8, 1);
		Add_Field ("LINK", INTEGER, 10);
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("TIME", STRING, TIME_BUFFER);
	} else {
		Add_Field ("LINK", INTEGER, 10);
		Add_Field ("DIR", INTEGER, 2);
		Add_Field ("OFFSET", DOUBLE, 8, 1);
		Add_Field ("TIME", STRING, TIME_BUFFER);

		for (i=0; i < num_bins; i++) {
			str_fmt (buffer, sizeof (buffer), "SPEED%d", i);
			Add_Field (buffer, INTEGER, 6);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Speed_Bin_File::Set_Field_Numbers (void)
{
	int i;
	char name1 [16], name2 [16], name3 [16];

	//---- required fields ----

	link = Required_Field ("LINK");
	end = Required_Field ("TIME");
	offset = Required_Field ("OFFSET", "DISTANCE");

	if (!link || !end || !offset) return (false);

	//---- speed bin fields ----

	num_bins = 0;

	for (i=0; i <= 50; i++) {
		str_fmt (name1, sizeof (name1), "COUNT%d", i);
		str_fmt (name2, sizeof (name2), "SPEED%d", i);
		str_fmt (name3, sizeof (name3), "BIN%d", i);

		if (Optional_Field (name1, name2, name3) == 0) break;
	}

	if (i == 0) return (false);

	bin = new int [i];
	if (bin == NULL) return (false);

	num_bins = i;
	
	for (i=0; i < num_bins; i++) {
		str_fmt (name1, sizeof (name1), "COUNT%d", i);
		str_fmt (name2, sizeof (name2), "SPEED%d", i);
		str_fmt (name3, sizeof (name3), "BIN%d", i);

		bin [i] = Optional_Field (name1, name2, name3);
	}

	//---- optional fields ----

	dir = Optional_Field ("DIR", "NODE");

	if (dir > 0) {
		if (Optional_Field ("DIR") == 0) {
			LinkDir_Type (LINK_NODE);
		} else {
			LinkDir_Type (LINK_DIR);
		}
	} else {
		LinkDir_Type (LINK_SIGN);
	}
	return (true);
}
