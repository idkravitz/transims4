//*********************************************************
//	Best_Lanes.cpp - Best Lanes for Plan Following
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Best_Lanes
//---------------------------------------------------------

void Microsimulator::Best_Lanes (Travel_Plan_Data *plan_ptr)
{
	int leg, num_leg, cells, in_lanes, out_lanes, total, lanes, lane_factor;
	int in_low, in_high, best_low, best_high, out_low, out_high, best;

	Travel_Leg_Data *to_leg, *from_leg;
	Grid_Data *grid_ptr;

	leg = num_leg = plan_ptr->Num_Travel_Legs ();
	from_leg = to_leg = plan_ptr->Travel_Leg (leg);
	total = 0;

	lane_factor = plan_follow * 2 / NUM_PRIORITY_LEVELS;
	if (lane_factor < 1) lane_factor = 1;

	while (leg > 0) {
		if (leg == num_leg) {
			cells = plan_ptr->End_Cell ();
		} else {
			grid_ptr = grid_data [to_leg->Dir ()];
			cells = grid_ptr->Cells ();
		}
		if (--leg > 0) {
			from_leg = plan_ptr->Travel_Leg (leg);
		} else {
			cells -= plan_ptr->Start_Cell ();
			from_leg = NULL;
		}

		in_low = best_low = to_leg->In_Lane_Low ();
		in_high = best_high = to_leg->In_Lane_High ();
		total += cells;

		if (total <= plan_follow) {
			lanes = total / lane_factor;
			best = to_leg->Out_Best_Low () - lanes;

			if (best_low < best) {
				best_low = MIN (best_high, best);
			}
			best = to_leg->Out_Best_High () + lanes;

			if (best_high > best) {
				best_high = MAX (best_low, best);
			}

		} else {
			total = cells;

			if (total <= plan_follow) {
				lanes = total / lane_factor;
				best = to_leg->Out_Lane_Low () - lanes;

				if (best_low < best) {
					best_low = MIN (best_high, best);
				}
				best = to_leg->Out_Lane_High () + lanes;

				if (best_high > best) {
					best_high = MAX (best_low, best);
				}
			}
		}
		if (total <= plan_follow && from_leg != NULL) {
			out_low = from_leg->Out_Best_Low ();
			out_high = from_leg->Out_Best_High ();

			if (best_low > in_low || best_high < in_high) {

				in_lanes = best_high - best_low + 1;
				out_lanes = out_high - out_low + 1;

				while (out_lanes > in_lanes) {
					if (best_low > in_low) {
						in_low++;
						out_low++;
						out_lanes--;
					}
					if (out_lanes > in_lanes && best_high < in_high) {
						in_high--;
						out_high--;
						out_lanes--;
					}
					if (best_low == in_low && best_high == in_high) break;
				}
			}
			from_leg->Out_Best_Low (out_low);
			from_leg->Out_Best_High (out_high);
		}
		to_leg->In_Best_Low (best_low);
		to_leg->In_Best_High (best_high);

		to_leg = from_leg;
	}
}
