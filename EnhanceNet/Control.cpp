//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "EnhanceNet.hpp"

#include "Link_File.hpp"
#include "Connect_File.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void EnhanceNet::Program_Control (void)
{

	//---- create the network files ----

	Network_Service::Program_Control ();

	//---- check the output link file ----

	if (Network_File_Flag (NEW_LINK)) {
		Link_File *file, *new_file;

		file = (Link_File *) Network_Db_File (LINK);
		new_file = (Link_File *) Network_Db_File (NEW_LINK);

		new_file->Clear_Fields ();
		new_file->Replicate_Fields (file);

		if (!file->Bearing_Flag ()) {
			new_file->Add_Field ("BEARING_A", INTEGER, 4);
			new_file->Add_Field ("BEARING_B", INTEGER, 4);
		}
		new_file->Write_Header ();
	}

	//---- check the output lane connectivity file ----

	if (Network_File_Flag (NEW_LANE_CONNECTIVITY)) {
		if (!Network_File_Flag (LANE_CONNECTIVITY)) {
			Error ("A Lane Connectivity file is required for Connection Type Processing");
		}
		Connect_File *file, *new_file;

		file = (Connect_File *) Network_Db_File (LANE_CONNECTIVITY);
		new_file = (Connect_File *) Network_Db_File (NEW_LANE_CONNECTIVITY);

		new_file->Clear_Fields ();
		new_file->Replicate_Fields (file);

		if (!file->Type_Flag ()) {
			new_file->Add_Field ("TYPE", STRING, 8);
			type_flag = true;
		}
		new_file->Write_Header ();

		connect_db.Replicate_Fields (file);
		connect_db.File_ID ("Connectivity");
		connect_flag = true;
	}
	return;
}

