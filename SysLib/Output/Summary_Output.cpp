//*********************************************************
//	Summary_Output.cpp - Output Interface Class
//*********************************************************

#include "Summary_Output.hpp"

#include "Network_Service.hpp"

char * Summary_Output::OUTPUT_SUMMARY_FILE_x		= "OUTPUT_SUMMARY_FILE_*";
char * Summary_Output::OUTPUT_SUMMARY_FORMAT_x		= "OUTPUT_SUMMARY_FORMAT_*";
char * Summary_Output::OUTPUT_SUMMARY_TIME_FORMAT_x	= "OUTPUT_SUMMARY_TIME_FORMAT_*";
char * Summary_Output::OUTPUT_SUMMARY_INCREMENT_x	= "OUTPUT_SUMMARY_INCREMENT_*";
char * Summary_Output::OUTPUT_SUMMARY_TIME_RANGE_x	= "OUTPUT_SUMMARY_TIME_RANGE_*";
char * Summary_Output::OUTPUT_SUMMARY_LINK_RANGE_x	= "OUTPUT_SUMMARY_LINK_RANGE_*";
char * Summary_Output::OUTPUT_SUMMARY_VEH_TYPES_x	= "OUTPUT_SUMMARY_VEH_TYPES_*";
char * Summary_Output::OUTPUT_SUMMARY_TURN_FLAG_x	= "OUTPUT_SUMMARY_TURN_FLAG_*";
char * Summary_Output::OUTPUT_SUMMARY_PCE_FLAG_x	= "OUTPUT_SUMMARY_PCE_FLAG_*";
char * Summary_Output::OUTPUT_SUMMARY_PERSON_FLAG_x	= "OUTPUT_SUMMARY_PERSON_FLAG_*";
char * Summary_Output::OUTPUT_SUMMARY_COORDINATES_x	= "OUTPUT_SUMMARY_COORDINATES_*";

//---------------------------------------------------------
//	Summary_Output constructor
//---------------------------------------------------------

Summary_Output::Summary_Output (void) : Static_Service ()
{
	output_data.Initialize (sizeof (Output_Data), 10);

	output_flag = veh_type_flag = false;
	step_time = veh_pce = veh_occ = 1;
	from_dir = from_speed = to_dir = to_speed = from_offset = to_offset = veh_type = 0;
	from_connect = to_connect = 0;
}

//---------------------------------------------------------
//	Summary_Output destructor
//---------------------------------------------------------

Summary_Output::~Summary_Output (void)
{
	Close ();
}

//---------------------------------------------------------
//	Add_Keys
//---------------------------------------------------------

void Summary_Output::Add_Keys (void)
{
	char *keys [] = {
		OUTPUT_SUMMARY_FILE_x,
		OUTPUT_SUMMARY_FORMAT_x,
		OUTPUT_SUMMARY_TIME_FORMAT_x,
		OUTPUT_SUMMARY_INCREMENT_x,
		OUTPUT_SUMMARY_TIME_RANGE_x,
		OUTPUT_SUMMARY_LINK_RANGE_x,
		OUTPUT_SUMMARY_VEH_TYPES_x,
		OUTPUT_SUMMARY_TURN_FLAG_x,
		OUTPUT_SUMMARY_PCE_FLAG_x,
		OUTPUT_SUMMARY_PERSON_FLAG_x,
		OUTPUT_SUMMARY_COORDINATES_x,
		NULL
	};
	if (exe != NULL) {
		exe->Key_List (keys);
	}
}

//---------------------------------------------------------
//	Read_Control
//---------------------------------------------------------

