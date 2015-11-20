//*********************************************************
//	LinkData.cpp - Convert Link Data to Link ID
//*********************************************************

#include "LinkData.hpp"

char * LinkData::LINK_NODE_EQUIVALENCE			= "LINK_NODE_EQUIVALENCE";
char * LinkData::DIRECTIONAL_DATA_FILE			= "DIRECTIONAL_DATA_FILE";
char * LinkData::FROM_NODE_FIELD_NAME			= "FROM_NODE_FIELD_NAME";
char * LinkData::TO_NODE_FIELD_NAME				= "TO_NODE_FIELD_NAME";
char * LinkData::VOLUME_DATA_FIELD_NAME			= "VOLUME_DATA_FIELD_NAME";
char * LinkData::SPEED_DATA_FIELD_NAME			= "SPEED_DATA_FIELD_NAME";
char * LinkData::NEW_LINK_DATA_FILE				= "NEW_LINK_DATA_FILE";
char * LinkData::NEW_AB_VOLUME_FIELD_NAME		= "NEW_AB_VOLUME_FIELD_NAME";
char * LinkData::NEW_BA_VOLUME_FIELD_NAME		= "NEW_BA_VOLUME_FIELD_NAME";
char * LinkData::NEW_AB_SPEED_FIELD_NAME		= "NEW_AB_SPEED_FIELD_NAME";
char * LinkData::NEW_BA_SPEED_FIELD_NAME		= "NEW_BA_SPEED_FIELD_NAME";
char * LinkData::NEW_DIRECTIONAL_DATA_FILE		= "NEW_DIRECTIONAL_DATA_FILE";

//---------------------------------------------------------
//	LinkData constructor
//---------------------------------------------------------

LinkData::LinkData (void) : Execution_Service ()
{
	Program ("LinkData");
	Version ("4.0.2");
	Title ("Convert Link Data to Link ID");

	char *keys [] = {
		LINK_NODE_EQUIVALENCE,
		DIRECTIONAL_DATA_FILE,
		FROM_NODE_FIELD_NAME,
		TO_NODE_FIELD_NAME,
		VOLUME_DATA_FIELD_NAME,
		SPEED_DATA_FIELD_NAME,
		NEW_LINK_DATA_FILE,
		NEW_AB_VOLUME_FIELD_NAME,
		NEW_BA_VOLUME_FIELD_NAME,
		NEW_AB_SPEED_FIELD_NAME,
		NEW_BA_SPEED_FIELD_NAME,
		NEW_DIRECTIONAL_DATA_FILE,
		NULL
	};

	Key_List (keys);

	speed_flag = output_flag = false;
	nequiv = nab = ndir = nlink = 0;

	link_equiv.Initialize (sizeof (Link_Equiv), 2);
	link_data.Initialize (sizeof (Link_Data));
}

//---------------------------------------------------------
//	LinkData destructor
//---------------------------------------------------------

LinkData::~LinkData (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	LinkData *exe = new LinkData ();

	return (exe->Start_Execution (commands, control));
}
