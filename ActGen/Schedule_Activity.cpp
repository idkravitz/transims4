//*********************************************************
//	Schedule_Activity.cpp - define activity time and duration
//*********************************************************

#include "ActGen.hpp"

//---------------------------------------------------------
//	Schedule_Activity
//---------------------------------------------------------

void ActGen::Schedule_Activity (void)
{
	int i, j, nact, person, start, end, anchor, ttime, tod0, tod1, tod2, type, duration;
	bool home_flag;

	Activity_Data *act_ptr, *anchor_ptr, *loc_ptr;

	nact = hhold_act_data.Num_Records ();

	//---- apply passenger constraints ----

	for (i=1; i <= nact; i++) {
		act_ptr = hhold_act_data [i];

		//if (act_ptr->Constraint () == PASSENGER) {
		//	for (j=1; j <= nact; j++) {
		//		loc_ptr = hhold_act_data [j];

		//		if (loc_ptr->Vehicle () == act_ptr->Vehicle () &&
		//			loc_ptr->Start_Time () == act_ptr->Start_Time () &&
		//			loc_ptr->Mode () == CARPOOL2 && loc_ptr->Constraint () != PASSENGER) {

		//			act_ptr->Constraint (loc_ptr->Constraint ());
		//			break;
		//		}
		//	}
		//}

		if (act_ptr->Passengers () > 0 && 
			(act_ptr->Mode () == DRIVE_ALONE || act_ptr->Mode () >= CARPOOL2)) {

			act_ptr->Constraint (START_TIME);
		}
	}

	//---- identify household tours ----

	home_flag = true;
	person = anchor = start = 0;
	anchor_ptr = 0;

	for (i=1; i <= nact; i++) {

		act_ptr = hhold_act_data [i];

		if (person != act_ptr->Person () || act_ptr->Purpose () == 0 || i == nact) {

			//---- process the tour ----

			if (!home_flag && person > 0) {
				end = i;

				//---- find the anchor activity ----

				for (j=start; j <= end; j++) {
					anchor_ptr = hhold_act_data [j];

					if (anchor_ptr->Priority () == PRIMARY_ANCHOR) {
						anchor = j;
						break;
					}
				}
				if (j > end) anchor = end;

				if (dump_flag) {
					Print (1, "Schedule Tour Activities %d to %d, Anchor=%d", 
						(hhold_act_data [start])->Activity (), act_ptr->Activity (), anchor_ptr->Activity ());
				}

				//---- cascade times forward ----

				tod0 = anchor_ptr->End_Time ();

				for (j=anchor-1; j >= start; j--) {
					ttime = anchor_ptr->Duration ();
					tod2 = anchor_ptr->Start_Time () - ttime;
					if (tod2 < 0) tod2 = 0;

					loc_ptr = hhold_act_data [j];

					type = loc_ptr->Constraint ();

					if (dump_flag) {
						Print (1, "Activity %d Constraint %d End Time Target=%d",
							loc_ptr->Activity (), type, tod2);
					}

					if (type == NO_CONSTRAINT || type == DURATION) {
						duration = loc_ptr->End_Time () - loc_ptr->Start_Time ();
						tod1 = tod2 - duration;
						if (tod1 < 0) tod1 = 0;

						loc_ptr->End_Time (tod2);
						if (j > start && loc_ptr->Purpose () != 0) {
							loc_ptr->Start_Time (tod1);

							if (dump_flag) {
								Print (1, "Activity %d Duration=%d, Start Time=%d", 
									loc_ptr->Activity (), duration, tod1);
							}
						}
					} else if (type == FIXED_TIME || type == END_TIME) {
						if (tod2 < loc_ptr->End_Time ()) {
							if (dump_flag) {
								Print (1, "Activity %d End Constraint=%d versus Target=%d",
									loc_ptr->Activity (), loc_ptr->End_Time (), tod2);
							}
							type = anchor_ptr->Constraint ();
							duration = anchor_ptr->End_Time () - anchor_ptr->Start_Time ();

							if (type != FIXED_TIME && type != START_TIME && type != PASSENGER) {
								tod1 = loc_ptr->End_Time () + ttime;

								if (tod1 < anchor_ptr->End_Time ()) {
									anchor_ptr->Start_Time (tod1);
									if (type == DURATION) {
										anchor_ptr->End_Time (tod1 + duration);
									}
								} else if (type != END_TIME) {
									anchor_ptr->Start_Time (tod1);

									tod2 = tod1 + duration;
									if (tod2 > tod0) {
										if (tod0 > tod1) {
											tod2 = tod0;
										} else {
											tod2 = tod1;
										}
									}
									anchor_ptr->End_Time (tod2);
									
									if (tod2 < tod1 + duration) {
										Activity_Problem (DURATION_PROBLEM, anchor_ptr);
									}
								} else {
									duration *= (anchor_ptr->End_Time () - loc_ptr->End_Time ()) / (ttime + duration);
									anchor_ptr->Start_Time (anchor_ptr->End_Time () - duration);

									Activity_Problem (DURATION_PROBLEM, anchor_ptr);
								}
								if (dump_flag) {
									Print (1, "Activity %d Reset to Start %d and End %d",
										anchor_ptr->Activity (), anchor_ptr->Start_Time (),
										anchor_ptr->End_Time ());
								}
							}
						}
					} else if (type == START_TIME || type == PASSENGER) {
						if (tod2 <= loc_ptr->Start_Time ()) {
							if (dump_flag) {
								Print (1, "Activity %d Start Constraint=%d versus Target=%d",
									loc_ptr->Activity (), loc_ptr->Start_Time (), tod2);
							}
							duration = loc_ptr->End_Time () - loc_ptr->Start_Time ();
							duration *= (anchor_ptr->Start_Time () - loc_ptr->Start_Time ()) / (ttime + duration);
							loc_ptr->End_Time (loc_ptr->Start_Time () + duration);

							if (dump_flag) {
								Print (1, "Activity %d End Time Reset to %d", loc_ptr->Activity (), loc_ptr->End_Time ());
							}
							Activity_Problem (DURATION_PROBLEM, loc_ptr);
						}
					}
					tod0 = anchor_ptr->Start_Time ();
					anchor_ptr = loc_ptr;
				}

				//---- cascade times backward ----
					
				anchor_ptr = hhold_act_data [anchor];
				tod0 = anchor_ptr->Start_Time ();

				for (j=anchor+1; j <= end; j++) {
					loc_ptr = hhold_act_data [j];

					ttime = loc_ptr->Duration ();

					tod1 = anchor_ptr->End_Time () + ttime;

					type = loc_ptr->Constraint ();

					if (dump_flag) {
						Print (1, "Activity %d Constraint %d Start Time Target=%d",
							loc_ptr->Activity (), type, tod1);
					}

					if (type == NO_CONSTRAINT || type == DURATION) {
						duration = loc_ptr->End_Time () - loc_ptr->Start_Time ();
						tod2 = tod1 + duration;

						loc_ptr->Start_Time (tod1);
						if (j < end && loc_ptr->Purpose () != 0) {
							loc_ptr->End_Time (tod2);

							if (dump_flag) {
								Print (1, "Activity %d Duration=%d, End Time=%d", 
									loc_ptr->Activity (), duration, tod2);
							}
						}
					} else if (type == FIXED_TIME || type == START_TIME || type == PASSENGER) {
						if (tod1 > loc_ptr->Start_Time ()) {
							if (dump_flag) {
								Print (1, "Activity %d Start Constraint=%d versus Target=%d",
									loc_ptr->Activity (), loc_ptr->Start_Time (), tod1);
							}
							type = anchor_ptr->Constraint ();
							duration = anchor_ptr->End_Time () - anchor_ptr->Start_Time ();

							if (type != FIXED_TIME && type != END_TIME) {
								tod2 = loc_ptr->Start_Time () - ttime;

								if (tod2 > anchor_ptr->Start_Time ()) {
									anchor_ptr->End_Time (tod2);
									if (type == DURATION) {
										tod1 = tod2 - duration;
										if (tod1 < 0) tod1 = 0;
										anchor_ptr->Start_Time (tod1);
									}
								} else if (type != START_TIME && type != PASSENGER) {
									anchor_ptr->End_Time (tod2);

									tod1 = tod2 - duration;
									if (tod1 < 0) tod1 = 0;
									if (tod1 < tod0) {
										if (tod0 < tod2) {
											tod1 = tod0;
										} else {
											tod1 = tod2;
										}
									}
									anchor_ptr->Start_Time (tod1);
									
									if (tod1 > tod2 - duration) {
										Activity_Problem (DURATION_PROBLEM, anchor_ptr);
									}
								} else {
									duration *= (loc_ptr->Start_Time () - anchor_ptr->Start_Time ()) / (ttime + duration);
									anchor_ptr->End_Time (anchor_ptr->Start_Time () + duration);

									Activity_Problem (DURATION_PROBLEM, anchor_ptr);
								}
								if (dump_flag) {
									Print (1, "Activity %d Reset to Start %d and End %d",
										anchor_ptr->Activity (), anchor_ptr->Start_Time (),
										anchor_ptr->End_Time ());
								}
							}
						}
					} else if (type == END_TIME) {
						if (tod1 >= loc_ptr->End_Time ()) {
							if (dump_flag) {
								Print (1, "Activity %d End Constraint=%d versus Target=%d",
									loc_ptr->Activity (), loc_ptr->End_Time (), tod1);
							}
							duration = loc_ptr->End_Time () - loc_ptr->Start_Time ();
							duration *= (loc_ptr->End_Time () - anchor_ptr->End_Time ()) / (ttime + duration);
							tod1 = loc_ptr->End_Time () - duration;
							if (tod1 < 0) tod1 = 0;
							loc_ptr->Start_Time (tod1);

							if (dump_flag) {
								Print (1, "Activity %d Start Time Reset to %d", loc_ptr->Activity (), loc_ptr->Start_Time ());
							}
							Activity_Problem (DURATION_PROBLEM, loc_ptr);
						}
					}
					tod0 = anchor_ptr->End_Time ();
					anchor_ptr = loc_ptr;
				}
			}

			//---- reset the data ----

			home_flag = true;
			person = act_ptr->Person ();
			start = i;

		} else {
			home_flag = false;
		}
	}

	//---- set the activity duration ----

	for (act_ptr = hhold_act_data.First (); act_ptr; act_ptr = hhold_act_data.Next ()) {
		if (act_ptr->End_Time () <= act_ptr->Start_Time ()) {
			act_ptr->End_Time (act_ptr->Start_Time () + 60);
		}
		act_ptr->Duration (act_ptr->End_Time () - act_ptr->Start_Time ());
	}
}
