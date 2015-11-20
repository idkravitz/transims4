//*********************************************************
//	Traveler_Output.cpp - Output Interface Class
//*********************************************************

#include "Traveler_Output.hpp"
#include "Network_Service.hpp"

#include "Utility.hpp"

char * Traveler_Output::OUTPUT_TRAVELER_RANGE_x			= "OUTPUT_TRAVELER_RANGE_*";
char * Traveler_Output::OUTPUT_TRAVELER_FILE_x			= "OUTPUT_TRAVELER_FILE_*";
char * Traveler_Output::OUTPUT_TRAVELER_FORMAT_x		= "OUTPUT_TRAVELER_FORMAT_*";
char * Traveler_Output::OUTPUT_TRAVELER_TIME_FORMAT_x	= "OUTPUT_TRAVELER_TIME_FORMAT_*";
char * Traveler_Output::OUTPUT_TRAVELER_TIME_RANGE_x	= "OUTPUT_TRAVELER_TIME_RANGE_*";
char * Traveler_Output::OUTPUT_TRAVELER_LINK_RANGE_x	= "OUTPUT_TRAVELER_LINK_RANGE_*";

//---------------------------------------------------------
//	Traveler_Output constructor
//---------------------------------------------------------

Traveler_Output::Traveler_Output (void) : Static_Service ()
{
	output_data.Initialize (sizeof (Output_Data), 10);

	output_flag = false;
	traveler = vehicle = time = from_dir = from_offset = to_dir = to_offset = speed = lane = 0;
}

//---------------------------------------------------------
//	Traveler_Output destructor
//---------------------------------------------------------

Traveler_Output::~Traveler_Output (void)
{
	Close ();
}

//---------------------------------------------------------
//	Add_Keys
//---------------------------------------------------------

void Traveler_Output::Add_Keys (void)
{
	char *keys [] = {
		OUTPUT_TRAVELER_RANGE_x,
		OUTPUT_TRAVELER_FILE_x,
		OUTPUT_TRAVELER_FORMAT_x,
		OUTPUT_TRAVELER_TIME_FORMAT_x,
		OUTPUT_TRAVELER_TIME_RANGE_x,
		OUTPUT_TRAVELER_LINK_RANGE_x,
		NULL
	};

	if (exe != NULL) {
		exe->Key_List (keys);
	}
}

//---------------------------------------------------------
//	Read_Control
//---------------------------------------------------------

