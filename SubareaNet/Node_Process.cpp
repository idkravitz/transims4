//*********************************************************
//	Node_Process.cpp - read the node file
//*********************************************************

#include "SubareaNet.hpp"

#include "Node_File.hpp"
#include "In_Polygon.hpp"

//---------------------------------------------------------
//	Node_Processing
//---------------------------------------------------------

bool SubareaNet::Node_Processing (Db_File *fh)
{
	Node_File *file = (Node_File *) fh;

	//---- reserve memory ----

	if (First_Record ()) {
		if (!node_db.Max_Records (file->Num_Records ())) {
			Error ("Insufficient Memory for Node Database");
		}
	}

	//---- do standard processing ----

	if (Network_Service::Node_Processing (fh)) {

		//---- copy the record to the node database ----

		node_db.Copy_Fields (file);

		if (!node_db.Add_Record ()) {
			Error ("Writing Node Database");
		}

		//---- check the subarea boundary ----

		Node_Data *node_ptr = node_data.New_Record ();

		if (In_Polygon (UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()), &subarea_boundary.points)) {
			node_ptr->Type (1);
		} else {
			node_ptr->Type (0);
		}
		return (true);
	}
	return (false);
}