void Summary_Output::Read_Control (int steps_per_second)
{
	int i, num;

	if (exe == NULL || steps_per_second < 1) return;

	Network_Service *net = (Network_Service *) exe;

	step_time = net->Round (1.0 / steps_per_second);

	//---- read the output delay data ----

	num = exe->Highest_Control_Group (OUTPUT_SUMMARY_FILE_x, 0);

	if (num != 0) {
		Output_Data *output;
		char *str_ptr, buffer [FIELD_BUFFER];
		int increment;

		//---- allocate array space ----

		if (!output_data.Max_Records (num)) {
			exe->Error ("Insufficient Memory for %d Output Summary Groups", num);
		}

		//---- process each file ----

		for (i=1; i <= num; i++) {

			str_ptr = exe->Get_Control_String (OUTPUT_SUMMARY_FILE_x, i);

			if (str_ptr == NULL) continue;
			exe->Print (1);

			char *name_ptr = exe->Project_Filename (str_ptr);

			output = (Output_Data *) output_data.New_Record ();

			output->time_range = new Time_Range (steps_per_second);
			output->link_range = NULL;
			output->summary_data = NULL;
			output->connect_sum = NULL;
			output->turn_flag = false;
			output->coord_flag = false;
			output->pce_flag = false;

			output->file = new Delay_File (Db_Code::CREATE);

			str_fmt (buffer, sizeof (buffer), "Output Summary File #%d", i);

			output->file->File_Type (buffer);

			//---- set the output format ----

			str_ptr = exe->Get_Control_String (OUTPUT_SUMMARY_FORMAT_x, i);

			if (str_ptr != NULL) {
				output->file->Dbase_Format (str_ptr);
			}
			output->file->Open (name_ptr);

			//---- get the time format ----

			str_ptr = exe->Get_Control_String (OUTPUT_SUMMARY_TIME_FORMAT_x, i);

			if (str_ptr != NULL) {
				if (!output->time_range->Format (str_ptr)) {
					exe->Error ("Output Summary Time Format %s was Unrecognized", str_ptr);
				}
				exe->Print (1, "Output Summary Time Format #%d = %s", i, output->time_range->Time_Code ());
			}

			//---- get the increment ----

			str_ptr = exe->Get_Control_String (OUTPUT_SUMMARY_INCREMENT_x, i);

			if (str_ptr != NULL) {
				output->time_range->Increment (str_ptr);
				increment = output->time_range->Increment ();

				if (increment < 1 || increment > output->time_range->Duration ("1:00")) {
					exe->Error ("Output Summary Increment %s is Out of Range", 
						output->time_range->Format_Duration (increment));
				}
			} else {
				output->time_range->Increment ("0:15");
				increment = output->time_range->Increment ();
			}
			exe->Print (1, "Output Summary Increment #%d = %s", i, 
				output->time_range->Format_Duration (increment));

			if (output->time_range->Format () == Time_Step::SECONDS) {
				exe->Print (0, " second%c", (increment != output->time_range->Duration (1) ? 's' : ' '));
			} else if (output->time_range->Format () == Time_Step::HOURS) {
				exe->Print (0, " hour%c", (increment != output->time_range->Duration (1.0) ? 's' : ' '));
			}

			//---- get the time range ----

			str_ptr = exe->Get_Control_String (OUTPUT_SUMMARY_TIME_RANGE_x, i);

			if (str_ptr != NULL) {
				if (!output->time_range->Add_Ranges (str_ptr)) {
					exe->File_Error ("Output Summary Link Range", str_ptr);
				}
				exe->Print (1, "Output Summary Time Range #%d = ", i);

				for (int j=1; j <= output->time_range->Num_Ranges (); j++) {
					str_ptr = output->time_range->Range_Format (j);

					if (str_ptr != NULL) {
						exe->Print (0, "%s ", str_ptr);
					}
				}
				if (output->time_range->Format () == Time_Step::SECONDS) {
					exe->Print (0, "seconds");
				} else if (output->time_range->Format () == Time_Step::HOURS) {
					exe->Print (0, "hours");
				}
			} else {
				output->time_range->Add_Ranges ("All");
			}

			//---- get the link range ----

			str_ptr = exe->Get_Control_String (OUTPUT_SUMMARY_LINK_RANGE_x, i);

			if (str_ptr != NULL) {
				exe->Print (1, "Output Summary Link Range #%d = %s", i, str_ptr);

				output->link_range = new Data_Range ();

				if (!output->link_range->Add_Ranges (str_ptr)) {
					exe->File_Error ("Output Summary Link Range", str_ptr);
				}
			}

			//---- get the veh types ----

			str_ptr = exe->Get_Control_String (OUTPUT_SUMMARY_VEH_TYPES_x, i);

			if (str_ptr != NULL) {
				exe->Print (1, "Output Summary Vehicle Types #%d = %s", i, str_ptr);

				output->veh_types = new Data_Range ();

				if (!output->veh_types->Add_Ranges (str_ptr)) {
					exe->File_Error ("Output Summary Vehicle Types", str_ptr);
				}
				veh_type_flag = true;
			}

			//---- get the turn flag ----

			str_ptr = exe->Get_Control_String (OUTPUT_SUMMARY_TURN_FLAG_x, i);

			if (str_ptr != NULL) {
				exe->Print (1, "Output Summary Turn Flag #%d = %s", i, str_ptr);

				output->turn_flag = exe->Get_Control_Flag (OUTPUT_SUMMARY_TURN_FLAG_x, i);
			}

			//---- get the pce flag ----

			str_ptr = exe->Get_Control_String (OUTPUT_SUMMARY_PCE_FLAG_x, i);

			if (str_ptr != NULL) {
				exe->Print (1, "Output Summary PCE Flag #%d = %s", i, str_ptr);

				output->pce_flag = exe->Get_Control_Flag (OUTPUT_SUMMARY_PCE_FLAG_x, i);
			}

			//---- get the person flag ----

			str_ptr = exe->Get_Control_String (OUTPUT_SUMMARY_PERSON_FLAG_x, i);

			if (str_ptr != NULL) {
				exe->Print (1, "Output Summary Person Flag #%d = %s", i, str_ptr);

				output->person_flag = exe->Get_Control_Flag (OUTPUT_SUMMARY_PERSON_FLAG_x, i);
			}

			//---- get the coordinates ----

			str_ptr = exe->Get_Control_String (OUTPUT_SUMMARY_COORDINATES_x, i);

			if (str_ptr != NULL) {
				double dvalue;
				Network_Service *net = (Network_Service *) exe;

				exe->Print (1, "Output Summary Coordinates #%d = %s", i, str_ptr);

				str_ptr = Get_Double (str_ptr, &dvalue);
				output->x1 = net->Round (dvalue);

				if (str_ptr == NULL) goto coord_error;

				str_ptr = Get_Double (str_ptr, &dvalue);
				output->y1 = net->Round (dvalue);

				if (str_ptr == NULL) goto coord_error;

				str_ptr = Get_Double (str_ptr, &dvalue);
				output->x2 = net->Round (dvalue);
				if (output->x2 < output->x1) {
					int x = output->x2;
					output->x2 = output->x1;
					output->x1 = x;
				}

				if (str_ptr == NULL) goto coord_error;

				str_ptr = Get_Double (str_ptr, &dvalue);
				output->y2 = net->Round (dvalue);
				if (output->y2 < output->y1) {
					int y = output->y2;
					output->y2 = output->y1;
					output->y1 = y;
				}
				output->coord_flag = true;
			}

			if (output->file->Dbase_Format () == Db_Code::VERSION3) {
				if (output->turn_flag) {
					exe->Error ("Output Summary Turns and VERSION3 Format are Incompatible");
				}
				output->time_range->Format (Time_Step::SECONDS);

				output->file->Increment (output->time_range->Increment () / output->time_range->Step_Size ());

				output->file->Create_Header ();
			}

			//---- initialize the summary data ----

			output->summary_data = new Data_Array (sizeof (Summary_Data));

			if (output->summary_data == NULL) {
				exe->Error ("Creating Summary Data");
			}
			if (output->turn_flag) {
				output->connect_sum = new Data_Array (sizeof (Connect_Sum));

				if (output->connect_sum == NULL) {
					exe->Error ("Creating Summary Turn Data");
				}
			}
			output->step = -1;

			//---- save the output record ----

			if (!output_data.Add ()) {
				exe->Error ("Adding Summary Output Group");
			}
			output_flag = true;
		}
	}
	return;

coord_error:
	exe->Error ("Output Summary Coordinate Data #%d", i);
}

