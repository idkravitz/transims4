//*********************************************************
//	Locate_OD.cpp - assign zones to activity locations
//*********************************************************

#include "ConvertTours.hpp"

#include "math.h"

//---------------------------------------------------------
//	Locate_OD
//---------------------------------------------------------

bool ConvertTours::Locate_OD (File_Group *group, int &org, int &des, int &stop, int &park, double &dist1, double &dist2)
{
	int loc, last, x, y, x2, y2, mode;
	double org_wt, des_wt, stop_wt, prob, cum_wt, dx, dy, share, max_dist;
	bool org_flag, des_flag, stop_flag, org_loc_flag, des_loc_flag;
	Integer_List *org_ptr, *des_ptr, *stop_ptr;

	Convert_Location *loc_ptr;

	//---- set the vehicle access flags ----

	if (org == 0 || des == 0) return (false);
	org_ptr = des_ptr = stop_ptr = 0;

	mode = group->Mode ();

	org_flag = (mode != DRIVE_ALONE && mode != PNR_OUT && mode != CARPOOL2 &&
				mode != CARPOOL3 && mode != CARPOOL4 && mode != KNR_OUT);
	des_flag = (mode != DRIVE_ALONE && mode != PNR_IN && mode != CARPOOL2 &&
				mode != CARPOOL3 && mode != CARPOOL4 && mode != KNR_IN);
	stop_flag = org_flag && des_flag;

	org_loc_flag = (org < 0);
	des_loc_flag = (des < 0);
	org = abs (org);
	des = abs (des);

	dist1 = dist2 = max_dist = org_wt = 0.0;

	//---- calculate the origin weight ----

	if (!org_loc_flag) {
		org_wt = 0.0;

		if (zone_loc_flag) {
			org_ptr = zone_loc_map.Locations (org);
		}
		for (loc = zone_loc [org]; loc; loc = loc_ptr->Zone_List ()) {
			loc_ptr = (Convert_Location *) location_data [loc];	

			if (org_flag || loc_ptr->Org_Parking () != 0) {
				org_wt += loc_ptr->Weight (group->Org_Wt ());
			}
		}
		if (org_ptr != 0) {
			for (loc = org_ptr->First (); loc != 0; loc = org_ptr->Next ()) {
				loc_ptr = (Convert_Location *) location_data [loc];	

				if (org_flag || loc_ptr->Org_Parking () > 0) {
					org_wt += loc_ptr->Weight (group->Org_Wt ());
				}
			}
		}
		if (org_wt == 0.0) return (false);
	}

	//---- check the destination weight ----
	
	if (!des_loc_flag) {
		des_wt = 0.0;

		if (zone_loc_flag) {
			des_ptr = zone_loc_map.Locations (des);
		}
		for (loc = zone_loc [des]; loc; loc = loc_ptr->Zone_List ()) {
			loc_ptr = (Convert_Location *) location_data [loc];	

			if (des_flag || loc_ptr->Des_Parking () != 0) {
				des_wt += loc_ptr->Weight (group->Des_Wt ());
			}
		}
		if (des_ptr != 0) {
			for (loc = des_ptr->First (); loc != 0; loc = des_ptr->Next ()) {
				loc_ptr = (Convert_Location *) location_data [loc];	

				if (des_flag || loc_ptr->Des_Parking () > 0) {
					des_wt += loc_ptr->Weight (group->Des_Wt ());
				}
			}
		}
		if (des_wt == 0.0) return (false);
	}

	//---- check the stop weight ----
	
	stop_wt = 0.0;
	
	if (stop > 0) {
		if (zone_loc_flag) {
			stop_ptr = zone_loc_map.Locations (stop);
		}
		for (loc = zone_loc [stop]; loc; loc = loc_ptr->Zone_List ()) {
			loc_ptr = (Convert_Location *) location_data [loc];	

			if (stop_flag || loc_ptr->Des_Parking () != 0) {
				stop_wt += loc_ptr->Weight (group->Stop_Wt ());
			}
		}
		if (stop_ptr != 0) {
			for (loc = stop_ptr->First (); loc != 0; loc = stop_ptr->Next ()) {
				loc_ptr = (Convert_Location *) location_data [loc];	

				if (stop_flag || loc_ptr->Des_Parking () > 0) {
					stop_wt += loc_ptr->Weight (group->Stop_Wt ());
				}
			}
		}
		if (stop_wt == 0.0) return (false);
	}

	//---- locate the trip origin ----

	if (!org_loc_flag) {
		prob = org_wt * random_org.Probability ();

		last = 0;
		cum_wt = 0.0;

		for (loc = zone_loc [org]; loc; loc = loc_ptr->Zone_List ()) {
			loc_ptr = (Convert_Location *) location_data [loc];	

			if (org_flag || loc_ptr->Org_Parking () != 0) {
				share = loc_ptr->Weight (group->Org_Wt ());

				if (share != 0.0) {
					cum_wt += share;
					if (prob < cum_wt) break;
					last = loc;
				}
			}
		}
		if (loc == 0 && org_ptr != 0) {
			for (loc = org_ptr->First (); loc != 0; loc = org_ptr->Next ()) {
				loc_ptr = (Convert_Location *) location_data [loc];	

				if (org_flag || loc_ptr->Org_Parking () != 0) {
					share = loc_ptr->Weight (group->Org_Wt ());

					if (share != 0.0) {
						cum_wt += share;
						if (prob < cum_wt) break;
						last = loc;
					}
				}
			}
		}
		if (loc == 0) {
			if (last == 0) return (false);
			loc_ptr = (Convert_Location *) location_data [last];
		}
		org = loc_ptr->ID ();
		park = loc_ptr->Org_Parking ();
	} else {
		loc_ptr = (Convert_Location *) location_data.Get (org);
	}
	x = loc_ptr->X ();
	y = loc_ptr->Y ();
	
	//---- calculate the destination weight ----

	if (!des_loc_flag) {
		des_wt = 0.0;

		for (loc = zone_loc [des]; loc; loc = loc_ptr->Zone_List ()) {
			loc_ptr = (Convert_Location *) location_data [loc];	

			if (loc_ptr->ID () != org && (des_flag || loc_ptr->Des_Parking () != 0)) {
				share = loc_ptr->Weight (group->Des_Wt ());

				if (share != 0.0) {

					if (group->Dist_Wt ()) {

						//--- apply the distance weight ----

						dx = loc_ptr->X () - x;
						dy = loc_ptr->Y () - y;

						dist1 = sqrt (dx * dx + dy * dy);
						if (dist1 == 0.0) dist1 = 0.01;
					
						des_wt += share * dist1;
					} else {
						des_wt += share;
					}
				}
			}
		}
		if (des_ptr != 0) {
			for (loc = des_ptr->First (); loc != 0; loc = des_ptr->Next ()) {
				loc_ptr = (Convert_Location *) location_data [loc];	

				if (loc_ptr->ID () != org && (des_flag || loc_ptr->Des_Parking () != 0)) {
					share = loc_ptr->Weight (group->Des_Wt ());

					if (share != 0.0) {

						if (group->Dist_Wt ()) {

							//--- apply the distance weight ----

							dx = loc_ptr->X () - x;
							dy = loc_ptr->Y () - y;

							dist1 = sqrt (dx * dx + dy * dy);
							if (dist1 == 0.0) dist1 = 0.01;
						
							des_wt += share * dist1;
						} else {
							des_wt += share;
						}
					}
				}
			}
		}
		if (des_wt == 0.0) return (false);

		//---- locate the destination ----

		prob = des_wt * random_des.Probability ();
		
		last = 0;
		cum_wt = 0.0;

		for (loc = zone_loc [des]; loc; loc = loc_ptr->Zone_List ()) {
			loc_ptr = (Convert_Location *) location_data [loc];	

			if (loc_ptr->ID () != org && (des_flag || loc_ptr->Des_Parking () != 0)) {
				share = loc_ptr->Weight (group->Des_Wt ());

				if (share != 0.0) {
					if (group->Dist_Wt ()) {

						//--- apply the distance weight ----

						dx = loc_ptr->X () - x;
						dy = loc_ptr->Y () - y;

						dist1 = sqrt (dx * dx + dy * dy);
						if (dist1 == 0.0) dist1 = 0.01;
					
						cum_wt += share * dist1;
					} else {
						cum_wt += share;
					}
					if (prob < cum_wt) break;
					last = loc;
				}
			}
		}
		if (loc == 0 && des_ptr != 0) {
			for (loc = des_ptr->First (); loc != 0; loc = des_ptr->Next ()) {
				loc_ptr = (Convert_Location *) location_data [loc];	

				if (loc_ptr->ID () != org && (des_flag || loc_ptr->Des_Parking () != 0)) {
					share = loc_ptr->Weight (group->Des_Wt ());

					if (share != 0.0) {
						if (group->Dist_Wt ()) {

							//--- apply the distance weight ----

							dx = loc_ptr->X () - x;
							dy = loc_ptr->Y () - y;

							dist1 = sqrt (dx * dx + dy * dy);
							if (dist1 == 0.0) dist1 = 0.01;
						
							cum_wt += share * dist1;
						} else {
							cum_wt += share;
						}
						if (prob < cum_wt) break;
						last = loc;
					}
				}
			}
		}
		if (loc == 0) {
			if (last == 0) return (false);
			loc_ptr = (Convert_Location *) location_data [last];	

			dx = loc_ptr->X () - x;
			dy = loc_ptr->Y () - y;
			dist1 = sqrt (dx * dx + dy * dy);
		}
		des = loc_ptr->ID ();
	} else {
		loc_ptr = (Convert_Location *) location_data.Get (des);
	}
	x2 = loc_ptr->X ();
	y2 = loc_ptr->Y ();

	//---- calculate the stop weight ----

	if (stop > 0) {

		//---- find the maximum distance ----

		max_dist = 0.0;

		for (loc = zone_loc [stop]; loc; loc = loc_ptr->Zone_List ()) {
			loc_ptr = (Convert_Location *) location_data [loc];	

			//--- apply the distance weight ----

			dx = loc_ptr->X () - x;
			dy = loc_ptr->Y () - y;

			dist1 = sqrt (dx * dx + dy * dy);
			if (dist1 == 0.0) dist1 = 0.01;

			dx = loc_ptr->X () - x2;
			dy = loc_ptr->Y () - y2;

			dist2 = sqrt (dx * dx + dy * dy);
			if (dist2 == 0.0) dist2 = 0.01;

			if ((dist1 + dist2) > max_dist) {
				max_dist = dist1 + dist2 + 1.0;
			}
		}
		if (stop_ptr != 0) {
			for (loc = stop_ptr->First (); loc != 0; loc = stop_ptr->Next ()) {
				loc_ptr = (Convert_Location *) location_data [loc];	

				//--- apply the distance weight ----

				dx = loc_ptr->X () - x;
				dy = loc_ptr->Y () - y;

				dist1 = sqrt (dx * dx + dy * dy);
				if (dist1 == 0.0) dist1 = 0.01;

				dx = loc_ptr->X () - x2;
				dy = loc_ptr->Y () - y2;

				dist2 = sqrt (dx * dx + dy * dy);
				if (dist2 == 0.0) dist2 = 0.01;

				if ((dist1 + dist2) > max_dist) {
					max_dist = dist1 + dist2 + 1.0;
				}
			}
		}
		if (max_dist == 0.0) return (false);

		//---- calculate the stop weight ----

		stop_wt = 0.0;

		for (loc = zone_loc [stop]; loc; loc = loc_ptr->Zone_List ()) {
			loc_ptr = (Convert_Location *) location_data [loc];	

			if (stop_flag || loc_ptr->Des_Parking () != 0) {
				share = loc_ptr->Weight (group->Stop_Wt ());

				if (share != 0.0) {

					//--- apply the distance weight ----

					dx = loc_ptr->X () - x;
					dy = loc_ptr->Y () - y;

					dist1 = sqrt (dx * dx + dy * dy);
					if (dist1 == 0.0) dist1 = 0.01;

					dx = loc_ptr->X () - x2;
					dy = loc_ptr->Y () - y2;

					dist2 = sqrt (dx * dx + dy * dy);
					if (dist2 == 0.0) dist2 = 0.01;

					if ((dist1 + dist2) < max_dist) {
						stop_wt += share * (max_dist - dist1 - dist2);
					}
				}
			}
		}
		if (stop_ptr != 0) {
			for (loc = stop_ptr->First (); loc != 0; loc = stop_ptr->Next ()) {
				loc_ptr = (Convert_Location *) location_data [loc];	

				if (stop_flag || loc_ptr->Des_Parking () != 0) {
					share = loc_ptr->Weight (group->Stop_Wt ());

					if (share != 0.0) {

						//--- apply the distance weight ----

						dx = loc_ptr->X () - x;
						dy = loc_ptr->Y () - y;

						dist1 = sqrt (dx * dx + dy * dy);
						if (dist1 == 0.0) dist1 = 0.01;

						dx = loc_ptr->X () - x2;
						dy = loc_ptr->Y () - y2;

						dist2 = sqrt (dx * dx + dy * dy);
						if (dist2 == 0.0) dist2 = 0.01;

						if ((dist1 + dist2) < max_dist) {
							stop_wt += share * (max_dist - dist1 - dist2);
						}
					}
				}
			}
		}
		if (stop_wt == 0.0) return (false);

		//---- locate the stop ----

		prob = stop_wt * random_stop.Probability ();
		
		last = 0;
		cum_wt = 0.0;

		for (loc = zone_loc [stop]; loc; loc = loc_ptr->Zone_List ()) {
			loc_ptr = (Convert_Location *) location_data [loc];	

			if (stop_flag || loc_ptr->Des_Parking () != 0) {
				share = loc_ptr->Weight (group->Stop_Wt ());

				if (share != 0.0) {

					//--- apply the distance weight ----

					dx = loc_ptr->X () - x;
					dy = loc_ptr->Y () - y;

					dist1 = sqrt (dx * dx + dy * dy);
					if (dist1 == 0.0) dist1 = 0.01;

					dx = loc_ptr->X () - x2;
					dy = loc_ptr->Y () - y2;

					dist2 = sqrt (dx * dx + dy * dy);
					if (dist2 == 0.0) dist2 = 0.01;

					if ((dist1 + dist2) < max_dist) {
						cum_wt += share * (max_dist - dist1 - dist2);

						if (prob < cum_wt) break;
						last = loc;
					}
				}
			}
		}
		if (loc == 0 && stop_ptr != 0) {
			for (loc = stop_ptr->First (); loc != 0; loc = stop_ptr->Next ()) {
				loc_ptr = (Convert_Location *) location_data [loc];	

				if (stop_flag || loc_ptr->Des_Parking () != 0) {
					share = loc_ptr->Weight (group->Stop_Wt ());

					if (share != 0.0) {

						//--- apply the distance weight ----

						dx = loc_ptr->X () - x;
						dy = loc_ptr->Y () - y;

						dist1 = sqrt (dx * dx + dy * dy);
						if (dist1 == 0.0) dist1 = 0.01;

						dx = loc_ptr->X () - x2;
						dy = loc_ptr->Y () - y2;

						dist2 = sqrt (dx * dx + dy * dy);
						if (dist2 == 0.0) dist2 = 0.01;

						if ((dist1 + dist2) < max_dist) {
							cum_wt += share * (max_dist - dist1 - dist2);

							if (prob < cum_wt) break;
							last = loc;
						}
					}
				}
			}
		}
		if (loc == 0) {
			if (last == 0) return (false);
			loc_ptr = (Convert_Location *) location_data [last];	

			dx = loc_ptr->X () - x;
			dy = loc_ptr->Y () - y;
			dist1 = sqrt (dx * dx + dy * dy);

			dx = loc_ptr->X () - x2;
			dy = loc_ptr->Y () - y2;
			dist2 = sqrt (dx * dx + dy * dy);
		}
		stop = loc_ptr->ID ();
	} else {
		loc_ptr = (Convert_Location *) location_data.Get (des);	

		dx = loc_ptr->X () - x;
		dy = loc_ptr->Y () - y;

		dist1 = sqrt (dx * dx + dy * dy);
	}
	return (org != des && des != 0);
}
