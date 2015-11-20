//*********************************************************
//	Set_Boundary.cpp - Identify Boundary Linkse
//*********************************************************

#include "SubareaPlans.hpp"

#include "In_Polygon.hpp"

//---------------------------------------------------------
//	Set_Boundary
//---------------------------------------------------------

void SubareaPlans::Set_Boundary (void)
{
	bool a_in, b_in;
	int type;
	double x, y;

	Node_Data *node_ptr;
	Link_Data *link_ptr;
	Boundary boundary_rec;
	Location_Data *loc_ptr;
	Stop_Data *stop_ptr;

	//---- process the node records ----

	for (node_ptr = node_data.First (); node_ptr != NULL; node_ptr = node_data.Next ()) {
		x = UnRound (node_ptr->X ());
		y = UnRound (node_ptr->Y ());

		if (In_Polygon (x, y, &subarea_boundary.points)) {
			node_ptr->Type (1);
		} else {
			node_ptr->Type (0);
		}
	}

	//---- process the link records ----

	memset (&boundary_rec, '\0', sizeof (boundary_rec));

	for (link_ptr = link_data.First (); link_ptr != NULL; link_ptr = link_data.Next ()) {

		node_ptr = node_data.Get (link_ptr->Anode ());
		a_in = (node_ptr != NULL && node_ptr->Type () > 0);

		node_ptr = node_data.Get (link_ptr->Bnode ());
		b_in = (node_ptr != NULL && node_ptr->Type () > 0);

		type = 0;
		if (a_in) type += 1;
		if (b_in) type += 2;

		link_ptr->Type (type);
		link_ptr->Use (0);

		//---- save boundary links ----

		if (a_in != b_in) {
			boundary_rec.link = link_ptr->Link ();
			boundary_rec.type = type;

			if (!boundary.Add (&boundary_rec)) {
				Error ("Adding a Link to the Boundary List");
			}
			nlink++;
		}
	}

	//---- set the location flags ----

	for (loc_ptr = location_data.First (); loc_ptr != NULL; loc_ptr = location_data.Next ()) {
		link_ptr = link_data.Get (loc_ptr->Link ());

		if (link_ptr == NULL) {
			loc_ptr->Zone (0);
		} else {
			if (zone_flag && loc_ptr->Zone () > 0) {
				link_ptr->Use (zone_equiv.Zone_Group (loc_ptr->Zone ()));
			}
			if (link_ptr->Type () > 0) {
				loc_ptr->Zone (1);
			} else {
				loc_ptr->Zone (0);
			}
		}
	}

	//---- set the stop flags ----

	if (transit_flag) {
		for (stop_ptr = stop_data.First (); stop_ptr; stop_ptr = stop_data.Next ()) {
			link_ptr = link_data.Get (stop_ptr->Link ());

			if (link_ptr != NULL && link_ptr->Type () > 0) {
				stop_ptr->Type (1);;
			} else {
				stop_ptr->Type (0);
			}
		}
	}
}
