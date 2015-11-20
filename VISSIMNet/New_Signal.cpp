//*********************************************************
//	New_Signal.cpp - write the new signal records
//*********************************************************

#include "VISSIMNet.hpp"

#include "Signal_File.hpp"
#include "Timing_File.hpp"
#include "Phasing_File.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	New_Signal
//---------------------------------------------------------

void VISSIMNet::New_Signal (void)
{
	int i, link, timing;
	char buffer [FIELD_BUFFER], string [FIELD_BUFFER];

	Integer_Sort timing_list;

	Signal_Data *signal_ptr, *new_signal_ptr;
	Timing_Data *timing_ptr, *new_timing_ptr;
	Phasing_Data *phasing_ptr, *new_phasing_ptr;
	Signal_File *signal_file;
	Timing_File *timing_file;
	Phasing_File *phasing_file;
	Link_Splits *split_ptr;

	//---- copy the signals ----

	for (signal_ptr = signal_data.First_Key (); signal_ptr; signal_ptr = signal_data.Next_Key ()) {

		if (!time_range.Span_Range (signal_ptr->Start (), signal_ptr->End ())) continue;

		timing = signal_ptr->Timing ();

		if (!timing_list.Add (timing)) {
			Error ("Adding to Timing List");
		}
		new_signal_ptr = new_signal_data.New_Record (true);

		memcpy (new_signal_ptr, signal_ptr, sizeof (Signal_Data));

		if (!new_signal_data.Add ()) {
			Error ("Adding New Signal Data");
		}
	}

	//---- copy the timing plans ----

	for (timing_ptr = timing_data.First_Key (); timing_ptr; timing_ptr = timing_data.Next_Key ()) {

		timing = timing_ptr->Timing ();

		if (timing_list.Get_Index (timing) == 0) continue;

		new_timing_ptr = new_timing_data.New_Record (true);

		memcpy (new_timing_ptr, timing_ptr, sizeof (Timing_Data));

		if (!new_timing_data.Add ()) {
			Error ("Adding New Timing Data");
		}
	}

	//---- copy the phasing plans ----

	for (phasing_ptr = phasing_data.First (); phasing_ptr; phasing_ptr = phasing_data.Next ()) {

		timing = phasing_ptr->Timing ();

		if (timing_list.Get_Index (timing) == 0) continue;

		new_phasing_ptr = new_phasing_data.New_Record (true);

		if (phasing_ptr->Num_Detectors () > 1) {
			new_phasing_ptr->Node (phasing_ptr->Node ());
			new_phasing_ptr->In_Link_Dir (phasing_ptr->In_Link_Dir ());
			new_phasing_ptr->Out_Link_Dir (phasing_ptr->Out_Link_Dir ());
			new_phasing_ptr->Timing (phasing_ptr->Timing ());
			new_phasing_ptr->Phase (phasing_ptr->Phase ());
			new_phasing_ptr->Protection (phasing_ptr->Protection ());
			new_phasing_ptr->Num_Detectors (phasing_ptr->Num_Detectors ());

			for (i=1; i <= phasing_ptr->Num_Detectors (); i++) {
				new_phasing_ptr->Detector (i, phasing_ptr->Detector (i));
			}
		} else {
			memcpy (new_phasing_ptr, phasing_ptr, sizeof (Phasing_Data));
		}

		if (phasing_ptr->In_Dir () == 1) {
			new_phasing_ptr->In_Link (phasing_ptr->In_Link () * MAX_SPLITS);
		} else {
			link = phasing_ptr->In_Link ();

			split_ptr = (Link_Splits *) link_splits.Get (&link);

			new_phasing_ptr->In_Link (phasing_ptr->In_Link () * MAX_SPLITS + split_ptr->num_splits);
		}

		if (phasing_ptr->Out_Dir () == 0) {
			new_phasing_ptr->Out_Link (phasing_ptr->Out_Link () * MAX_SPLITS);
		} else {
			link = phasing_ptr->Out_Link ();

			split_ptr = (Link_Splits *) link_splits.Get (&link);

			new_phasing_ptr->Out_Link (phasing_ptr->Out_Link () * MAX_SPLITS + split_ptr->num_splits);
		}

		if (!new_phasing_data.Add ()) {
			Error ("Adding New Phasing Data");
		}
	}

	//---- write the new signal file ----

	if (Network_File_Flag (NEW_SIGNALIZED_NODE)) {
		signal_file = (Signal_File *) Network_Db_Base (NEW_SIGNALIZED_NODE);

		for (signal_ptr = new_signal_data.First (); signal_ptr; signal_ptr = new_signal_data.Next ()) {

			signal_file->Node (signal_ptr->Node ());
			signal_file->Type (Signal_Code ((Signal_Type) signal_ptr->Type ()));
			signal_file->Timing (signal_ptr->Timing ());
			signal_file->Offset (signal_ptr->Offset ());
			signal_file->Start (time_range.Format_Step (signal_ptr->Start ()));
			signal_file->Coordinator (signal_ptr->Coordinator ());
			signal_file->Ring (Ring_Code ((Ring_Type) signal_ptr->Rings ()));

			if (!signal_file->Write ()) {
				Error ("Writing %s", signal_file->File_Type ());
			}
		}
		signal_file->Close ();

		Print (2, "Number of New Signal Records = %d", new_signal_data.Num_Records ());
	}

	//---- write the new timing file ----

	if (Network_File_Flag (NEW_TIMING_PLAN)) {
		timing_file = (Timing_File *) Network_Db_Base (NEW_TIMING_PLAN);

		for (timing_ptr = new_timing_data.First (); timing_ptr; timing_ptr = new_timing_data.Next ()) {

			timing_file->Timing (timing_ptr->Timing ());
			timing_file->Phase (timing_ptr->Phase ());

			str_fmt (buffer, sizeof (buffer), "%d", timing_ptr->Next_Phase ());

			timing_file->Next_Phase (buffer);

			timing_file->Min_Green (timing_ptr->Min_Green ());
			timing_file->Max_Green (timing_ptr->Max_Green ());
			timing_file->Ext_Green (timing_ptr->Ext_Green ());
			timing_file->Yellow (timing_ptr->Yellow ());
			timing_file->Red_Clear (timing_ptr->Red_Clear ());
			timing_file->Ring (timing_ptr->Ring ());
			timing_file->Group (timing_ptr->Group ());

			if (!timing_file->Write ()) {
				Error ("Writing %s", timing_file->File_Type ());
			}
		}
		timing_file->Close ();

		Print (2, "Number of New Timing Plan Records = %d", new_timing_data.Num_Records ());
	}

	//---- write the new phasing file ----

	if (Network_File_Flag (NEW_PHASING_PLAN)) {
		phasing_file = (Phasing_File *) Network_Db_Base (NEW_PHASING_PLAN);

		for (phasing_ptr = new_phasing_data.First (); phasing_ptr; phasing_ptr = new_phasing_data.Next ()) {

			phasing_file->Node (phasing_ptr->Node ());
			phasing_file->Timing (phasing_ptr->Timing ());
			phasing_file->Phase (phasing_ptr->Phase ());
			phasing_file->In_Link (phasing_ptr->In_Link ());
			phasing_file->Out_Link (phasing_ptr->Out_Link ());
			phasing_file->Protection (Protection_Code ((Protection_Type) phasing_ptr->Protection ()));

			string [0] = '\0';

			for (i=1; i <= phasing_ptr->Num_Detectors (); i++) {
				if (i > 1) {
					str_fmt (buffer, sizeof (buffer), "/%d", phasing_ptr->Detector (i));
				} else {
					str_fmt (buffer, sizeof (buffer), "%d", phasing_ptr->Detector (i));
				}
				str_cat (string, sizeof (string), buffer);
			}
			phasing_file->Detectors (string);

			if (!phasing_file->Write ()) {
				Error ("Writing %s", phasing_file->File_Type ());
			}
		}
		phasing_file->Close ();

		Print (2, "Number of New Phasing Plan Records = %d", new_phasing_data.Num_Records ());
	}
}
