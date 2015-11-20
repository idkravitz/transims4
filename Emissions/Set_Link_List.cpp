//*********************************************************
//	Set_Link_List.cpp - process the link equivalence data
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Set_Link_List
//---------------------------------------------------------

void Emissions::Set_Link_List (void)
{
	int i, num, link, lnk, dir;

	Link_Data *link_ptr;
	Integer_List *group;

	num = link_equiv.Max_Group ();

	for (i=1; i <= num; i++) {

		group = link_equiv.Group_List (i);
		if (group == NULL) continue;

		//---- process each link in the link group ----

		for (link = group->First (); link != 0; link = group->Next ()) {

			if (vol_spd_flag) {
				dir = abs (link);
			} else {
				lnk = abs (link);

				link_ptr = link_data.Get (lnk);

				if (link_ptr == NULL) continue;

				if (link < 0) {
					dir = link_ptr->BA_Dir ();
				} else {
					dir = link_ptr->AB_Dir ();
				}
			}

			//---- add to the link list ----

			if (link_list.Get_Index (dir) == 0) {
				if (!link_list.Add (dir)) {
					Error ("Adding Link to the Link List");
				}
			}
		}
	}
}
