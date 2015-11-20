//*********************************************************
//	TPPlusNet.cpp - TP+ Network Conversion
//*********************************************************

#include "TPPlusNet.hpp"

char * TPPlusNet::TPPLUS_LINK_FILE		= "TPPLUS_LINK_FILE";
char * TPPlusNet::TPPLUS_LINK_FORMAT	= "TPPLUS_LINK_FORMAT";
char * TPPlusNet::TPPLUS_NODE_FILE		= "TPPLUS_NODE_FILE";
char * TPPlusNet::TPPLUS_NODE_FORMAT	= "TPPLUS_NODE_FORMAT";
char * TPPlusNet::TPPLUS_SPDCAP_FILE	= "TPPLUS_SPDCAP_FILE";
char * TPPlusNet::TPPLUS_SPDCAP_FORMAT	= "TPPLUS_SPDCAP_FORMAT";
char * TPPlusNet::CONVERT_MPH_TO_MPS	= "CONVERT_MPH_TO_MPS";
char * TPPlusNet::FACILITY_INDEX_FIELD	= "FACILITY_INDEX_FIELD";
char * TPPlusNet::AREA_TYPE_INDEX_FIELD = "AREA_TYPE_INDEX_FIELD";
char * TPPlusNet::CONVERSION_SCRIPT		= "CONVERSION_SCRIPT";
char * TPPlusNet::EXTERNAL_ZONE_NUMBER	= "EXTERNAL_ZONE_NUMBER";
char * TPPlusNet::NEW_LINK_DETAIL_FILE	= "NEW_LINK_DETAIL_FILE";

//---------------------------------------------------------
//	TPPlusNet constructor
//---------------------------------------------------------

TPPlusNet::TPPlusNet (void) : Network_Service ()
{
	Program ("TPPlusNet");
	Version ("4.0.4");
	Title ("TPPlus Network Conversion");

	Network_File required_network [] = {
		NEW_NODE, NEW_LINK, END_NETWORK
	};

	Network_File optional_network [] = {
		NEW_DIRECTORY, NEW_SHAPE, END_NETWORK
	};

	char *keys [] = {
		TPPLUS_LINK_FILE,
		TPPLUS_LINK_FORMAT,
		TPPLUS_NODE_FILE,
		TPPLUS_NODE_FORMAT,
		TPPLUS_SPDCAP_FILE,
		TPPLUS_SPDCAP_FORMAT,
		CONVERT_MPH_TO_MPS,
		FACILITY_INDEX_FIELD,
		AREA_TYPE_INDEX_FIELD,
		CONVERSION_SCRIPT,
		EXTERNAL_ZONE_NUMBER,
		NEW_LINK_DETAIL_FILE,
		NULL
	};

	char *reports [] = {
		"CONVERSION_SCRIPT",
		"CONVERSION_STACK",
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);

	Key_List (keys);
	Report_List (reports);

	AB_Key_Flag (true);

	proj_service.Add_Keys ();

	ext_zone = fac_fld = at_fld = 0;
	convert_flag = spdcap_flag = link_shape_flag = node_shape_flag = units_flag = detail_flag = false;

	link_array = (Link_Array *) new Link_Name_Array ();
}

//---------------------------------------------------------
//	TPPlusNet destructor
//---------------------------------------------------------

TPPlusNet::~TPPlusNet (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	TPPlusNet *exe = new TPPlusNet ();

	return (exe->Start_Execution (commands, control));
}

