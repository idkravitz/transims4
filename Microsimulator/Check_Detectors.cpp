//*********************************************************
//	Check_Detectors.cpp - check for vehicles on detectors
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Check_Detectors
//---------------------------------------------------------

bool Microsimulator::Check_Detectors (int index)
{
	int i, l, c, timing, num, index0;
	bool no_detectors;

	Grid_Data *grid_ptr;
	Phasing_Data *phasing_ptr;
	Detector_Data *detector_ptr;

	//---- process each movement in the phase ----

	index0 = index;
	timing = 0;
	no_detectors = true;

	while (index > 0) {	
		phasing_ptr = phasing_data [index];
		if (phasing_ptr == NULL) goto ptr_error;

		if (timing == 0) {
			timing = phasing_ptr->Timing_Index ();
		} else {
			if (phasing_ptr->Timing_Index () != timing) break;
		}
		num = phasing_ptr->Num_Detectors ();

		if (num > 0) no_detectors = false;

		for (i=1; i <= num; i++) {
			detector_ptr = detector_data [phasing_ptr->Detector (i)];
			if (detector_ptr == NULL) goto ptr_error;

			grid_ptr = grid_data [detector_ptr->Link_Dir ()];
			if (grid_ptr == NULL) goto ptr_error;

			for (l=detector_ptr->Low_Lane (); l <= detector_ptr->High_Lane (); l++) {
				for (c=detector_ptr->Low_Cell (); c <= detector_ptr->High_Cell (); c++) {
					if (grid_ptr->Data (c, l) > 0) return (true);
				}
			}
		}
		index = phasing_ptr->Phase_List ();
		if (index == index0) break;
	}
	return (no_detectors);

ptr_error:
	Error ("Detector Pointer is Null");
	return (false);
}