//---------------------------------------------------------
//	In_Range
//---------------------------------------------------------

bool Summary_Output::In_Range (int step, int dir, int type)
{
	if (output_flag) {
		Output_Data *output;
		veh_type = type;

		for (output = (Output_Data *) output_data.First (); output; output = (Output_Data *) output_data.Next ()) {
			if (output->veh_types != NULL && veh_type > 0) {
				if (!output->veh_types->In_Range (veh_type)) continue;
			}
			if (output->time_range->In_Range (step)) {
				Network_Service *net = (Network_Service *) exe;
				Dir_Data *dir_ptr = net->dir_array->Record (dir);

				if (dir_ptr != NULL) {
					Link_Data *link_ptr = net->link_array->Record (dir_ptr->Link ());

					if (link_ptr != NULL) {
						if (output->link_range != NULL) {
							if (!output->link_range->In_Range (link_ptr->Link ())) continue;
						}
						if (output->coord_flag) {
							Node_Data *node_ptr;

							node_ptr = net->node_array->Record (link_ptr->Anode ());
							if (node_ptr->X () < output->x1 || node_ptr->X () > output->x2 ||
								node_ptr->Y () < output->y1 || node_ptr->Y () > output->y2) {

								node_ptr = net->node_array->Record (link_ptr->Bnode ());

								if (node_ptr->X () < output->x1 || node_ptr->X () > output->x2 ||
									node_ptr->Y () < output->y1 || node_ptr->Y () > output->y2) {
									continue;
								}
							}
						}
					}
				}
				return (true);
			}
		}
	}
	return (false);
}

