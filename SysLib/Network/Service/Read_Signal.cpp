//*********************************************************
//	Read_Signal.cpp - read the signal file
//*********************************************************

#include "Network_Service.hpp"

#include "Signal_File.hpp"

//---------------------------------------------------------
//	Read_Signal
//---------------------------------------------------------

void Network_Service::Read_Signal (void)
{
	Db_File *file = Network_Db_File (SIGNALIZED_NODE);

	//---- store the signal data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress ();

	if (Check_Data () || Renumber ()) { 
		if (!Network_File_Flag (NODE) && Network_Option_Flag (NODE)) {
			Error (file_error, "Node", file->File_ID ());
		}
		if (!Network_File_Flag (TIMING_PLAN) && Network_Option_Flag (TIMING_PLAN)) {
			Error (file_error, "Timing Plan", file->File_ID ());
		}
	}
	file->Num_Records ();
	First_Record (true);

	while (file->Read ()) {
		Show_Progress ();

		if (Signal_Processing (file)) {
			if (!signal_data.Add ()) {
				Error ("Adding Record to the Signal Data List");
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = signal_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Signalized Node Data Records = %d", num);
	}
	signal_data.Optimize ();

	if (num > 0) {
		Network_Data_True (SIGNALIZED_NODE);

		//---- set the end time ----

		Signal_Data *signal_ptr, *last_ptr;
		last_ptr = NULL;

		for (signal_ptr = signal_data.First_Key (); signal_ptr; signal_ptr = signal_data.Next_Key ()) {
			if (last_ptr != NULL) {
				if (last_ptr->Node () == signal_ptr->Node ()) {
					last_ptr->End (signal_ptr->Start () - 1);
				} else {
					last_ptr->End (MIDNIGHT);
				}
			}
			last_ptr = signal_ptr;
		}
		if (last_ptr != NULL) {
			last_ptr->End (MIDNIGHT);
		}
	}
}

//---------------------------------------------------------
//	Signal_Processing
//---------------------------------------------------------

bool Network_Service::Signal_Processing (Db_File *fh)
{
	bool node_flag, timing_flag, coord_flag;
	char *svalue;
	int lvalue, index;

	Time_Step time_step;
	Signal_Data *signal_ptr = NULL;

	Signal_File *file = (Signal_File *) fh;

	node_flag = Network_Data_Flag (NODE);
	timing_flag = Network_Data_Flag (TIMING_PLAN);
	coord_flag = Network_Data_Flag (SIGNAL_COORDINATOR);

	//---- reserve memory ----

	if (signal_data.Max_Records () == 0) {
		if (!signal_data.Max_Records (file->Num_Records ())) goto mem_error;
	}

	//---- read and save the data ----
		
	signal_ptr = signal_data.New_Record ();
	if (signal_ptr == NULL) goto mem_error;

	lvalue = file->Node ();

	if (lvalue == 0) return (false);

	if (node_flag) {
		index = node_data.Get_Index (lvalue);

		if (index == 0) {
			return (Check_Message ("Signalized Node %d was Not Found in the Node file", lvalue));
		}
		signal_ptr->Node (Renumber () ? index : lvalue);
	} else {
		signal_ptr->Node (lvalue);
	}

	//---- convert the signal type ----

	signal_ptr->Type (Signal_Code (file->Type ()));

	//---- check/convert the timing number ----

	lvalue = file->Timing ();

	if (timing_flag) {
		Timing_Data timing_rec, *timing_ptr;

		timing_rec.Timing (lvalue);
		timing_rec.Phase (1);

		timing_ptr = timing_data.Get_GE (&timing_rec);

		if (timing_ptr == NULL || timing_ptr->Timing () != lvalue) {
			return (Check_Message ("Signal Timing Plan %d was Not Found in the Timing Plan file", lvalue));
		}
	}
	signal_ptr->Timing (lvalue);
	
	signal_ptr->Offset (file->Offset ());

	//----- optional fields ----

	lvalue = file->Coordinator ();

	if (coord_flag && lvalue > 0) {
		index = coordinator_data.Get_Index (lvalue);

		if (index == 0) {
			Warning ("Signal Coordinator %d was Not Found in the Coordinator file", lvalue);
		}
		signal_ptr->Coordinator (Renumber () ? index : lvalue);
	} else {
		signal_ptr->Coordinator (lvalue);
	}

	//---- start time ----

	svalue = file->Start ();

	if (*svalue != '\0') {
		signal_ptr->Start (time_step.Step (svalue));		//---- NOT rounded ----
	} else {
		signal_ptr->Start (0);
	}

	//---- convert the ring code ----

	signal_ptr->Rings (Ring_Code (file->Ring ()));

	signal_ptr->Groups (0);

	return (true);

mem_error:
	Error ("Insufficient Memory for Signal Data");
	return (false);
}
