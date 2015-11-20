//*********************************************************
//	Check_Ahead.cpp - Check Speed Constraints Ahead
//*********************************************************

#include "Microsimulator.hpp"

#include <math.h>

//---------------------------------------------------------
//	Check_Ahead
//---------------------------------------------------------

bool Microsimulator::Check_Ahead (Msim_Vehicle *vehicle_ptr, bool save_flag)
{
	int i, j, k, c, l, cell, lane, speed, offset, decel, decel2, length, stop_dist, dir, thru_dir;
	int max_dist, max_cells, out_low, out_high, min_save, to_speed, from_lane, react, min_time;
	int dwell, max_dwell, min_dwell;
	int vehicle, veh_speed, veh_decel2, veh_dist, veh_len, gap, gap_dist, index, traveler, next_rider;
	int first_cell, last_cell, end_cell, stop_cell, stop_num, stop, run, seconds, schedule, off_num;
	double b, delay;
	bool flag, external_stop, link_flag, hold_flag;

	Travel_Plan_Data *plan_ptr, *transit_ptr;
	Traveler_Data *traveler_ptr;
	Msim_Vehicle *veh_ptr;
	Veh_Type_Data *veh_type_ptr;
	Grid_Data *grid_ptr, *conflict_ptr;
	Travel_Leg_Data *leg_ptr, *from_leg;
	Control_Data *control_ptr;
	Rider_Data *rider_ptr = 0;
	Stop_Data *stop_ptr;

	length = decel = decel2 = run = stop_num = gap_dist = 0;
	flag = external_stop = false;

	//---- initialize the path condition ---

	if (vehicle_ptr->Dir () == 0) return (false);

	hold_time = vehicle_ptr->Hold ();
	hold_flag = vehicle_ptr->Hold_Flag ();
	lane_change = problem_code = max_speed = 0;
	blocked_flag = false;
	if (hold_time > 1) return (false);

	//---- get the move position ----

	cell = vehicle_ptr->Cell ();
	offset = vehicle_ptr->Offset ();
	lane = vehicle_ptr->Lane ();
	speed = vehicle_ptr->Speed ();

	grid_ptr = grid_data [vehicle_ptr->Dir ()];
	if (grid_ptr == NULL) return (false);

	first_cell = grid_ptr->Cell_In ();
	last_cell = grid_ptr->Cell_Out ();
	problem_dir = vehicle_ptr->Dir ();
	problem_lane = lane;

	//---- vehicle performance ----

	veh_type_ptr = veh_type_data [vehicle_ptr->Type ()];
	if (veh_type_ptr == NULL) return (false);

	if (speed_method) {
		length = veh_type_ptr->Length ();
		decel = veh_type_ptr->Max_Decel ();
		decel2 = 2 * decel;

		stop_dist = speed * speed / decel2;

		if (min_speed == 0) {
			min_speed = speed - decel;
			if (min_speed < 0) min_speed = 0;
		}
	} else {
		min_speed = stop_dist = 0;
	}

	//---- check the use restrictions ----

	if (cell >= first_cell && cell < last_cell) {
		if (!Cell_Use (cell, lane, grid_ptr, veh_type_ptr->Use ())) {

			//---- check if lane change is possible ----

			flag = false;

			if (lane > 1 && grid_ptr->Data (cell, lane-1) >= 0) {
				flag = Cell_Use (cell, lane-1, grid_ptr, veh_type_ptr->Use ());
			}
			if (!flag && lane < grid_ptr->Lanes () && grid_ptr->Data (cell, lane+1) >= 0) {
				flag = Cell_Use (cell, lane+1, grid_ptr, veh_type_ptr->Use ());
			}
			if (flag) {
				lane_change = 1;
				problem_code = USE_PROBLEM;
			} else {
				lane_change = 0;
				problem_code = LINK_ACCESS_PROBLEM;
			}
			max_speed = min_speed;
			return (false);
		}
	}

	//---- set the plan status values ----

	index = vehicle_ptr->Leg ();
	plan_ptr = vehicle_ptr->Travel_Plan ();
	if (plan_ptr == NULL || index == 0) return (false);

	leg_ptr = plan_ptr->Travel_Leg (index);
	if (leg_ptr == NULL) return (false);

	control_ptr = NULL;

	//----- find the next stop cell ----

	stop_cell = 0;

	if (plan_ptr->Mode () == DRIVER_MODE) {
		rider_ptr = rider_data [plan_ptr->Traveler ()];	
		run = rider_data.Run (vehicle_ptr->Vehicle ());

		stop_num = leg_ptr->Stop_Number ();

		if (stop_num > 0) {
			for (; stop_num <= rider_ptr->Stops (); stop_num++) {
				stop = rider_ptr->Stop (stop_num);

				stop_ptr = stop_data [stop];
				external_stop = (stop_ptr->Type () == EXTLOAD);

				if (stop_ptr->Link_Dir () != grid_ptr->Link_Dir ()) {
					stop_cell = 0;
					break;
				}
				stop_cell = stop_ptr->Cell ();

				if (cell <= stop_cell) break;
			}
		}

		//---- check the departure schedule ----

		if (cell == stop_cell && (!hold_flag || external_stop)) {
			if (external_stop) {
				dwell = MAX_INTEGER;
				schedule = step;
			} else {
				max_dwell = Resolve (veh_type_ptr->Max_Dwell ());
				min_dwell = Resolve (veh_type_ptr->Min_Dwell ());

				if (max_dwell > 0) {
					schedule = rider_ptr->Schedule (run, stop_num);

					if (step <= schedule) {
						dwell = step + max_dwell;
						if (dwell < schedule) {
							dwell = MIN (dwell + max_dwell / 2, schedule);
						}
					} else {
						dwell = MAX (MAX (schedule + max_dwell, step + max_dwell / 2), step + min_dwell);
					}
					dwell = Round (dwell);

					if (!rider_ptr->Time_Flag (stop_num)) {
						schedule = step;
					}
				} else {
					dwell = MAX_INTEGER;

					if (rider_ptr->Time_Flag (stop_num)) {
						schedule = rider_ptr->Schedule (run, stop_num);
					} else {
						schedule = step;
					}
				}
			}

			//---- let riders off ---

			if (save_flag) {
				delay = min_time = Round (step);

				for (traveler = rider_ptr->Off_Rider (run, stop_num); traveler; traveler = traveler_ptr->Next_Rider ()) {
					traveler_ptr = traveler_data [traveler];

					if (!external_stop) {
						delay += veh_type_ptr->Unloading ();
					}
					traveler_ptr->End_Time (Resolve ((int) (delay + 0.5)));
					traveler_ptr->Active (false);

					//---- update the line load ----

					rider_ptr->Alight (run, stop_num, 1);
					nalight++;

					//---- end time event ----
				
					transit_ptr = traveler_ptr->Travel_Plan ();

					if (event_output.In_Range (transit_ptr->End_Time (), END_EVENT, transit_ptr->Mode ())) {
						int offset = cell * cell_size - vehicle_ptr->Offset ();

						Link_Data *link_ptr = link_data [grid_ptr->Link ()];

						if (offset > link_ptr->Length ()) {
							offset = link_ptr->Length ();
						} else if (offset < 0) {
							offset = 0;
						}
						event_output.Household (traveler_ptr->Traveler () / Traveler_Scale ());
						event_output.Person (traveler_ptr->Traveler () % Traveler_Scale ());
						event_output.Trip (transit_ptr->Trip ());
						event_output.Leg (transit_ptr->Leg ());
						event_output.Mode (transit_ptr->Mode ());
						event_output.Event (END_EVENT);
						event_output.Schedule (transit_ptr->End_Time ());
						event_output.Actual (step);
						if (grid_ptr->Dir () > 0) {
							event_output.Link (-link_ptr->Link ());
						} else {
							event_output.Link (link_ptr->Link ());
						}
						event_output.Offset (Resolve (offset));

						event_output.Output ();
					}
				}
				rider_ptr->Off_Rider (run, stop_num, 0);

				min_time = (int) (delay + 0.5);

				if (veh_type_ptr->Method ()) {
					delay = Round (step);
				}

				//---- board rider ----

				for (traveler = rider_ptr->On_Rider (stop_num); traveler; traveler = next_rider) {
					traveler_ptr = traveler_data [traveler];
					next_rider = traveler_ptr->Next_Rider ();

					transit_ptr = traveler_ptr->Travel_Plan ();

					//---- check the vehicle capacity ----

					if (rider_ptr->Load (run) >= veh_type_ptr->Capacity ()) {
						Problem_Plan (Set_Problem (BOARDING_PROBLEM), transit_ptr, vehicle_ptr->Dir (), 0, 0);
						continue;
					}
					if (delay >= dwell) {
						Problem_Plan (Set_Problem (DWELL_PROBLEM), transit_ptr, vehicle_ptr->Dir (), 0, 0);
						continue;
					}

					//---- board the route and set the alighting stop number ----

					off_num = transit_ptr->End_Index ();

					traveler_ptr->Active (true);
					traveler_ptr->Next_Rider (rider_ptr->Off_Rider (run, off_num));
					rider_ptr->Off_Rider (run, off_num, traveler);
					rider_ptr->On_Rider (stop_num, next_rider);
					rider_ptr->Board (run, stop_num, 1);
					nboard++;

					if (!external_stop) {
						delay += veh_type_ptr->Loading ();
					}

					//---- start time event ----

					if (event_output.In_Range (transit_ptr->Start_Time (), START_EVENT, transit_ptr->Mode ())) {
						int offset = cell * cell_size - vehicle_ptr->Offset ();

						Link_Data *link_ptr = link_data [grid_ptr->Link ()];

						if (offset > link_ptr->Length ()) {
							offset = link_ptr->Length ();
						} else if (offset < 0) {
							offset = 0;
						}
						event_output.Household (traveler_ptr->Traveler () / Traveler_Scale ());
						event_output.Person (traveler_ptr->Traveler () % Traveler_Scale ());
						event_output.Trip (transit_ptr->Trip ());
						event_output.Leg (transit_ptr->Leg ());
						event_output.Mode (transit_ptr->Mode ());
						event_output.Event (START_EVENT);
						event_output.Schedule (transit_ptr->Start_Time ());
						event_output.Actual (step);
						if (grid_ptr->Dir () > 0) {
							event_output.Link (-link_ptr->Link ());
						} else {
							event_output.Link (link_ptr->Link ());
						}
						event_output.Offset (Resolve (offset));

						event_output.Output ();
					}
				}

				//---- set to the maximum ----

				if (veh_type_ptr->Method ()) {
					if (delay > min_time) {
						min_time = (int) (delay + 0.5);
					}
				} else {
					min_time = (int) (delay + 0.5);
				}
				min_time = Resolve (min_time);

				if (step < min_time) {
					delay = min_time - step;
					if (veh_type_ptr->Min_Dwell () > Round (delay)) {
						min_time = step + Resolve (veh_type_ptr->Min_Dwell ());
					}
				}
				if (min_time < schedule) {
					min_time = schedule;
				}
				//---- save the departure time ----

				rider_ptr->Time (run, stop_num, min_time);
				vehicle_ptr->Passengers (rider_ptr->Load (run));

				//----- stop the vehicle ----

				if (step < min_time) {
					hold_time = min_time - step + 1;
					max_speed = 0;

					if (min_speed > 0) {
						problem_code = STOP_PROBLEM;
						return (false);
					}
					return (true);
				}

			} else if (step < schedule || 
					rider_ptr->Off_Rider (run, stop_num) > 0 || 
					(rider_ptr->On_Rider (stop_num) > 0 && 
					rider_ptr->Load (run) < veh_type_ptr->Capacity ())) {

				//----- stop the vehicle ----

				max_speed = 0;

				if (min_speed > 0) {
					problem_code = STOP_PROBLEM;
					return (false);
				}
				return (true);
			}
		}
	}

	//---- find the end of the link ----

	if (index == plan_ptr->Num_Travel_Legs ()) {
		end_cell = plan_ptr->End_Cell ();

		if (cell > end_cell) {
			problem_code = PARKING_PROBLEM;
			return (true);
		}
		if (cell == end_cell) {
			if (parking_flag) {
				if (lane < leg_ptr->Out_Lane_Low () || lane > leg_ptr->Out_Lane_High ()) {
					lane_change = 1;
					max_speed = 0;
				} else {
					max_speed = leg_ptr->Max_Speed ();
				}
			} else {
				if (lane < leg_ptr->Out_Lane_Low () || lane > leg_ptr->Out_Lane_High ()) {
					problem_code = PARKING_PROBLEM;
				}
			}
			return (true);
		}

	} else {

		end_cell = grid_ptr->Cells ();

		//---- check for a traffic control ----

		if (cell == last_cell && leg_ptr->Control () > 0) {
			control_ptr = control_data [leg_ptr->Control ()];

			if (control_ptr->Control_Status () == RED_LIGHT ||
				(speed > 0 && (control_ptr->Control_Status () == STOP_SIGN || 
					control_ptr->Control_Status () == STOP_GREEN))) {

				max_speed = 0;

				if (min_speed > 0) {
					problem_code = CONTROL_PROBLEM;
					return (false);
				}
				return (true);
			}
		}
	}

	//---- check for the need to change lanes ----

	if (lane < leg_ptr->Out_Lane_Low () || lane > leg_ptr->Out_Lane_High ()) {
		if (speed_method) {
			gap_dist = (last_cell - cell) * cell_size + offset;
		} else {
			//gap_dist = end_cell - cell;
			gap_dist = last_cell - cell;
		}
		if (gap_dist < 0) gap_dist = 0;

		if (gap_dist <= stop_dist) {
			max_speed = min_speed;
			lane_change = 1;

			if (gap_dist < stop_dist) {
				problem_code = LANE_PROBLEM;
				return (false);
			}
			return (true);
		}
	}

	//---- check max distance ahead ----

	max_speed = speed + veh_type_ptr->Max_Accel ();

	if (max_speed > veh_type_ptr->Max_Speed ()) {
		max_speed = veh_type_ptr->Max_Speed ();
	}
	if (max_speed > grid_ptr->Speed ()) {
		max_speed = grid_ptr->Speed ();
	}

	//---- check plan following ----

	max_cells = last_cell - cell;

	if (max_cells > 0 && max_cells < plan_follow && 
		(lane < leg_ptr->Out_Best_Low () || lane > leg_ptr->Out_Best_High ())) {

		if (lane < leg_ptr->Out_Best_Low ()) {
			l = leg_ptr->Out_Best_Low () - lane;
			if (l > 1 && leg_ptr->Out_Lane_Low () < leg_ptr->Out_Best_Low ()) l--;
		} else {
			l = lane - leg_ptr->Out_Best_High ();
			if (l > 1 && leg_ptr->Out_Lane_High () > leg_ptr->Out_Best_High ()) l--;
		}
		lane_change = (NUM_PRIORITY_LEVELS * max_cells / l + plan_follow / 2) / plan_follow; 
		if (lane_change < 1) lane_change = 1;

		//---- slow down ----

		if (lane_change == 1) {
			if (speed_method) {
				if (max_speed > decel) max_speed -= decel;
			} else {
				if (max_speed > 1) max_speed--;	
			}
		} else if (lane_change == 2 && speed_method) {
			if (max_speed > decel) max_speed -= decel / 2;
		}
	}

	//---- check max distance ahead ----

	if (speed_method) {
		max_dist = (speed + max_speed) / 2 + max_speed * max_speed / decel2;
		
		if (reaction_flag) {
			b = reaction_time [grid_ptr->Type ()] * max_speed;
			react = (int) b;
			b -= react;
			if (b > random.Probability ()) react++;
		} else {
			react = 0;
		}
		max_cells = (max_dist + react - offset + cell_round) / cell_size;
	} else {
		max_cells = max_speed;
	}

	link_flag = false;
	vehicle = 0;
	if (control_ptr == NULL && leg_ptr->Control () > 0) {
		control_ptr = control_data [leg_ptr->Control ()];
	}

	for (gap=0, c=cell, l=lane; gap < max_cells; gap++) {

		//---- check the traffic control ----

		if (control_ptr != NULL) {
			if (c == last_cell) {
				if (control_ptr->Control_Status () == RED_LIGHT || 
					(speed > 0 && (control_ptr->Control_Status () == STOP_SIGN || 
								control_ptr->Control_Status () == STOP_GREEN))) {

					//---- stop the vehicle ----

					if (speed_method) {
						gap_dist = gap * cell_size + offset;
						if (gap_dist < 0) gap_dist = 0;
					} else {
						gap_dist = gap;
					}
					if (gap_dist < stop_dist) {
						max_speed = MIN (min_speed, max_speed);
						problem_code = CONTROL_PROBLEM;
						return (false);
					} else if (speed_method) {
						veh_speed = ((int) ((sqrt ((double) (decel * decel + 4 * decel * (2 * gap_dist - speed))) + 0.5)) - decel) / 2;
						if (veh_speed < 0) veh_speed = 0;
					} else {
						veh_speed = gap;
						if (veh_speed > 1) {
							veh_speed--;	//---- slow down ----
						}
					}
					if (veh_speed < max_speed) max_speed = veh_speed;
					break;

				} else if (control_ptr->Control_Status () == YELLOW_LIGHT) {

					//---- stop if you can ----

					if (speed_method) {
						gap_dist = gap * cell_size + offset;

						if (gap_dist >= stop_dist) {
							veh_speed = ((int) ((sqrt ((double) (decel * decel + 4 * decel * (2 * gap_dist - speed))) + 0.5)) - decel) / 2;

							if (veh_speed < 0) veh_speed = 0;
							if (veh_speed < max_speed) max_speed = veh_speed;
						}
					} else if (speed < gap) {
						veh_speed = gap;
						if (veh_speed > 1) {
							veh_speed--;		//---- slow down ----
						}
						if (veh_speed < max_speed) max_speed = veh_speed;
					}

				} else if (control_ptr->Control_Status () != PROTECTED_GREEN) {

					i = (end_cell - last_cell) * control_ptr->Max_Conflicts ();
					
					for (seconds=1; i > 0; seconds++) {
						i -= seconds * veh_type_ptr->Max_Accel ();
					}

					for (i=1; i <= control_ptr->Max_Conflicts (); i++) {
						dir = control_ptr->Conflict_Dir (i);
						out_low = control_ptr->Low_Lane (i);
						out_high = control_ptr->High_Lane (i);

						if (dir == 0) continue;

						conflict_ptr = grid_data [dir];
						conflict_veh.Dir (dir);
						conflict_veh.Offset (0);
						conflict_veh.Speed (conflict_ptr->Speed ());
						conflict_veh.Type (vehicle_ptr->Type ());
						conflict_veh.Travel_Plan (plan_ptr);
						min_save = min_speed;

						for (j=conflict_ptr->Cell_Out ()+1; j <= conflict_ptr->Cells (); j++) {

							conflict_veh.Cell (j);

							for (k=out_low; k <= out_high; k++) {
								conflict_veh.Lane (k);

								if (!Check_Behind (&conflict_veh, seconds)) {

									//---- stop the vehicle ----

									if (speed_method) {
										gap_dist = gap * cell_size + offset;
										if (gap_dist < 0) gap_dist = 0;
									} else {
										gap_dist = gap;
									}
									min_speed = min_save;

									if (gap_dist < stop_dist) {
										max_speed = MIN (min_speed, max_speed);
										problem_code = CONTROL_PROBLEM;
										return (false);
									} else if (speed_method) {
										veh_speed = ((int) ((sqrt ((double) (decel * decel + 4 * decel * (2 * gap_dist - speed))) + 0.5)) - decel) / 2;
										if (veh_speed < 0) veh_speed = 0;
									} else {
										veh_speed = gap;
										if (veh_speed > 1) {
											veh_speed--;		//---- slow down ----
										}
									}
									if (veh_speed < max_speed) max_speed = veh_speed;
									return (true);
								}
							}
						}
						min_speed = min_save;
					}
				}
			} else if (c > last_cell && 
				(control_ptr->Control_Status () == STOP_SIGN || 
				control_ptr->Control_Status () == STOP_GREEN ||
				control_ptr->Control_Status () == PERMITTED_GREEN)) {

				i = (end_cell - c) * control_ptr->Max_Conflicts ();
				
				for (seconds=1; i > 0; seconds++) {
					i -= seconds * veh_type_ptr->Max_Accel ();
				}

				for (i=1; i <= control_ptr->Max_Conflicts (); i++) {
					dir = control_ptr->Conflict_Dir (i);
					out_low = control_ptr->Low_Lane (i);
					out_high = control_ptr->High_Lane (i);

					if (dir == 0) continue;

					conflict_ptr = grid_data [dir];
					conflict_veh.Dir (dir);
					conflict_veh.Offset (0);
					conflict_veh.Speed (conflict_ptr->Speed ());
					conflict_veh.Type (vehicle_ptr->Type ());
					conflict_veh.Travel_Plan (plan_ptr);
					min_save = min_speed;

					for (j=conflict_ptr->Cell_Out ()+1; j <= conflict_ptr->Cells (); j++) {

						conflict_veh.Cell (j);

						for (k=out_low; k <= out_high; k++) {
							conflict_veh.Lane (k);

							if (!Check_Behind (&conflict_veh, seconds)) {

								//---- stop the vehicle ----

								if (speed_method) {
									gap_dist = gap * cell_size + offset;
									if (gap_dist < 0) gap_dist = 0;
								} else {
									gap_dist = gap;
								}
								min_speed = min_save;

								if (gap_dist < stop_dist) {
									max_speed = MIN (min_speed, max_speed);
									problem_code = CONTROL_PROBLEM;
									return (false);
								} else if (speed_method) {
									veh_speed = ((int) ((sqrt ((double) (decel * decel + 4 * decel * (2 * gap_dist - speed))) + 0.5)) - decel) / 2;
									if (veh_speed < 0) veh_speed = 0;
								} else {
									veh_speed = gap;
									if (veh_speed > 1) {
										veh_speed--;		//---- slow down ----
									}
								}
								if (veh_speed < max_speed) max_speed = veh_speed;
								return (true);
							}
						}
					}
					min_speed = min_save;
				}
			}
		}

		//---- intersection approach lane ----

		if (++c > last_cell) {
			if (speed_method) {
				gap_dist = gap * cell_size + offset;
				if (gap_dist < 0) gap_dist = 0;

				//---- check the exit lane ----

				if (l < leg_ptr->Out_Lane_Low () || l > leg_ptr->Out_Lane_High ()) {
					if (gap_dist <= stop_dist) {
						max_speed = MIN (min_speed, max_speed);
						lane_change = 1;

						if (gap_dist < stop_dist) {
							problem_code = LANE_PROBLEM;
						}
						return (false);
					} else {
						veh_speed = ((int) ((sqrt ((double) (decel * decel + 4 * decel * (2 * gap_dist - speed))) + 0.5)) - decel) / 2;

						if (veh_speed > 0) {
							max_speed = MIN (veh_speed, max_speed);
							if (max_speed > 0) {
								l = gap_dist / max_speed;
								if (l < 1) {
									l = 1;
								} else if (l > NUM_PRIORITY_LEVELS) {
									l = NUM_PRIORITY_LEVELS;
								}
							} else {
								l = 1;
							}
						} else {
							max_speed = 0;
							l = 1;
						}
						lane_change = l;
						break;
					}
				}

			} else {

				if (l < leg_ptr->Out_Lane_Low () || l > leg_ptr->Out_Lane_High ()) {
					veh_speed = gap;

					if (veh_speed > 1) {
						veh_speed--;		//---- slow down ----
					}
					if (veh_speed < max_speed) max_speed = veh_speed;
					lane_change = 1;
					break;
				}
			}
		}

		//---- update the stop information ----

		if (c > stop_cell && stop_cell > 0) {
			stop_cell = 0;
			stop_num = leg_ptr->Stop_Number ();

			if (stop_num > 0) {
				for (; stop_num <= rider_ptr->Stops (); stop_num++) {
					stop = rider_ptr->Stop (stop_num);

					stop_ptr = stop_data [stop];
					external_stop = (stop_ptr->Type () == EXTLOAD);

					if (stop_ptr->Link_Dir () != grid_ptr->Link_Dir ()) {
						stop_cell = 0;
						break;
					}
					stop_cell = stop_ptr->Cell ();

					if (c <= stop_cell) break;
				}
			}
		}

		//---- check for a transit stop ----

		if (c == stop_cell) {
			if (rider_ptr->Time_Flag (stop_num)) {
				schedule = rider_ptr->Schedule (run, stop_num);
			} else {
				schedule = step;
			}

			if (step < schedule ||
				rider_ptr->Off_Rider (run, stop_num) > 0 || 
				(rider_ptr->On_Rider (stop_num) > 0 &&
				rider_ptr->Load (run) < veh_type_ptr->Capacity ())) {

				//---- stop the vehicle at the stop ----

				if (speed_method) {
					gap_dist = (gap + 1) * cell_size + offset;
					if (gap_dist < 0) gap_dist = 0;
				} else {
					gap_dist = gap + 1;
				}

				if (gap_dist < stop_dist) {
					max_speed = MIN (min_speed, max_speed);
					problem_code = STOP_PROBLEM;
					return (false);
				} else if (speed_method) {
					veh_speed = ((int) ((sqrt ((double) (decel * decel + 4 * decel * (2 * gap_dist - speed))) + 0.5)) - decel) / 2;
					if (veh_speed < 0) veh_speed = 0;
				} else {
					veh_speed = gap + 1;
					if (veh_speed > 1) {
						veh_speed--;	//---- slow down ----
					}
				}
				if (veh_speed < max_speed) max_speed = veh_speed;
				break;
			}

			//---- save the departure time ----

			if (save_flag) {
				rider_ptr->Time (run, stop_num, step - 1);
			}
		}

		//---- end of the link ----

		if (c > end_cell) {

			//---- check the exit speed ----

			if (speed_method) {
				gap_dist = gap * cell_size + offset;
				if (gap_dist < 0) gap_dist = 0;

				veh_speed = leg_ptr->Max_Speed ();

				if (veh_speed < speed) {
					veh_dist = veh_speed * veh_speed / decel2 + gap_dist;

					if (veh_dist < stop_dist) {
						max_speed = MIN (min_speed, max_speed);
						problem_code = TURN_PROBLEM;
						return (false);
					} else {
						veh_speed = ((int) ((sqrt ((double) (decel * decel + 4 * decel * (2 * veh_dist - speed))) + 0.5)) - decel) / 2;

						if (veh_speed < leg_ptr->Max_Speed ()) {
							veh_speed = leg_ptr->Max_Speed ();
						}
						if (veh_speed < max_speed) max_speed = veh_speed;
					}
				}
			} else {
				veh_speed = leg_ptr->Max_Speed ();
				if (veh_speed < speed) {
					if (gap > veh_speed) {
						max_speed = gap - 1;
					} else {
						max_speed = veh_speed;
					}
				}
			}
			from_leg = leg_ptr;
			from_lane = l;

			//---- next link ----
		
			if (index == plan_ptr->Num_Travel_Legs ()) break;

			leg_ptr = plan_ptr->Travel_Leg (++index);
			if (leg_ptr == NULL) break;

			control_ptr = control_data [leg_ptr->Control ()];

			grid_ptr = grid_data [leg_ptr->Dir ()];
			if (grid_ptr == NULL) break;

			if (index == plan_ptr->Num_Travel_Legs ()) {
				end_cell = plan_ptr->End_Cell ();
			} else {
				end_cell = grid_ptr->Cells ();
			}
			first_cell = grid_ptr->Cell_In ();
			last_cell = grid_ptr->Cell_Out ();
			problem_dir = leg_ptr->Dir ();

			c = 1;
			l = Entry_Lane (l, from_leg, leg_ptr);
			problem_lane = l;
			link_flag = true;

			//---- check for use restrictions ----

			if (!Cell_Use (c, l, grid_ptr, veh_type_ptr->Use ())) {

				//---- check if lane change is possible ----

				flag = false;

				if (l > 1 && grid_ptr->Data (c, l-1) >= 0) {
					flag = Cell_Use (c, l-1, grid_ptr, veh_type_ptr->Use ());
				}
				if (!flag && l < grid_ptr->Lanes () && grid_ptr->Data (c, l+1) >= 0) {
					flag = Cell_Use (c, l+1, grid_ptr, veh_type_ptr->Use ());
				}
				lane_change = (flag) ? 1 : 0;

				if (speed_method) {
					if (gap_dist < stop_dist) {
						max_speed = MIN (min_speed, max_speed);
						problem_code = (flag) ? USE_PROBLEM : LINK_ACCESS_PROBLEM;
						return (false);
					} else {
						veh_speed = ((int) ((sqrt ((double) (decel * decel + 4 * decel * (2 * gap_dist - speed))) + 0.5)) - decel) / 2;
						if (veh_speed < 0) veh_speed = 0;
					}
				} else {
					veh_speed = gap;

					if (veh_speed > 1) {
						veh_speed--;		//---- slow down ----
					}
				}
				if (veh_speed < max_speed) max_speed = veh_speed;
				if (!flag) {
					problem_code = LINK_ACCESS_PROBLEM;
					return (false);
				}
				break;
			}

			//---- check for merge vehicles ----

			thru_dir = grid_ptr->Connect_Dir (l);

			if (thru_dir > 0 && from_leg->Dir () != thru_dir &&
				from_lane != grid_ptr->Connect_Lane (l)) {

				conflict_veh.Driver (vehicle_ptr->Driver ());
				conflict_veh.Dir (leg_ptr->Dir ());
				conflict_veh.Cell (c);
				conflict_veh.Lane (l);
				conflict_veh.Offset (0);
				conflict_veh.Speed (max_speed);
				conflict_veh.Type (vehicle_ptr->Type ());
				conflict_veh.Travel_Plan (plan_ptr);
				min_save = min_speed;

				if (!Check_Behind (&conflict_veh)) {
					min_speed = min_save;

					if (speed_method) {
						if (gap_dist < stop_dist) {
							max_speed = MIN (min_speed, max_speed);
							problem_code = MERGE_PROBLEM;
							return (false);
						} else {
							veh_speed = ((int) ((sqrt ((double) (decel * decel + 4 * decel * (2 * gap_dist - speed))) + 0.5)) - decel) / 2;
							if (veh_speed < 0) veh_speed = 0;
						}
					} else {
						veh_speed = gap;
						if (veh_speed > 1) {
							veh_speed--;		//---- slow down ----
						}
					}
					if (veh_speed < max_speed) max_speed = veh_speed;
					break;
				}
				min_speed = min_save;
			}

			//---- update the stop information ----

			stop_cell = 0;
			stop_num = leg_ptr->Stop_Number ();

			if (stop_num > 0) {
				stop = rider_ptr->Stop (stop_num);

				stop_ptr = stop_data [stop];

				stop_cell = stop_ptr->Cell ();
			}
		}

		//---- check for lane blockage ----

		if (!Cell_Use (c, l, grid_ptr, veh_type_ptr->Use ())) {

			//---- check if lane change is possible ----

			if (!link_flag) {
				flag = false;

				if (l > 1 && grid_ptr->Data (c, l-1) >= 0) {
					flag = Cell_Use (c, l-1, grid_ptr, veh_type_ptr->Use ());
				}
				if (!flag && l < grid_ptr->Lanes () && grid_ptr->Data (c, l+1) >= 0) {
					flag = Cell_Use (c, l+1, grid_ptr, veh_type_ptr->Use ());
				}
				if (flag) {
					lane_change = 1;
					blocked_flag = true;
				}
			} else {
				flag = true;
			}
			if (speed_method) {
				if (gap_dist < stop_dist) {
					max_speed = MIN (min_speed, max_speed);
					problem_code = (flag) ? USE_PROBLEM : LINK_ACCESS_PROBLEM;
					return (false);
				} else {
					veh_speed = ((int) ((sqrt ((double) (decel * decel + 4 * decel * (2 * gap_dist - speed))) + 0.5)) - decel) / 2;
					if (veh_speed < 0) veh_speed = 0;
				}
			} else {
				veh_speed = gap;

				if (veh_speed > 1) {
					veh_speed--;		//---- slow down ----
				}
			}
			if (veh_speed < max_speed) max_speed = veh_speed;
			if (!flag) {
				problem_code = LINK_ACCESS_PROBLEM;
				return (false);
			}
			break;
		}

		vehicle = grid_ptr->Data (c, l);

		//---- check for the end of a pocket lane ----

		if (vehicle < 0) {
			if (speed_method) {
				gap_dist = gap * cell_size + offset;
				if (gap_dist < 0) gap_dist = 0;

				if (gap_dist <= stop_dist) {
					max_speed = MIN (min_speed, max_speed);
					lane_change = 1;

					if (gap_dist < stop_dist) {
						problem_code = POCKET_PROBLEM;
					}
					return (false);
				} else {
					veh_speed = ((int) ((sqrt ((double) (decel * decel + 4 * decel * (2 * gap_dist - speed))) + 0.5)) - decel) / 2;

					if (veh_speed > 0) {
						max_speed = MIN (veh_speed, max_speed);
						if (max_speed > 0) {
							l = gap_dist / max_speed;
							if (l < 1) {
								l = 1;
							} else if (l > NUM_PRIORITY_LEVELS) {
								l = NUM_PRIORITY_LEVELS;
							}
						} else {
							l = 1;
						}
					} else {
						max_speed = 0;
						l = 1;
					}
					lane_change = l;
					break;
				}
			} else {
				veh_speed = gap;
				if (veh_speed > 1) {
					veh_speed--;		//---- slow down ----
				}
				max_speed = MIN (veh_speed, max_speed);
				lane_change = 1;
				blocked_flag = true;
				break;
			}
		}

		//---- check the vehicle speed ----

		if (vehicle > 0) {
			veh_ptr = vehicle_data [vehicle];

			if (veh_ptr->Dir () == 0) continue;

			veh_speed = veh_ptr->Speed ();

			if (speed_method) {
				if (veh_ptr->Type () != vehicle_ptr->Type ()) {
					veh_type_ptr = veh_type_data [vehicle_ptr->Type ()];

					veh_decel2 = 2 * veh_type_ptr->Max_Decel ();
					veh_len = veh_type_ptr->Length ();
				} else {
					veh_decel2 = decel2;
					veh_len = length;
				}
				if (veh_ptr->Status () > 0) {
					if (veh_ptr->Status () == 1) {
						to_speed = veh_ptr->To_Speed ();
					} else {
						to_speed = (veh_ptr->To_Speed () + veh_speed) / 2;
					}
					veh_dist = (veh_speed + to_speed) / 2 + to_speed * to_speed / veh_decel2;
				} else {
					veh_dist = veh_speed * veh_speed / veh_decel2;
				}
				veh_len = veh_ptr->Num_Cells () * cell_size - veh_ptr->Offset () - veh_len;

				gap_dist = gap * cell_size + offset + veh_len + veh_dist;
				if (gap_dist < 0) gap_dist = 0;

				veh_dist = gap_dist;

				if (reaction_flag) {
					b = reaction_time [grid_ptr->Type ()] * speed;
					react = (int) b;
					b -= react;
					if (b > random.Probability ()) react++;
					veh_dist -= react;
				}
				if (veh_dist < stop_dist) {
					if (gap_dist < stop_dist) {
						b = decel * (1 + 2 * reaction_time [grid_ptr->Type ()]);
						veh_speed = (int) ((sqrt (b * b - 4 * decel * (speed - 2 * gap_dist)) - b) / 2.0 + 0.5);

						max_speed = MAX (veh_speed, 0);
						if (max_speed < min_speed) {
							problem_code = SPACING_PROBLEM;
						}
						return (false);
					} else {
						max_speed = MIN (min_speed, max_speed);
					}
				} else {
					b = decel * (1 + 2 * reaction_time [grid_ptr->Type ()]);
					veh_speed = (int) ((sqrt (b * b - 4 * decel * (speed - 2 * gap_dist)) - b) / 2.0 + 0.5);

					if (veh_speed < max_speed) max_speed = MAX (veh_speed, 0);
				}
			} else {
				if (veh_ptr->Status () > 0) {
					if (veh_ptr->Status () == 1) {
						veh_speed = veh_ptr->To_Speed ();
					} else {
						veh_speed = (veh_ptr->To_Speed () + veh_speed) / 2;
					}
				}
				veh_speed += gap;

				if (reaction_flag) {
					double spd = veh_speed / (1.0 + reaction_time [grid_ptr->Type ()]);
					veh_speed = (int) spd;
					spd -= veh_speed;

					if (spd > random.Probability ()) veh_speed++;
				}
				if (veh_ptr->Status () == 0 && veh_speed > gap) {
					veh_speed = gap;
				}
				if (veh_speed < min_speed) {
					veh_speed = min_speed;
					problem_code = SPACING_PROBLEM;
				}
				if (veh_speed < max_speed) max_speed = veh_speed;
			}
		}

		//---- check the moved vehicles ----

		vehicle = grid_ptr->To_Data (c, l);

		if (vehicle > 0 && vehicle != plan_ptr->Vehicle ()) {
			if (speed_method) {
				gap_dist = gap * cell_size + offset;
				if (gap_dist < 0) gap_dist = 0;

				if (gap_dist < stop_dist) {
					max_speed = MIN (min_speed, max_speed);
					problem_code = MERGE_PROBLEM;
					return (false);
				} else {
					veh_speed = ((int) ((sqrt ((double) (decel * decel + 4 * decel * (2 * gap_dist - speed))) + 0.5)) - decel) / 2;
					if (veh_speed < 0) veh_speed = 0;
				}
			} else {
				veh_speed = gap;
			}
			if (veh_speed < max_speed) max_speed = veh_speed;
		}
	}
	return ((max_speed >= min_speed));
}