//---------------------------------------------------------
//	Output_Check
//---------------------------------------------------------

void Summary_Output::Output_Check (int step)
{
	if (!output_flag) return;

	Output_Data *output;
	Summary_Data *data_ptr;

	for (output = (Output_Data *) output_data.First (); output; output = (Output_Data *) output_data.Next ()) {
		if (output->time_range->In_Range (step)) {

			//---- initialize the link data ----

			Network_Service *net = (Network_Service *) exe;

			if (output->step < 0) {
				if (!output->summary_data->Num_Records (net->dir_array->Num_Records ())) {
					exe->Error ("Insufficient Memory for Summary Data");
				}
				if (output->turn_flag) {
					if (!output->connect_sum->Num_Records (net->connect_array->Num_Records ())) {
						exe->Error ("Insufficient Memory for Summary Turn Data");
					}
				}
			} else {

				//---- update the density data ----

				for (data_ptr = (Summary_Data *) output->summary_data->First (); data_ptr;
					data_ptr = (Summary_Data *) output->summary_data->Next ()) {

					if (data_ptr->veh_step > 0) {
						if (data_ptr->max_density < data_ptr->veh_step) {
							data_ptr->max_density = data_ptr->veh_step;
						}
						data_ptr->density += data_ptr->veh_step;
						data_ptr->veh_step = 0;

						if (data_ptr->max_queue < data_ptr->num_stop) {
							data_ptr->max_queue = data_ptr->num_stop;
						}
						data_ptr->queue += data_ptr->num_stop;
						data_ptr->num_stop = 0;
					}
				}

				//---- check the output increment ----

				if (output->time_range->At_Increment (step)) {
					Write_Summary (step, output);
				}
			}
			output->step = step;
		}
	}
}

//---------------------------------------------------------
//	Summarize
//---------------------------------------------------------

