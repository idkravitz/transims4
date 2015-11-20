//*********************************************************
//	Connect_Process.cpp - additional lane connectivity processing
//*********************************************************

#include "EnhanceNet.hpp"

#include "Connect_File.hpp"

//---------------------------------------------------------
//	Connectivity_Processing
//---------------------------------------------------------

bool EnhanceNet::Connectivity_Processing (Db_File *fh)
{
	Connect_File *file;
	bool stat;

	stat = Network_Service::Connectivity_Processing (fh);
		
	file = (Connect_File *) fh;

	if (!stat) {
		if (file->Node () == 0) return (false);

	} else if (type_flag || Connect_Code (file->Type ()) == NO_TYPE) {

		Connect_Data *connect_ptr = connect_data.New_Record ();

		connect_ptr->Type (NO_TYPE);
	}

	//---- save the record to the database ----

	if (First_Record ()) {
		if (!connect_db.Max_Records (file->Num_Records ())) {
			Error ("Insufficient Memory for Lane Connectivity Database");
		}
	}
	connect_db.Copy_Fields (file);

	if (!connect_db.Add_Record ()) {
		Error ("Writing Lane Connectivity Database");
	}
	return (stat);
}
