//********************************************************* 
//	Delay_File.cpp - Link Delay Input/Output
//*********************************************************

#include "Delay_File.hpp"

#include "Utility.hpp"
#include "Date_Time.hpp"
#include "Scanf.hpp"

//-----------------------------------------------------------
//	Delay_File constructors
//-----------------------------------------------------------

Delay_File::Delay_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Delay_File::Delay_File (char *filename, Access_Type access, Format_Type format, int _increment) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);

	if (_increment > 0) {
		increment = _increment;
		Create_Header ();
	}
}

//-----------------------------------------------------------
//	Delay_File destructor
//-----------------------------------------------------------

Delay_File::~Delay_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Delay_File::Setup (void)
{
	File_Type ("Link Delay File");
	File_ID ("Delay");

	if (Dbase_Format () != VERSION3) {
		Nested (true);
	}
	Header_Lines (2);

	link = dir = tod = start = end = volume = enter = exit = speed = speed2 = 0;
	ttime = ttime2 = delay = density = max_den = ratio = vmt = vht = 0;
	lane = turn = vehicles = queue = max_que = fail = 0;
	nconnect = out_link = out_dir = out_turn = out_time = 0;

	increment = 900;
	iteration = 1;
}

//-----------------------------------------------------------
//	Read_Header
//-----------------------------------------------------------

