//*********************************************************
//	Save_Route.cpp - save the route data
//*********************************************************

#include "TPPlusRoute.hpp"

#include "Utility.hpp"
	
//---------------------------------------------------------
//	Save_Route
//---------------------------------------------------------

void TPPlusRoute::Save_Route (void)
{
	int i;
	char buffer [FIELD_BUFFER];
	double freq1, freq2, avg_time;

	Route_Data route_rec, *route_ptr;

	if (flip) {
		str_cat (notes, sizeof (notes), " Flip");
	}

	//---- match to existing lines ----

	for (route_ptr = route_data.First (); route_ptr; route_ptr = route_data.Next ()) {

		if (route_ptr->Mode () != mode) continue;
		if (route_ptr->Nodes () != nnode) continue;

		for (i=1; i <= nnode; i++) {
			if (flip) {
				if (route_ptr->Node (i) != node [nnode-i]) break;
			} else {
				if (route_ptr->Node (i) != node [i-1]) break;
			}
		}
		if (i > nnode) break;
	}

	//---- update existing line ----

	if (route_ptr != NULL) {
		if (strstr (route_ptr->Name (), name) == NULL) {
			str_fmt (buffer, sizeof (buffer), "%s %s", route_ptr->Name (), name);
			route_ptr->Name (buffer);
		}
		str_fmt (buffer, sizeof (buffer), "%s %s", route_ptr->Notes (), notes);
		route_ptr->Notes (buffer);

		for (i=1; i <= num_periods; i++) {
			if (headway [i] > 0) {
				if (route_ptr->Headway (i) > 0) {
					freq1 = 60.0 / route_ptr->Headway (i);
					freq2 = 60.0 / headway [i];

					avg_time = ttime * freq2 + route_ptr->TTime (i) * freq1;
					freq2 += freq1;

					route_ptr->Headway (i, (int) (60.0 / freq2 + 0.5));
					route_ptr->TTime (i, (int) (avg_time / freq2 + 0.5));
				} else {
					route_ptr->Headway (i, headway [i]);
					route_ptr->TTime (i, ttime);
				}
				route_ptr->Offset (i, offset);
			}
		}
		num_match++;

	} else {

		//---- create a new line ----

		route_rec.Route (route_id++);

		if (!route_data.Add (&route_rec)) {
			Error ("Adding Transit Route");
		}
		route_ptr = route_data.Record ();

		route_ptr->Name (name);
		route_ptr->Mode (mode);
		route_ptr->Notes (notes);

		route_ptr->Nodes (nnode);
		route_ptr->Periods (num_periods);

		for (i=1; i <= num_periods; i++) {
			if (headway [i] > 0) {
				route_ptr->Headway (i, headway [i]);
				route_ptr->Offset (i, offset);
				route_ptr->TTime (i, ttime);
			}
		}
		for (i=1; i <= nnode; i++) {
			if (flip) {
				route_ptr->Node (i, node [nnode-i]);
			} else {
				route_ptr->Node (i, node [i-1]);
			}
		}
	}

	//---- split twoway lines ----

	if (!oneway) {

		str_cat (notes, sizeof (notes), " Return");

		//---- match to existing lines ----

		for (route_ptr = route_data.First (); route_ptr; route_ptr = route_data.Next ()) {

			if (route_ptr->Mode () != mode) continue;
			if (route_ptr->Nodes () != nnode) continue;

			for (i=1; i <= nnode; i++) {
				if (flip) {
					if (route_ptr->Node (i) != node [i-1]) break;
				} else {
					if (route_ptr->Node (i) != node [nnode-i]) break;
				}
			}
			if (i > nnode) break;
		}

		//---- update existing line ----

		if (route_ptr != NULL) {
			if (strstr (route_ptr->Name (), name) == NULL) {
				str_fmt (buffer, sizeof (buffer), "%s %s", route_ptr->Name (), name);
				route_ptr->Name (buffer);
			}
			str_fmt (buffer, sizeof (buffer), "%s %s", route_ptr->Notes (), notes);
			route_ptr->Notes (buffer);

			for (i=1; i <= num_periods; i++) {
				if (headway [i] > 0) {
					if (route_ptr->Headway (i) > 0) {
						freq1 = 60.0 / route_ptr->Headway (i);
						freq2 = 60.0 / headway [i];

						avg_time = ttime * freq2 + route_ptr->TTime (i) * freq1;
						freq2 += freq1;

						route_ptr->Headway (i, (int) (60.0 / freq2 + 0.5));
						route_ptr->TTime (i, (int) (avg_time / freq2 + 0.5));
					} else {
						route_ptr->Headway (i, headway [i]);
						route_ptr->TTime (i, ttime);
					}
					route_ptr->Offset (i, offset);
				}
			}
			num_match++;

		} else {

			//---- create a new line ----

			route_rec.Route (route_id++);

			if (!route_data.Add (&route_rec)) {
				Error ("Adding Transit Route");
			}
			route_ptr = route_data.Record ();

			route_ptr->Name (name);
			route_ptr->Mode (mode);
			route_ptr->Notes (notes);

			route_ptr->Nodes (nnode);
			route_ptr->Periods (num_periods);

			for (i=1; i <= num_periods; i++) {
				if (headway [i] > 0) {
					route_ptr->Headway (i, headway [i]);
					route_ptr->Offset (i, offset);
					route_ptr->TTime (i, ttime);
				}
			}
			for (i=1; i <= nnode; i++) {
				if (flip) {
					route_ptr->Node (i, node [i-1]);
				} else {
					route_ptr->Node (i, node [nnode-i]);
				}
			}
		}
	}
}
