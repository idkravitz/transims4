//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "IntControl.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void IntControl::Execute (void)
{
	if (delete_flag) {
		Read_Deletes ();
	}

	//---- read the network files ----

	Network_Service::Execute ();

	//---- read the turn volumes ----

	if (turn_flag) {
		Read_Turn ();
	} else if (delay_flag) {
		Read_Delay ();
	}

	//---- read the sign file ----

	if (input_sign_flag) {
		Read_Signs ();
	}

	//---- read the signal file ----

	if (input_signal_flag) {
		Read_Signals ();
	}

	//---- prepare the data ----

	Data_Setup ();

	//---- process the sign data ----

	if (sign_flag) {
		Write_Signs ();
	}

	//---- process the signal data ----

	if (signal_flag) {
		if (!regen_flag && (delete_flag || !update_flag)) {
			Copy_Signals ();
		}
		if (update_flag) {
			if (input_signal_flag) {
				Pocket_Lanes ();

				Update_Signals ();
			}
		} else {
			Pocket_Lanes ();

			Connection_List ();

			Write_Signals ();
		}
	}

	//---- processing results ----

	if (sign_flag) {
		Write (2, "Number of Unsignalized Node Changes = %d", nsign);
	}
	if (signal_flag) {
 		if (update_flag && !delete_flag) {
			Write (2, "Number of Timing Plan Changes = %d", ntiming);
		} else {
			Break_Check (6);
			Write (2, "Number of Signalized Node Changes = %d", nsignal);
			Write (1, "Number of Timing Plan Changes = %d", ntiming);
			Write (1, "Number of Phasing Plan Changes = %d", nphasing);
			Write (1, "Number of Detector Changes = %d", ndetector);
			Write (1, "Number of Signal Coordinator Changes = %d", ncoord);
		}
	}

	//---- end the program ----

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void IntControl::Page_Header (void)
{
	switch (Header_Number ()) {
		case TIMING_UPDATE:			//---- Signal Timing Update ----
			Update_Header ();
			break;
		default:
			break;
	}
}
