//*********************************************************
//	Start_Controller.cpp - Initialize Signal Controller
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Start_Controller
//---------------------------------------------------------

void Microsimulator::Start_Controller (Controller_Data *controller_ptr)
{
	int time = step / steps_per_second;
	int index, ring, timing, offset, cycle, index0, phase, new_status, new_time, min_time, group;
	bool next_phase;

	Phasing_Data *phasing_ptr;
	Timing_Data *timing_ptr;
	Signal_Data *signal_ptr;
	Control_Data *control_ptr;

	//---- get the signal data ----

	signal_ptr = signal_data.Get_LE (controller_ptr->Node (), time);

	controller_ptr->Type (signal_ptr->Type ());
	controller_ptr->Rings (signal_ptr->Rings ());
	controller_ptr->Groups (signal_ptr->Groups ());

	timing = signal_ptr->Timing ();
	offset = signal_ptr->Offset ();

	//---- set the end time to the beginning of the next plan ----

	signal_ptr = signal_data.Next_Key ();

	if (signal_ptr == NULL || signal_ptr->Node () != controller_ptr->Node ()) {
		controller_ptr->End_Time (MAX_INTEGER);
	} else {
		controller_ptr->End_Time (signal_ptr->Start ());
	}
	controller_ptr->Group (0);

	//---- identify each movement ---

	for (ring=1; ring <= controller_ptr->Max_Rings (); ring++) {
		controller_ptr->Phase (ring, 0);
	}
	index = controller_ptr->Movement ();

	while (index > 0) {
		phasing_ptr = phasing_data [index];

		timing_ptr = timing_data [phasing_ptr->Timing_Index ()];

		if (timing_ptr->Timing () == timing) {
			ring = timing_ptr->Ring ();

			if (ring > 0) {
				controller_ptr->Phase (ring, index);
				controller_ptr->Start (ring, time);
				controller_ptr->Check (ring, 0);
				controller_ptr->Status (ring, RED_LIGHT);
			}
			control_ptr = control_data [phasing_ptr->Control_Index ()];
			control_ptr->Control_Status (RED_LIGHT);
		}
		index = phasing_ptr->Node_List ();
	}

	//---- process multiple rings with barrier groups ----

	if (controller_ptr->Rings () > 1 && controller_ptr->Groups () > 1) {

		//---- set the minimum time per group ----

		for (group=1; group <= controller_ptr->Groups (); group++) {
			min_time = 0;

			for (ring=1; ring <= controller_ptr->Rings (); ring++) {
				index = index0 = controller_ptr->Phase (ring);
				timing = new_time = 0;
				next_phase = false;

				while (index > 0) {	
					phasing_ptr = phasing_data [index];

					if (timing != phasing_ptr->Timing_Index ()) {
						timing = phasing_ptr->Timing_Index ();
						timing_ptr = timing_data [timing];

						if (timing_ptr->Group () > group) break;
						if (timing_ptr->Group () == group || next_phase) {
							next_phase = true;
							new_time += timing_ptr->Min_Green () + 
								timing_ptr->Yellow () + timing_ptr->Red_Clear ();
						}
					}
					index = phasing_ptr->Phase_List ();
					if (index == index0) break;
				}
				if (new_time > min_time) {
					min_time = new_time;
				}
			}
			controller_ptr->Min_Time (group, min_time);
		}

		//---- apply the offset ----

		cycle = 0;

		if (offset > 0) {

			//---- find the combined cycle length ----

			for (group=1; group <= controller_ptr->Groups (); group++) {
				cycle += controller_ptr->Min_Time (group);
			}
			if (cycle == 0) return;
			offset %= cycle;
			cycle = 0;

			//---- set the group ----

			if (offset > 0) {
				for (group=1; group <= controller_ptr->Groups (); group++) {
					min_time = cycle + controller_ptr->Min_Time (group);
					if (min_time > offset) break;
					cycle = min_time;
				}
			} else {
				group = 1;
			}
		} else {
			offset = 0;
			group = 1;
		}
		controller_ptr->Group (group);

		//---- reset the phase timers ----

		for (ring=1; ring <= controller_ptr->Rings (); ring++) {

			index = index0 = controller_ptr->Phase (ring);
			timing = new_time = 0;
			next_phase = false;
			new_status = RED_LIGHT;

			min_time = controller_ptr->Min_Time (group);
			new_time = cycle;

			controller_ptr->Check (ring, time + min_time + cycle - offset);
			controller_ptr->Start (ring, controller_ptr->Check (ring) - min_time);

			while (index > 0) {
				phasing_ptr = phasing_data [index];

				if (timing != phasing_ptr->Timing_Index ()) {
					timing = phasing_ptr->Timing_Index ();
					timing_ptr = timing_data [timing];

					if (timing_ptr->Group () > group) break;
					if (timing_ptr->Group () == group || next_phase) {
						if (new_time > offset) break;
						next_phase = true;

						new_time += phase = timing_ptr->Min_Green ();

						if (new_time > offset) {
							new_status = PROTECTED_GREEN;
						} else {
							new_time += phase = timing_ptr->Yellow ();
							
							if (new_time > offset) {
								new_status = YELLOW_LIGHT;
							} else {
								new_time += phase = timing_ptr->Red_Clear ();
							}
						}
						if (new_time > offset) {
							controller_ptr->Phase (ring, index);
							controller_ptr->Check (ring, time + new_time - offset);
							controller_ptr->Start (ring, controller_ptr->Check (ring) - phase);
							controller_ptr->Status (ring, new_status);

							//---- timing change event ----

							if (system_event.In_Range (step, System_Event_Output::TIMING_CHANGE, controller_ptr->Node ())) {

								system_event.Time (step);
								system_event.Event (System_Event_Output::TIMING_CHANGE);
								system_event.Node (controller_ptr->Node ());
								system_event.Plan (timing_ptr->Timing ());
								system_event.Phase (timing_ptr->Phase ());
								system_event.Ring (ring);
								system_event.Group (group);

								if (new_status == RED_LIGHT) {
									system_event.Status ("Red");
								} else if (new_status == YELLOW_LIGHT) {
									system_event.Status ("Yellow");
								} else {
									system_event.Status ("Green");
								}
								system_event.Output ();
							}
						}
					}
				}

				//---- set the control status ----

				control_ptr = control_data [phasing_ptr->Control_Index ()];

				control_ptr->Control_Status (new_status);

				if (new_status == PROTECTED_GREEN) {
					control_ptr->Control_Status (new_status + phasing_ptr->Protection ());
				}
				index = phasing_ptr->Phase_List ();
				if (index == index0) break;
			}
		}
		return;
	}

	//---- adjust each ring ----

	for (ring=1; ring <= controller_ptr->Rings (); ring++) {

		index = index0 = controller_ptr->Phase (ring);
		timing = cycle = 0;

		//---- apply the offset ----

		if (offset > 0) {

			//---- calculate the cycle length ----

			while (index > 0) {	
				phasing_ptr = phasing_data [index];

				if (timing != phasing_ptr->Timing_Index ()) {
					timing = phasing_ptr->Timing_Index ();
					timing_ptr = timing_data [timing];

					cycle += timing_ptr->Min_Green () + timing_ptr->Yellow () + timing_ptr->Red_Clear ();
				}
				index = phasing_ptr->Phase_List ();
				if (index == index0) break;
			}
			if (cycle == 0) continue;

			offset %= cycle;
			index = index0;
			timing = cycle = 0;
		} else {
			offset = 0;
		}
			
		//---- reset the phase timers ----

		next_phase = true;
		new_status = RED_LIGHT;

		while (index > 0) {	
			phasing_ptr = phasing_data [index];

			if (timing != phasing_ptr->Timing_Index ()) {
				if (!next_phase) break;
				next_phase = false;

				timing = phasing_ptr->Timing_Index ();
				timing_ptr = timing_data [timing];

				cycle += phase = timing_ptr->Min_Green ();

				if (cycle > offset) {
					new_status = PROTECTED_GREEN;
				} else {
					cycle += phase = timing_ptr->Yellow ();
					
					if (cycle > offset) {
						new_status = YELLOW_LIGHT;
					} else {
						cycle += phase = timing_ptr->Red_Clear ();

						if (cycle <= offset) {
							next_phase = true;
						}
					}
				}
				if (!next_phase) {
					controller_ptr->Phase (ring, index);
					controller_ptr->Check (ring, time + cycle - offset);
					controller_ptr->Start (ring, controller_ptr->Check (ring) - phase);
					controller_ptr->Status (ring, new_status);

					//---- timing change event ----

					if (system_event.In_Range (step, System_Event_Output::TIMING_CHANGE, controller_ptr->Node ())) {

						system_event.Time (step);
						system_event.Event (System_Event_Output::TIMING_CHANGE);
						system_event.Node (controller_ptr->Node ());
						system_event.Plan (timing_ptr->Timing ());
						system_event.Phase (timing_ptr->Phase ());
						system_event.Ring (ring);
						system_event.Group (0);

						if (new_status == RED_LIGHT) {
							system_event.Status ("Red");
						} else if (new_status == YELLOW_LIGHT) {
							system_event.Status ("Yellow");
						} else {
							system_event.Status ("Green");
						}
						system_event.Output ();
					}
				}
			}

			//---- set the control status ----

			control_ptr = control_data [phasing_ptr->Control_Index ()];

			control_ptr->Control_Status (new_status);

			if (new_status == PROTECTED_GREEN) {
				control_ptr->Control_Status (new_status + phasing_ptr->Protection ());
			}
			index = phasing_ptr->Phase_List ();
			if (index == index0) break;
		}
	}
}
