//*********************************************************
//	Read_Reference.cpp - read the reference node file
//*********************************************************

#include "CoordMatch.hpp"

//---------------------------------------------------------
//	Read_Reference
//---------------------------------------------------------

void CoordMatch::Read_Reference (void)
{
	Node_Data node_rec;

	//---- read the node map file ----

	Show_Message ("Reading %s -- Record", ref_node_file.File_Type ());
	Set_Progress (100);

	if (!ref_node_data.Max_Records (ref_node_file.Num_Records ())) {
		Error ("Insufficient Memory for Reference Nodes");
	}

	//---- process each record ----

	while (ref_node_file.Read ()) {
		Show_Progress ();

		//---- read and save the data ----

		node_rec.Node (ref_node_file.Node ());
		if (node_rec.Node () == 0) continue;

		node_rec.X (Round (ref_node_file.X ()));
		node_rec.Y (Round (ref_node_file.Y ()));
		node_rec.Z (0);								//---- map node ----

		if (!ref_node_data.Add (&node_rec)) {
			Error ("Adding Reference Node");
		}
	}
	End_Progress ();

	ref_node_file.Close ();

	Print (2, "Number of %s Records = %d", ref_node_file.File_Type (), ref_node_data.Num_Records ());
}
