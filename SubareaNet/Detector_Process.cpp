//*********************************************************
//	Detector_Process.cpp - read the detector file
//*********************************************************

#include "SubareaNet.hpp"

#include "Detector_File.hpp"

//---------------------------------------------------------
//	Detector_Processing
//---------------------------------------------------------

bool SubareaNet::Detector_Processing (Db_File *fh)
{
	static Detector_File *new_file;

	int id, node;
	Node_Data *node_ptr;

	Detector_File *file = (Detector_File *) fh;

	if (First_Record ()) {
		new_file = (Detector_File *) Network_Db_Base (NEW_DETECTOR);
	}

	//---- read and save the data ----

	id = file->ID ();
	if (id == 0 || new_file == NULL) return (false);

	node = file->Node ();

	node_ptr = node_data.Get (node);

	//---- check the node number ----

	if (node_ptr->Type () == 1) {

		//---- copy the fields to the subarea file ----

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error ("Writing %s", new_file->File_Type ());
		}
		ndetector++;
	}
	
	//---- don't save the record ----

	return (false);
}
