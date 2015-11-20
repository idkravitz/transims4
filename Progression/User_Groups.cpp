//*********************************************************
//	User_Groups.cpp - set groups from link equivalence
//*********************************************************

#include "Progression.hpp"

//---------------------------------------------------------
//	User_Groups
//---------------------------------------------------------

void Progression::User_Groups (void)
{
	int i, num, group, link, dir, node, to_node;
	int ttim, ttime, len, length, hrs, hours, volume;
	bool first;

	Link_Data *in_link_ptr, *out_link_ptr;
	TTime_Data *in_dir_ptr, *out_dir_ptr;
	Signal_Data *signal_ptr;
	Sign_Data *sign_ptr;
	Group_Link link_rec;
	Group_Data *group_ptr;
	Signal_Offset *offset_ptr;
	Green_Phase *green_ptr;
	Integer_List *list;

	Show_Message ("Building Link Groups -- Group");
	Set_Progress (1);

	for (group = link_equiv.First_Group (); group != 0 ; group = link_equiv.Next_Group ()) {
		Show_Progress ();

		list = link_equiv.Group_List (group);

		if (list == NULL) continue;

		group_ptr = new Group_Data (group);
		num = group_data.Num_Records () + 1;

		in_link_ptr = NULL;
		in_dir_ptr = NULL;
		node = ttime = length = hours = 0;
		first = true;

		for (link = list->First (); link != 0; link = list->Next ()) {
			if (link < 0) {
				link = -link;
				dir = 1;
			} else {
				dir = 0;
			}
			out_link_ptr = link_data.Get (link);

			if (out_link_ptr == NULL) goto link_error;

			if (dir == 0) {
				to_node = out_link_ptr->Bnode ();
				dir = out_link_ptr->AB_Dir ();
			} else {
				to_node = out_link_ptr->Anode ();
				dir = out_link_ptr->BA_Dir ();
			}
			out_dir_ptr = ttime_data [dir];

			if (out_dir_ptr == NULL) goto link_error;

			if (in_dir_ptr != NULL) {

				//---- get the travel time ----

				if (speed_flag) {
					ttim = (int) (UnRound (in_link_ptr->Length ()) / progression_speed + 0.5);
				} else {
					ttim = Resolve (in_dir_ptr->TTime (mid_period));
				}
				if (ttim < 1) ttim = 1;

				len = in_link_ptr->Length ();

				volume = in_dir_ptr->Volume (mid_period);
				if (volume == 0) volume = 1;

				hrs = volume * ttim;

				//---- get the node controls ----

				signal_ptr = signal_data.Get_LE (node, progression_time);

				if (signal_ptr == NULL || signal_ptr->Node () != node) {
					sign_ptr = sign_data.Get (in_dir_ptr->Link_Dir ());

					if (sign_ptr != NULL && sign_ptr->Sign () == STOP_SIGN) {
						Error ("Link %d on Link Group %d has a Stop Sign", in_link_ptr->Link (), group);
					}
					ttime += ttim;
					length += len;
					hours += hrs;

				} else if (signal_ptr->Type () == ACTUATED) {

					Error ("Link %d on Link Group %d has an Actuated Signal", in_link_ptr->Link (), group);

				} else {

					offset_ptr = signal_offset.Get (signal_data.Record_Index ());

					if (offset_ptr == NULL) {
						Error ("Signal at Node %d was Not Found", node);
					}
					if (!offset_ptr->Add_Group (num)) {
						Error ("Adding Group to Signal %d", node);
					}

					//---- get the green phase ----

					green_ptr = offset_ptr->Get_Phase (in_dir_ptr->Link_Dir (), out_dir_ptr->Link_Dir ());

					if (green_ptr == NULL) {
						Error ("A Green Phase between Links %d and %d was Not Found", in_dir_ptr->Link (), out_dir_ptr->Link ());
					}

					//---- add the link record ----

					if (first) {
						first = false;
						ttime = length = hours = 0;
					} else {
						ttime += ttim;
						length += len;
						hours += hrs;
					}
					link_rec.Node (node);
					link_rec.Green (green_ptr->Green_Time ());
					link_rec.Offset (green_ptr->Phase_Offset ());
					link_rec.Signal (signal_offset.Record_Index ());
					link_rec.TTime (ttime);
					link_rec.Length (length);
					link_rec.VHT (hours);

					if (!group_ptr->Add_Link (&link_rec)) {
						Error ("Adding Link to Group %d", group);
					}
					ttime = length = hours = 0;
				}

				//---- link shape points ----

				if (arcview_flag) {
					dir = in_dir_ptr->Dir ();

					Link_Shape (in_link_ptr, dir, &points, ((dir) ? -1.0 : 0.0), -1.0, link_offset);

					for (i=1; i <= points.Num_Points (); i++) {
						if (!group_ptr->points.Add (points [i])) {
							Error ("Insufficient Memory for Shape Points");
						}
					}
				}
			}
			in_link_ptr = out_link_ptr;
			in_dir_ptr = out_dir_ptr;
			node = to_node;
		}

		//---- draw the last link ----

		if (arcview_flag) {
			dir = in_dir_ptr->Dir ();

			Link_Shape (in_link_ptr, dir, &points, ((dir) ? -1.0 : 0.0), -1.0, link_offset);

			for (i=1; i <= points.Num_Points (); i++) {
				if (!group_ptr->points.Add (points [i])) {
					Error ("Insufficient Memory for Shape Points");
				}
			}
		}
		if (!group_data.Add (group_ptr)) {
			Error ("Adding Group Data");
		}
	}
	End_Progress ();
	return;

link_error:
	Error ("Link %d on Link Group %d was Not Found", link, group);
}
