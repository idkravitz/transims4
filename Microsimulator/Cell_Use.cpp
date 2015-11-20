//*********************************************************
//	Cell_Use.cpp - check the cell use restrictions
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Cell_Use
//---------------------------------------------------------

bool Microsimulator::Cell_Use (int cell, int lane, Grid_Data *grid_ptr, Use_Type use, bool use_flag)
{
	Link_Use_Data *use_ptr;

	if (use_flag) {
		if (use != BUS && use != HOV2 && use != HOV3 && use != HOV4) return (false);
	}
	if (lane < 1 || lane > grid_ptr->Lanes ()) return (false);

	for (int i = grid_ptr->TOD_Use (lane); i; i = use_ptr->TOD_Use ()) {
		use_ptr = link_use_data [i];

		if (use_ptr->Low_Cell () <= cell && cell <= use_ptr->High_Cell ()) {
			if (use_flag && Use_Permission (use_ptr->Use (), SOV)) {
				return (false);
			}
			return (Use_Permission (use_ptr->Use (), use));
		}
	}
	if (cell >= grid_ptr->Cell_In () && cell <= grid_ptr->Cell_Out ()) {
		if (use_flag && Use_Permission (grid_ptr->Lane_Use (lane), SOV)) {
			return (false);
		}
		return (Use_Permission (grid_ptr->Lane_Use (lane), use));
	} else if (use_flag) {
		return (!Use_Permission (grid_ptr->Lane_Use (lane), SOV));
	} else {
		return (true);
	}
}
