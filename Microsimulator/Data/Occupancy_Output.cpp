//*********************************************************
//	Occupancy_Output.cpp - Output Interface Class
//*********************************************************

#include "../Microsimulator.hpp"

char * Occupancy_Output::OUTPUT_OCCUPANCY_FILE_x		= "OUTPUT_OCCUPANCY_FILE_*";
char * Occupancy_Output::OUTPUT_OCCUPANCY_FORMAT_x		= "OUTPUT_OCCUPANCY_FORMAT_*";
char * Occupancy_Output::OUTPUT_OCCUPANCY_TIME_FORMAT_x	= "OUTPUT_OCCUPANCY_TIME_FORMAT_*";
char * Occupancy_Output::OUTPUT_OCCUPANCY_INCREMENT_x	= "OUTPUT_OCCUPANCY_INCREMENT_*";
char * Occupancy_Output::OUTPUT_OCCUPANCY_TIME_RANGE_x	= "OUTPUT_OCCUPANCY_TIME_RANGE_*";
char * Occupancy_Output::OUTPUT_OCCUPANCY_LINK_RANGE_x	= "OUTPUT_OCCUPANCY_LINK_RANGE_*";
char * Occupancy_Output::OUTPUT_OCCUPANCY_MAX_FLAG_x	= "OUTPUT_OCCUPANCY_MAX_FLAG_*";
char * Occupancy_Output::OUTPUT_OCCUPANCY_COORDINATES_x	= "OUTPUT_OCCUPANCY_COORDINATES_*";

//---------------------------------------------------------
//	Occupancy_Output constructor
//---------------------------------------------------------

Occupancy_Output::Occupancy_Output (void) : Static_Service ()
{
	output_data.Initialize (sizeof (Output_Data), 10);

	output_flag = false;
	cell_size = 7.5;
}

//---------------------------------------------------------
//	Occupancy_Output destructor
//---------------------------------------------------------

Occupancy_Output::~Occupancy_Output (void)
{
	Close ();
}

//---------------------------------------------------------
//	Add_Keys
//---------------------------------------------------------

void Occupancy_Output::Add_Keys (void)
{
	char *keys [] = {
		OUTPUT_OCCUPANCY_FILE_x,
		OUTPUT_OCCUPANCY_FORMAT_x,
		OUTPUT_OCCUPANCY_TIME_FORMAT_x,
		OUTPUT_OCCUPANCY_INCREMENT_x,
		OUTPUT_OCCUPANCY_TIME_RANGE_x,
		OUTPUT_OCCUPANCY_LINK_RANGE_x,
		OUTPUT_OCCUPANCY_MAX_FLAG_x,
		OUTPUT_OCCUPANCY_COORDINATES_x,
		NULL
	};
	if (exe != NULL) {
		exe->Key_List (keys);
	}
}

//---------------------------------------------------------
//	Read_Control
//---------------------------------------------------------

