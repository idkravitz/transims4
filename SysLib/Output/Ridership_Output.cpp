//*********************************************************
//	Ridership_Output.cpp - Output Interface Class
//*********************************************************

#include "Ridership_Output.hpp"
#include "Demand_Service.hpp"

#include "Utility.hpp"

char * Ridership_Output::OUTPUT_RIDERSHIP_FILE_x		= "OUTPUT_RIDERSHIP_FILE_*";
char * Ridership_Output::OUTPUT_RIDERSHIP_FORMAT_x		= "OUTPUT_RIDERSHIP_FORMAT_*";
char * Ridership_Output::OUTPUT_RIDERSHIP_TIME_FORMAT_x	= "OUTPUT_RIDERSHIP_TIME_FORMAT_*";
char * Ridership_Output::OUTPUT_RIDERSHIP_TIME_RANGE_x	= "OUTPUT_RIDERSHIP_TIME_RANGE_*";
char * Ridership_Output::OUTPUT_RIDERSHIP_ROUTE_RANGE_x	= "OUTPUT_RIDERSHIP_ROUTE_RANGE_*";
char * Ridership_Output::OUTPUT_RIDERSHIP_ALL_STOPS_x	= "OUTPUT_RIDERSHIP_ALL_STOPS_*";

//---------------------------------------------------------
//	Ridership_Output constructor
//---------------------------------------------------------

Ridership_Output::Ridership_Output (void) : Static_Service ()
{
	output_data.Initialize (sizeof (Output_Data), 10);
	output_flag = false;
}

//---------------------------------------------------------
//	Ridership_Output destructor
//---------------------------------------------------------

Ridership_Output::~Ridership_Output (void)
{
	Close ();
}

//---------------------------------------------------------
//	Add_Keys
//---------------------------------------------------------

void Ridership_Output::Add_Keys (void)
{
	char *keys [] = {
		OUTPUT_RIDERSHIP_FILE_x,
		OUTPUT_RIDERSHIP_FORMAT_x,
		OUTPUT_RIDERSHIP_TIME_FORMAT_x,
		OUTPUT_RIDERSHIP_TIME_RANGE_x,
		OUTPUT_RIDERSHIP_ROUTE_RANGE_x,
		OUTPUT_RIDERSHIP_ALL_STOPS_x,
		NULL
	};

	if (exe != NULL) {
		exe->Key_List (keys);
	}
}

//---------------------------------------------------------
//	Read_Control
//---------------------------------------------------------

