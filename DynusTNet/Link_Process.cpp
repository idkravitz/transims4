//*********************************************************
//	Link_Process.cpp - Read the Link File
//*********************************************************

#include "DynusTNet.hpp"

#include "Link_File.hpp"

//---------------------------------------------------------
//	Link_Processing
//---------------------------------------------------------

bool DynusTNet::Link_Processing (Db_File *fh)
{
	Link_Name *link_ptr = NULL;
	Link_File *file;

	if (Network_Service::Link_Processing (fh)) {
		file = (Link_File *) fh;	

		link_ptr = link_data.New_Record ();

		//---- store street names ----

		link_ptr->Name (file->Street ());

		if (strlen (link_ptr->Name ()) > 0) name_flag = true;

		return (true);
	}
	return (false);
}
