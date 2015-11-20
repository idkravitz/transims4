//*********************************************************
//	ArcDiff.cpp - ArcView Link Difference Files
//*********************************************************

#include "ArcDiff.hpp"

char * ArcDiff::ARCVIEW_DELAY_FILE_1		= "ARCVIEW_DELAY_FILE_1";
char * ArcDiff::ARCVIEW_DELAY_FILE_2		= "ARCVIEW_DELAY_FILE_2";
char * ArcDiff::ARCVIEW_LINK_DIR_FILE_1		= "ARCVIEW_LINK_DIR_FILE_1";
char * ArcDiff::ARCVIEW_LINK_DIR_FILE_2		= "ARCVIEW_LINK_DIR_FILE_2";
char * ArcDiff::ARCVIEW_RIDERSHIP_FILE_1	= "ARCVIEW_RIDERSHIP_FILE_1";
char * ArcDiff::ARCVIEW_RIDERSHIP_FILE_2	= "ARCVIEW_RIDERSHIP_FILE_2";
char * ArcDiff::NEW_ARCVIEW_DIFF_FILE		= "NEW_ARCVIEW_DIFF_FILE";
char * ArcDiff::ARCVIEW_DELAY_FILE_1_x		= "ARCVIEW_DELAY_FILE_1_*";
char * ArcDiff::ARCVIEW_DELAY_FILE_2_x		= "ARCVIEW_DELAY_FILE_2_*";
char * ArcDiff::ARCVIEW_LINK_DIR_FILE_1_x	= "ARCVIEW_LINK_DIR_FILE_1_*";
char * ArcDiff::ARCVIEW_LINK_DIR_FILE_2_x	= "ARCVIEW_LINK_DIR_FILE_2_*";
char * ArcDiff::ARCVIEW_RIDERSHIP_FILE_1_x	= "ARCVIEW_RIDERSHIP_FILE_1_*";
char * ArcDiff::ARCVIEW_RIDERSHIP_FILE_2_x	= "ARCVIEW_RIDERSHIP_FILE_2_*";
char * ArcDiff::NEW_ARCVIEW_DIFF_FILE_x		= "NEW_ARCVIEW_DIFF_FILE_*";
char * ArcDiff::PERCENT_DIFFERENCE_FLAG		= "PERCENT_DIFFERENCE_FLAG";

//---------------------------------------------------------
//	ArcDiff constructor
//---------------------------------------------------------

ArcDiff::ArcDiff (void) : Execution_Service ()
{
	Program ("ArcDiff");
	Version ("4.0.5");
	Title ("ArcView Difference Files");

	char *keys [] = {
		ARCVIEW_DELAY_FILE_1,
		ARCVIEW_DELAY_FILE_2,
		ARCVIEW_LINK_DIR_FILE_1,
		ARCVIEW_LINK_DIR_FILE_2,
		ARCVIEW_RIDERSHIP_FILE_1,
		ARCVIEW_RIDERSHIP_FILE_2,
		NEW_ARCVIEW_DIFF_FILE,
		ARCVIEW_DELAY_FILE_1_x,
		ARCVIEW_DELAY_FILE_2_x,
		ARCVIEW_LINK_DIR_FILE_1_x,
		ARCVIEW_LINK_DIR_FILE_2_x,
		ARCVIEW_RIDERSHIP_FILE_1_x,
		ARCVIEW_RIDERSHIP_FILE_2_x,
		NEW_ARCVIEW_DIFF_FILE_x,
		PERCENT_DIFFERENCE_FLAG,
		NULL
	};
	Key_List (keys);

	percent_flag = false;

	delay_group.Initialize (sizeof (Delay_Group), 5);
}

//---------------------------------------------------------
//	ArcDiff destructor
//---------------------------------------------------------

ArcDiff::~ArcDiff (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ArcDiff *exe = new ArcDiff ();

	return (exe->Start_Execution (commands, control));
}
