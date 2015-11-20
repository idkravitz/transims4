//*********************************************************
//	Read_Timing.cpp - read the timing plan file
//*********************************************************

#include "Network_Service.hpp"

#include "Timing_File.hpp"

//---------------------------------------------------------
//	Read_Timing
//---------------------------------------------------------

void Network_Service::Read_Timing (void)
{
	Db_File *file = Network_Db_File (TIMING_PLAN);

	//---- store the timing plan data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress ();
	
	file->Num_Records ();
	First_Record (true);

	while (file->Read ()) {
		Show_Progress ();

		if (Timing_Processing (file)) {
			if (!timing_data.Add ()) {
				Error ("Adding Record to the Timing Data List");
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = timing_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Timing Plan Data Records = %d", num);
	}
	timing_data.Optimize ();

	if (num > 0) Network_Data_True (TIMING_PLAN);
	return;
}

//---------------------------------------------------------
//	Timing_Processing
//---------------------------------------------------------

bool Network_Service::Timing_Processing (Db_File *fh)
{
	int lvalue;
	char *svalue;	
	
	Timing_Data *timing_ptr = NULL;

	Timing_File *file = (Timing_File *) fh;

	//---- reserve memory ----

	if (timing_data.Max_Records () == 0) {
		if (!timing_data.Max_Records (file->Num_Records ())) goto mem_error;
	}

	//---- read and save the data ----

	lvalue = file->Timing ();
	if (lvalue == 0) return (false);

	//---- store the plan id ----

	timing_ptr = timing_data.New_Record ();
	if (timing_ptr == NULL) goto mem_error;

	timing_ptr->Timing (lvalue);
	timing_ptr->Phase (file->Phase ());
	timing_ptr->Min_Green (file->Min_Green ());
	timing_ptr->Max_Green (file->Max_Green ());
	timing_ptr->Ext_Green (file->Ext_Green ());
	timing_ptr->Yellow (file->Yellow ());
	timing_ptr->Red_Clear (file->Red_Clear ());
	timing_ptr->Ring (file->Ring ());
	timing_ptr->Group (file->Group ());

	//---- process the next phases ----

	svalue = file->Next_Phase ();

	if (*svalue == '\0') {
		return (Check_Message ("Timing Plan %d Next Phase string was Not Found", timing_ptr->Timing ()));
	}
	timing_ptr->Next_Phase (atoi (svalue));

	//---- check the ring and group codes ----

	lvalue = timing_ptr->Ring ();

	if (lvalue < 0 || lvalue > MAX_RING) {
		return (Check_Message ("Timing Plan %d Ring %d is Out of Range (0..%d)", timing_ptr->Timing (), lvalue, MAX_RING));
	}

	lvalue = timing_ptr->Group ();

	if (lvalue < 0 || lvalue > 8) {
		return (Check_Message ("Timing Plan %d Group %d is Out of Range (0..8)", timing_ptr->Timing (), lvalue));
	}
	return (true);

mem_error:
	Error ("Insufficient Memory for Timing Plan Data");
	return (false);
}
