//*********************************************************
//	Phasing_Process.cpp - read the phasing file
//*********************************************************

#include "Progression.hpp"

//---------------------------------------------------------
//	Signal_Processing
//---------------------------------------------------------

bool Progression::Phasing_Processing (Db_File *fh)
{
	Phasing_Data *phasing_ptr;
	Signal_Data *signal_ptr;
	Green_Phase *green_ptr, green_rec;
	Signal_Offset *offset_ptr;
	Connect_Data *connect_ptr;

	if (Network_Service::Phasing_Processing (fh)) {

		phasing_ptr = phasing_data.New_Record ();

		//---- check the lane connectivity and movement type ----

		connect_ptr = connect_data.Get (phasing_ptr->In_Link_Dir (), phasing_ptr->Out_Link_Dir ());

		if (connect_ptr == NULL) return (false);

		if (!equiv_flag && connect_ptr->Type () != THRU) return (false);

		//---- process each signal time period ----

		signal_ptr = signal_data.Get_GE (phasing_ptr->Node (), 0);

		for (; signal_ptr; signal_ptr = signal_data.Next_Key ()) {
			
			if (signal_ptr->Node () != phasing_ptr->Node ()) break;
			if (signal_ptr->Timing () != phasing_ptr->Timing ()) continue;

			//---- get the signal offset record ----

			offset_ptr = signal_offset.Get (signal_data.Record_Index ());

			if (offset_ptr == NULL) continue;

			//---- add or update the green phase data ----

			green_ptr = offset_ptr->Get_Phase (phasing_ptr->In_Link_Dir (), phasing_ptr->Out_Link_Dir ());

			if (green_ptr == NULL) {
				green_rec.In_Link_Dir (phasing_ptr->In_Link_Dir ());
				green_rec.Out_Link_Dir (phasing_ptr->Out_Link_Dir ());
				green_rec.Movement (connect_ptr->Type ());

				if (phasing_ptr->Protection () == PROTECTED) {
					green_rec.Protected (phasing_ptr->Phase ());
					green_rec.Permitted (0);
				} else {
					green_rec.Permitted (phasing_ptr->Phase ());
					green_rec.Protected (0);
				}
				if (!offset_ptr->Add_Phase (&green_rec)) {
					Error ("Adding Green Phase to Signal Data");
				}
			} else {
				if (phasing_ptr->Protection () == PROTECTED) {
					if (green_ptr->Protected () > 0) {
						green_ptr->Permitted (phasing_ptr->Phase ());
					} else {
						green_ptr->Protected (phasing_ptr->Phase ());
					}
				} else if (green_ptr->Permitted () == 0) {
					green_ptr->Permitted (phasing_ptr->Phase ());
				}	
			}
		}
		//---- don't save the data ----
	}
	return (false);
}
