//*********************************************************
//	Route_Header_Process.cpp - read the route header file
//*********************************************************

#include "ArcNet.hpp"

#include "Route_Header_File.hpp"

//---------------------------------------------------------
//	Route_Header_Processing
//---------------------------------------------------------

bool ArcNet::Route_Header_Processing (Db_File *fh)
{
	Route_Header_File *file;

	if (Network_Service::Route_Header_Processing (fh)) {

		file = (Route_Header_File *) fh;

		if (First_Record ()) {
			if (!route_header_db.Max_Records (route_data.Max_Records ())) {
				Error ("Insufficient Memory for Route Header Database");
			}
		}
		route_header_db.Copy_Fields (file);

		if (!route_header_db.Add_Record ()) {
			Error ("Writing Route Header Database");
		}
		return (true);
	}
	return (false);
}
