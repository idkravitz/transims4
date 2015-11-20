//*********************************************************
//	Copy_Signals.cpp - Output Existing Signal Data
//*********************************************************

#include "IntControl.hpp"

#include "Timing_File.hpp"
#include "Phasing_File.hpp"
#include "Detector_File.hpp"
#include "Coordinator_File.hpp"

//---------------------------------------------------------
//	Copy_Signals
//---------------------------------------------------------

void IntControl::Copy_Signals (void)
{
	int i, node, index;

	Phasing_Data *phasing_ptr;
	Signal_Data *signal_ptr;

	Signal_Array copy_signal;
	Integer_Sort copy_timing;
	Integer_Sort copy_detector;
	Integer_Sort copy_coord;

	Signal_File *signal_file = (Signal_File *) Network_Db_Base (NEW_SIGNALIZED_NODE);
	Timing_File *timing_file = (Timing_File *) Network_Db_Base (NEW_TIMING_PLAN);
	Phasing_File *phasing_file = (Phasing_File *) Network_Db_Base (NEW_PHASING_PLAN);
	Detector_File *detector_file = (Detector_File *) Network_Db_Base (NEW_DETECTOR);
	Coordinator_File *coord_file = (Coordinator_File *) Network_Db_Base (NEW_SIGNAL_COORDINATOR);

	//---- write the signal files ----

	Show_Message ("Writing %s -- Record", signal_file->File_Type ());
	Set_Progress ();

	signal_db.Rewind ();

	while (signal_db.Read_Record ()) {
		Show_Progress ();

		signal_file->Copy_Fields (&signal_db);

		//---- skip deleted records ----

		if (delete_flag) {
			node = signal_file->Node ();

			if (delete_node.Get_Index (node) > 0) {
				nsignal++;
				continue;
			}
		}
		signal_db.Get_Field (signal_index, &index);

		signal_ptr = signal_data [index];
		if (signal_ptr == NULL) continue;

		if (!update_flag) {
			if (input_signal.Get (signal_ptr) != NULL) {
				nsignal++;
				continue;
			}
		}
		if (!signal_file->Write ()) {
			Error ("Writing %s", signal_file->File_Type ());
		}

		//---- save the signal reference ----

		if (!copy_signal.Add (signal_ptr)) {
			Error ("Copying Signal Data");
		}

		//---- save the timing plan id ----

		index = signal_ptr->Timing ();

		if (!copy_timing.Add (index)) {
			Error ("Copying Timing Data");
		}

		//---- save the signal coordinator id ----

		if (coord_flag) {
			index = signal_ptr->Coordinator ();

			if (!copy_coord.Add (index)) {
				Error ("Copying Coordinator Data");
			}
		}
	}
	End_Progress ();

	//---- write the timing files ----

	Show_Message ("Writing %s -- Record", timing_file->File_Type ());
	Set_Progress ();

	timing_db.Rewind ();

	while (timing_db.Read_Record ()) {
		Show_Progress ();

		timing_file->Copy_Fields (&timing_db);

		index = timing_file->Timing ();

		if (copy_timing.Get_Index (index) != 0) {
			if (update_flag) {
				for (signal_ptr = copy_signal.First (); signal_ptr; signal_ptr = copy_signal.Next ()) {
					if (signal_ptr->Timing () == index) {
						if (input_signal.Get (signal_ptr) == NULL) {
							if (!timing_file->Write ()) {
								Error ("Writing %s", timing_file->File_Type ());
							}
						} else {
							ntiming++;
						}
						break;
					}
				}
				if (signal_ptr == NULL) {
					ntiming++;
				}
			} else {
				if (!timing_file->Write ()) {
					Error ("Writing %s", timing_file->File_Type ());
				}
			}
		} else {
			ntiming++;
		}
	}
	End_Progress ();

	//---- write the phasing files ----

	Show_Message ("Writing %s -- Record", phasing_file->File_Type ());
	Set_Progress ();

	phasing_db.Rewind ();

	while (phasing_db.Read_Record ()) {
		Show_Progress ();

		phasing_file->Copy_Fields (&phasing_db);

		index = phasing_file->Timing ();

		if (copy_timing.Get_Index (index) != 0) {
			if (!phasing_file->Write ()) {
				Error ("Writing %s", phasing_file->File_Type ());
			}

			//---- save the detector ID's ----

			if (detector_flag) {
				phasing_db.Get_Field (phasing_index, &index);

				phasing_ptr = phasing_data [index];
				if (phasing_ptr == NULL) continue;

				for (i=1; i <= phasing_ptr->Num_Detectors (); i++) {
					index = phasing_ptr->Detector (i);

					if (copy_detector.Get_Index (index) == 0) {
						if (!copy_detector.Add (index)) {
							Error ("Copying Detector Data");
						}
					}
				}
			}
		} else {
			nphasing++;
		}
	}
	End_Progress ();

	//---- write the detector files ----

	if (detector_flag) {
		Show_Message ("Writing %s -- Record", detector_file->File_Type ());
		Set_Progress ();

		detector_db.Rewind ();

		while (detector_db.Read_Record ()) {
			Show_Progress ();

			detector_file->Copy_Fields (&detector_db);

			index = detector_file->ID ();

			if (copy_detector.Get_Index (index) != 0) {
				if (!detector_file->Write ()) {
					Error ("Writing %s", detector_file->File_Type ());
				}
			} else {
				ndetector++;
			}
		}
		End_Progress ();
	}

	//---- write the coordinator files ----

	if (coord_flag) {
		Show_Message ("Writing %s -- Record", coord_file->File_Type ());
		Set_Progress ();

		coord_db.Rewind ();

		while (coord_db.Read_Record ()) {
			Show_Progress ();

			coord_file->Copy_Fields (&coord_db);

			index = coord_file->ID ();

			if (copy_coord.Get_Index (index) != 0) {
				if (!coord_file->Write ()) {
					Error ("Writing %s", coord_file->File_Type ());
				}
			} else {
				ncoord++;
			}
		}
		End_Progress ();
	}
}