void Occupancy_Output::Read_Control (void)
{
	int i, num;

	if (exe == NULL) return;

	Microsimulator *msim = (Microsimulator *) exe;

	if (msim->steps_per_second == 0) return;

	cell_size = msim->UnRound (msim->cell_size);

	//---- read the output delay data ----

	num = exe->Highest_Control_Group (OUTPUT_OCCUPANCY_FILE_x, 0);

	if (num != 0) {
		Output_Data *output;
		char *str_ptr, buffer [FIELD_BUFFER];
		int increment;

		//---- allocate array space ----

		if (!output_data.Max_Records (num)) {
			exe->Error ("Insufficient Memory for %d Output Occupancy Groups", num);
		}

		//---- process each file ----

		for (i=1; i <= num; i++) {

			str_ptr = exe->Get_Control_String (OUTPUT_OCCUPANCY_FILE_x, i);

			if (str_ptr == NULL) continue;
			exe->Print (1);

			char *name_ptr = exe->Project_Filename (str_ptr);

			output = (Output_Data *) output_data.New_Record ();

			output->time_range = new Time_Range (msim->steps_per_second);
			output->link_range = NULL;
			output->cell_data = NULL;
			output->max_flag = false;
			output->coord_flag = false;
			output->start_time = output->end_time = -1;

			output->file = new Occupancy_File (Db_Code::CREATE);

			str_fmt (buffer, sizeof (buffer), "Output Occupancy File #%d", i);

			output->file->File_Type (buffer);

			//---- set the output format ----

			str_ptr = exe->Get_Control_String (OUTPUT_OCCUPANCY_FORMAT_x, i);

			if (str_ptr != NULL) {
				output->file->Dbase_Format (str_ptr);
			}
			output->file->Open (name_ptr);

			//---- get the time format ----

			str_ptr = exe->Get_Control_String (OUTPUT_OCCUPANCY_TIME_FORMAT_x, i);

			if (str_ptr != NULL) {
				if (!output->time_range->Format (str_ptr)) {
					exe->Error ("Output Occupancy Time Format %s was Unrecognized", str_ptr);
				}
				exe->Print (1, "Output Occupancy Time Format #%d = %s", i, output->time_range->Time_Code ());
			}

			//---- get the increment ----

			str_ptr = exe->Get_Control_String (OUTPUT_OCCUPANCY_INCREMENT_x, i);

			if (str_ptr != NULL) {
				output->time_range->Increment (str_ptr);
				increment = output->time_range->Increment ();

				if (increment < 1 || increment > output->time_range->Duration ("1:00")) {
					exe->Error ("Output Occupancy Increment %s is Out of Range", 
						output->time_range->Format_Duration (increment));
				}
			} else {
				output->time_range->Increment ("0:15");
				increment = output->time_range->Increment ();
			}
			exe->Print (1, "Output Occupancy Increment #%d = %s", i, 
				output->time_range->Format_Duration (increment));

			if (output->time_range->Format () == Time_Step::SECONDS) {
				exe->Print (0, " second%c", (increment != output->time_range->Duration (1) ? 's' : ' '));
			} else if (output->time_range->Format () == Time_Step::HOURS) {
				exe->Print (0, " hour%c", (increment != output->time_range->Duration (1.0) ? 's' : ' '));
			}

			//---- get the time range ----

			str_ptr = exe->Get_Control_String (OUTPUT_OCCUPANCY_TIME_RANGE_x, i);

			if (str_ptr != NULL) {
				if (!output->time_range->Add_Ranges (str_ptr)) {
					exe->File_Error ("Output Occupancy Link Range", str_ptr);
				}
				exe->Print (1, "Output Occupancy Time Range #%d = ", i);

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

			str_ptr = exe->Get_Control_String (OUTPUT_OCCUPANCY_LINK_RANGE_x, i);

			if (str_ptr != NULL) {
				exe->Print (1, "Output Occupancy Link Range #%d = %s", i, str_ptr);

				output->link_range = new Data_Range ();

				if (!output->link_range->Add_Ranges (str_ptr)) {
					exe->File_Error ("Output Occupancy Link Range", str_ptr);
				}
			}

			//---- get the max flag ----

			str_ptr = exe->Get_Control_String (OUTPUT_OCCUPANCY_MAX_FLAG_x, i);

			if (str_ptr != NULL) {
				exe->Print (1, "Output Occupancy Maximum Flag #%d = %s", i, str_ptr);

				output->max_flag = exe->Get_Control_Flag (OUTPUT_OCCUPANCY_MAX_FLAG_x, i);
			}

			//---- get the coordinates ----

			str_ptr = exe->Get_Control_String (OUTPUT_OCCUPANCY_COORDINATES_x, i);

			if (str_ptr != NULL) {
				double dvalue;

				exe->Print (1, "Output Occupancy Coordinates #%d = %s", i, str_ptr);

				str_ptr = Get_Double (str_ptr, &dvalue);
				output->x1 = msim->Round (dvalue);

				if (str_ptr == NULL) goto coord_error;

				str_ptr = Get_Double (str_ptr, &dvalue);
				output->y1 = msim->Round (dvalue);

				if (str_ptr == NULL) goto coord_error;

				str_ptr = Get_Double (str_ptr, &dvalue);
				output->x2 = msim->Round (dvalue);
				if (output->x2 < output->x1) {
					int x = output->x2;
					output->x2 = output->x1;
					output->x1 = x;
				}

				if (str_ptr == NULL) goto coord_error;

				str_ptr = Get_Double (str_ptr, &dvalue);
				output->y2 = msim->Round (dvalue);
				if (output->y2 < output->y1) {
					int y = output->y2;
					output->y2 = output->y1;
					output->y1 = y;
				}
				output->coord_flag = true;
			}

			//---- initialize the cell data ----

			output->cell_data = new Data_Array (sizeof (Cell_Data));

			if (output->cell_data == NULL) {
				exe->Error ("Creating Occupancy Data");
			}

			//---- save the output record ----

			if (!output_data.Add ()) {
				exe->Error ("Adding Occupancy Output Group");
			}
			output_flag = true;
		}
	}
	return;

coord_error:
	exe->Error ("Output Occupancy Coordinate Data #%d", i);
}

//---------------------------------------------------------
//	Initialize_Cells
//---------------------------------------------------------

void Occupancy_Output::Initialize_Cells (void)
{
	if (!output_flag) return;

	int num_rec;

	Output_Data *output;
	Grid_Data *grid_ptr;
	Link_Data *link_ptr;
	Node_Data *node_ptr;
	Cell_Data *data_ptr;

	Microsimulator *msim = (Microsimulator *) exe;

	num_rec = msim->dir_array->Num_Records ();

	for (output = (Output_Data *) output_data.First (); output; output = (Output_Data *) output_data.Next ()) {

		if (!output->cell_data->Num_Records (num_rec)) goto mem_error;

		for (grid_ptr = (Grid_Data *) msim->dir_array->First (); grid_ptr;
			grid_ptr = (Grid_Data *) msim->dir_array->Next ()) {

			//---- initialize the data record ----

			data_ptr = (Cell_Data *) output->cell_data->Record (msim->dir_array->Record_Index ());

			data_ptr->lanes = 0;
			data_ptr->cells = 0;
			data_ptr->total = 0;
			data_ptr->data = NULL;

			//---- check the selection criteria ----

			link_ptr = msim->link_array->Record (grid_ptr->Link ());

			if (output->link_range != NULL) {
				if (!output->link_range->In_Range (link_ptr->Link ())) continue;
			}
			if (output->coord_flag) {
				node_ptr = msim->node_array->Record (link_ptr->Anode ());
				if (node_ptr->X () < output->x1 || node_ptr->X () > output->x2 ||
					node_ptr->Y () < output->y1 || node_ptr->Y () > output->y2) {

					node_ptr = msim->node_array->Record (link_ptr->Bnode ());

					if (node_ptr->X () < output->x1 || node_ptr->X () > output->x2 ||
						node_ptr->Y () < output->y1 || node_ptr->Y () > output->y2) {
						continue;
					}
				}
			}

			//---- allocate cell memory ----

			data_ptr->lanes = grid_ptr->Lanes ();
			data_ptr->cells = grid_ptr->Cells ();

			data_ptr->data = new int [data_ptr->lanes * data_ptr->cells];
			if (data_ptr == NULL) goto mem_error;

			memset (data_ptr->data, '\0', data_ptr->lanes * data_ptr->cells * sizeof (int));
		}
	}
	return;

mem_error:
	exe->Error ("Insufficient Memory for Occupancy Data");
}

//---------------------------------------------------------
//	Output_Check
//---------------------------------------------------------

void Occupancy_Output::Output_Check (void)
{
	if (!output_flag) return;

	int step, lane, cell, total;

	Output_Data *output;
	Grid_Data *grid_ptr;
	Cell_Data *data_ptr;

	Microsimulator *msim = (Microsimulator *) exe;

	step = msim->step;

	for (output = (Output_Data *) output_data.First (); output; output = (Output_Data *) output_data.Next ()) {
		if (output->time_range->In_Range (step)) {

			//---- check the output increment ----

			if (output->time_range->At_Increment (step)) {
				Write_Occupancy (output);
			}

			//---- initialize the next increment ----

			if (output->start_time < 0) output->start_time = step;
			output->end_time = step;

			for (data_ptr = (Cell_Data *) output->cell_data->First (); data_ptr;
				data_ptr = (Cell_Data *) output->cell_data->Next ()) {

				if (data_ptr->lanes == 0) continue;

				grid_ptr = (Grid_Data *) msim->dir_array->Record (output->cell_data->Record_Index ());

				//---- check for maximum ----

				if (output->max_flag) {
					total = 0;

					for (lane=0; lane < data_ptr->lanes; lane++) {
						for (cell=0; cell < data_ptr->cells; cell++) {
							if (grid_ptr->Data (cell+1, lane+1) > 0) {
								total++;
							}
						}
					}
					if (total <= data_ptr->total) continue;
					memset (data_ptr->data, '\0', data_ptr->lanes * data_ptr->cells * sizeof (int));
					data_ptr->total = 0;
				}

				//---- mark occupied cells ----

				total = 0;

				for (lane=0; lane < data_ptr->lanes; lane++) {
					for (cell=0; cell < data_ptr->cells; cell++) {
						if (grid_ptr->Data (cell+1, lane+1) > 0) {
							data_ptr->data [lane * data_ptr->cells + cell]++;
							total++;
						}
					}
				}
				data_ptr->total += total;
			}
		}
	}
}

//---------------------------------------------------------
//	Close
//---------------------------------------------------------

void Occupancy_Output::Close (void)
{
	if (!output_flag) return;

	Output_Data *output;

	for (output = (Output_Data *) output_data.First (); output; output = (Output_Data *) output_data.Next ()) {
		if (output->end_time >= 0 && !output->time_range->At_Increment (output->end_time)) {
			Write_Occupancy (output);
		}
		if (output->file != NULL) {
			output->file->Close ();
			delete output->file;
			output->file = NULL;
		}
		if (output->cell_data != NULL) {
			Cell_Data *data_ptr;

			for (data_ptr = (Cell_Data *) output->cell_data->First (); data_ptr;
				data_ptr = (Cell_Data *) output->cell_data->Next ()) {

				if (data_ptr->data != NULL) {
					delete [] data_ptr->data;
				}
				data_ptr->data = NULL;
				data_ptr->lanes = 0;
				data_ptr->total = 0;
			}
			delete output->cell_data;
			output->cell_data = NULL;
		}
		if (output->link_range != NULL) {
			delete output->link_range;
			output->link_range = NULL;
		}
		output->start_time = output->end_time = -1;
	}
}

//---------------------------------------------------------
//	Write_Occupancy
//---------------------------------------------------------

void Occupancy_Output::Write_Occupancy (Output_Data *output)
{
	if (output->start_time < 0 || output->end_time <= 0) return;

	int lane, cell, occupancy;

	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Cell_Data *data_ptr;

	Microsimulator *msim = (Microsimulator *) exe;

	//---- output the current increment ----

	for (data_ptr = (Cell_Data *) output->cell_data->First (); data_ptr;
		data_ptr = (Cell_Data *) output->cell_data->Next ()) {

		if (data_ptr->lanes == 0 || data_ptr->total == 0) continue;

		dir_ptr = msim->dir_array->Record (output->cell_data->Record_Index ());
		link_ptr = msim->link_array->Record (dir_ptr->Link ());

		output->file->Link (link_ptr->Link ());
		output->file->Dir (dir_ptr->Dir ());
		output->file->Start (output->time_range->Format_Step (output->start_time));
		output->file->End (output->time_range->Format_Step (output->end_time + 1));

		for (lane=0; lane < data_ptr->lanes; lane++) {
			for (cell=0; cell < data_ptr->cells; cell++) {
				occupancy = data_ptr->data [lane * data_ptr->cells + cell];

				if (occupancy > 0) {
					output->file->Lane (lane + 1);
					output->file->Offset (cell * cell_size + cell_size);
					output->file->Occupancy (occupancy);

					if (!output->file->Write ()) {;
						exe->Error ("Writing Occupancy Output File");
					}
				}
			}
		}
		memset (data_ptr->data, '\0', data_ptr->lanes * data_ptr->cells * sizeof (int));
		data_ptr->total = 0;
	}
	output->start_time = output->end_time = -1;
}
