//*********************************************************
//	Write_Results.cpp - write convergence results
//*********************************************************

#include "AdjustPlans.hpp"

#include "Time_Step.hpp"

//---------------------------------------------------------
//	Write_Results
//---------------------------------------------------------

void AdjustPlans::Write_Results (void)
{
	int anode, bnode;

	Node_Data *node_ptr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Count_Data *count_ptr;

	Time_Step time_data (1, Time_Step::CLOCK24);

	Show_Message ("Writing %s -- Record", converge_file.File_Type ());
	Set_Progress (1000);

	//---- write the count data ----

	for (count_ptr = count_data.First (); count_ptr; count_ptr = count_data.Next ()) {
		Show_Progress ();

		if (equiv_flag) {
			converge_file.Put_Field ("GROUP", count_ptr->Dir_Index ());
		} else {
			dir_ptr = dir_data [count_ptr->Dir_Index ()];

			link_ptr = link_data [dir_ptr->Link ()];

			converge_file.Put_Field ("LINK", link_ptr->Link ());
			converge_file.Put_Field ("DIR", dir_ptr->Dir ());

			if (dir_ptr->Dir () == 0) {
				anode = link_ptr->Anode ();
				bnode = link_ptr->Bnode ();
			} else {
				anode = link_ptr->Bnode ();
				bnode = link_ptr->Anode ();
			}

			node_ptr = node_data [anode];

			converge_file.Put_Field ("ANODE", node_ptr->Node ());

			node_ptr = node_data [bnode];

			converge_file.Put_Field ("BNODE", node_ptr->Node ());
		}

		converge_file.Put_Field ("START", time_data.Format_Time (Resolve (count_ptr->Start_Time ())));

		converge_file.Put_Field ("END", time_data.Format_Time (Resolve (count_ptr->End_Time ())));

		converge_file.Put_Field ("COUNT", count_ptr->Count ());

		converge_file.Put_Field ("VOLUME", count_ptr->Volume ());

		converge_file.Put_Field ("DIFFERENCE", count_ptr->Difference ());

		if (!converge_file.Write ()) {
			Error ("Writing the Convergence File");
		}
	}
	End_Progress ();

	converge_file.Close ();
}