void Summary_Output::Summarize (int step)
{
	if (!output_flag || from_dir <= 0 || to_dir <= 0) return;

	Summary_Data *data_ptr;
	Connect_Sum *sum_ptr;

	Output_Data *output = (Output_Data *) output_data.Record ();

	if (output == NULL) return;

	//---- validate the offset data ----

	int vehicles;
	bool link_flag;
	Dir_Data *from_ptr, *to_ptr;
	Link_Data *from_link, *to_link;
	Network_Service *net = (Network_Service *) exe;

	from_ptr = net->dir_array->Record (from_dir);
	from_link = net->link_array->Record (from_ptr->Link ());

	if (from_offset < 0) {
		from_offset = 0;
	} else if (from_offset > from_link->Length ()) {
		from_offset = from_link->Length ();
	}

	if (to_dir != from_dir) {
		to_ptr = net->dir_array->Record (to_dir);
		to_link = net->link_array->Record (to_ptr->Link ());
	} else {
		to_ptr = from_ptr;
		to_link = from_link;
	}

	if (to_offset < 0) {
		to_offset = 0;
	} else if (to_offset > to_link->Length ()) {
		to_offset = to_link->Length ();
	}

	//---- sum the data ----

	for (; output; output = (Output_Data *) output_data.Next ()) {
		if (output->veh_types != NULL && veh_type > 0) {
			if (!output->veh_types->In_Range (veh_type)) continue;
		}
		if (output->time_range->In_Range (step)) {

			link_flag = (output->link_range != NULL && output->link_range->Num_Ranges () > 0);

			if (output->person_flag && veh_occ > 0) {
				vehicles = veh_occ;
			} else {
				vehicles = ((output->pce_flag) ? veh_pce : 1);
			}
			if (from_dir != to_dir) {
				int time, distance;

				time = step_time;
				distance = from_link->Length () - from_offset + to_offset;

				if (distance > 0) {
					time = (time * to_offset + distance / 2) / distance;
				}
				if (!link_flag || output->link_range->In_Range (to_link->Link ())) {
					if (output->coord_flag) {
						Node_Data *node_ptr;

						if (to_ptr->Dir () == 0) {
							node_ptr = net->node_array->Record (to_link->Bnode ());
						} else {
							node_ptr = net->node_array->Record (to_link->Anode ());
						}
						if (node_ptr->X () < output->x1 || node_ptr->X () > output->x2 ||
							node_ptr->Y () < output->y1 || node_ptr->Y () > output->y2) goto from_check;
					}
					data_ptr = (Summary_Data *) output->summary_data->Record (to_dir);

					data_ptr->veh_enter += vehicles;
					data_ptr->veh_step += vehicles;
					data_ptr->distance += to_offset * vehicles;
					data_ptr->time += time * vehicles;

					if (to_connect < 0) {
						data_ptr->veh_exit += vehicles;
					} else if (output->turn_flag && to_connect > 0) {
						sum_ptr = (Connect_Sum *) output->connect_sum->Record (to_connect);

						sum_ptr->distance += to_offset * vehicles;
						sum_ptr->time += time * vehicles;
					}
				}
from_check:
				if (!link_flag || output->link_range->In_Range (from_link->Link ())) {
					if (output->coord_flag) {
						Node_Data *node_ptr;

						if (from_ptr->Dir () == 0) {
							node_ptr = net->node_array->Record (from_link->Bnode ());
						} else {
							node_ptr = net->node_array->Record (from_link->Anode ());
						}
						if (node_ptr->X () < output->x1 || node_ptr->X () > output->x2 ||
							node_ptr->Y () < output->y1 || node_ptr->Y () > output->y2) continue;
					}
					data_ptr = (Summary_Data *) output->summary_data->Record (from_dir);

					data_ptr->veh_exit += vehicles;
					data_ptr->veh_step += vehicles;
					data_ptr->distance += (distance - to_offset) * vehicles;
					data_ptr->time += (step_time - time) * vehicles;

					if (from_connect < 0) {
						data_ptr->veh_enter += vehicles;
					} else if (output->turn_flag && from_connect > 0) {
						sum_ptr = (Connect_Sum *) output->connect_sum->Record (from_connect);

						sum_ptr->volume += vehicles;
						sum_ptr->distance += (distance - to_offset) * vehicles;
						sum_ptr->time += (step_time - time) * vehicles;
					}
				}

			} else if (!link_flag || output->link_range->In_Range (to_link->Link ())) {
				if (output->coord_flag) {
					Node_Data *node_ptr;

					if (to_ptr->Dir () == 0) {
						node_ptr = net->node_array->Record (to_link->Bnode ());
					} else {
						node_ptr = net->node_array->Record (to_link->Anode ());
					}
					if (node_ptr->X () < output->x1 || node_ptr->X () > output->x2 ||
						node_ptr->Y () < output->y1 || node_ptr->Y () > output->y2) continue;
				}
				data_ptr = (Summary_Data *) output->summary_data->Record (to_dir);

				data_ptr->veh_step += vehicles;
				data_ptr->distance += (to_offset - from_offset) * vehicles;
				data_ptr->time += step_time * vehicles;

				if (from_speed == 0 && to_speed == 0) {
					data_ptr->num_stop += vehicles;
				}
				if (from_connect < 0) {
					data_ptr->veh_enter += vehicles;
				}
				if (to_connect < 0) {
					data_ptr->veh_exit += vehicles;
				} else if (output->turn_flag && to_connect > 0) {
					sum_ptr = (Connect_Sum *) output->connect_sum->Record (to_connect);

					sum_ptr->distance += (to_offset - from_offset) * vehicles;
					sum_ptr->time += step_time * vehicles;
				}
			}
		}
	}
}

//---------------------------------------------------------
//	Cycle_Failure
//---------------------------------------------------------

