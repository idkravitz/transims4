//*********************************************************
//	Read_List.cpp - read link list file
//*********************************************************

#include "SmoothPlans.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Read_List
//---------------------------------------------------------

void SmoothPlans::Read_List (void)
{
	int link;

	Link_Data *link_ptr;

	Show_Message ("Reading %s -- Record", list_file.File_Type ());
	Set_Progress ();

	//---- read the count file ----

	while (list_file.Read ()) {
		Show_Progress ();

		Get_Integer (list_file.Record (), &link);

		if (link == 0) continue;

		link_ptr = link_data.Get (link);

		if (link_ptr == NULL) {
			Error ("Link %d was Not Found in the Link File", link);
		}
		if (link_ptr->AB_Dir () > 0) {
			if (!link_list.Add (link_ptr->AB_Dir ())) goto link_error;
		}
		if (link_ptr->BA_Dir () > 0) {
			if (!link_list.Add (link_ptr->BA_Dir ())) goto link_error;
		}
	}
	End_Progress ();

	list_file.Close ();
	return;

link_error:
	Error ("Adding to Link List");
}