void Ridership_Output::Read_Control (int steps_per_second)
{
	if (exe == NULL) return;

	//---- read the output traveler data ----

	int num = exe->Highest_Control_Group (OUTPUT_RIDERSHIP_FILE_x, 0);

	if (num != 0) {
		Output_Data *output;
		char *str_ptr, buffer [FIELD_BUFFER];

		Demand_Service *demand = (Demand_Service *) exe;

		//---- allocate array space ----

		if (!output_data.Max_Records (num)) {
			exe->Error ("Insufficient Memory for %d Output Ridership Groups", num);
		}

		//---- process each file ----

		for (int i=1; i <= num; i++) {

			str_ptr = exe->Get_Control_String (OUTPUT_RIDERSHIP_FILE_x, i);

			if (str_ptr == NULL) continue;

			char *name_ptr = exe->Project_Filename (str_ptr);

			output = (Output_Data *) output_data.New_Record ();

			output->time_range = new Time_Range (steps_per_second);
			output->route_range = NULL;
			output->all_stops = false;

			output->file = new Ridership_File (Db_Code::CREATE);

			exe->Break_Check (7);
			exe->Print (1);
			str_fmt (buffer, sizeof (buffer), "Output Ridership File #%d", i);

			output->file->File_Type (buffer);

			//---- set the output format ----

			str_ptr = exe->Get_Control_String (OUTPUT_RIDERSHIP_FORMAT_x, i);

			if (str_ptr != NULL) {
				output->file->Dbase_Format (str_ptr);
			}
			output->file->Open (name_ptr);

			//---- check the class pointers ----

			if (!output_flag) {
				if (!demand->Network_File_Flag (Network_Service::TRANSIT_STOP) ||
					!demand->Network_File_Flag (Network_Service::TRANSIT_ROUTE) ||
					!demand->Network_File_Flag (Network_Service::TRANSIT_SCHEDULE)) {

					demand->Error ("Transit Network Data is Required for Ridership Output");
				}
				if (demand->line_array == NULL) {
					demand->line_array = (Line_Array *) new Rider_Array ();
				}
			}

			//---- get the time format ----

			str_ptr = exe->Get_Control_String (OUTPUT_RIDERSHIP_TIME_FORMAT_x, i);

			if (str_ptr != NULL) {
				if (!output->time_range->Format (str_ptr)) {
					exe->Error ("Output Ridership Time Format %s was Unrecognized", str_ptr);
				}
				exe->Print (1, "Output Ridership Time Format #%d = %s", i, output->time_range->Time_Code ());
			}

			//---- get the time range ----

			str_ptr = exe->Get_Control_String (OUTPUT_RIDERSHIP_TIME_RANGE_x, i);

			if (str_ptr != NULL) {
				if (!output->time_range->Add_Ranges (str_ptr)) {
					exe->File_Error ("Output Ridership Time Range", str_ptr);
				}
				exe->Print (1, "Output Ridership Time Range #%d = ", i);

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

			//---- get the route range ----

			str_ptr = exe->Get_Control_String (OUTPUT_RIDERSHIP_ROUTE_RANGE_x, i);

			if (str_ptr != NULL) {
				exe->Print (1, "Output Ridership Route Range #%d = %s", i, str_ptr);

				output->route_range = new Data_Range ();

				if (!output->route_range->Add_Ranges (str_ptr)) {
					exe->File_Error ("Output Ridership Route Range", str_ptr);
				}
			}
			if (output->file->Dbase_Format () == Db_Code::VERSION3) {
				output->time_range->Format (Time_Step::SECONDS);
			}

			//---- all stops flag ----

			str_ptr = exe->Get_Control_String (OUTPUT_RIDERSHIP_ALL_STOPS_x, i);

			if (str_ptr != NULL) {
				exe->Print (1, "Output Ridership for All Stops #%d = %s", i, str_ptr);

				output->all_stops = exe->Get_Control_Flag (OUTPUT_RIDERSHIP_ALL_STOPS_x, i);
			}
			if (!output_data.Add ()) {
				exe->Error ("Adding Ridership Output Group");
			}
			output_flag = true;
		}
	}
}

//---------------------------------------------------------
//	Output
//---------------------------------------------------------

void Ridership_Output::Output (void)
{
	if (!output_flag) return;

	//---- validate the offset data ----

	int time, schedule, run, stop, board, alight, load;
	double capacity;
	bool route_flag, capacity_flag;
	char mode [20];

	Rider_Data *rider_ptr;
	Stop_Data *stop_ptr;
	Driver_Data *driver_ptr;
	Veh_Type_Data *veh_type_ptr;

	Demand_Service *demand = (Demand_Service *) exe;

	capacity_flag = (demand->veh_type_array != NULL && demand->driver_array != NULL);

	if (!capacity_flag) {
		capacity = 0.0;
	}

	Rider_Array *rider = (Rider_Array *) (demand->line_array);

	Output_Data *output = (Output_Data *) output_data.First ();

	for (; output != NULL; output = (Output_Data *) output_data.Next ()) {

		route_flag = (output->route_range != NULL && output->route_range->Num_Ranges () > 0);

		for (rider_ptr = rider->First (); rider_ptr; rider_ptr = rider->Next ()) {

			//---- check the route selection criteria -----

			if (route_flag) {
				if (!output->route_range->In_Range (rider_ptr->Route ())) continue;
			}

			//---- convert the mode code to a string ----

			str_cpy (mode, sizeof (mode), demand->Transit_Code ((Network_Code::Transit_Type) rider_ptr->Mode ()));

			//---- get the vehicle capacity ----

			if (capacity_flag) {
				capacity = 0.0;

				driver_ptr = demand->driver_array->Record (rider_ptr->Driver ());

				if (driver_ptr != NULL) {
					veh_type_ptr = demand->veh_type_array->Record (driver_ptr->Type ());

					if (veh_type_ptr != NULL) {
						capacity = veh_type_ptr->Capacity ();

						if (capacity > 1.0) {
							capacity = 1.0 / capacity;
						}
					}
				}
			}

			//---- process each run stop ----

			for (run=1; run <= rider_ptr->Runs (); run++) {
				load = 0;

				for (stop=1; stop <= rider_ptr->Stops (); stop++) {

					//---- check for riders ----
					
					board = rider_ptr->Board (run, stop);
					alight = rider_ptr->Alight (run, stop);

					load += board - alight;

					if (!output->all_stops && load == 0 && board == 0 && alight == 0) continue;

					//---- check the time range criteria ----

					time = rider_ptr->Time (run, stop);
					schedule = rider_ptr->Schedule (run, stop);

					if (time == 0 && schedule != 0) continue;

					if (!output->time_range->In_Range (time)) continue;

					//---- output the data record ----

					stop_ptr = demand->stop_array->Record (rider_ptr->Stop (stop));

					output->file->Mode (mode);
					output->file->Route (rider_ptr->Route ());
					output->file->Run (run);
					output->file->Stop (stop_ptr->Stop ());
					output->file->Schedule (output->time_range->Format_Step (schedule));
					output->file->Time (output->time_range->Format_Step (time));
					output->file->Board (board);
					output->file->Alight (alight);
					output->file->Load (load);
					output->file->Factor (load * capacity);

					if (!output->file->Write ()) {
						exe->Error ("Writing Ridership Output File");
					}
				}
			}
		}
	}
}

//---------------------------------------------------------
//	Close
//---------------------------------------------------------

void Ridership_Output::Close (void)
{
	if (!output_flag) return;

	Output_Data *output;

	for (output = (Output_Data *) output_data.First (); output != NULL; output = (Output_Data *) output_data.Next ()) {
		output->file->Close ();
		delete output->file;

		if (output->time_range != NULL) {
			delete output->time_range;
			output->time_range = NULL;
		}
		if (output->route_range != NULL) {
			delete output->route_range;
			output->route_range = NULL;
		}
	}
	output_flag = false;
}
