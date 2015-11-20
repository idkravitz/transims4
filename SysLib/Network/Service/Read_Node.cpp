//*********************************************************
//	Read_Node.cpp - read the node file
//*********************************************************

#include "Network_Service.hpp"

#include "Node_File.hpp"

//---------------------------------------------------------
//	Read_Node
//---------------------------------------------------------

void Network_Service::Read_Node (void)
{
	Db_File *file = Network_Db_File (NODE);

	//---- store the node data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress ();
	
	file->Num_Records ();
	First_Record (true);

	while (file->Read ()) {
		Show_Progress ();

		if (Node_Processing (file)) {
			if (!node_data.Add ()) {
				Error ("Adding Record to the Node Data List");
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = node_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Node Data Records = %d", num);
	}
	node_data.Optimize ();

	if (num > 0) Network_Data_True (NODE);
}

//---------------------------------------------------------
//	Node_Processing
//---------------------------------------------------------

bool Network_Service::Node_Processing (Db_File *fh)
{
	double x, y;

	Node_Data *node_ptr;

	Node_File *file = (Node_File *) fh;

	//---- reserve memory ----

	if (node_data.Max_Records () == 0) {
		if (!node_data.Max_Records (file->Num_Records ())) goto mem_error;
	}

	//---- read and save the data ----

	node_ptr = node_data.New_Record ();
	if (node_ptr == NULL) goto mem_error;

	node_ptr->Node (file->Node ());
	if (node_ptr->Node () == 0) return (false);

	x = file->X ();
	y = file->Y ();

	if (Projection_Flag ()) {
		if (!(Get_Projection ()->Convert (&x, &y))) {
			return (Check_Message ("Converting Coordinate %lf, %lf", x, y));
		}
	}
	node_ptr->X (Round (x));
	node_ptr->Y (Round (y));
	node_ptr->Z (Round (file->Z ()));

	return (true);

mem_error:
	Error ("Insufficient Memory for Node Data");
	return (false);
}
