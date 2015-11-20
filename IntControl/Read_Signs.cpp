//*********************************************************
//	Read_Signs.cpp - read the sign file
//*********************************************************

#include "IntControl.hpp"

//---------------------------------------------------------
//	Read_Signs
//---------------------------------------------------------

void IntControl::Read_Signs (void)
{
	int lvalue, index, num;
	Link_Data *link_ptr;
	Sign_Data *sign_ptr;

	//---- store the sign data ----

	Show_Message ("Reading %s -- Record", sign_file.File_Type ());
	Set_Progress ();

	if (!input_sign.Max_Records (sign_file.Num_Records ())) {
		Error ("Insufficient Memory for Input Sign Data");
	}

	while (sign_file.Read ()) {
		Show_Progress ();

		//---- read and save the data ----
			
		sign_ptr = input_sign.New_Record (true);
		if (sign_ptr == NULL) {
			Error ("Insufficient Memory for Input Sign Data");
		}

		//---- sign code ----

		sign_ptr->Sign (Sign_Code (sign_file.Sign ()));

		//---- don't keep unsigned approaches ----

		if (sign_ptr->Sign () == NO_SIGN) continue;

		//---- check/convert the link number ----

		lvalue = sign_file.Link ();
		sign_ptr->Link (lvalue);

		index = link_data.Get_Index (lvalue);

		if (index == 0) {
			Warning ("Unsignalized Link %d was Not Found in the Link file", lvalue);
			continue;
		}
		link_ptr = link_data [index];

		//---- determine the link direction ----

		lvalue = sign_file.Node ();
		index = node_data.Get_Index (lvalue);

		if (index == 0) {
			Warning ("Unsignalized Node %d was Not Found in the Node file", lvalue);
			continue;
		}
		if (link_ptr->Anode () == lvalue) {
			if (link_ptr->BA_Dir () == 0) {
				Warning ("No Lanes on Link %d to Unsignalized Node %d", link_ptr->Link (), lvalue);
				continue;
			}
			sign_ptr->Dir (1);
		} else if (link_ptr->Bnode () == lvalue) {
			if (link_ptr->AB_Dir () == 0) {
				Warning ("No Lanes on Link %d to Unsignalized Node %d", link_ptr->Link (), lvalue);
				continue;
			}
			sign_ptr->Dir (0);
		} else {
			Warning ("Unsignalized Node %d is Not on Link %d", lvalue, link_ptr->Link ());
			continue;
		}
		if (!input_sign.Add ()) {
			Error ("Adding Record to the Sign Data List");
		}
	}
	End_Progress ();

	sign_file.Close ();

	Print (2, "Number of %s Records = %d", sign_file.File_Type (), Progress_Count ());

	num = input_sign.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Traffic Control Signs = %d", num);
	}
}
