//*********************************************************
//	Pocket_Lanes.cpp - set the pocket lane fields
//*********************************************************

#include "IntControl.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Pocket_Lanes
//---------------------------------------------------------

void IntControl::Pocket_Lanes (void)
{
	int dir;

	Dir_Connect *dir_ptr;
	Link_Data *link_ptr;
	Pocket_Data *pocket_ptr;

	Show_Message ("Placing Pocket Lanes on Links - Record");
	Set_Progress ();

	//---- process the pocket lanes ----

	for (pocket_ptr = pocket_data.First (); pocket_ptr; pocket_ptr = pocket_data.Next ()) {
		Show_Progress ();

		link_ptr = link_data.Get (pocket_ptr->Link ());

		if (pocket_ptr->Dir () > 0) {
			dir = link_ptr->BA_Dir ();
		} else {
			dir = link_ptr->AB_Dir ();
		}
		if (dir == 0) {
			Error ("Pocket Lane %d and Link %d Direction are Incompatible", pocket_ptr->ID (), link_ptr->Link ());
		}
		dir_ptr = dir_data [dir];

		if (pocket_ptr->Type () == TURN_LANE) {
			if (pocket_ptr->Lane () <= dir_ptr->Left ()) {
				dir_ptr->Left_Turn (dir_ptr->Left_Turn () + 1);

				if (dir_ptr->Offset () == 0 || dir_ptr->Offset () > pocket_ptr->Length ()) {
					dir_ptr->Offset (pocket_ptr->Length ());
				}
			} else {
				dir_ptr->Right_Turn (dir_ptr->Right_Turn () + 1);
			}
		} else if (pocket_ptr->Type () == MERGE_LANE) {
			if (pocket_ptr->Lane () <= dir_ptr->Left ()) {
				dir_ptr->Left_Merge (dir_ptr->Left_Merge () + 1);
			} else {
				dir_ptr->Right_Merge (dir_ptr->Right_Merge () + 1);
			}
		}
	}
	End_Progress ();
}
