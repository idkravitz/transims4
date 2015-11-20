//*********************************************************
//	Signal_Process.cpp - read the signal file
//*********************************************************

#include "SubareaNet.hpp"

#include "Signal_File.hpp"

//---------------------------------------------------------
//	Signal_Processing
//---------------------------------------------------------

bool SubareaNet::Signal_Processing (Db_File *fh)
{
	static Signal_File *new_file;

	int node;
	Node_Data *node_ptr;
	Signal_Data *signal_ptr;

	Signal_File *file = (Signal_File *) fh;	

	if (First_Record ()) {
		new_file = (Signal_File *) Network_Db_Base (NEW_SIGNALIZED_NODE);

		if (!signal_data.Max_Records (file->Num_Records ())) goto mem_error;
	}

	//---- read and save the data ----

	node = file->Node ();
	if (node == 0 || new_file == NULL) return (false);

	node_ptr = node_data.Get (node);

	//---- check the node number ----

	if (node_ptr->Type () == 1) {

		//---- copy the fields to the subarea file ----

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error ("Writing %s", new_file->File_Type ());
		}
		nsignal++;

		//---- save the signal data ----

		signal_ptr = signal_data.New_Record ();
		if (signal_ptr == NULL) goto mem_error;

		signal_ptr->Node (node);
		signal_ptr->Timing (file->Timing ());
		signal_ptr->Coordinator (file->Coordinator ());
		return (true);
	}
	return (false);

mem_error:
	Error ("Insufficient Memory for Signal Data");
	return (false);
}
