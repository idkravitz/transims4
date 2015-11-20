//*********************************************************
//	Read_Link.cpp - read the previous link file
//*********************************************************

#include "LinkDelay.hpp"

//---------------------------------------------------------
//	Read_Previous_Link
//---------------------------------------------------------

void LinkDelay::Read_Previous_Link (void)
{

	//---- store the link data ----

	Show_Message ("Reading %s -- Record", previous_link.File_Type ());
	Set_Progress ();

	//---- reserve memory ----

	previous_data.Max_Records (previous_link.Num_Records ());

	while (previous_link.Read ()) {
		Show_Progress ();

		int lvalue;
		Link_Data *link_ptr;

		//---- read and save the data ----

		link_ptr = previous_data.New_Record ();
		if (link_ptr == NULL) continue;

		lvalue = previous_link.Link ();
		if (lvalue == 0) continue;

		link_ptr->Link (lvalue);
		link_ptr->Anode (previous_link.Anode ());
		link_ptr->Bnode (previous_link.Bnode ());

		previous_data.Add ();
	}
	End_Progress ();
	previous_link.Close ();

	Print (2, "Number of %s Records = %d", previous_link.File_Type (), Progress_Count ());

	previous_data.Optimize ();
}