bool Delay_File::Read_Header (bool stat)
{
	if (!Db_Header::Read_Header (stat)) return (false);

	if (Dbase_Format () == VERSION3) {
		tscanf (Header_Record (), "%*s\t%*s\t%*s\t%d\t%*s\t%d", &increment, &iteration);

		//---- process the increment metadata field ----

		if (increment < 60 && increment > 7200) {
			if (Send_Messages ()) {
				exe->Error ("%s Summary Increment %d is Out of Range (60..7200)", File_Type (), increment);
			}
			return (Status (FILE_HEADER));
		}
		if (Send_Messages ()) {
			exe->Print (1, "%s Summary Increment = %d seconds (%.1lf minutes)", File_Type (), increment, (increment / 60.0));
		}

		//---- process the iteration metadata field ----

		if (iteration != 1) {
			if (iteration < 1) {
				if (Send_Messages ()) {
					exe->Error ("%s Iteration %d is Out of Range (> 0)", File_Type (), iteration);
				}
				return (Status (FILE_HEADER));
			}
			if (Send_Messages ()) {
				exe->Print (1, "%s Iteration Count = %d", File_Type (), iteration);
			}
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Create_Header
//---------------------------------------------------------

bool Delay_File::Create_Header (void) 
{
	if (Dbase_Format () == VERSION3) {
		time_t ltime;
		char header [STRING_BUFFER];

		time (&ltime);
		if (iteration > 1) {
			str_fmt (header, sizeof (header), "METADATA\t%s\tTIME_STEP\t%d\tITERATION\t%d\n", c_time (&ltime), increment, iteration);
		} else {
			str_fmt (header, sizeof (header), "METADATA\t%s\tTIME_STEP\t%d\n", c_time (&ltime), increment);
		}
		return (Db_Header::Write_Header (header));
	} else {
		return (Db_Header::Write_Header ());
	}
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Delay_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Nested (false);
		Add_Field ("COUNT", INTEGER, 10);
		Add_Field ("LANE", INTEGER, 2);
		Add_Field ("LINK", INTEGER, 10);
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("SUM", DOUBLE, 14, 2);
		Add_Field ("SUMSQUARES", DOUBLE, 20, 2);
		Add_Field ("TIME", INTEGER, 10);
		Add_Field ("TURN", INTEGER, 2);
		Add_Field ("VCOUNT", INTEGER, 10);
		Add_Field ("VSUM", DOUBLE, 14, 2);
		Add_Field ("VSUMSQUARES", DOUBLE, 20, 2);
	} else {
		Nested (true);
		Add_Field ("LINK", INTEGER, 10);
		Add_Field ("DIR", INTEGER, 1);
		Add_Field ("START_TIME", STRING, TIME_BUFFER);
		Add_Field ("END_TIME", STRING, TIME_BUFFER);
		Add_Field ("AVG_VOLUME", INTEGER, 10);
		Add_Field ("IN_VOLUME", INTEGER, 10);
		Add_Field ("OUT_VOLUME", INTEGER, 10);
		Add_Field ("AVG_SPEED", DOUBLE, 10, 2);
		Add_Field ("AVG_TIME", DOUBLE, 10, 2);
		Add_Field ("AVG_DELAY", DOUBLE, 10, 2);
		Add_Field ("AVG_DENSITY", DOUBLE, 10, 2);
		Add_Field ("MAX_DENSITY", DOUBLE, 10, 2);
		Add_Field ("TIME_RATIO", DOUBLE, 10, 2);
		Add_Field ("AVG_QUEUE", DOUBLE, 10, 2);
		Add_Field ("MAX_QUEUE", INTEGER, 10);
		Add_Field ("NUM_FAIL", INTEGER, 10);
		Add_Field ("VMT", DOUBLE, 12, 1);
		Add_Field ("VHT", DOUBLE, 12, 1);
		Add_Field ("NCONNECT", INTEGER, 2);
		Add_Field ("OUT_LINK", INTEGER, 10, 0, -1, false, true);
		Add_Field ("OUT_DIR", INTEGER, 1, 0, -1, false, true);
		Add_Field ("OUT_TURN", INTEGER, 10, 0, -1, false, true);
		Add_Field ("OUT_TIME", DOUBLE, 10, 2, -1, false, true);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Delay_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	link = Required_Field ("LINK");
	volume = Required_Field ("AVG_VOLUME", "VOLUME", "COUNT");
	ttime = Required_Field ("AVG_TIME", "TTIME", "SUM");

	if (!link || !volume || !ttime) return (false);

	//---- other required fields ----

	tod = Optional_Field ("PERIOD", "TIME");

	if (tod == 0) {
		start = Required_Field ("START", "START_TIME", "STARTTIME");
		end = Required_Field ("END", "END_TIME", "ENDTIME", "TIME");
	}

	//---- optional fields ----

	dir = Optional_Field ("DIR", "NODE");
	enter = Optional_Field ("IN_VOLUME", "ENTER");
	exit = Optional_Field ("OUT_VOLUME", "EXIT");
	speed = Optional_Field ("AVG_SPEED", "SPEED", "VSUM");
	speed2 = Optional_Field ("SPEED2", "VSUMSQUARES");
	ttime2 = Optional_Field ("TTIME2", "SUMSQUARES");
	delay = Optional_Field ("AVG_DELAY", "DELAY");
	density = Optional_Field ("AVG_DENSITY", "DENSITY");
	max_den = Optional_Field ("MAX_DENSITY", "MAX_DEN");
	ratio = Optional_Field ("TIME_RATIO", "RATIO");
	queue = Optional_Field ("AVG_QUEUE", "QUEUE");
	max_que = Optional_Field ("MAX_QUEUE", "MAX_QUE");
	fail = Optional_Field ("NUM_FAIL", "CYCLE_FAIL", "FAILURE");
	vmt = Optional_Field ("VMT", "VEH_DIST");
	vht = Optional_Field ("VHT", "VEH_TIME");
	lane = Optional_Field ("LANE");
	turn = Optional_Field ("TURN");
	vehicles = Optional_Field ("VEHICLES", "VCOUNT");

	//---- nested optional fields ----

	nconnect = Optional_Field ("NCONNECT", "NUM_CONNECT", "NUM_TURNS", "NUM_MOVES");
	out_link = Optional_Field ("OUT_LINK", "OUTLINK");
	out_dir = Optional_Field ("OUT_DIR", "OUTDIR");
	out_turn = Optional_Field ("OUT_TURN", "OUTTURN", "TURN", "MOVEMENT");
	out_time = Optional_Field ("OUT_TIME", "OUTTIME", "OUT_DELAY", "PENALTY");

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

//-----------------------------------------------------------
//	Default_Definition
//-----------------------------------------------------------

bool Delay_File::Default_Definition (void)
{
	Nested (true);
	Create_Fields ();

	return (Write_Def_Header (NULL));
}
