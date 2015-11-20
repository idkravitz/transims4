//*********************************************************
//	Write_Node_Path - draw the path between link offsets
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Write_Node_Path
//---------------------------------------------------------

void ArcPlan::Write_Node_Path (Offset_Data *org_ptr, Offset_Data *des_ptr, bool veh_flag)
{
	int i, j, dir, num_nodes, *node, anode;
	double offset, length, side, end_offset;
	bool offset_flag;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	AB_Data *ab_ptr;

	offset_flag = (link_offset != 0.0);
	side = 0.0;

	//---- get the path list ----

	anode = 0;
	offset = org_ptr->Offset ();
	node = plan_file.Path (&num_nodes);

	for (i=1; i <= num_nodes; i++, node++) {
		if (i == 1) {
			link_ptr = link_data.Get (org_ptr->Link ());
			length = UnRound (link_ptr->Length ());

			if (link_ptr->Bnode () == *node) {
				dir = 0;
				length = length - offset - link_offset;
			} else if (link_ptr->Anode () == *node) {
				dir = 1;
				length = offset - link_offset;
			} else {
				Warning ("Traveler %d Node %d is not on Origin Link %d", 
					plan_file.Traveler (), *node, link_ptr->Link ());
				return;
			}
		} else {
			ab_ptr = ab_key.Get (anode, *node);

			if (ab_ptr == NULL && !veh_flag) {
				ab_ptr = ab_key.Get (*node, anode);
			}
			if (ab_ptr == NULL) {
				Warning ("A Link between %d and %d was Not Found", anode, *node);
				return;
			}
			dir_ptr = dir_data [ab_ptr->dir];

			link_ptr = link_data.Get (dir_ptr->Link ());

			length = UnRound (link_ptr->Length ());

			if (link_ptr->Bnode () == *node) {
				dir = 0;
				offset = link_offset;
			} else {
				dir = 1;
				offset = length - link_offset;
			}
			length -= 2 * link_offset;
		}

		//---- collect the points ----

		if (offset_flag) {
			if (link_ptr->AB_Dir () > 0 && link_ptr->BA_Dir () > 0) {
				side = link_offset;
			} else {
				side = 0.0;
			}
		}
		Link_Shape (link_ptr, dir, &points, offset, length, side);

		//---- draw the link segment ----

		if (side == 0.0 && i > 1) {
			j = 2;
		} else {
			j = 1;
		}
		for (; j <= points.Num_Points (); j++) {
			if (!arcview_plan.points.Add (points [j])) goto point_error;
		}
		anode = *node;
	}

	//---- set the destination offset ----

	end_offset = des_ptr->Offset ();

	link_ptr = link_data.Get (des_ptr->Link ());
	length = UnRound (link_ptr->Length ());

	if (anode == 0) {
		if (end_offset >= offset) {
			dir = 0;
			length = end_offset - offset;
			if (length == 0.0) length = 0.1;
		} else {
			dir = 1;
			length = offset - end_offset;
		}
	} else if (link_ptr->Anode () == anode) {
		dir = 0;
		offset = link_offset;
		length = end_offset - offset;
	} else if (link_ptr->Bnode () == anode) {
		dir = 1;
		offset = length - link_offset;
		length = length - end_offset - link_offset;
	} else {
		Warning ("Traveler %d Node %d is not on Destination Link %d", 
			plan_file.Traveler (), anode, link_ptr->Link ());
		return;
	}

	//---- collect the points ----

	if (offset_flag) {
		if (link_ptr->AB_Dir () > 0 && link_ptr->BA_Dir () > 0) {
			side = link_offset;
		} else {
			side = 0.0;
		}
	}
	Link_Shape (link_ptr, dir, &points, offset, length, side);

	//---- draw the link segment ----

	if (side == 0.0 && anode != 0) {
		j = 2;
	} else {
		j = 1;
	}
	for (; j <= points.Num_Points (); j++) {
		if (!arcview_plan.points.Add (points [j])) goto point_error;
	}
	return;

point_error:
	Error ("Insufficient Memory for %d Points", arcview_plan.points.Num_Points ());

}
