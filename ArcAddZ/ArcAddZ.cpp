//*********************************************************
//	ArcAddZ.cpp - Add Z Coordinates to ArcView Shape Files
//*********************************************************

#include "ArcAddZ.hpp"

char * ArcAddZ::ARCVIEW_SHAPE_FILE			= "ARCVIEW_SHAPE_FILE";
char * ArcAddZ::NEW_ARCVIEW_SHAPE_FILE		= "NEW_ARCVIEW_SHAPE_FILE";
char * ArcAddZ::ARCVIEW_SHAPE_FILE_x		= "ARCVIEW_SHAPE_FILE_*";
char * ArcAddZ::NEW_ARCVIEW_SHAPE_FILE_x	= "NEW_ARCVIEW_SHAPE_FILE_*";
char * ArcAddZ::ADD_Z_AS_MEASURE			= "ADD_Z_AS_MEASURE";
char * ArcAddZ::DEFAULT_Z_VALUE				= "DEFAULT_Z_VALUE";

//---------------------------------------------------------
//	ArcAddZ constructor
//---------------------------------------------------------

ArcAddZ::ArcAddZ (void) : Execution_Service ()
{
	Program ("ArcAddZ");
	Version ("4.0.1");
	Title ("Add Z Coordinates to ArcView Shape Files");

	char *keys [] = {
		ARCVIEW_SHAPE_FILE,
		NEW_ARCVIEW_SHAPE_FILE,
		ARCVIEW_SHAPE_FILE_x,
		NEW_ARCVIEW_SHAPE_FILE_x,
		ADD_Z_AS_MEASURE,
		DEFAULT_Z_VALUE,
		NULL
	};
	Key_List (keys);

	measure_flag = true;
	z_coord = 0.0;

	file_group.Initialize (sizeof (File_Group), 5);
}

//---------------------------------------------------------
//	ArcAddZ destructor
//---------------------------------------------------------

ArcAddZ::~ArcAddZ (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ArcAddZ *exe = new ArcAddZ ();

	return (exe->Start_Execution (commands, control));
}
