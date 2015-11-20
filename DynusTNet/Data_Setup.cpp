//*********************************************************
//	Data_Setup.cpp - set the node and link data
//*********************************************************

#include "DynusTNet.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Data_Setup
//---------------------------------------------------------

void DynusTNet::Data_Setup (void)
{
	int dir, zone;
	double dx, dy, diff, min_diff;

	Node_List *anode_ptr, *bnode_ptr;
	Link_Name *link_ptr;
	Dir_List *dir_ptr;
	Zone_Data *zone_ptr;

	//---- initialize the zone map ----

	nzone = 0;

	for (zone_ptr = zone_data.First (); zone_ptr; zone_ptr = zone_data.Next ()) {
		zone_ptr->Area_Type (0);
	}

	//---- initialize the node data ----

	for (anode_ptr = node_data.First (); anode_ptr; anode_ptr = node_data.Next ()) {
		anode_ptr->Type (0);
		anode_ptr->From_Dir (0);
		anode_ptr->To_Dir (0);

		zone = 0;
		min_diff = -1.0;

		for (zone_ptr = zone_data.First (); zone_ptr; zone_ptr = zone_data.Next ()) {
			dx = zone_ptr->X () - anode_ptr->X ();
			dy = zone_ptr->Y () - anode_ptr->Y ();

			diff = dx * dx + dy * dy;
			if (diff < min_diff || min_diff < 0) {
				min_diff = diff;
				zone = zone_data.Record_Index ();
			}
		}
		if (zone > 0) {
			zone_ptr = zone_data [zone];

			if (zone_ptr->Area_Type () == 0) {
				nzone++;
				zone_ptr->Area_Type (nzone);
				anode_ptr->Type (nzone);
			} else {
				anode_ptr->Type (zone_ptr->Area_Type ());
			}
		}
	}

	//---- write the zone number map ----

	fprintf (zmap_file.File (), "TRANSIMS\tDYNUST\n");

	for (zone_ptr = zone_data.First_Key (); zone_ptr; zone_ptr = zone_data.Next_Key ()) {
		if (zone_ptr->Area_Type () > 0) {
			fprintf (zmap_file.File (), "%d\t%d\n", zone_ptr->Zone (), zone_ptr->Area_Type ());
		}
	}
	Print (2, "Number of %s Records = %d", zmap_file.File_Type(), nzone);

	//---- build the link list ----

	for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {

		//---- add to the link lists ----

		anode_ptr = node_data.Get (link_ptr->Anode ());
		bnode_ptr = node_data.Get (link_ptr->Bnode ());

		dir = link_ptr->AB_Dir ();

		if (dir > 0) {
			dir_ptr = dir_data [dir];

			dir_ptr->From_List (anode_ptr->From_Dir ());
			anode_ptr->From_Dir (dir);

			dir_ptr->To_List (bnode_ptr->To_Dir ());
			bnode_ptr->To_Dir (dir);
		}
		dir = link_ptr->BA_Dir ();

		if (dir > 0) {
			dir_ptr = dir_data [dir];

			dir_ptr->To_List (anode_ptr->To_Dir ());
			anode_ptr->To_Dir (dir);

			dir_ptr->From_List (bnode_ptr->From_Dir ());
			bnode_ptr->From_Dir (dir);
		}
	}
}
