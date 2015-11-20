//*********************************************************
//	Write_Node.cpp - write the subarea node file
//*********************************************************

#include "SubareaNet.hpp"

#include "Node_File.hpp"

//---------------------------------------------------------
//	Write_Node
//---------------------------------------------------------

void SubareaNet::Write_Node (void)
{
	int node, node_field;
	char string [STRING_BUFFER];

	Node_Data *node_ptr;

	Node_File *node_file = (Node_File *) Network_Db_Base (NEW_NODE);

	char *ext_dir [] = {
		"Subarea", "External"
	};

	//---- process each subarea nodes ----
	
	Show_Message ("Writing Subarea Node Data -- Record");
	Set_Progress (1000);

	node_field = node_db.Required_Field ("NODE", "ID");

	node_db.Rewind ();

	while (node_db.Read_Record ()) {
		Show_Progress ();

		node_db.Get_Field (node_field, &node);

		node_ptr = node_data.Get (node);

		if (node_ptr == NULL || node_ptr->Type () == 0) continue;

		node_file->Copy_Fields (&node_db);

		str_fmt (string, sizeof (string), "%s Node", ext_dir [node_ptr->Type () - 1]);
		node_file->Put_Field ("NOTES", string);

		if (!node_file->Write ()) {
			Error ("Writing %s", node_file->File_Type ());
		}
		nnode++;
	}
	End_Progress ();
}
