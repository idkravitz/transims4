//*********************************************************
//	Sign_Process.cpp - additional sign processing
//*********************************************************

#include "IntControl.hpp"

#include "Sign_File.hpp"

//---------------------------------------------------------
//	Sign_Processing
//---------------------------------------------------------

bool IntControl::Sign_Processing (Db_File *fh)
{
	int node;
	bool stat;

	Sign_File *file = (Sign_File *) fh;

	if (delete_flag) {
		node = file->Node ();
		if (delete_node.Get_Index (node) > 0) {
			nsign++;
			return (false);
		}
	}
	stat = Network_Service::Sign_Processing (fh);

	if (input_sign_flag && sign_flag) {
		if (stat) {
			node = sign_data.Num_Records () + 1;
		} else {
			node = 0;
		}
		sign_db.Copy_Fields (file);
		sign_db.Put_Field (sign_index, node);
	
		if (!sign_db.Add_Record ()) {
			Error ("Writing Sign Database");
		}
	}
	return (stat);
}
