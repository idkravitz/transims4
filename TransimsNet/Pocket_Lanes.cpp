//*********************************************************
//	Pocket_Lanes.cpp - Thru Link and Pocket Lanes
//*********************************************************

#include "TransimsNet.hpp"

#include "Pocket_File.hpp"

//---------------------------------------------------------
//	Pocket_Lanes
//---------------------------------------------------------

void TransimsNet::Pocket_Lanes (Dir_Connect *dir_ptr, Node_List *node_ptr, int link_len)
{
	int i, zone, area_type, funcl, len, max_len, left, right, link, node, id, min_to, lanes;
	double length;

	Zone_Data *zone_ptr;
	Connection *connect_ptr;
	Pocket_File *pocket_file;
	Link_Detail_Data *detail_ptr;
		
	dir_ptr->Left (0);
	dir_ptr->Right (0);

	//---- get the area type ----

	if (zone_flag) {
		zone = Closest_Zone (node_ptr->X (), node_ptr->Y ());

		zone_ptr = zone_data.Get (zone);

		if (zone_ptr == NULL) {
			Error ("Zone %d was Not Found in the Zone file", zone);
		}
		area_type = zone_ptr->Area_Type ();
	} else {
		area_type = 2;
	}

	//---- get the pocket length ----

	funcl = dir_ptr->Type ();

	max_len = pocket_len [funcl].Best (area_type);

	if (max_len == 0 || (max_len / 2) > link_len) return;

	//---- turn pockets ----

	if (dir_ptr->Num_To () > 1) {
		lanes = 0;

		if (detail_flag) {
			detail_ptr = detail_data.Get (dir_ptr->Link_Dir ());
			if (detail_ptr != NULL) {
				lanes = detail_ptr->Left () + detail_ptr->Left_Thru () + detail_ptr->Thru () + detail_ptr->Right_Thru () + detail_ptr->Right ();
			}
		}
		if (lanes == 0) {
			left = right = EXTERNAL;

			for (connect_ptr = dir_ptr->To_First (); connect_ptr; connect_ptr = dir_ptr->To_Next ()) {
				if (connect_ptr->Is_Right () || connect_ptr->Is_R_Split ()) {
					if (funcl == FREEWAY) {
						if (connect_ptr->Facility () == FREEWAY || connect_ptr->Facility () == RAMP) right = FREEWAY;
					} else {
						if (connect_ptr->Facility () < right) right = connect_ptr->Facility ();
					}
				} else if (connect_ptr->Is_Left () || connect_ptr->Is_L_Split ()) {
					if (funcl == FREEWAY) {
						if (connect_ptr->Facility () == FREEWAY || connect_ptr->Facility () == RAMP) left = FREEWAY;
					} else {
						if (connect_ptr->Facility () < left) left = connect_ptr->Facility ();
					}
				}
			}

			//---- fix up the turns ----

			if (pocket_to_flag) {
				min_to = pocket_to.Best (area_type);
				if (min_to < left) left = 0;
				if (min_to < right) right = 0;
			} else {
				if (left == EXTERNAL) {
					left = 0;
				} else if (pocket_len [left].Best (area_type) == 0) {
					left = 0;
				}
				if (right == EXTERNAL) {
					right = 0;
				} else if (pocket_len [right].Best (area_type) == 0) {
					right = 0;
				}
			}
			if (right != 0) {
				if (funcl >= MINOR) {
					right = 0;
				} else if (funcl == MAJOR) {
					if (right >= MINOR) {
						right = 0;
					}
				}
			}

			//---- set the turn pockets ----

			if (left > 0) {
				dir_ptr->Left_Turn (1);
			}
			if (right > 0) {
				dir_ptr->Right_Turn (1);
			}
		} else {

			//---- set the turn pockets ----

			if (lanes > dir_ptr->Thru ()) {
				if (detail_ptr->Left () > 0) {
					for (connect_ptr = dir_ptr->To_First (); connect_ptr; connect_ptr = dir_ptr->To_Next ()) {
						if (connect_ptr->Is_Left () || connect_ptr->Is_L_Split ()) {
							dir_ptr->Left_Turn (detail_ptr->Left ());
							break;
						}
					}
				}
				if (detail_ptr->Right () > 0) {
					for (connect_ptr = dir_ptr->To_First (); connect_ptr; connect_ptr = dir_ptr->To_Next ()) {
						if (connect_ptr->Is_Right () || connect_ptr->Is_R_Split ()) {
							dir_ptr->Right_Turn (detail_ptr->Right ());
							break;
						}
					}
				}
				lanes = detail_ptr->Left_Thru () + detail_ptr->Thru () + detail_ptr->Right_Thru ();

				while (lanes < dir_ptr->Thru () && (dir_ptr->Left_Turn () > 0 || dir_ptr->Right_Turn () > 0)) {
					if (dir_ptr->Right_Turn () > 0) {
						lanes++;
						dir_ptr->Right_Turn (dir_ptr->Right_Turn () - 1);
					}
					if (lanes < dir_ptr->Thru () && dir_ptr->Left_Turn () > 0) {
						lanes++;
						dir_ptr->Left_Turn (dir_ptr->Left_Turn () - 1);
					}
				}
			}
		}
	}

	//---- merge pockets ----

	if (funcl == FREEWAY && dir_ptr->Num_From () > 1) {

		for (connect_ptr = dir_ptr->From_First (); connect_ptr; connect_ptr = dir_ptr->From_Next ()) {
			if (connect_ptr->Is_Right () || connect_ptr->Is_R_Merge ()) {
				if (connect_ptr->Facility () == FREEWAY || connect_ptr->Facility () == RAMP) dir_ptr->Right_Merge (1);
			} else if (connect_ptr->Is_Left () || connect_ptr->Is_L_Merge ()) {
				if (connect_ptr->Facility () == FREEWAY || connect_ptr->Facility () == RAMP) dir_ptr->Left_Merge (1);
			}
		}
	}

	//---- write pocket lanes ----

	left = MAX (dir_ptr->Left_Turn (), dir_ptr->Left_Merge ());
	right = MAX (dir_ptr->Right_Turn (), dir_ptr->Right_Merge ());

	if (left == 0 && right == 0) return;

	link = dir_ptr->Link ();
	node = node_ptr->Node ();

	pocket_file = (Pocket_File *) Network_Db_Base (NEW_POCKET_LANE);

	//---- left side pocket lanes ----

	if (left > 0) {
		dir_ptr->Left (left);

		lanes = 0;
		if (dir_ptr->Left_Turn () > 0) lanes++;
		if (dir_ptr->Left_Merge () > 0) lanes++;

		len = max_len * lanes;

		if (len > link_len) {
			len = link_len;
		}
		length = UnRound (len / lanes);

		for (i=1; i <= dir_ptr->Left_Turn (); i++) {
			npocket++;
			id = mpocket + npocket;

			pocket_file->ID (id);
			pocket_file->Link (link);
			pocket_file->Node (node);
			pocket_file->Offset (0.0);
			pocket_file->Lane (dir_ptr->Left () - i + 1);
			pocket_file->Type ("T");
			pocket_file->Length (length);
			pocket_file->Notes ("Left Turn Lane");

			if (!pocket_file->Write ()) {
				Error ("Writing Pocket Lane Record %d", npocket);
			}
		}

		for (i=1; i <= dir_ptr->Left_Merge (); i++) {
			npocket++;
			id = mpocket + npocket;

			pocket_file->ID (id);
			pocket_file->Link (link);
			pocket_file->Node (node);
			pocket_file->Offset (0.0);
			pocket_file->Lane (dir_ptr->Left() - i + 1);
			pocket_file->Type ("M");
			pocket_file->Length (length);
			pocket_file->Notes ("Left Merge Lane");

			if (!pocket_file->Write ()) {
				Error ("Writing Pocket Lane Record %d", npocket);
			}
		}
	}

	//---- right side pocket lanes ----

	if (right > 0) {
		dir_ptr->Right (right);

		left = dir_ptr->Thru () + dir_ptr->Left ();

		lanes = 0;
		if (dir_ptr->Right_Turn () > 0) lanes++;
		if (dir_ptr->Right_Merge () > 0) lanes++;

		len = max_len * right;

		if (len > link_len) {
			len = link_len;
		}
		length = UnRound (len / lanes);

		for (i=1; i <= dir_ptr->Right_Turn (); i++) {
			npocket++;
			id = mpocket + npocket;

			pocket_file->ID (id);
			pocket_file->Link (link);
			pocket_file->Node (node);
			pocket_file->Offset (0.0);
			pocket_file->Lane (left + i);
			pocket_file->Type ("T");
			pocket_file->Length (length);
			pocket_file->Notes ("Right Turn Lane");

			if (!pocket_file->Write ()) {
				Error ("Writing Pocket Lane Record %d", npocket);
			}
		}

		for (i=1; i <= dir_ptr->Right_Merge (); i++) {
			npocket++;
			id = mpocket + npocket;

			pocket_file->ID (id);
			pocket_file->Link (link);
			pocket_file->Node (node);
			pocket_file->Offset (0.0);
			pocket_file->Lane (left + i);
			pocket_file->Type ("M");
			pocket_file->Length (length);
			pocket_file->Notes ("Right Merge Lane");

			if (!pocket_file->Write ()) {
				Error ("Writing Pocket Lane Record %d", npocket);
			}
		}
	}
} 
