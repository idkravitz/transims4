//*********************************************************
//	Connection_List.cpp - build connection lists
//*********************************************************

#include "IntControl.hpp"

//---------------------------------------------------------
//	Connection_List
//---------------------------------------------------------

void IntControl::Connection_List (void)
{
	int dir, change;
	bool left_flag;
	
	Node_List *node_ptr;
	Dir_Connect *to_dir, *from_dir;
	Connect_Data *connect_ptr;
	Connection connection_rec;
	
	int thru_diff = compass.Num_Points () * 50 / 360;

	Show_Message ("Building Connection Lists - Record");
	Set_Progress ();

	//---- build the connection lists ----

	for (node_ptr = node_data.First(); node_ptr; node_ptr = node_data.Next ()) {
		Show_Progress ();

		if (!node_ptr->Type ()) continue;

		//---- set connections for each entry link ----

		left_flag = false;

		for (dir = node_ptr->To_Dir (); dir; dir = to_dir->To_List ()) {
			to_dir = dir_data [dir];

			for (dir = node_ptr->From_Dir (); dir; dir = from_dir->From_List ()) {
				from_dir = dir_data [dir];

				connect_ptr = connect_data.Get (to_dir->Link_Dir (), from_dir->Link_Dir ());
				if (connect_ptr == NULL) continue;

				change = compass.Change (to_dir->Out_Bearing (), from_dir->In_Bearing ());

				connection_rec.Dir (dir);
				connection_rec.Facility (from_dir->Type ());
				connection_rec.Change (change);
				connection_rec.Index (connect_data.Record_Index ());
				connection_rec.Type (connect_ptr->Type ());

				if (connect_ptr->Type () == LEFT || connect_ptr->Type () == UTURN) left_flag = true;

				if (!to_dir->To_Connection (&connection_rec)) goto connect_error;
			}

			//---- check left turn pocket and left movements ----

			if (to_dir->Left_Turn () && !left_flag) {
				Warning ("Signal Node %d Link %d has Left Turn Issues", 
					node_ptr->Node (), to_dir->Link ());
				to_dir->Left_Turn (0);
			}
		}

		//---- set connections for each exit link ----

		for (dir = node_ptr->From_Dir (); dir; dir = from_dir->From_List ()) {
			from_dir = dir_data [dir];

			for (dir = node_ptr->To_Dir (); dir; dir = to_dir->To_List ()) {
				to_dir = dir_data [dir];

				connect_ptr = connect_data.Get (to_dir->Link_Dir (), from_dir->Link_Dir ());
				if (connect_ptr == NULL) continue;

				change = compass.Change (to_dir->Out_Bearing (), from_dir->In_Bearing ());

				connection_rec.Dir (dir);
				connection_rec.Facility (to_dir->Type ());
				connection_rec.Change (change);
				connection_rec.Index (connect_data.Record_Index ());
				connection_rec.Type (connect_ptr->Type ());

				if (!from_dir->From_Connection (&connection_rec)) goto connect_error;
			}
		}
	}
	End_Progress ();
	return;

connect_error:
	Error ("Adding Connection Record");
}
