//*********************************************************
//	Read_Sign.cpp - read the sign file
//*********************************************************

#include "Network_Service.hpp"

#include "Sign_File.hpp"

//---------------------------------------------------------
//	Read_Sign
//---------------------------------------------------------

void Network_Service::Read_Sign (void)
{
	Db_File *file = Network_Db_File (UNSIGNALIZED_NODE);

	//---- store the sign data ----

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

		if (Sign_Processing (file)) {
			if (!sign_data.Add ()) {
				Error ("Adding Record to the Sign Data List");
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = sign_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Traffic Control Signs = %d", num);
	}
	sign_data.Optimize ();

	if (num > 0) Network_Data_True (UNSIGNALIZED_NODE);
}

//---------------------------------------------------------
//	Sign_Processing
//---------------------------------------------------------

bool Network_Service::Sign_Processing (Db_File *fh)
{
	bool node_flag, link_flag;
	int lvalue, index;

	Link_Data *link_ptr = NULL;
	Sign_Data *sign_ptr = NULL;

	Sign_File *file = (Sign_File *) fh;

	node_flag = Network_Data_Flag (NODE);
	link_flag = Network_Data_Flag (LINK);

	//---- reserve memory ----

	if (sign_data.Max_Records () == 0) {
		if (!sign_data.Max_Records (file->Num_Records ())) goto mem_error;
	}

	//---- read and save the data ----
		
	sign_ptr = sign_data.New_Record ();
	if (sign_ptr == NULL) goto mem_error;

	//---- sign code ----

	sign_ptr->Sign (Sign_Code (file->Sign ()));

	//---- don't keep unsigned approaches ----

	if (sign_ptr->Sign () == NO_SIGN) return (false);

	//---- check/convert the link number ----

	lvalue = file->Link ();

	if (link_flag) {
		index = link_data.Get_Index (lvalue);

		if (index == 0) {
			return (Check_Message ("Unsignalized Link %d was Not Found in the Link file", lvalue));
		}
		link_ptr = link_data [index];

		sign_ptr->Link (Renumber () ? index : lvalue);
	} else {
		sign_ptr->Link (lvalue);
	}

	//---- determine the link direction ----

	lvalue = file->Node ();

	if (node_flag) {
		index = node_data.Get_Index (lvalue);

		if (index == 0) {
			return (Check_Message ("Unsignalized Node %d was Not Found in the Node file", lvalue));
		}
		if (link_flag) {
			if (!Renumber ()) {
				index = lvalue;
			}
			if (link_ptr->Anode () == index) {
				if (link_ptr->BA_Dir () == 0) goto dir_error;
				sign_ptr->Dir (1);
			} else if (link_ptr->Bnode () == index) {
				if (link_ptr->AB_Dir () == 0) goto dir_error;
				sign_ptr->Dir (0);
			} else {
				goto link_error;
			}
		} else {
			sign_ptr->Dir (0);
		}
	} else if (link_flag) {
		if (link_ptr->Anode () == lvalue) {
			if (link_ptr->BA_Dir () == 0) goto dir_error;
			sign_ptr->Dir (1);
		} else if (link_ptr->Bnode () == lvalue) {
			if (link_ptr->AB_Dir () == 0) goto dir_error;
			sign_ptr->Dir (0);
		} else {
			goto link_error;
		}
	} else {
		sign_ptr->Dir (0);
	}
	return (true);

mem_error:
	Error ("Insufficient Memory for Sign Data");
	return (false);

link_error:
	return (Check_Message ("Unsignalized Node %d is Not on Link %d", lvalue, link_ptr->Link ()));

dir_error:
	return (Check_Message ("No Lanes on Link %d to Unsignalized Node %d", link_ptr->Link (), lvalue));
}
