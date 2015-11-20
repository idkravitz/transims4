//*********************************************************
//	Read_Turn.cpp - read the turn prohibition file
//*********************************************************

#include "Network_Service.hpp"

#include "Turn_File.hpp"

//---------------------------------------------------------
//	Read_Turn
//---------------------------------------------------------

void Network_Service::Read_Turn (void)
{
	Turn_File *file = (Turn_File *) Network_Db_File (TURN_PROHIBITION);

	//---- store the turn prohibition data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress ();

	if (Check_Data () || Renumber ()) {
		if (!Network_File_Flag (NODE) && Network_Option_Flag (NODE) && file->Node_Based ()) {
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

		if (Turn_Processing (file)) {
			if (!turn_data.Add ()) {
				Error ("Adding Record to the Turn Data List");
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = turn_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Turn Prohibition Data Records = %d", num);
	}
	turn_data.Optimize ();

	if (num > 0) Network_Data_True (TURN_PROHIBITION);
}

//---------------------------------------------------------
//	Turn_Processing
//---------------------------------------------------------

bool Network_Service::Turn_Processing (Db_File *fh)
{
	bool node_flag, link_flag, ab_flag;
	int lvalue, num, node_num, link;
	char *str_ptr;

	Link_Data *link_ptr = NULL;	
	Turn_Data *turn_ptr = NULL;
	Time_Step time_step;

	Turn_File *file = (Turn_File *) fh;

	node_flag = Network_Data_Flag (NODE);
	link_flag = Network_Data_Flag (LINK);
	ab_flag = file->Node_Based ();

	//---- reserve memory ----

	if (turn_data.Max_Records () == 0) {
		if (!turn_data.Max_Records (file->Num_Records ())) goto mem_error;
	}

	//---- read and save the data ----

	turn_ptr = turn_data.New_Record ();
	if (turn_ptr == NULL) goto mem_error;

	node_num = file->Node ();

    if (node_num == 0) return (false);

	if (node_flag) {
		num = node_data.Get_Index (node_num);

		if (num == 0) {
			return (Check_Message ("Turn Prohibition Node %d was Not Found in the Node file", node_num));
		}
		if (!Renumber ()) {
			num = node_num;
		}
	} else {
		num = node_num;
	}

	if (ab_flag) {

		//---- node-based method ----

		AB_Data *ab_ptr;
		Dir_Data *dir_ptr;

		lvalue = file->In_Node ();

		ab_ptr = ab_key.Get (lvalue, node_num);

		if (ab_ptr == NULL) {
			return (Check_Message ("A Link between %d and %d was Not Found", lvalue, node_num));
		}
		dir_ptr = dir_data [ab_ptr->dir];

		turn_ptr->In_Link_Dir (dir_ptr->Link_Dir ());

		lvalue = file->Out_Node ();

		ab_ptr = ab_key.Get (node_num, lvalue);

		if (ab_ptr == NULL) {
			return (Check_Message ("A Link between %d and %d was Not Found", node_num, lvalue));
		}

		dir_ptr = dir_data [ab_ptr->dir];

		turn_ptr->Out_Link_Dir (dir_ptr->Link_Dir ());

	} else {

		//---- link-based method ----
		
		lvalue = file->In_Link ();

		if (link_flag) {
			link = link_data.Get_Index (lvalue);

			if (link == 0) {
				return (Check_Message ("Turn Prohibition In-Link %d was Not Found in the Link file", lvalue));
			}
			link_ptr = link_data [link];

			if (!Renumber ()) {
				link = lvalue;
			}
			turn_ptr->In_Link (link);
			
			if (link_ptr->Anode () == num) {
				turn_ptr->In_Dir (1);
			} else if (link_ptr->Bnode () == num) {
				turn_ptr->In_Dir (0);
			} else {
				return (Check_Message ("Turn Prohibition In-Link %d is Not Attached to Node %d", lvalue, node_num));
			}
		} else {
			turn_ptr->In_Link (lvalue);
		}

		//---- check/convert the out-link number ----
		
		lvalue = file->Out_Link ();

		if (link_flag) {
			link = link_data.Get_Index (lvalue);

			if (link == 0) {
				return (Check_Message ("Turn Prohibition Out-Link %d was Not Found in the Link file", lvalue));
			}
			link_ptr = link_data [link];

			if (!Renumber ()) {
				link = lvalue;
			}
			turn_ptr->Out_Link (link);

			if (link_ptr->Bnode () == num) {
				turn_ptr->Out_Dir (1);
			} else if (link_ptr->Anode () == num) {
				turn_ptr->Out_Dir (0);
			} else {
				return (Check_Message ("Turn Prohibition Out-Link %d is Not Attached to Node %d", lvalue, node_num));
			}
		} else {
			turn_ptr->Out_Link (lvalue);
		}
	}
	str_ptr = file->Start ();

	if (*str_ptr == '\0') {
		turn_ptr->Start (0);
	} else {
		turn_ptr->Start (Round (time_step.Step (str_ptr)));
	}
	str_ptr = file->End ();

	if (*str_ptr == '\0') {
		turn_ptr->End (Round (MIDNIGHT));
	} else {
		turn_ptr->End (Round (time_step.Step (str_ptr)));
	}
	str_ptr = file->Use ();

	if (*str_ptr == '\0') {
		str_ptr = "ANY";
	}
	turn_ptr->Use (Use_Code (str_ptr));

	turn_ptr->Penalty (file->Penalty ());

	return (true);

mem_error:
	Error ("Insufficient Memory for Turn Prohibition Records");
	return (false);
}
