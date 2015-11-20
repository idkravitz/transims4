//*********************************************************
//	Read_Signals.cpp - read the signal file
//*********************************************************

#include "IntControl.hpp"

#include "Range_Data.hpp"

//---------------------------------------------------------
//	Read_Signals
//---------------------------------------------------------

void IntControl::Read_Signals (void)
{
	int node, num;
	char *str_ptr;

	Signal_Data *signal_ptr, signal_rec, *old_ptr, *last_ptr;
	Range_Data *range_ptr;

	//---- store the signal data ----

	Show_Message ("Reading %s -- Record", signal_file.File_Type ());
	Set_Progress ();

	num = signal_file.Num_Records ();
	if (!update_flag) {
		num *= time_range.Num_Ranges ();
	}
	if (!input_signal.Max_Records (num)) goto mem_error;

	memset (&signal_rec, '\0', sizeof (signal_rec));

	while (signal_file.Read ()) {
		Show_Progress ();

		//---- read and save the data ----
			
		node = signal_file.Node ();
		if (node == 0) continue;

		if (node_data.Get_Index (node) == 0) {
			Warning ("Signalized Node %d was Not Found in the Node file", node);
			continue;
		}
		signal_rec.Node (node);

		if (!update_flag) {
			signal_rec.Start (0);

			//---- ignore input time periods ----

			if (input_signal.Get (&signal_rec) != NULL) continue;

			//---- convert the signal type ----

			signal_rec.Type (Signal_Code (signal_file.Type ()));

			//---- convert the ring code ----

			signal_rec.Rings (Ring_Code (signal_file.Ring ()));

			//---- create records for each time period ----

			if (time_flag) {
				for (range_ptr = time_range.First (); range_ptr; range_ptr = time_range.Next ()) {
					signal_rec.Start (range_ptr->Low ());
					signal_rec.End (range_ptr->High ());

					if (!input_signal.Add (&signal_rec)) goto add_error;
				}
			} else {
				signal_rec.End (MIDNIGHT);

				if (!input_signal.Add (&signal_rec)) goto add_error;
			}

		} else {

			//---- check update start time ----

			str_ptr = signal_file.Start ();

			if (*str_ptr != '\0' && str_cmp (str_ptr, "ALL") != 0) {
				signal_rec.Start (time_range.Step (str_ptr));		//---- NOT rounded ----

				signal_ptr = signal_data.Get (&signal_rec);

				if (signal_ptr == NULL) {
					Warning ("Update Signal %d Start %s was Not Found", signal_rec.Node (), str_ptr);
				} else {
					if (!input_signal.Add (signal_ptr)) goto add_error;
				}
			} else {
				signal_rec.Start (0);

				signal_ptr = signal_data.Get (&signal_rec);

				if (signal_ptr == NULL) {
					Warning ("Update Signal %d was Not Found", signal_rec.Node ());
				} else {
					for (; signal_ptr; signal_ptr = signal_data.Next_Key ()) {
						if (signal_ptr->Node () != signal_rec.Node ()) break;
						if (range_flag) {
							if (!time_range.Span_Range (signal_ptr->Start (), signal_ptr->End ())) continue;
						}
						if (!input_signal.Add (signal_ptr)) goto add_error;
					}
				}
			}
		}
	}
	End_Progress ();
	signal_file.Close ();

	Print (2, "Number of %s Records = %d", signal_file.File_Type (), Progress_Count ());

	num = input_signal.Num_Records ();

	if (num) {
		if (num != Progress_Count ()) {
			Print (1, "Number of Signalized Node Data Records = %d", num);
		}

		//---- set the end time ----

		last_ptr = NULL;

		for (signal_ptr = input_signal.First_Key (); signal_ptr; signal_ptr = input_signal.Next_Key ()) {
			if (last_ptr != NULL) {
				if (last_ptr->Node () == signal_ptr->Node ()) {
					last_ptr->End (signal_ptr->Start () - 1);
				} else if (update_flag && range_flag) {
					old_ptr = signal_data.Get (last_ptr);
					if (old_ptr != NULL) {
						last_ptr->End (old_ptr->End ());
					} else {
						last_ptr->End (MIDNIGHT);
					}
				} else {
					last_ptr->End (MIDNIGHT);
				}
			}
			last_ptr = signal_ptr;
		}
		if (last_ptr != NULL) {
			if (update_flag && range_flag) {
				old_ptr = signal_data.Get (last_ptr);
				if (old_ptr != NULL) {
					last_ptr->End (old_ptr->End ());
				} else {
					last_ptr->End (MIDNIGHT);
				}
			} else {
				last_ptr->End (MIDNIGHT);
			}
		}
	}
	return;

mem_error:
	Error ("Insufficient Memory for Signal Data");
	return;

add_error:
	Error ("Adding Record to the Signal Data List");
	return;
}
