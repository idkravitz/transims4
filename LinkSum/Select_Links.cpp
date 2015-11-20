//*********************************************************
//	Select_Links.cpp - flag selected links
//*********************************************************

#include "LinkSum.hpp"

#include "In_Polygon.hpp"

//---------------------------------------------------------
//	Select_Links
//---------------------------------------------------------

void LinkSum::Select_Links (void)
{
	int i, num, link, count;
	bool ab_flag, ba_flag;

	Link_Data *link_ptr;
	Integer_List *group;

	Show_Message ("Selecting Links -- Record");
	Set_Progress (10000);

	count = 0;
	if (group_select) {
		num = link_equiv.Max_Group ();
	} else {
		num = 0;
	}

	for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {
		Show_Progress ();
		if (!funcl_flag [link_ptr->Type ()]) {
			link_ptr->Use (0);
			continue;
		}
		if (subarea_flag) {
			Node_Data *node_ptr = node_data.Get (link_ptr->Anode ());

			if (!In_Polygon (UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()), &select_subarea.points)) {
				node_ptr = node_data.Get (link_ptr->Bnode ());

				if (!In_Polygon (UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()), &select_subarea.points)) {
					link_ptr->Use (0);
					continue;
				}
			}
		}
		if (group_select) {
			ab_flag = ba_flag = false;

			for (i=1; i <= num; i++) {
				group = link_equiv.Group_List (i);
				if (group == NULL) continue;

				//---- compare each link in the link group ----

				for (link = group->First (); link != 0; link = group->Next ()) {
					if (abs (link) == link_ptr->Link ()) {
						if (link > 0) {
							ab_flag = true;
						} else {
							ba_flag = true;
						}
						if (ab_flag && ba_flag) break;
					}
				}
				if (ab_flag && ba_flag) break;
			}
			if (!ab_flag && !ba_flag) {
				link_ptr->Use (0);
				continue;
			} else if (!ab_flag) {
				link_ptr->Use (-2);
			} else if (!ba_flag) {
				link_ptr->Use (-1);
			}
		}
		count++;
	}
	End_Progress ();

	Print (2, "Number of Selected Links = %d", count);
}
