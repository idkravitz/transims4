//*********************************************************
//	Data_Setup.cpp - set the node and link data
//*********************************************************

#include "TransimsNet.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Data_Setup
//---------------------------------------------------------

void TransimsNet::Data_Setup (void)
{
	int i, zone, dir, diff, num, num_in, num_out, num_turn, num_ramp, remove, link_num [20];
	int left_turn, right_turn, left_merge, right_merge;
	bool other_flag, keep_flag, skip_flag, keep_node_flag;

	Node_List *node_ptr, *anode_ptr, *bnode_ptr;
	Link_Data *link_ptr;
	Dir_Connect *dir_ptr, *to_dir, *from_dir;

	//---- initialize the node data ----

	for (node_ptr = node_data.First (); node_ptr; node_ptr = node_data.Next ()) {
		node_ptr->Type (0);
		node_ptr->From_Dir (0);
		node_ptr->To_Dir (0);
	}

	//---- read the keep node list ----

	if (keep_nodes.Is_Open ()) {
		Show_Message ("Reading %s - Record", keep_nodes.File_Type ());
		Set_Progress ();

		while (keep_nodes.Read ()) {
			Show_Progress ();

			Get_Integer (keep_nodes.Record (), &num);

			if (num > 0) {
				node_ptr = node_data.Get (num);

				if (node_ptr == NULL) {
					Warning ("Keep Node %d is not in the Node file", num);
					Show_Message ("Record");
				} else {
					node_ptr->Type (KEEP_NODE);
				}
			}
		}
		End_Progress ();
	}

	Show_Message ("Setting the Node Type - Record");
	Set_Progress ();

	//---- build the link list ----

	for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {
		Show_Progress ();
			
		keep_flag = (update_flag && !update_links.In_Range (link_ptr->Link ()));
		skip_flag = (delete_flag && delete_links.In_Range (link_ptr->Link ()));

		//---- add to the link lists ----

		anode_ptr = node_data.Get (link_ptr->Anode ());
		bnode_ptr = node_data.Get (link_ptr->Bnode ());

		dir = link_ptr->AB_Dir ();

		if (dir > 0) {
			dir_ptr = dir_data [dir];

			if (keep_flag) {
				left_turn = dir_ptr->Left_Turn ();
				right_turn = dir_ptr->Right_Turn ();
				left_merge = dir_ptr->Left_Merge ();
				right_merge = dir_ptr->Right_Merge ();

				dir_ptr->Set_Dir ();

				dir_ptr->Left_Turn (left_turn);
				dir_ptr->Right_Turn (right_turn);
				dir_ptr->Left_Merge (left_merge);
				dir_ptr->Right_Merge (right_merge);
			} else {
				dir_ptr->Set_Dir ();
			}
			dir_ptr->Type (link_ptr->Type ());
			dir_ptr->Length (link_ptr->Length ());

			if (!skip_flag) {
				dir_ptr->From_List (anode_ptr->From_Dir ());
				anode_ptr->From_Dir (dir);

				dir_ptr->To_List (bnode_ptr->To_Dir ());
				bnode_ptr->To_Dir (dir);
			}
		}
		dir = link_ptr->BA_Dir ();

		if (dir > 0) {
			dir_ptr = dir_data [dir];

			if (keep_flag) {
				left_turn = dir_ptr->Left_Turn ();
				right_turn = dir_ptr->Right_Turn ();
				left_merge = dir_ptr->Left_Merge ();
				right_merge = dir_ptr->Right_Merge ();

				dir_ptr->Set_Dir ();

				dir_ptr->Left_Turn (left_turn);
				dir_ptr->Right_Turn (right_turn);
				dir_ptr->Left_Merge (left_merge);
				dir_ptr->Right_Merge (right_merge);
			} else {
				dir_ptr->Set_Dir ();
			}
			dir_ptr->Type (link_ptr->Type ());
			dir_ptr->Length (link_ptr->Length ());

			if (!skip_flag) {
				dir_ptr->To_List (anode_ptr->To_Dir ());
				anode_ptr->To_Dir (dir);

				dir_ptr->From_List (bnode_ptr->From_Dir ());
				bnode_ptr->From_Dir (dir);
			}
		}

		//---- process external zone connectors ----

		if (link_ptr->Type () == EXTERNAL) {
			if (link_ptr->Anode () > link_ptr->Bnode ()) {
				zone = link_ptr->Bnode ();
			} else {
				zone = link_ptr->Anode ();
			}
			if (zone < ext_zone || ext_zone == 0) ext_zone = zone;
			if (zone > high_zone) high_zone = zone;
		}
	}
	Set_Progress ();

	//---- set the node type ----

	remove = 0;

	for (node_ptr = node_data.First(); node_ptr; node_ptr = node_data.Next ()) {
		Show_Progress ();
		keep_node_flag = (node_ptr->Type () == KEEP_NODE);

		if (node_ptr->Node () <= high_zone) {
			if (node_ptr->Node () < ext_zone && !keep_node_flag && !connect_flag && !replicate_flag) {
				node_ptr->Type (DELETE_NODE);
				remove++;
			} else {
				node_ptr->Type (ZONE_CENTROID);
			}
			continue;
		}
		num_in = num_out = 0;

		for (dir = node_ptr->To_Dir (); dir; dir = dir_ptr->To_List ()) {
			dir_ptr = dir_data [dir];
			num_in++;
		}
		for (dir = node_ptr->From_Dir (); dir; dir = from_dir->From_List ()) {
			from_dir = dir_data [dir];
			num_out++;
		}
		if (num_in == 0 && num_out == 0) {
			if (!keep_node_flag) {
				node_ptr->Type (DELETE_NODE);
				remove++;
			}
			continue;
		}
		if (num_in == 0) {
			Warning ("Node %d has Exit Links but No Entry Links", node_ptr->Node ());
			node_ptr->Type (DEAD_END);
			continue;
		}
		if (num_out == 0) {
			Warning ("Node %d has Entry Links but No Exit Links", node_ptr->Node ());
			node_ptr->Type (DEAD_END);
			continue;
		}

		//---- find different links ----

		num = num_turn = num_ramp = 0;
		other_flag = false;
		memset (link_num, '\0', sizeof (link_num));

		for (dir = node_ptr->To_Dir (); dir; dir = to_dir->To_List ()) {
			to_dir = dir_data [dir];

			if (!other_flag) {
				if (to_dir->Type () != FREEWAY && to_dir->Type () != RAMP &&
					to_dir->Type () != EXPRESSWAY) {
					other_flag = true;
				}
			}
			for (i=0; i < num; i++) {
				if (to_dir->Link () == link_num [i]) break;
			}
			if (i == num) {
				if (num < 20) {
					link_num [num++] = to_dir->Link ();
					if (to_dir->Type () == RAMP) num_ramp++;
				}
			}

			for (dir = node_ptr->From_Dir (); dir; dir = from_dir->From_List ()) {
				from_dir = dir_data [dir];

				if (!other_flag) {
					if (from_dir->Type () != FREEWAY && from_dir->Type () != RAMP &&
						from_dir->Type () != EXPRESSWAY) {
						other_flag = true;
					}
				}
				if (to_dir->Link () != from_dir->Link ()) {
					diff = compass.Difference (to_dir->Out_Bearing (), from_dir->In_Bearing ());

					if (max_angle >= diff && diff > thru_diff) num_turn++;

					for (i=0; i < num; i++) {
						if (from_dir->Link () == link_num [i]) break;
					}
					if (i == num) {
						if (num < 20) {
							link_num [num++] = from_dir->Link ();
							if (from_dir->Type () == RAMP) num_ramp++;
						}
					}
				}
			}
		}
		if (num == 1) {
			node_ptr->Type (DEAD_END);
			continue;
		}
		if (num > 2) {
			if (other_flag && (num_in == 1 || (num_turn == 0 && 
				((num - num_ramp) <= 2 || (num_in < num && num_out < num))))) {
				other_flag = false;
			}
			node_ptr->Type ((other_flag) ? INTERSECTION : INTERCHANGE);
			continue;
		}

		//---- compare in and out links ----

		for (dir = node_ptr->To_Dir (); dir; dir = to_dir->To_List ()) {
			to_dir = dir_data [dir];

			for (dir = node_ptr->From_Dir (); dir; dir = from_dir->From_List ()) {
				from_dir = dir_data [dir];

				if (to_dir->Link () != from_dir->Link ()) break;
			}
			if (dir == 0) {
				dir = 1;
				break;
			}
			
			//---- compare links ----

			if (from_dir->Type () != to_dir->Type ()) break;
			if (from_dir->Thru () != to_dir->Thru ()) break;
			if (from_dir->Speed () != to_dir->Speed ()) break;

//			if (from_dir->Type () != to_dir->Type ()) {
//Warning ("Links %d and %d at Node %d have Facility Type differences (%d vs %d)", from_dir->Link (), to_dir->Link (), node_ptr->Node (), from_dir->Type (), to_dir->Type ());
//				break;
//			}
//			if (from_dir->Thru () != to_dir->Thru ()) {
//Warning ("Links %d and %d at Node %d have Thru Lane differences (%d vs %d)", from_dir->Link (), to_dir->Link (), node_ptr->Node (), from_dir->Thru (), to_dir->Thru ());
//				break;
//			}
//			if (from_dir->Speed () != to_dir->Speed ()) {
//Warning ("Links %d and %d at Node %d have Speeds differences (%d vs %d)", from_dir->Link (), to_dir->Link (), node_ptr->Node (), from_dir->Speed (), to_dir->Speed ());
//				break;
//			}
			if (!shape_flag) {
				diff = compass.Difference (to_dir->Out_Bearing (), from_dir->In_Bearing ());

				if (diff > 2 * straight_diff) break;

				if (diff > straight_diff && to_dir->Length () > short_length && 
					from_dir->Length () > short_length) break;
			}
		}
		if (dir > 0) {
			node_ptr->Type (CHANGE_POINT);
			continue;
		}

		//---- delete the node ----

		if (collapse_flag && !keep_node_flag) {
			if (Delete_Node (node_ptr)) {
				node_ptr->Type (DELETE_NODE);
				remove++;
			}
		} else {
			node_ptr->Type (CHANGE_POINT);
		}
	}
	End_Progress ();

	if (remove > 0) {
		Print (1);
		Write (1, "Number of Deleted Nodes = %d", remove);
		Show_Message (1);
	}
}
