//*********************************************************
//	Node_Plan.cpp - Build a Node-based Plan
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Node_Plan
//---------------------------------------------------------

int Router::Node_Plan (Use_Type type)
{
	Link_Data *link_ptr;

	if (trip_org.ID () == trip_des.ID ()) {
		trip_des.TOD (trip_org.TOD ());
		return (0);
	}

	//---- check the origin link ----

	link_ptr = link_data [trip_org.Link ()];
		
	if (!Use_Permission (link_ptr->Use (), type)) {
		return (Set_Problem (LINK_PROBLEM));
	}

	//---- check the destination link ----

	link_ptr = link_data [trip_des.Link ()];

	if (!Use_Permission (link_ptr->Use (), type)) {
		return (Set_Problem (LINK_PROBLEM));
	}

	//---- initialize the path data ----

	org_array.Reset ();
	des_array.Reset ();

	org_array.Add (&trip_org);
	des_array.Add (&trip_des);

	//---- build the path between the origin and destination activity locations ----

	if (!Node_Path (type)) {
		if (length_flag) {
			return (Set_Problem ((type == WALK) ? WALK_PROBLEM : BIKE_PROBLEM));
		} else if (time_flag) {
			return (Set_Problem (TIME_PROBLEM));
		}
		return (Set_Problem (PATH_PROBLEM));
	}

	//---- save the plan record ----

	memcpy (&trip_des, des_array.First (), sizeof (trip_des));

	return (Save_Plan (&trip_org, &trip_des, ((type == WALK) ? Plan_File::WALK_MODE : Plan_File::BIKE_MODE)));
}