void Summary_Output::Cycle_Failure (int step, int dir, int vehicles, int persons, int veh_type)
{
	if (!output_flag || dir <= 0 || vehicles <= 0) return;

	Summary_Data *data_ptr;

	Output_Data *output = (Output_Data *) output_data.Record ();

	if (output == NULL) return;

	//---- get the link data ----

	bool link_flag;
	Dir_Data *dir_ptr = NULL;
	Link_Data *link_ptr = NULL;
	Network_Service *net = (Network_Service *) exe;

	//---- sum the data ----

	for (; output; output = (Output_Data *) output_data.Next ()) {
		if (output->veh_types != NULL && veh_type > 0) {
			if (!output->veh_types->In_Range (veh_type)) continue;
		}
		if (output->time_range->In_Range (step)) {

			link_flag = (output->link_range != NULL && output->link_range->Num_Ranges () > 0);

			if (link_flag && link_ptr == NULL) {
				dir_ptr = net->dir_array->Record (dir);
				link_ptr = net->link_array->Record (dir_ptr->Link ());
			}
			if (!link_flag || output->link_range->In_Range (link_ptr->Link ())) {
				if (output->coord_flag) {
					Node_Data *node_ptr;

					if (dir_ptr->Dir () == 0) {
						node_ptr = net->node_array->Record (link_ptr->Bnode ());
					} else {
						node_ptr = net->node_array->Record (link_ptr->Anode ());
					}
					if (node_ptr->X () < output->x1 || node_ptr->X () > output->x2 ||
						node_ptr->Y () < output->y1 || node_ptr->Y () > output->y2) continue;
				}
				data_ptr = (Summary_Data *) output->summary_data->Record (dir);

				if (output->person_flag) {
					data_ptr->num_fail += persons;
				} else {
					data_ptr->num_fail += vehicles;
				}
			}
		}
	}
}

//---------------------------------------------------------
//	Close
//---------------------------------------------------------

void Summary_Output::Close (void)
{
	if (!output_flag) return;

	Output_Data *output;

	for (output = (Output_Data *) output_data.First (); output; output = (Output_Data *) output_data.Next ()) {
		if (output->step >= 0 && !output->time_range->At_Increment (output->step)) {
			output->step = (output->step / output->time_range->Increment () + 1) * output->time_range->Increment ();

			Write_Summary (output->step, output);
		}
		if (output->file != NULL) {
			output->file->Close ();
			delete output->file;
			output->file = NULL;
		}
		if (output->summary_data != NULL) {
			delete output->summary_data;
			output->summary_data = NULL;
		}
		if (output->turn_flag && output->connect_sum != NULL) {
			delete output->connect_sum;
			output->connect_sum = NULL;
		}
		if (output->link_range != NULL) {
			delete output->link_range;
			output->link_range = NULL;
		}
		if (output->veh_types != NULL) {
			delete output->veh_types;
			output->veh_types = NULL;
		}
		output->step = -1;
	}
}

//---------------------------------------------------------
//	Write_Summary
//---------------------------------------------------------

