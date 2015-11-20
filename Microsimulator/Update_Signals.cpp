//*********************************************************
//	Update_Signals.cpp - update traffic signal controls
//*********************************************************

#include "Microsimulator.hpp"

//---------------------------------------------------------
//	Update_Signals
//---------------------------------------------------------

void Microsimulator::Update_Signals (void)
{
	int time, new_time, index, ring, timing, new_status, index0, group;
	bool next_index, next_phase, next_group, group_break, group_flag, actuated_flag;

	Phasing_Data *phasing_ptr;
	Timing_Data *timing_ptr;
	Controller_Data *controller_ptr;
	Control_Data *control_ptr;

	time = step / steps_per_second;
	new_status = 0;

	for (controller_ptr = controller_data.First (); controller_ptr != NULL; controller_ptr = controller_data.Next ()) {

		//---- check for a new timing plan ----

		if (time >= controller_ptr->End_Time ()) {
			Start_Controller (controller_ptr);
		}

		actuated_flag = (controller_ptr->Type () == ACTUATED);
		group_flag = (controller_ptr->Rings () > 1 && controller_ptr->Groups () > 1);
		group = controller_ptr->Group ();
		next_group = true;
		group_break = false;

		//---- check each ring ----

		for (ring=1; ring <= controller_ptr->Rings (); ring++) {
			if (controller_ptr->Check (ring) > time) {
				next_group = false;
				continue;
			}
			index = index0 = controller_ptr->Phase (ring);
			timing = 0;
			next_phase = next_index = false;

			//---- process each movement in the phase ----

			while (index > 0) {
				phasing_ptr = phasing_data [index];

				if (timing == 0) {
					timing = phasing_ptr->Timing_Index ();
					timing_ptr = timing_data [timing];

					if (group_flag && timing_ptr->Group () > 0 && timing_ptr->Group () != group) {
						group_break = true;
						break;
					}

					//---- update the control status ----

					new_status = controller_ptr->Status (ring);

					if (new_status == RED_LIGHT) {

						next_phase = true;
						new_status = RED_LIGHT;

					} else if (new_status == YELLOW_LIGHT) {

						if (timing_ptr->Red_Clear () > 0) {
							controller_ptr->Start (ring, time);
							controller_ptr->Check (ring, time + timing_ptr->Red_Clear ());
						} else {
							next_phase = true;
						}
						new_status = RED_LIGHT;

					} else {	//---- currently green ----

						new_status = 0;

						if (actuated_flag) {
							if (timing_ptr->Ext_Green () > 0 && timing_ptr->Max_Green () > 0) {
								if ((time - controller_ptr->Start (ring)) < timing_ptr->Max_Green ()) {

									//---- check the detectors ----

									if (Check_Detectors (index)) {
										controller_ptr->Check (ring, time + timing_ptr->Ext_Green ());
										new_status = PROTECTED_GREEN;
									}
								}
							}
						}

						//---- complete the phase ----

						if (new_status == 0) {
							if (timing_ptr->Yellow () > 0) {
								controller_ptr->Start (ring, time);
								controller_ptr->Check (ring, time + timing_ptr->Yellow ());
								new_status = YELLOW_LIGHT;
							} else {
								if (timing_ptr->Red_Clear () > 0) {
									controller_ptr->Start (ring, time);
									controller_ptr->Check (ring, time + timing_ptr->Red_Clear ());
								} else {
									next_phase = true;
								}
								new_status = RED_LIGHT;
							}
						}
					}
					controller_ptr->Status (ring, new_status);

					//---- timing change event ----

					if (system_event.In_Range (step, System_Event_Output::PHASE_CHANGE, controller_ptr->Node ())) {

						system_event.Time (step);
						system_event.Event (System_Event_Output::PHASE_CHANGE);
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

				} else {

					//---- check for a new phase ----

					if (phasing_ptr->Timing_Index () != timing || index == index0) {
						if (!next_phase) {
							next_group = false;
							break;
						}
						if (group_flag && index == index0) {
							group_break = true;
							break;
						}
						timing = phasing_ptr->Timing_Index ();
						timing_ptr = timing_data [timing];

						//---- check for a group boundary ----

						if (group_flag && timing_ptr->Group () > 0) {
							controller_ptr->Phase (ring, index);
							group_break = true;
							break;
						}

						//---- check the detectors ----

						if (actuated_flag && index != index0 && 
							!Check_Detectors (index)) {
						
							//---- skip the next phase ----

							if (!next_index) {
								index0 = index;
								next_index = true;
								
							}
							next_phase = true;
							new_status = RED_LIGHT;

						} else {

							//---- initialize the next phase ----

							next_phase = next_group = false;
							controller_ptr->Phase (ring, index);
							controller_ptr->Start (ring, time);

							if (timing_ptr->Min_Green () > 0) {
								new_time = timing_ptr->Min_Green ();
								new_status = PROTECTED_GREEN;
							} else if (timing_ptr->Yellow () > 0) {
								new_time = timing_ptr->Yellow ();
								new_status = YELLOW_LIGHT;
							} else {
								if (timing_ptr->Red_Clear () > 0) {
									new_time = timing_ptr->Red_Clear ();
								} else {
									new_time = 1;
								}
								new_status = RED_LIGHT;
							}
							controller_ptr->Check (ring, time + new_time);
							controller_ptr->Status (ring, new_status);

							//---- timing change event ----

							if (system_event.In_Range (step, System_Event_Output::PHASE_CHANGE, controller_ptr->Node ())) {

								system_event.Time (step);
								system_event.Event (System_Event_Output::PHASE_CHANGE);
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

				//---- set the movement control ----

				control_ptr = control_data [phasing_ptr->Control_Index ()];

				control_ptr->Control_Status (new_status);

				if (new_status == PROTECTED_GREEN) {
					control_ptr->Control_Status (new_status + phasing_ptr->Protection ());
				}

				//---- manage cycle failures ----

				if (new_status != YELLOW_LIGHT) {
					Cycle_Failure (control_ptr);
				}
				index = phasing_ptr->Phase_List ();
			}
		}
		if (!group_flag || !next_group) {
			if (group_break) {
				new_time = time;

				for (ring=1; ring <= controller_ptr->Rings (); ring++) {
					if (controller_ptr->Check (ring) > new_time) {
						new_time = controller_ptr->Check (ring);
					}
				}
				for (ring=1; ring <= controller_ptr->Rings (); ring++) {
					if (controller_ptr->Check (ring) <= time) {
						controller_ptr->Check (ring, new_time);
					}
				}
			}
			continue;
		}

		//---- move to the next group ----

		if (group == controller_ptr->Groups ()) {
			group = 1;
		} else {
			group++;
		}
		controller_ptr->Group (group);
		next_group = true;
		group_break = false;

		//---- initialize each ring ----

		for (ring=1; ring <= controller_ptr->Rings (); ring++) {

			index = index0 = controller_ptr->Phase (ring);
			timing = 0;
			next_phase = true;

			//---- process each movement in the phase ----

			while (index > 0) {
				phasing_ptr = phasing_data [index];

				if (timing == 0) {
					timing = phasing_ptr->Timing_Index ();
					timing_ptr = timing_data [timing];

					//---- check for a group boundary ----

					if (timing_ptr->Group () > 0 && timing_ptr->Group () != group) {
						group_break = true;
						break;
					}
					
					//---- check the detectors ----

					if (actuated_flag && !Check_Detectors (index)) {

						new_time = 0;
						new_status = RED_LIGHT;

					} else {

						next_phase = next_group = false;


						if (timing_ptr->Min_Green () > 0) {
							new_time = timing_ptr->Min_Green ();
							new_status = PROTECTED_GREEN;
						} else if (timing_ptr->Yellow () > 0) {
							new_time = timing_ptr->Yellow ();
							new_status = YELLOW_LIGHT;
						} else {
							if (timing_ptr->Red_Clear () > 0) {
								new_time = timing_ptr->Red_Clear ();
							} else {
								new_time = 1;
							}
							new_status = RED_LIGHT;
						}
						controller_ptr->Phase (ring, index);
						controller_ptr->Start (ring, time);
						controller_ptr->Check (ring, time + new_time);
						controller_ptr->Status (ring, new_status);

						//---- timing change event ----

						if (system_event.In_Range (step, System_Event_Output::PHASE_CHANGE, controller_ptr->Node ())) {

							system_event.Time (step);
							system_event.Event (System_Event_Output::PHASE_CHANGE);
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

				} else {

					//---- check for a new phase ----

					if (phasing_ptr->Timing_Index () != timing || index == index0) {
						if (!next_phase) {
							break;
						}

						if (index == index0) {
							group_break = true;
							break;
						}

						timing = phasing_ptr->Timing_Index ();
						timing_ptr = timing_data [timing];

						//---- check for a group boundary ----

						if (timing_ptr->Group () > 0 && timing_ptr->Group () != group) {
							group_break = true;
							break;
						}

						//---- check the detectors ----

						if (actuated_flag && index != index0 &&
							!Check_Detectors (index)) {

							new_status = RED_LIGHT;

						} else {

							//---- initialize the next phase ----

							next_phase = next_group = false;
							controller_ptr->Phase (ring, index);
							controller_ptr->Start (ring, time);

							if (timing_ptr->Min_Green () > 0) {
								new_time = timing_ptr->Min_Green ();
								new_status = PROTECTED_GREEN;
							} else if (timing_ptr->Yellow () > 0) {
								new_time = timing_ptr->Yellow ();
								new_status = YELLOW_LIGHT;
							} else {
								if (timing_ptr->Red_Clear () > 0) {
									new_time = timing_ptr->Red_Clear ();
								} else {
									new_time = 1;
								}
								new_status = RED_LIGHT;
							}
							controller_ptr->Check (ring, time + new_time);
							controller_ptr->Status (ring, new_status);

							//---- timing change event ----

							if (system_event.In_Range (step, System_Event_Output::PHASE_CHANGE, controller_ptr->Node ())) {

								system_event.Time (step);
								system_event.Event (System_Event_Output::PHASE_CHANGE);
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

				//---- set the movement control ----

				control_ptr = control_data [phasing_ptr->Control_Index ()];

				control_ptr->Control_Status (new_status);

				if (new_status == PROTECTED_GREEN) {
					control_ptr->Control_Status (new_status + phasing_ptr->Protection ());
				}

				//---- manage cycle failures ----

				if (new_status != YELLOW_LIGHT) {
					Cycle_Failure (control_ptr);
				}
				index = phasing_ptr->Phase_List ();
			}
		}

		//---- clean up ----

		if (!next_group) {
			if (group_break) {
				new_time = time;

				if (actuated_flag) {
					for (ring=1; ring <= controller_ptr->Rings (); ring++) {
						if (controller_ptr->Check (ring) > new_time) {
							new_time = controller_ptr->Check (ring);
						}
					}
				} else {
					new_time += controller_ptr->Min_Time (group);
				}
				for (ring=1; ring <= controller_ptr->Rings (); ring++) {
					if (controller_ptr->Check (ring) <= time) {
						controller_ptr->Check (ring, new_time);
					}
				}
			}

		} else {

			//---- set the phase to green ----

			for (ring=1; ring <= controller_ptr->Rings (); ring++) {

				index = index0 = controller_ptr->Phase (ring);
				timing = 0;

				//---- process each movement in the phase ----

				while (index > 0) {
					phasing_ptr = phasing_data [index];

					if (timing == 0) {
						timing = phasing_ptr->Timing_Index ();
						timing_ptr = timing_data [timing];

						//---- check for a group boundary ----

						if (timing_ptr->Group () > 0 && timing_ptr->Group () != group) {
							break;
						}
						if (timing_ptr->Min_Green () > 0) {
							new_time = timing_ptr->Min_Green ();
							new_status = PROTECTED_GREEN;
						} else if (timing_ptr->Yellow () > 0) {
							new_time = timing_ptr->Yellow ();
							new_status = YELLOW_LIGHT;
						} else {
							if (timing_ptr->Red_Clear () > 0) {
								new_time = timing_ptr->Red_Clear ();
							} else {
								new_time = 1;
							}
							new_status = RED_LIGHT;
						}
						controller_ptr->Phase (ring, index);
						controller_ptr->Start (ring, time);
						controller_ptr->Check (ring, time + new_time);
						controller_ptr->Status (ring, new_status);

						//---- timing change event ----

						if (system_event.In_Range (step, System_Event_Output::PHASE_CHANGE, controller_ptr->Node ())) {

							system_event.Time (step);
							system_event.Event (System_Event_Output::PHASE_CHANGE);
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

					} else {

						if (phasing_ptr->Timing_Index () != timing || index == index0) {
							break;
						}
					}

					//---- set the movement control ----

					control_ptr = control_data [phasing_ptr->Control_Index ()];

					control_ptr->Control_Status (new_status);

					if (new_status == PROTECTED_GREEN) {
						control_ptr->Control_Status (new_status + phasing_ptr->Protection ());
					}

					//---- manage cycle failures ----

					if (new_status != YELLOW_LIGHT) {
						Cycle_Failure (control_ptr);
					}
					index = phasing_ptr->Phase_List ();
				}
			}

			//---- adjust the times ----

			new_time = time;

			if (actuated_flag) {
				for (ring=1; ring <= controller_ptr->Rings (); ring++) {
					if (controller_ptr->Check (ring) > new_time) {
						new_time = controller_ptr->Check (ring);
					}
				}
			} else {
				new_time += controller_ptr->Min_Time (group);
			}
			for (ring=1; ring <= controller_ptr->Rings (); ring++) {
				if (controller_ptr->Check (ring) <= time) {
					controller_ptr->Check (ring, new_time);
				}
			}
		}
	}
}
