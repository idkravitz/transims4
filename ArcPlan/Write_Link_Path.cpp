//*********************************************************
//	Write_Link_Path - draw the path between link offsets
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Write_Link_Path
//---------------------------------------------------------

void ArcPlan::Write_Link_Path (Offset_Data *org_ptr, Offset_Data *des_ptr, bool veh_flag)
{
	int i, j, dir, num_links, *link, lnk;
	double offset, length, side, end_offset;
	bool offset_flag;

	Link_Data *link_ptr;

	offset_flag = (link_offset != 0.0);
	side = 0.0;

	//---- get the link list ----

	offset = org_ptr->Offset ();
	link = plan_file.Path (&num_links);

	for (i=1; i <= num_links; i++, link++) {
		if (*link < 0) {
			lnk = -(*link);
			dir = 1;
		} else {
			lnk = *link;
			dir = 0;
		}
		link_ptr = link_data.Get (lnk);

		if (link_ptr == NULL) {
			Warning ("Traveler %d Link %d was Not Found", plan_file.Traveler (), lnk);
			return;
		}
		length = UnRound (link_ptr->Length ());

		//---- process the origin offset ----

		if (i == 1) {
			if (org_ptr->Link () != lnk) {
				Warning ("Traveler %d Link %d does not match Origin Link %d", 
					plan_file.Traveler (), lnk, org_ptr->Link ());
				return;
			}
			if (i == num_links) {
				if (des_ptr->Link () != lnk) goto des_error;

				end_offset = des_ptr->Offset ();

				if (dir == 0) {
					length = end_offset - offset;
					if (length == 0.0) length = 0.1;
				} else {
					length = offset - end_offset;
				}
			} else if (dir == 0) {
				length = length - offset - link_offset;
			} else {
				length = offset - link_offset;
			}

		} else if (i == num_links) {

			//---- process the destination offset ----

			if (des_ptr->Link () != lnk) goto des_error;

			end_offset = des_ptr->Offset ();

			if (dir == 0) {
				offset = link_offset;
				length = end_offset - offset;
			} else {
				offset = length - link_offset;
				length = length - end_offset - link_offset;
			}

		} else {

			if (dir == 0) {
				offset = link_offset;
			} else {
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
	}
	return;

point_error:
	Error ("Insufficient Memory for %d Points", arcview_plan.points.Num_Points ());

des_error:
	Warning ("Traveler %d Link %d does not match Destination Link %d", 
		plan_file.Traveler (), lnk, des_ptr->Link ());
}
