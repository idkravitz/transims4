//*********************************************************
//	Write_Zone.cpp - convert the zone data
//*********************************************************

#include "VISSIMNet.hpp"

//---------------------------------------------------------
//	Write_Zone
//---------------------------------------------------------

void VISSIMNet::Write_Zone (void)
{
	int node;

	Link_Name *link_ptr;
	Location_Data *location_ptr;
	Access_Data *access_ptr;

	fprintf (file, "\n<ZONES>");

	for (location_ptr = location_data.First_Key (); location_ptr; location_ptr = location_data.Next_Key ()) {

		fprintf (file, "\n<ZONE NO=\"%d\" XCOORD=\"%.2lf\" YCOORD=\"%.2lf\" >",
			location_ptr->Location (), UnRound (location_ptr->X ()), UnRound (location_ptr->Y ()));

		//---- find the attrach node ----

		link_ptr = link_data.Get (location_ptr->Link ());

		if (location_ptr->Offset () > link_ptr->Length () / 2) {
			node = link_ptr->Bnode ();
		} else {
			node = link_ptr->Anode ();
		}

		//---- process links ----

		fprintf (file, "\n<CONNECTORS>");

		for (access_ptr = access_data.First (); access_ptr; access_ptr = access_data.Next ()) {
			if (access_ptr->From_Type () == LOCATION_ID && access_ptr->From_ID () == location_ptr->Location ()) {
				fprintf (file, "\n<CONNECTOR NODENO=\"%d\" DIRECTION=\"false\" />", node);
			}
			if (access_ptr->To_Type () == LOCATION_ID && access_ptr->To_ID () == location_ptr->Location ()) {
				fprintf (file, "\n<CONNECTOR NODENO=\"%d\" DIRECTION=\"true\" />", node);
			}
		}
		fprintf (file, "\n</CONNECTORS>");
		fprintf (file, "\n</ZONE>");
	}
	fprintf (file, "\n</ZONES>");
}
