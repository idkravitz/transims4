//*********************************************************
//	Cycle_Failure - manage vehicles on signal approach links
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Cycle_Failure
//---------------------------------------------------------

void Microsimulator::Cycle_Failure (Control_Data *control_ptr)
{
	if (control_ptr->Control_Status () == YELLOW_LIGHT) return;

	int dir = control_ptr->Approach_Dir ();

	if (!summary_output.In_Range (step, dir)) return;

	int c, cells, l, lanes, last_veh, vehicle, index, record, type;

	Grid_Data *grid_ptr;
	Msim_Vehicle *veh_ptr;
	Travel_Plan_Data *plan_ptr;
	Travel_Leg_Data *leg_ptr;

	//---- scan the link for vehicles assigned to this control ----

	grid_ptr = grid_data [dir];

	cells = grid_ptr->Cells ();
	lanes = grid_ptr->Lanes ();
	last_veh = 0;
	record = control_data.Record_Index ();

	memset (num_veh, '\0', num_veh_type * sizeof (int));
	memset (num_per, '\0', num_veh_type * sizeof (int));

	for (c=1; c <= cells; c++) {
		for (l=1; l <= lanes; l++) {
			vehicle = grid_ptr->Data (c, l);

			//---- match the vehicle control ----

			if (vehicle > 0 && vehicle != last_veh) {
				last_veh = vehicle;

				veh_ptr = vehicle_data [vehicle];

				if (veh_ptr->Dir () != dir) continue;

				plan_ptr = veh_ptr->Travel_Plan ();
				index = veh_ptr->Leg ();

				if (plan_ptr == NULL || index == 0) continue;

				leg_ptr = plan_ptr->Travel_Leg (index);
				if (leg_ptr == NULL) continue;

				if (leg_ptr->Control () != record) continue;
			
				//---- process the vehicle ----

				if (control_ptr->Control_Status () == RED_LIGHT) {
					if (veh_ptr->Cycle_Dir () == dir) {
						type = (summary_output.Veh_Type_Flag ()) ? veh_ptr->Type () : 0;
						num_veh [type]++;
						num_per [type] += veh_ptr->Passengers () + 1;
						veh_ptr->Cycle_Dir (0);
					}
				} else if (veh_ptr->Speed () == 0) {
					veh_ptr->Cycle_Dir (dir);
				}
			}
		}
	}

	//---- save the cycle failures ----

	if (summary_output.Veh_Type_Flag ()) {
		for (type=1; type < num_veh_type; type++) {
			if (num_veh [type] > 0) {
				summary_output.Cycle_Failure (step, dir, num_veh [type], num_per [type], veh_type_map [type]);
			}
		}
	} else if (num_veh [0] > 0) {
		summary_output.Cycle_Failure (step, dir, num_veh [0], num_per [0]);
	}
}