void Summary_Output::Write_Summary (int step, Output_Data *output)
{
	int i, volume, index, num, time [20], turn_vol [20], link_dir [20], tim, increment;
	double speed, ttime, density, length, factor;

	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Summary_Data *data_ptr;
	Connect_Sum *sum_ptr;
	Connect_Data *connect_ptr;
	Network_Service *net = (Network_Service *) exe;

	increment = output->time_range->Increment ();
	if (increment < 1) increment = 1;

	//---- output the current increment ----

	for (data_ptr = (Summary_Data *) output->summary_data->First (); data_ptr;
		data_ptr = (Summary_Data *) output->summary_data->Next ()) {

		if (data_ptr->time == 0) continue;

		dir_ptr = net->dir_array->Record (output->summary_data->Record_Index ());
		link_ptr = net->link_array->Record (dir_ptr->Link ());

		if (output->link_range != NULL && output->link_range->Num_Ranges () > 0) {
			if (!output->link_range->In_Range (link_ptr->Link ())) continue;
		}

		//---- save the link direction data ----

		if (output->file->LinkDir_Type () == Db_Code::LINK_SIGN) {
			if (dir_ptr->Dir () == 1) {
				output->file->Link (-link_ptr->Link ());
			} else {
				output->file->Link (link_ptr->Link ());
			}
		} else {
			output->file->Link (link_ptr->Link ());	

			if (output->file->LinkDir_Type () == Db_Code::LINK_NODE) {
				int node = (dir_ptr->Dir ()) ? link_ptr->Bnode () : link_ptr->Anode ();
				Node_Data *node_ptr = net->node_array->Record (node);

				output->file->Dir (node_ptr->Node ());
			} else {
				output->file->Dir (dir_ptr->Dir ());
			}
		}

		//---- calculate the performance ----

		speed = (double) data_ptr->distance / data_ptr->time;
		if (speed < 0.5) speed = 0.5;

		length = net->UnRound (link_ptr->Length ());
		if (length < 0.1) length = 0.1;

		ttime = length / speed;
		if (ttime <= 0.1) ttime = 0.1;

		volume = (data_ptr->veh_enter + data_ptr->veh_exit + 1) / 2;
		//volume = MAX (data_ptr->veh_enter, data_ptr->veh_exit);
		if (volume < 1) volume = 1;

		density = (double) data_ptr->density / increment;
		if (density < 0.01) density = 0.01;

		//---- save the Version 3 fields ----

		if (output->file->Dbase_Format () == Db_Code::VERSION3) {

			output->file->Time (step / output->time_range->Step_Size ());
			output->file->Lane (dir_ptr->Left () + 1);
			output->file->Turn (0);

			output->file->Volume (volume);
			output->file->TTime (volume * ttime);
			output->file->TTime2 (volume * ttime * ttime);

			output->file->Vehicles ((int) (density + 0.5));
			output->file->Speed (density * speed);
			output->file->Speed2 (density * speed * speed);

			if (!output->file->Write ()) goto write_error;

			memset (data_ptr, '\0', sizeof (Summary_Data));

		} else {

			output->file->Start_Time (output->time_range->Format_Step (step - output->time_range->Increment ()));
			output->file->End_Time (output->time_range->Format_Step (step));

			output->file->Volume (volume);
			output->file->Enter (data_ptr->veh_enter);
			output->file->Exit (data_ptr->veh_exit);

			output->file->Speed (speed);
			output->file->TTime (ttime);

			factor = dir_ptr->Thru ();
			if (factor < 1) factor = 1;

			factor = 1000.0 / (length * factor);

			density *= factor;
			if (density > 0.0 && density < 0.01) density = 0.01;
			output->file->Density (density);

			density = (double) data_ptr->max_density * factor;
			if (density > 0.0 && density < 0.01) density = 0.01;
			output->file->Max_Density (density);

			output->file->Avg_Queue ((double) data_ptr->queue / increment);
			output->file->Max_Queue (data_ptr->max_queue);
			output->file->Cycle_Failure (data_ptr->num_fail);

			factor = net->UnRound (dir_ptr->Time0 ());
			if (factor < 0.1) factor = 0.1;

			output->file->Delay (ttime - factor);
			output->file->Time_Ratio (ttime / factor);

			output->file->VMT (net->UnRound (data_ptr->distance));
			output->file->VHT (net->UnRound (data_ptr->time));

			memset (data_ptr, '\0', sizeof (Summary_Data));

			if (!output->turn_flag) {
				output->file->Num_Connect (0);
				if (!output->file->Write (false)) goto write_error;
				continue;
			}

			//---- summarize the connection data ----

			num = 0;
			index = dir_ptr->Link_Dir ();

			for (connect_ptr = net->connect_array->Get_GE (index, 0); connect_ptr; connect_ptr = net->connect_array->Next_Key ()) {
				if (connect_ptr->In_Link_Dir () != dir_ptr->Link_Dir () || num == 20) break;

				index = net->connect_array->Record_Index ();
				sum_ptr = (Connect_Sum *) output->connect_sum->Record (index);

				if (sum_ptr->time == 0) continue;

				speed = (double) sum_ptr->distance / sum_ptr->time;
				if (speed < 0.5) speed = 0.5;

				ttime = length / speed;
				tim = net->Round (ttime);
				if (tim < 1) tim = 1;

				time [num] = tim;
				turn_vol [num] = sum_ptr->volume;
				link_dir [num] = connect_ptr->Out_Link_Dir ();
				num++;

				memset (sum_ptr, '\0', sizeof (Connect_Sum));
			}
			if (num < 1) {
				output->file->Num_Connect (0);
				if (!output->file->Write (false)) goto write_error;
				continue;
			}
			output->file->Num_Connect (num);

			if (!output->file->Write (false)) goto write_error;

			for (i=0; i < num; i++) {
				index = link_dir [i];

				link_ptr = net->link_array->Record ((index >> 1));

				output->file->Out_Link (link_ptr->Link ());
				output->file->Out_Dir ((index & 1));
				output->file->Out_Turn (turn_vol [i]);
				output->file->Out_Time (net->UnRound (time [i]));

				if (!output->file->Write (true)) goto write_error;
			}
		}
	}
	return;

write_error:
	exe->Error ("Writing Summary Output File");
}

