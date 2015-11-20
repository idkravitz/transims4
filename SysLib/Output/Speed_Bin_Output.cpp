//*********************************************************
//	Speed_Bin_Output.cpp - Output Interface Class
//*********************************************************

#include "Speed_Bin_Output.hpp"

#include "Network_Service.hpp"
#include "Demand_Service.hpp"
#include "Utility.hpp"

char * Speed_Bin_Output::OUTPUT_SPEED_FILE_x			= "OUTPUT_SPEED_FILE_*";
char * Speed_Bin_Output::OUTPUT_SPEED_FORMAT_x			= "OUTPUT_SPEED_FORMAT_*";
char * Speed_Bin_Output::OUTPUT_SPEED_VEHICLE_TYPE_x	= "OUTPUT_SPEED_VEHICLE_TYPE_*";
char * Speed_Bin_Output::OUTPUT_SPEED_FILTER_x			= "OUTPUT_SPEED_FILTER_*";
char * Speed_Bin_Output::OUTPUT_SPEED_TIME_FORMAT_x		= "OUTPUT_SPEED_TIME_FORMAT_*";
char * Speed_Bin_Output::OUTPUT_SPEED_INCREMENT_x		= "OUTPUT_SPEED_INCREMENT_*";
char * Speed_Bin_Output::OUTPUT_SPEED_TIME_RANGE_x		= "OUTPUT_SPEED_TIME_RANGE_*";
char * Speed_Bin_Output::OUTPUT_SPEED_LINK_RANGE_x		= "OUTPUT_SPEED_LINK_RANGE_*";
char * Speed_Bin_Output::OUTPUT_SPEED_SAMPLE_TIME_x		= "OUTPUT_SPEED_SAMPLE_TIME_*";
char * Speed_Bin_Output::OUTPUT_SPEED_BOX_LENGTH_x		= "OUTPUT_SPEED_BOX_LENGTH_*";
char * Speed_Bin_Output::OUTPUT_SPEED_NUM_BINS_x		= "OUTPUT_SPEED_NUM_BINS_*";

//---------------------------------------------------------
//	Speed_Bin_Output constructor
//---------------------------------------------------------

Speed_Bin_Output::Speed_Bin_Output (void) : Static_Service ()
{
	output_data.Initialize (sizeof (Output_Data), 10);

	output_flag = false;
	dir = offset = speed = veh_type = 0;
}

//---------------------------------------------------------
//	Speed_Bin_Output destructor
//---------------------------------------------------------

Speed_Bin_Output::~Speed_Bin_Output (void)
{
	Close ();
}

//---------------------------------------------------------
//	Add_Keys
//---------------------------------------------------------

void Speed_Bin_Output::Add_Keys (void)
{
	char *keys [] = {
		OUTPUT_SPEED_FILE_x,
		OUTPUT_SPEED_FORMAT_x,
		OUTPUT_SPEED_VEHICLE_TYPE_x,
		OUTPUT_SPEED_FILTER_x,
		OUTPUT_SPEED_TIME_FORMAT_x,
		OUTPUT_SPEED_INCREMENT_x,
		OUTPUT_SPEED_TIME_RANGE_x,
		OUTPUT_SPEED_LINK_RANGE_x,
		OUTPUT_SPEED_SAMPLE_TIME_x,
		OUTPUT_SPEED_BOX_LENGTH_x,
		OUTPUT_SPEED_NUM_BINS_x,
		NULL
	};

	if (exe != NULL) {
		exe->Key_List (keys);
	}
}

//---------------------------------------------------------
//	Read_Control
//---------------------------------------------------------

