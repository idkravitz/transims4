//*********************************************************
//	Read_HHList.cpp - Read the household list files
//*********************************************************

#include "PlanPrep.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Read_HHList
//---------------------------------------------------------

void PlanPrep::Read_HHList (void)
{
	int hhold;

	Partition *part_ptr, part_rec;

	for (nout=0; ; nout++) {

		//---- open the household list plans ----

		if (!hhlist_file.Open (nout)) break;

		if (hhlist_file.Extend ()) {
			Show_Message ("Reading %s %s -- Record", hhlist_file.File_Type (), hhlist_file.Extension ());
		} else {
			Show_Message ("Reading %s -- Record", hhlist_file.File_Type ());
		}
		Set_Progress (100000);

		//---- read the household list file ----

		while (hhlist_file.Read ()) {
			Show_Progress ();

			Get_Integer (hhlist_file.Record (), &hhold);

			part_ptr = (Partition *) partition.Get (&hhold);

			if (part_ptr == NULL) {
				part_rec.traveler = hhold;
				part_rec.partition = nout;

				if (!partition.Add (&part_rec)) {
					Error ("Adding Partition Record");
				}
			} else if (part_ptr->partition != nout) {
				Error ("Household %d is in Partition %d and %d", 
					hhold, part_ptr->partition, nout);
			}
		}
		End_Progress ();

		hhlist_file.Close ();
	}
	if (nout == 0) {
		File_Error ("No Household List Files were Found", hhlist_file.Filename ());
	}

	//---- open the output plan files ----

	plan_file = new Plan_File * [nout];

	if (plan_file == NULL) {
		Error ("Insufficient Memory to Plan Partitioning");	
	}
	for (int i=0; i < nout; i++) {
		plan_file [i] = new Plan_File (CREATE);

		if (plan_file [i] == NULL) {
			Error ("Insufficient Memory for Plan File");
		}
		plan_file [i]->Filename (output_name);
		plan_file [i]->Plan_Sort (output_plans.Plan_Sort ());
		plan_file [i]->Extend (hhlist_file.Extend ());
		plan_file [i]->File_Format (output_plans.File_Format ());
		plan_file [i]->File_Type (output_plans.File_Type ());
		plan_file [i]->Node_Based_Flag (type_flag);

		if (!plan_file [i]->Open (i)) {
			File_Error ("Opening Output Plan File", plan_file [i]->Filename ());
		}
	}
}