void Traveler_Output::Read_Control (int steps_per_second)
{
	if (exe == NULL) return;

	//---- read the output traveler data ----

	int num = exe->Highest_Control_Group (OUTPUT_TRAVELER_RANGE_x, 0);

	if (num != 0) {
		Output_Data *output;
		char *str_ptr, buffer [FIELD_BUFFER];

		//---- allocate array space ----

		if (!output_data.Max_Records (num)) {
			exe->Error ("Insufficient Memory for %d Output Traveler Groups", num);
		}

		//---- process each file ----

		for (int i=1; i <= num; i++) {
			
			str_ptr = exe->Get_Control_String (OUTPUT_TRAVELER_RANGE_x, i);

			if (str_ptr == NULL) continue;

			exe->Break_Check (8);
			exe->Print (2, "Output Traveler Range #%d = %s", i, str_ptr);

			output = (Output_Data *) output_data.New_Record ();

			output->range = new Data_Range ();
			output->time_range = new Time_Range (steps_per_second);
			output->link_range = NULL;

			if (!output->range->Add_Ranges (str_ptr)) {
				exe->Error ("Processing Traveler Output Range %s", str_ptr);
			}

			str_ptr = exe->Get_Control_String (OUTPUT_TRAVELER_FILE_x, i);

			if (str_ptr == NULL) {
				output->file_flag = false;
			} else {
				char *name_ptr = exe->Project_Filename (str_ptr);

				output->file_flag = true;
				output->file = new Traveler_File (Db_Code::CREATE);

				str_fmt (buffer, sizeof (buffer), "Output Traveler File #%d", i);

				output->file->File_Type (buffer);

				//---- set the output format ----

				str_ptr = exe->Get_Control_String (OUTPUT_TRAVELER_FORMAT_x, i);

				if (str_ptr != NULL) {
					output->file->Dbase_Format (str_ptr);
				}
				output->file->Open (name_ptr);
			}

			//---- get the time format ----

			str_ptr = exe->Get_Control_String (OUTPUT_TRAVELER_TIME_FORMAT_x, i);

			if (str_ptr != NULL) {
				if (!output->time_range->Format (str_ptr)) {
					exe->Error ("Output Traveler Time Format %s was Unrecognized", str_ptr);
				}
				exe->Print (1, "Output Traveler Time Format #%d = %s", i, output->time_range->Time_Code ());
			}

			//---- get the time range ----

			str_ptr = exe->Get_Control_String (OUTPUT_TRAVELER_TIME_RANGE_x, i);

			if (str_ptr != NULL) {
				if (!output->time_range->Add_Ranges (str_ptr)) {
					exe->File_Error ("Output Traveler Time Range", str_ptr);
				}
				exe->Print (1, "Output Traveler Time Range #%d = ", i);

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

			str_ptr = exe->Get_Control_String (OUTPUT_TRAVELER_LINK_RANGE_x, i);

			if (str_ptr != NULL) {
				exe->Print (1, "Output Traveler Link Range #%d = %s", i, str_ptr);

				output->link_range = new Data_Range ();

				if (!output->link_range->Add_Ranges (str_ptr)) {
					exe->File_Error ("Output Traveler Link Range", str_ptr);
				}
			}

			if (output->file_flag) {
				if (output->file->Dbase_Format () == Db_Code::VERSION3) {
					output->time_range->Format (Time_Step::SECONDS);
				}
			}

			if (!output_data.Add ()) {
				exe->Error ("Adding Traveler Output Group");
			}
			output_flag = true;
		}
	}
}

//---------------------------------------------------------
//	In_Range
//---------------------------------------------------------

bool Traveler_Output::In_Range (int traveler, int step, int dir)
{
	if (output_flag) {
		Output_Data *output;

		for (output = (Output_Data *) output_data.First (); output != NULL; output = (Output_Data *) output_data.Next ()) {
			if (output->range->In_Range (traveler) && output->time_range->In_Range (step)) {
				if (dir > 0 && output->link_range != NULL && output->link_range->Num_Ranges () > 0) {
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
	return (false);
}

//---------------------------------------------------------
//	Output
//---------------------------------------------------------

void Traveler_Output::Output (void)
{
	if (!output_flag) return;

	//---- validate the offset data ----

	bool printed = false;
	int distance;

	Network_Service *net = (Network_Service *) exe;
	Dir_Data *from_ptr, *to_ptr;
	Link_Data *from_link, *to_link;

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

	if (from_dir != to_dir) {
		distance = from_link->Length () - from_offset + to_offset;
	} else {
		distance = to_offset - from_offset;
	}

	Output_Data *output = (Output_Data *) output_data.Record ();

	for (; output != NULL; output = (Output_Data *) output_data.Next ()) {
		if (output->range->In_Range (traveler) && output->time_range->In_Range (time)) {
			if (output->link_range != NULL && output->link_range->Num_Ranges () > 0) {
				if (!output->link_range->In_Range (to_link->Link ())) continue;
			}

			if (output->file_flag) {
				output->file->Traveler (traveler);
				output->file->Vehicle (vehicle);
				output->file->Time (output->time_range->Format_Step (time));
				output->file->Distance (net->UnRound (distance));
				output->file->Link (to_link->Link ());
				output->file->Dir (to_ptr->Dir ());
				output->file->Offset (net->UnRound (to_offset));
				output->file->Lane (lane);
				output->file->Speed (net->UnRound (speed));

				if (!output->file->Write ()) {
					exe->Error ("Writing Traveler Output File");
				}
			} else if (!printed) {
				printed = true;
				if (exe != NULL) continue;

				exe->Print (1, "Traveler=%d, Vehicle=%d, Time=%s, Distance=%.1lf, Link=%d-%d, Offset=%.1lf, Lane=%d, Speed=%.1lf", 
					traveler, vehicle, output->time_range->Format_Step (time), net->UnRound (distance), 
					to_link->Link (), to_ptr->Dir (), net->UnRound (to_offset), lane, net->UnRound (speed));
			}
		}
	}
}

//---------------------------------------------------------
//	Close
//---------------------------------------------------------

void Traveler_Output::Close (void)
{
	if (!output_flag) return;

	Output_Data *output;

	for (output = (Output_Data *) output_data.First (); output != NULL; output = (Output_Data *) output_data.Next ()) {
		if (output->file_flag) {
			output->file->Close ();
			delete output->file;
			output->file_flag = false;
		}
		if (output->range != NULL) {
			delete output->range;
			output->range = NULL;
		}
		if (output->time_range != NULL) {
			delete output->time_range;
			output->time_range = NULL;
		}
		if (output->link_range != NULL) {
			delete output->link_range;
			output->link_range = NULL;
		}
	}
	output_flag = false;
}
