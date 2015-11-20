//*********************************************************
//	Signal_Groups.cpp - set signal groups for the region
//*********************************************************

#include "Progression.hpp"

#include "Integer_Array.hpp"

#define MAX_LENGTH		3200

//---------------------------------------------------------
//	Signal_Groups
//---------------------------------------------------------

void Progression::Signal_Groups (void)
{
	int i, signal, nsignal, link_dir, node, group, link, dir, to_node, start_dir;
	int ttim, ttime, len, length, hrs, hours, volume, max_length;
	bool first, flag;

	Signal_Offset *offset_ptr;
	Green_Phase *green_ptr;
	Thru_Connect *thru_ptr;
	Sign_Data *sign_ptr;
	Signal_Data *signal_ptr;
	Link_Data *in_link_ptr, *out_link_ptr;
	TTime_Data *in_dir_ptr, *out_dir_ptr;
	Group_Link link_rec, *leg_ptr;
	Group_Data *group_ptr;
	Connect_Data *connect_ptr;

	Integer_Sort link_list;

	max_length = Round (MAX_LENGTH);

	//---- process each fixed timed signal ----

	Show_Message ("Building Link Groups -- Signal");
	Set_Progress (10);

	nsignal = signal_offset.Num_Records ();

	for (signal = 1; signal <= nsignal; signal++) {
		Show_Progress ();
		offset_ptr = signal_offset [signal];

		if (offset_ptr->Period () != period) continue;

		//---- look for movements that have not been processed ----

		for (green_ptr = offset_ptr->First_Phase (); green_ptr; green_ptr = offset_ptr->Next_Phase ()) {
			if (green_ptr->Movement () == THRU) break;
		}
		if (green_ptr == NULL) continue;

		//---- find the beginning of the link list ----

		length = 0;
		link_dir = start_dir = green_ptr->In_Link_Dir ();
		green_ptr->Movement (NO_TYPE);

		link_list.Reset ();

		while (link_dir > 0) {
			thru_ptr = thru_connect.Get (link_dir);
			if (thru_ptr == NULL) break;

			//---- check for a stop sign ----

			sign_ptr = sign_data.Get (thru_ptr->Thru_Link_Dir ());

			if (sign_ptr != NULL && sign_ptr->Sign () == STOP_SIGN) break;

			//---- check for link restrictions ----

			in_link_ptr = link_data.Get (thru_ptr->Thru_Link ());

			if (Use_Permission (in_link_ptr->Use (), RESTRICTED)) break;
			if (in_link_ptr->Type () == FREEWAY || in_link_ptr->Type () == EXPRESSWAY) break;

			length += in_link_ptr->Length ();
			if (length > max_length) break;

			//---- get the node number ----

			if (thru_ptr->Thru_Dir () == 1) {
				node = in_link_ptr->Anode ();
			} else {
				node = in_link_ptr->Bnode ();
			}

			//---- check for signals ----

			signal_ptr = signal_data.Get_LE (node, progression_time);

			if (signal_ptr != NULL && signal_ptr->Node () == node) {
				if (signal_ptr->Type () == ACTUATED) break;
				start_dir = thru_ptr->Thru_Link_Dir ();
			}
			link_list.Add (link_dir);

			//---- backup ----

			link_dir = thru_ptr->Thru_Link_Dir ();

			//---- check for circles ----

			if (link_list.Get_Index (link_dir) != 0) break;
		}

		//---- create a new group ----

		group = group_data.Num_Records () + 1;

		group_ptr = new Group_Data (group);

		in_link_ptr = NULL;
		in_dir_ptr = NULL;
		node = ttime = length = hours = 0;
		first = true;
		flag = false;

		link_dir = start_dir;

		link_list.Reset ();

		while (link_dir > 0) {
			link = (link_dir >> 1);
			dir = (link_dir & 1);

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

				//---- check for link restrictions ----

				if (Use_Permission (in_link_ptr->Use (), RESTRICTED)) break;
				if (in_link_ptr->Type () == FREEWAY || in_link_ptr->Type () == EXPRESSWAY) break;

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

					if (sign_ptr != NULL && sign_ptr->Sign () == STOP_SIGN) break;

					ttime += ttim;
					length += len;
					hours += hrs;

					if (length > max_length) break;

				} else {

					if (signal_ptr->Type () == ACTUATED) break;

					offset_ptr = signal_offset.Get (signal_data.Record_Index ());

					if (offset_ptr == NULL) {
						Error ("Signal at Node %d was Not Found", node);
					}

					//---- get the green phase ----

					green_ptr = offset_ptr->Get_Phase (in_dir_ptr->Link_Dir (), out_dir_ptr->Link_Dir ());

					if (green_ptr == NULL) break;

					//---- mark the movement as used ----

					green_ptr->Movement (NO_TYPE);

					//---- add the link record ----

					if (first) {
						first = false;
						ttime = length = hours = 0;
					} else {
						ttime += ttim;
						length += len;
						hours += hrs;
						flag = true;
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

			//---- find the next thru connection ----
			
			connect_ptr = connect_data.Get_GE (link_dir, 0);

			if (connect_ptr == NULL || connect_ptr->In_Link_Dir () != link_dir) break;

			if (delay_flag) {
				while (connect_ptr->Type () != THRU) {
					connect_ptr = connect_data.Next_Key ();
					if (connect_ptr == NULL || connect_ptr->In_Link_Dir () != link_dir) break;
				}
				if (connect_ptr == NULL || connect_ptr->In_Link_Dir () != link_dir) break;			
			}

			link_list.Add (link_dir);

			link_dir = connect_ptr->Out_Link_Dir ();

			//---- check for circles ----

			if (link_list.Get_Index (link_dir) != 0) break;
		}
		if (flag) {

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
			
			//---- market the signal records ----

			for (leg_ptr = group_ptr->First_Link (); leg_ptr; leg_ptr = group_ptr->Next_Link ()) {
				offset_ptr = signal_offset [leg_ptr->Signal ()];

				if (!offset_ptr->Add_Group (group)) {
					Error ("Adding Signal Group Code");
				}
			}
			if (!group_data.Add (group_ptr)) {
				Error ("Adding Group Data");
			}
		} else {
			delete group_ptr;
		}
	}
	End_Progress ();
	return;

link_error:
	Error ("Link %d on Link Equivalence Group %d was Not Found", link, group);
}