void Speed_Bin_Output::Read_Control (int steps_per_second, double _cell_size)
{
	int i, j, bins;

	if (exe == NULL || steps_per_second < 1 || _cell_size < 1.0) return;

	cell_size = _cell_size;
					
	Network_Service *net = (Network_Service *) exe;

	//---- read the output delay data ----

	int num = exe->Highest_Control_Group (OUTPUT_SPEED_FILE_x, 0);

	if (num != 0) {
		Output_Data *output;
		char *str_ptr, buffer [FIELD_BUFFER], *ptr;
		int increment;

		//---- allocate array space ----

		if (!output_data.Max_Records (num)) {
			exe->Error ("Insufficient Memory for %d Output Speed Groups", num);
		}

		//---- process each file ----

		for (i=1; i <= num; i++) {

			str_ptr = exe->Get_Control_String (OUTPUT_SPEED_FILE_x, i);

			if (str_ptr == NULL) continue;

			char *name_ptr = exe->Project_Filename (str_ptr);

			output = (Output_Data *) output_data.New_Record ();

			output->time_range = new Time_Range (steps_per_second);
			output->link_range = NULL;
			output->speed_bin = NULL;

			output->file = new Speed_Bin_File (Db_Code::CREATE);

			exe->Break_Check (8);
			exe->Print (1);	
			str_fmt (buffer, sizeof (buffer), "Output Speed File #%d", i);

			output->file->File_Type (buffer);

			//---- set the output format ----

			str_ptr = exe->Get_Control_String (OUTPUT_SPEED_FORMAT_x, i);

			if (str_ptr != NULL) {
				output->file->Dbase_Format (str_ptr);
			}
			output->file->Open (name_ptr);

			//---- get the vehicle type ----

			str_ptr = exe->Get_Control_String (OUTPUT_SPEED_VEHICLE_TYPE_x, i);

			if (str_ptr != NULL) {
				str_ptr = str_cpy (buffer, sizeof (buffer), str_ptr);
				ptr = strchr (str_ptr, ':');

				if (ptr != NULL) {
					*ptr++ = '\0';
				}
				if (*str_ptr >= '0' && *str_ptr <= '9') {
					output->veh_type = atoi (str_ptr);
				} else {
					output->veh_type = net->Veh_Use_Code (str_ptr);
				}
				if (output->veh_type < 0) {
					exe->Error ("Output Speed Vehicle Type %d is Out of Range (>= 0)", output->veh_type);
				}
				if (ptr != NULL) {
					output->subtype = atoi (ptr);
				} else {
					output->subtype = 0;
				}
			} else {
				output->veh_type = 0;
				output->subtype = 0;
			}
			output->veh_code = 0;

			if (output->veh_type == 0) {
				exe->Print (1, "Output Speed Vehicle Type #%d = All Vehicle Types", i);
			} else {
				str_ptr = exe->Get_Control_String (OUTPUT_SPEED_VEHICLE_TYPE_x, i);

				exe->Print (1, "Output Speed Vehicle Type #%d = %s", i, str_ptr);
			}

			//---- get the filter ----

			str_ptr = exe->Get_Control_String (OUTPUT_SPEED_FILTER_x, i);

			if (str_ptr != NULL) {
				output->filter = atoi (str_ptr);

				if (output->filter < 1) {
					exe->Error ("Output Speed Filter %d is Out of Range (>= 1)", output->filter);
				}
			} else {
				output->filter = 1;
			}

			exe->Print (1, "Output Speed Filter #%d = %d vehicle%c", i, output->filter,
				((output->filter != 1) ? 's' : ' '));

			//---- get the time format ----

			str_ptr = exe->Get_Control_String (OUTPUT_SPEED_TIME_FORMAT_x, i);

			if (str_ptr != NULL) {
				if (!output->time_range->Format (str_ptr)) {
					exe->Error ("Output Speed Time Format %s was Unrecognized", str_ptr);
				}
				exe->Print (1, "Output Speed Time Format #%d = %s", i, output->time_range->Time_Code ());
			}

			//---- get the increment ----

			str_ptr = exe->Get_Control_String (OUTPUT_SPEED_INCREMENT_x, i);

			if (str_ptr != NULL) {
				output->time_range->Increment (str_ptr);
				increment = output->time_range->Increment ();

				if (increment < 1) {
					exe->Error ("Output Speed Increment %s is Out of Range", 
						output->time_range->Format_Duration (increment));
				}
			} else {
				output->time_range->Increment ("24:00");
				increment = output->time_range->Increment ();
			}
			exe->Print (1, "Output Speed Increment #%d = %s", i, 
				output->time_range->Format_Duration (increment));

			if (output->time_range->Format () == Time_Step::SECONDS) {
				exe->Print (0, " second%c", (increment != output->time_range->Duration (1) ? 's' : ' '));
			} else if (output->time_range->Format () == Time_Step::HOURS) {
				exe->Print (0, " hour%c", (increment != output->time_range->Duration (1.0) ? 's' : ' '));
			}

			//---- get the time range ----

			str_ptr = exe->Get_Control_String (OUTPUT_SPEED_TIME_RANGE_x, i);

			if (str_ptr != NULL) {
				if (!output->time_range->Add_Ranges (str_ptr)) {
					exe->File_Error ("Output Speed Time Range", str_ptr);
				}
				exe->Print (1, "Output Speed Time Range #%d = ", i);

				for (j=1; j <= output->time_range->Num_Ranges (); j++) {
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

			str_ptr = exe->Get_Control_String (OUTPUT_SPEED_LINK_RANGE_x, i);

			if (str_ptr != NULL) {
				exe->Print (1, "Output Speed Link Range #%d = %s", i, str_ptr);

				output->link_range = new Data_Range ();

				if (!output->link_range->Add_Ranges (str_ptr)) {
					exe->File_Error ("Output Speed Link Range", str_ptr);
				}
			}

			//---- get the sample time ----

			str_ptr = exe->Get_Control_String (OUTPUT_SPEED_SAMPLE_TIME_x, i);

			if (str_ptr != NULL) {
				output->sample = atoi (str_ptr);

				if (output->sample < 1) {
					exe->Error ("Output Speed Sample Time %d is Out of Range (>= 1)", output->sample);
				}
			} else {
				output->sample = 1;
			}

			exe->Print (1, "Output Speed Sample Time #%d = %d", i, output->sample);

			//---- get the box length ----

			str_ptr = exe->Get_Control_String (OUTPUT_SPEED_BOX_LENGTH_x, i);

			if (str_ptr != NULL) {
				output->box_length = net->Round (atoi (str_ptr));

				if (output->box_length < 0) {
					exe->Error ("Output Speed Box Length %.1lf is Out of Range (>= 0)", 
						net->UnRound (output->box_length));
				}
			} else {
				output->box_length = 0;
			}
			if (output->box_length == 0) {
				exe->Print (1, "Output Speed Box Length #%d = Full Link Length", i);
			} else {
				exe->Print (1, "Output Speed Box Length #%d = %.1lf meters", i, 
					net->UnRound (output->box_length));
			}

			//---- get the num bins ----

			bins = (int) (80 * MPHTOMPS / cell_size + 0.5);

			str_ptr = exe->Get_Control_String (OUTPUT_SPEED_NUM_BINS_x, i);

			if (str_ptr != NULL) {
				output->num_bins = atoi (str_ptr);

				if (output->num_bins < 1) {
					exe->Error ("Output Speed Num Bins %d is Out of Range (>= 1)", output->num_bins);
				}
				if (output->num_bins < bins) {
					exe->Warning ("Output Speed Num Bins %d is Less than 80 mph", output->num_bins);
				}
			} else {
				output->num_bins = bins;
			}
			exe->Print (1, "Output Speed Number of Bins #%d = %d", i, output->num_bins);

			//----- initialize file metadata ----

			output->file->Box_Length (net->UnRound (output->box_length));
			output->file->Cell_Length (cell_size);
			output->file->Sample_Time (output->sample);
			output->file->Increment (increment);
			output->file->Vehicle_Type (output->veh_type);
			output->file->Veh_SubType (output->subtype);
			output->file->Num_Bins (output->num_bins);

			output->file->Clear_Fields ();
			output->file->Create_Fields ();

			output->file->Create_Header ();

			//---- initialize the summary data ----

			output->speed_bin = new Speed_Bin_Array ();

			if (output->speed_bin == NULL) {
				exe->Error ("Creating Speed Bin Data");
			}
			output->speed_bin->Num_Bins (output->num_bins);
			output->step = -1;

			//---- save the output record ----

			if (!output_data.Add ()) {
				exe->Error ("Adding Speed Output Group");
			}
			output_flag = true;
		}
	}
}

//---------------------------------------------------------
//	In_Range
//---------------------------------------------------------

bool Speed_Bin_Output::In_Range (int step, int dir, int veh_type)
{
	if (output_flag) {
		Output_Data *output;

		for (output = (Output_Data *) output_data.First (); output; output = (Output_Data *) output_data.Next ()) {
			if (output->veh_code == 0 || output->veh_code == veh_type) {
				if (output->time_range->In_Range (step)) {
					if (output->sample == 0 || (step % output->sample) == 0) {
						if (dir > 0 && output->link_range != NULL) {
							Network_Service *net = (Network_Service *) exe;
							Dir_Data *dir_ptr = net->dir_array->Record (dir);

							if (dir_ptr != NULL) {
								Link_Data *link_ptr = net->link_array->Record (dir_ptr->Link ());

								if (link_ptr != NULL) {
									if (!output->link_range->In_Range (link_ptr->Link ())) continue;
								}
							}
						}
						return (true);
					}
				}
			}
		}
	}
	return (false);
}

//---------------------------------------------------------
//	Output_Check
//---------------------------------------------------------

void Speed_Bin_Output::Output_Check (int step)
{
	if (!output_flag) return;

	Output_Data *output;

	for (output = (Output_Data *) output_data.First (); output; output = (Output_Data *) output_data.Next ()) {
		if (output->time_range->In_Range (step)) {
			if (output->step < 0) {

				//---- initialize the speed bins data ----

				Demand_Service *net = (Demand_Service *) exe;
				Link_Data *link_ptr;
				Speed_Bin_Data *bin_ptr;
				int i, nbins;

				//---- convert the vehicle type ----

				if (output->veh_type > 0) {
					Veh_Type_Data *veh_type_ptr;

					veh_type_ptr = net->veh_type_array->Get (output->veh_type, output->subtype);
				
					if (veh_type_ptr == NULL) {
						net->Error ("Vehicle Type %d-%d was Not Found in the Vehicle Type File", 
							output->veh_type, output->subtype);
					}
					output->veh_code = net->veh_type_array->Record_Index ();
				}

				//---- allocate memory ----

				nbins = 0;

				for (link_ptr = net->link_array->First (); link_ptr; link_ptr = net->link_array->Next ()) {
					if (output->link_range != NULL) {
						if (!output->link_range->In_Range (link_ptr->Link ())) continue;
					}
					if (output->box_length > 0) {
						i = (link_ptr->Length () + (3 * output->box_length / 4)) / output->box_length;
					} else {
						i = 1;
					}
					nbins += ((link_ptr->AB_Dir () > 0) ? i : 0) + ((link_ptr->BA_Dir () > 0) ? i : 0);
				}
				if (!output->speed_bin->Max_Records (nbins)) goto mem_error;

				//---- initialize the data records ----

				for (link_ptr = net->link_array->First (); link_ptr; link_ptr = net->link_array->Next ()) {
					if (output->link_range != NULL) {
						if (!output->link_range->In_Range (link_ptr->Link ())) continue;
					}
					if (output->box_length > 0) {
						nbins = (link_ptr->Length () + (3 * output->box_length / 4)) / output->box_length;
					} else {
						nbins = 1;
					}

					//---- AB direction boxes ---

					if (link_ptr->AB_Dir () > 0) {
						for (i=1; i <= nbins; i++) {
							bin_ptr = output->speed_bin->New_Record (true);

							bin_ptr->link_dir = link_ptr->AB_Dir ();

							if (i == nbins) {
								bin_ptr->offset = link_ptr->Length ();
							} else {
								bin_ptr->offset = i * output->box_length;
							}
							if (!output->speed_bin->Add ()) goto mem_error;
						}
					}

					//---- BA direction boxes ---

					if (link_ptr->BA_Dir () > 0) {
						for (i=1; i <= nbins; i++) {
							bin_ptr = output->speed_bin->New_Record (true);

							bin_ptr->link_dir = link_ptr->BA_Dir ();

							if (i == nbins) {
								bin_ptr->offset = link_ptr->Length ();
							} else {
								bin_ptr->offset = i * output->box_length;
							}
							if (!output->speed_bin->Add ()) goto mem_error;
						}
					}
				}

			} else {

				//---- check the output increment ----

				if (output->time_range->At_Increment (step)) {
					Write_Speed (step, output);
				}
			}
			output->step = step;
		}
	}
	return;

mem_error:
	exe->Error ("Insufficient Memory for Speed Bin Data");
}

//---------------------------------------------------------
//	Summarize
//---------------------------------------------------------

void Speed_Bin_Output::Summarize (int step)
{
	if (output_flag) {
		Speed_Bin_Data *bin_ptr, bin_rec;

		if (dir <= 0 || offset < 0 || veh_type < 0) {
			exe->Error ("Speed Bin Output Keys Undefined");
		}
		Output_Data *output = (Output_Data *) output_data.Record ();

		for (; output; output = (Output_Data *) output_data.Next ()) {
			if ((output->veh_code == 0 || output->veh_code == veh_type) && 
				output->time_range->In_Range (step) && 
				(output->sample == 0 || (step % output->sample) == 0)) {

				bin_rec.link_dir = dir;
				bin_rec.offset = offset;

				bin_ptr = output->speed_bin->Get_GE (&bin_rec);

				if (bin_ptr == NULL || bin_ptr->link_dir != dir) continue;

				if (speed >= output->speed_bin->Num_Bins ()) {
					speed = output->speed_bin->Num_Bins () - 1;
				} else if (speed < 0) {
					speed = 0;
				}
				bin_ptr->bin [speed]++;
			}
		}
	}
}

//---------------------------------------------------------
//	Close
//---------------------------------------------------------

void Speed_Bin_Output::Close (void)
{
	if (!output_flag) return;

	Output_Data *output;

	for (output = (Output_Data *) output_data.First (); output; output = (Output_Data *) output_data.Next ()) {
		if (output->step >= 0 && !output->time_range->At_Increment (output->step)) {
			output->step = (output->step / output->time_range->Increment () + 1) * output->time_range->Increment ();

			Write_Speed (output->step, output);
		}
		if (output->file != NULL) {
			output->file->Close ();
			delete output->file;
			output->file = NULL;
		}
		if (output->time_range != NULL) {
			delete output->time_range;
			output->time_range = NULL;
		}
		if (output->link_range != NULL) {
			delete output->link_range;
			output->link_range = NULL;
		}
		if (output->speed_bin != NULL) {
			delete output->speed_bin;
			output->speed_bin = NULL;
		}
		output->step = -1;
	}
	output_flag = false;
}

//---------------------------------------------------------
//	Write_Speed
//---------------------------------------------------------

void Speed_Bin_Output::Write_Speed (int step, Output_Data *output)
{
	Speed_Bin_Data *bin_ptr = NULL;
	Dir_Data *dir_ptr = NULL;
	Link_Data *link_ptr = NULL;
	Node_Data *node_ptr = NULL;

	int i, last_dir, link, total;

	link = last_dir = 0;
	Network_Service *net = (Network_Service *) exe;

	for (bin_ptr = output->speed_bin->First (); bin_ptr; bin_ptr = output->speed_bin->Next ()) {
		total = 0;

		for (i=0; i < output->speed_bin->Num_Bins (); i++) {
			total += bin_ptr->bin [i];
		}
		if (total < output->filter) {
			if (total > 0) {
				for (i=0; i < output->speed_bin->Num_Bins (); i++) {
					bin_ptr->bin [i] = 0;
				}
			}
			continue;
		}
		if (last_dir != bin_ptr->link_dir) {
			last_dir = bin_ptr->link_dir;

			dir_ptr = net->dir_array->Record (last_dir);

			link_ptr = net->link_array->Record (dir_ptr->Link ());

			link = link_ptr->Link ();
		}
		output->file->Link (link_ptr->Link ());

		if (output->file->Dbase_Format () == Db_Code::VERSION3 || output->file->LinkDir_Type () == net->LINK_NODE) {
			if (dir_ptr->Dir () == 0) {
				node_ptr = net->node_array->Record (link_ptr->Bnode ());
			} else {
				node_ptr = net->node_array->Record (link_ptr->Anode ());
			}
			if (node_ptr != NULL) {
				output->file->Dir (node_ptr->Node ());
			}
		} else {
			output->file->Dir (dir_ptr->Dir ());
		}
		output->file->Offset (net->UnRound (bin_ptr->offset));
		output->file->End_Time (output->time_range->Format_Step (step));

		for (i=0; i < output->speed_bin->Num_Bins (); i++) {
			output->file->Bin (i, bin_ptr->bin [i]);
			bin_ptr->bin [i] = 0;
		}
		if (!output->file->Write ()) {
			exe->Error ("Writing Speed Bin File");
		}
	}
}

