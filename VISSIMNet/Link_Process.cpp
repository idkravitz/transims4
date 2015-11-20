//*********************************************************
//	Link_Process.cpp - Read the Link File
//*********************************************************

#include "VISSIMNet.hpp"

#include "Link_File.hpp"

//---------------------------------------------------------
//	Link_Processing
//---------------------------------------------------------

bool VISSIMNet::Link_Processing (Db_File *fh)
{
	Link_Name *link_ptr = NULL;
	Link_File *file;

	if (Network_Service::Link_Processing (fh)) {
		file = (Link_File *) fh;	

		link_ptr = link_data.New_Record ();

		//---- store street names ----

		link_ptr->Name (file->Street ());

		return (true);
	}
	return (false);
}
