//*********************************************************
//	Read_Connect.cpp - read the lane connectivity file
//*********************************************************

#include "Network_Service.hpp"

#include "Connect_File.hpp"

//---------------------------------------------------------
//	Read_Connect
//---------------------------------------------------------

void Network_Service::Read_Connect (void)
{
	Connect_Data *connect_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;

	bool dir_flag;
	int in_low, in_high, out_low, out_high, lane, dir, pocket, in, out;

	Db_File *file = Network_Db_File (LANE_CONNECTIVITY);

	//---- store the lane connectivity data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress ();

	if (Check_Data () || Renumber ()) { 
		if (!Network_File_Flag (NODE) && Network_Option_Flag (NODE)) {
			Error (file_error, "Node", file->File_ID ());
		}
		if (!Network_File_Flag (LINK) && Network_Option_Flag (LINK)) {
			Error (file_error, "Link", file->File_ID ());
		}
	}
	file->Num_Records ();
	First_Record (true);

	while (file->Read ()) {
		Show_Progress ();

		if (Connectivity_Processing (file)) {
			if (!connect_data.Add ()) {
				Error ("Adding Record to the Connectivity Data List");
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = connect_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Lane Connectivity Data Records = %d", num);
	}
	connect_data.Optimize ();

	if (num > 0) Network_Data_True (LANE_CONNECTIVITY);

	//---- adjust thru lanes ----
		
	dir_flag = Network_Data_Flag (LINK);

	for (connect_ptr = connect_data.First (); connect_ptr != NULL; connect_ptr = connect_data.Next ()) {
		if (connect_ptr->In_Thru_Low () > 0 && connect_ptr->Out_Thru_Low () > 0) {
			in_low = in_high = connect_ptr->In_Thru_Low ();
			out_low = out_high = connect_ptr->Out_Thru_Low ();

			in = connect_ptr->In_Lane_Low ();
			out = connect_ptr->Out_Lane_Low ();

			while (in_low > in && out_low > out) {
				in_low--; 
				out_low--;
			}
			in = connect_ptr->In_Lane_High ();
			out = connect_ptr->Out_Lane_High ();
			
			while (in_high < in && out_high < out) {
				in_high++;
				out_high++;
			}

		} else {

			in_low = connect_ptr->In_Lane_Low ();
			in_high = connect_ptr->In_Lane_High ();
			out_low = connect_ptr->Out_Lane_Low ();
			out_high = connect_ptr->Out_Lane_High ();

			lane = in_high - in_low + out_low - out_high;

			if (lane > 0) { 	//---- too many input lanes ----

				if (dir_flag) {
					if (Renumber ()) {
						link_ptr = link_data [connect_ptr->In_Link ()];
					} else {
						link_ptr = link_data.Get (connect_ptr->In_Link ());
					}
					if (connect_ptr->In_Dir ()) {
						dir = link_ptr->BA_Dir ();
					} else {
						dir = link_ptr->AB_Dir ();
					}
					if (dir == 0) continue;

					dir_ptr = dir_data [dir];

					//---- remove left pocket lanes ----

					pocket = dir_ptr->Left ();

					while (in_low <= pocket && lane > 0) {
						in_low++;
						lane--;
					}

					//---- remove right pocket lanes ----

					if (lane > 0) {
						pocket += dir_ptr->Thru ();

						while (in_high > pocket && lane > 0) {
							in_high--;
							lane--;
						}
					}
				}

				//---- force outside lanes to merge ----

				while (lane > 0) {

					//---- force left lane to merge ----

					in_low++;
					lane--;

					//---- force right lane to merge ----

					if (lane > 0) {
						in_high--;
						lane--;
					}
				}

			} else if (lane < 0) {		//---- too many output lanes ----

				if (dir_flag) {
					if (Renumber ()) {
						link_ptr = link_data [connect_ptr->Out_Link ()];
					} else {
						link_ptr = link_data.Get (connect_ptr->Out_Link ());
					}
					if (connect_ptr->Out_Dir ()) {
						dir = link_ptr->BA_Dir ();
					} else {
						dir = link_ptr->AB_Dir ();
					}
					if (dir == 0) continue;

					dir_ptr = dir_data [dir];

					//---- remove left pocket lanes ----

					pocket = dir_ptr->Left ();

					while (out_low <= pocket && lane < 0) {
						out_low++;
						lane++;
					}

					//---- remove right pocket lanes ----

					if (lane < 0) {
						pocket += dir_ptr->Thru ();

						while (out_high > pocket && lane < 0) {
							out_high--;
							lane++;
						}
					}
				}

				//---- outside lanes diverge ----

				while (lane < 0) {

					//---- left lane diverge ----

					out_low++;
					lane++;

					//---- right lane diverge ----

					if (lane < 0) {
						out_high--;
						lane++;
					}
				}
			}
		}

		//---- set the thru lane ----

		connect_ptr->In_Thru_Low (in_low);
		connect_ptr->In_Thru_High (in_high);

		connect_ptr->Out_Thru_Low (out_low);
		connect_ptr->Out_Thru_High (out_high);
	}
}

//---------------------------------------------------------
//	Connectivity_Processing
//---------------------------------------------------------

bool Network_Service::Connectivity_Processing (Db_File *fh)
{
	bool node_flag, link_flag;

	char lane_error [] = "Connectivity Lanes %d-%d are Out of Range (1..%d) for Link %d";
	char link_error [] = "Lane Connectivity Link %d was Not Found in the Link file";
	char link_node [] = "Lane Connectivity Link %d is Not Attached to Node %d";
	char dir_error [] = "Lane Connectivity Link %d is Not Available in the %s Direction";

	Connect_Data *connect_ptr;

	Connect_File *file = (Connect_File *) fh;

	node_flag = Network_Data_Flag (NODE);
	link_flag = Network_Data_Flag (LINK);

	//---- reserve memory ----

	if (connect_data.Max_Records () == 0) {
		if (!connect_data.Max_Records (file->Num_Records ())) goto mem_error;
	}

	//---- read and save the data ----

	int num, node_num, link, dir, in_link, out_link, max_in, max_out, in_bear, out_bear;
	int in_low, in_high, out_low, out_high, type, penalty, speed, capacity, change;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
		
	connect_ptr = connect_data.New_Record (true);
	if (connect_ptr == NULL) goto mem_error;

	node_num = file->Node ();

    if (node_num == 0) return (false);

	if (node_flag) {
		num = node_data.Get_Index (node_num);

		if (num == 0) {
			return (Check_Message ("Lane Connectivity Node %d was Not Found in the Node file", node_num));
		}
		if (!Renumber ()) {
			num = node_num;
		}
	} else {
		num = node_num;
	}

	//---- check/convert the in-link number ----
	
	in_link = file->In_Link ();
	dir = 0;

	if (link_flag) {
		link = link_data.Get_Index (in_link);

		if (link == 0) {
			return (Check_Message (link_error, in_link));
		}
		link_ptr = link_data [link];

		if (!Renumber ()) {
			link = in_link;
		}
		connect_ptr->In_Link (link);
		
		if (link_ptr->Anode () == num) {
			connect_ptr->In_Dir (1);
			dir = link_ptr->BA_Dir ();
		} else if (link_ptr->Bnode () == num) {
			connect_ptr->In_Dir (0);
			dir = link_ptr->AB_Dir ();
		} else {
			return (Check_Message (link_node, in_link, node_num));
		}
		if (dir == 0) {
			return (Check_Message (dir_error, in_link, ((connect_ptr->In_Dir () == 0) ? "AB" : "BA")));
		}
		dir_ptr = dir_data [dir];
		max_in = dir_ptr->Thru () + dir_ptr->Left () + dir_ptr->Right ();
		in_bear = dir_ptr->Out_Bearing ();
	} else {
		connect_ptr->In_Link (in_link);
		max_in = in_bear = 0;
	}

	//---- check/convert the out-link number ----
	
	out_link = file->Out_Link ();

	if (link_flag) {
        link = link_data.Get_Index (out_link);

		if (link == 0) {
			return (Check_Message (link_error, out_link));
		}
		link_ptr = link_data [link];

		if (!Renumber ()) {
			link = out_link;
		}
		connect_ptr->Out_Link (link);

		if (link_ptr->Bnode () == num) {
			connect_ptr->Out_Dir (1);
			dir = link_ptr->BA_Dir ();
        } else if (link_ptr->Anode () == num) {
			connect_ptr->Out_Dir (0);
			dir = link_ptr->AB_Dir ();
		} else {
			return (Check_Message (link_node, out_link, node_num));
		}
		if (dir == 0) {
			return (Check_Message (dir_error, out_link, ((connect_ptr->Out_Dir () == 0) ? "AB" : "BA")));
		}
		dir_ptr = dir_data [dir];
		max_out = dir_ptr->Thru () + dir_ptr->Left () + dir_ptr->Right ();
		out_bear = dir_ptr->In_Bearing ();
	} else {
		connect_ptr->Out_Link (out_link);
		max_out = out_bear = 0;
	}

	//---- optional fields ----

	in_low = file->In_Low ();
	in_high = file->In_High ();
	
	if (in_low > in_high || (max_in > 0 && in_high > max_in)) {
		return (Check_Message (lane_error, in_low, in_high, max_in, in_link));
	}

	out_low = file->Out_Low ();
	out_high = file->Out_High ();

	if (out_low > out_high || (max_out > 0 && out_high > max_out)) {
		return (Check_Message (lane_error, out_low, out_high, max_out, out_link));
	}	
	if (file->Type_Flag ()) {
		type = Connect_Code (file->Type ());
	} else {
		type = 0;
	}
	penalty = file->Penalty ();
	speed = Round (file->Speed ());
	capacity = file->Capacity ();

	//---- check for duplicate records ----

	if (connect_data.Get_Index (connect_ptr) == 0) {
		if (in_low > 0) {
			connect_ptr->In_Lane_Low (in_low);
			connect_ptr->In_Lane_High (in_high);
			connect_ptr->In_Thru_Low (in_low);
		}
		if (out_low > 0) {
			connect_ptr->Out_Lane_Low (out_low);
			connect_ptr->Out_Lane_High (out_high);
			connect_ptr->Out_Thru_Low (out_low);
		}
		if (type == 0 && compass.Num_Points () > 0) {
			change = compass.Change (in_bear, out_bear) * 360 / compass.Num_Points ();

			if (change >= -45 && change <= 45) {
				type = THRU;
			} else if (change < -165 || change > 165) {
				type = UTURN;
			} else if (change < 0) {
				type = LEFT;
			} else {
				type = RIGHT;
			}
		}
		connect_ptr->Type (type);
		connect_ptr->Penalty (penalty);
		connect_ptr->Speed (speed);
		connect_ptr->Capacity (capacity);
		return (true);

	} else {

		//---- update optional fields ----

		connect_ptr = connect_data.Record ();

		if (in_low > 0) {
			if (in_low < connect_ptr->In_Lane_Low ()) {
				connect_ptr->In_Lane_Low (in_low);
			} else if (in_high > connect_ptr->In_Lane_High ()) {
				connect_ptr->In_Lane_High (in_high);
			}
		}
		if (out_low > 0) {
			if (out_low < connect_ptr->Out_Lane_Low ()) {
				connect_ptr->Out_Lane_Low (out_low);
			} else if (out_high > connect_ptr->Out_Lane_High ()) {
				connect_ptr->Out_Lane_High (out_high);
			}
		}
		if (abs (connect_ptr->In_Thru_Low ()) == in_low) {
			connect_ptr->In_Thru_Low (-in_low);
		} else if (abs (connect_ptr->Out_Thru_Low ()) == out_low) {
			connect_ptr->Out_Thru_Low (-out_low);
		} else if (connect_ptr->In_Thru_Low () <= 0 || connect_ptr->Out_Thru_Low () <= 0) {
			connect_ptr->In_Thru_Low (in_low);
			connect_ptr->Out_Thru_Low (out_low);
		}
		if (type > connect_ptr->Type ()) {
			connect_ptr->Type (type);
		}
		if (penalty > 0 && penalty > connect_ptr->Penalty ()) {
			connect_ptr->Penalty (penalty);
		}
		if (speed > 0 && speed > connect_ptr->Speed ()) {
			connect_ptr->Speed (speed);
		}
		connect_ptr->Capacity (connect_ptr->Capacity () + capacity);
	}
	return (false);

mem_error:
	Error ("Insufficient Memory for Lane Connectivity Records");
	return (false);
}
