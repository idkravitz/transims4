//*********************************************************
//	Link_AreaType.cpp - Set the Link Area Type
//*********************************************************

#include "TransitNet.hpp"

//---------------------------------------------------------
//	Link_AreaType
//---------------------------------------------------------

void TransitNet::Link_AreaType (void)
{
	int type, zone;
	double dx, dy, closest, distance;

	Dir_Path_Data *dir_ptr;
	Link_Data *link_ptr;
	Node_Data *node_ptr;
	Zone_Data *zone_ptr;

	for (dir_ptr = dir_data.First (); dir_ptr; dir_ptr = dir_data.Next ()) {

		zone = dir_ptr->Zone ();

		if (zone == 0) {
			link_ptr = link_data.Get (dir_ptr->Link ());

			if (dir_ptr->Dir () > 0) {
				node_ptr = node_data.Get (link_ptr->Anode ());
			} else {
				node_ptr = node_data.Get (link_ptr->Bnode ());
			}

			//---- find the closest zone ----

			for (zone_ptr = zone_data.First (); zone_ptr; zone_ptr = zone_data.Next ()) {
				dx = zone_ptr->X () - node_ptr->X ();
				dy = zone_ptr->Y () - node_ptr->Y ();

				distance = dx * dx + dy * dy;

				if (zone == 0 || distance < closest) {
					zone = zone_ptr->Zone ();
					closest = distance;
				}
			}
		}

		//---- change facility type to area type ----

		type = dir_ptr->Type ();

		if (type == FREEWAY || type == BRIDGE) {
			type = 0;
		} else {
			type = 0;

			if (zone > 0) {
				zone_ptr = zone_data.Get (zone);

				if (zone_ptr != NULL && zone_ptr->Area_Type () != 0) {
					type = zone_ptr->Area_Type ();
				}
			}
			if (type == 0) type = 2;
		}
		dir_ptr->Type (type);

		//---- change zone number to fare zone ----

		if (equiv_flag && zone > 0) {
			dir_ptr->Zone (zone_equiv.Zone_Group (zone));
		} else {
			dir_ptr->Zone (zone);
		}
	}
	return;
}
