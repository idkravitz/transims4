//*********************************************************
//	Event_Output.cpp - Output Interface Class
//*********************************************************

#include "Event_Output.hpp"

#include "Utility.hpp"

char * Event_Output::OUTPUT_EVENT_TYPE_x		= "OUTPUT_EVENT_TYPE_*";
char * Event_Output::OUTPUT_EVENT_FILE_x		= "OUTPUT_EVENT_FILE_*";
char * Event_Output::OUTPUT_EVENT_FORMAT_x		= "OUTPUT_EVENT_FORMAT_*";
char * Event_Output::OUTPUT_EVENT_FILTER_x		= "OUTPUT_EVENT_FILTER_*";
char * Event_Output::OUTPUT_EVENT_TIME_FORMAT_x = "OUTPUT_EVENT_TIME_FORMAT_*";
char * Event_Output::OUTPUT_EVENT_TIME_RANGE_x	= "OUTPUT_EVENT_TIME_RANGE_*";
char * Event_Output::OUTPUT_EVENT_MODE_RANGE_x	= "OUTPUT_EVENT_MODE_RANGE_*";

//---------------------------------------------------------
//	Event_Output constructor
//---------------------------------------------------------

Event_Output::Event_Output (void) : Demand_Code ()
{
	output_data.Initialize (sizeof (Output_Data), 10);

	memset (output_flag, '\0', sizeof (output_flag));
	hhold = person = mode = trip = leg = schedule = actual = link = offset = 0;
}

//---------------------------------------------------------
//	Event_Output destructor
//---------------------------------------------------------

Event_Output::~Event_Output (void)
{
	Close ();
}

//---------------------------------------------------------
//	Add_Keys
//---------------------------------------------------------

void Event_Output::Add_Keys (void)
{
	char *keys [] = {
		OUTPUT_EVENT_TYPE_x,
		OUTPUT_EVENT_FILE_x,
		OUTPUT_EVENT_FORMAT_x,
		OUTPUT_EVENT_FILTER_x,
		OUTPUT_EVENT_TIME_FORMAT_x,
		OUTPUT_EVENT_TIME_RANGE_x,
		OUTPUT_EVENT_MODE_RANGE_x,
		NULL
	};

	if (exe != NULL) {
		exe->Key_List (keys);
	}
}

//---------------------------------------------------------
//	Read_Control
//---------------------------------------------------------

void Event_Output::Read_Control (int steps_per_second)
{
	Event_Type type = START_EVENT;

	if (exe == NULL || steps_per_second < 1) return;

	//---- read the output delay data ----

	int num = exe->Highest_Control_Group (OUTPUT_EVENT_FILE_x, 0);

	if (num != 0) {
		Output_Data *output;
		char *str_ptr, buffer [FIELD_BUFFER];

		//---- allocate array space ----

		if (!output_data.Max_Records (num)) {
			exe->Error ("Insufficient Memory for %d Output Event Groups", num);
		}

		//---- process each file ----

		for (int i=1; i <= num; i++) {

			str_ptr = exe->Get_Control_String (OUTPUT_EVENT_FILE_x, i);

			if (str_ptr == NULL) continue;

			char *name_ptr = exe->Project_Filename (str_ptr);

			output = (Output_Data *) output_data.New_Record ();

			output->time_range = new Time_Range (steps_per_second);
			output->mode_range = NULL;

			//---- get the file type ----

			str_ptr = exe->Get_Control_String (OUTPUT_EVENT_TYPE_x, i);

			if (str_ptr == NULL) {
				exe->Error ("Missing Control Key = %s", exe->Current_Key ());
			}
			exe->Break_Check (7);
			exe->Print (2, "Output Event Type #%d = %s", i, str_ptr);

			memset (output->type, '\0', sizeof (output->type));

			while (str_ptr != NULL) {
				str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

				if (buffer [0] != '\0') {
					type = Event_Code (buffer);
					output->type [type] = true;
					output_flag [type] = true;
				}
			}
			output->file = new Event_File (Db_Code::CREATE);

			str_fmt (buffer, sizeof (buffer), "Output Event File #%d", i);

			output->file->File_Type (buffer);

			//---- set the output format ----

			str_ptr = exe->Get_Control_String (OUTPUT_EVENT_FORMAT_x, i);

			if (str_ptr != NULL) {
				output->file->Dbase_Format (str_ptr);
			}
			output->file->Open (name_ptr);

			//---- get the filter ----

			str_ptr = exe->Get_Control_String (OUTPUT_EVENT_FILTER_x, i);

			if (str_ptr != NULL) {
				output->filter = atoi (str_ptr);

				if (output->filter < 0 || output->filter > 10000) {
					exe->Error ("Output Event Filter %d is Out of Range (1..10000)", output->filter);
				}
			} else {
				output->filter = 0;
			}

			exe->Print (1, "Output Event Filter #%d = %d second%c", i, output->filter,
				((output->filter != 1) ? 's' : ' '));


			//---- get the time format ----

			str_ptr = exe->Get_Control_String (OUTPUT_EVENT_TIME_FORMAT_x, i);

			if (str_ptr != NULL) {
				if (!output->time_range->Format (str_ptr)) {
					exe->Error ("Output Event Time Format %s was Unrecognized", str_ptr);
				}
				exe->Print (1, "Output Event Time Format #%d = %s", i, output->time_range->Time_Code ());
			}

			//---- get the time range ----

			str_ptr = exe->Get_Control_String (OUTPUT_EVENT_TIME_RANGE_x, i);

			if (str_ptr != NULL) {
				if (!output->time_range->Add_Ranges (str_ptr)) {
					exe->File_Error ("Output Event Time Range", str_ptr);
				}
				exe->Print (1, "Output Event Time Range #%d = ", i);

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

			str_ptr = exe->Get_Control_String (OUTPUT_EVENT_MODE_RANGE_x, i);

			if (str_ptr != NULL) {
				exe->Print (1, "Output Event Mode Range #%d = %s", i, str_ptr);

				output->mode_range = new Data_Range ();

				if (!output->mode_range->Add_Ranges (str_ptr)) {
					exe->File_Error ("Output Event Mode Range", str_ptr);
				}
			}

			//---- save the output record ----

			if (!output_data.Add ()) {
				exe->Error ("Adding Event Output Group");
			}
			output_flag [type] = true;
		}
	}
}

//---------------------------------------------------------
//	In_Range
//---------------------------------------------------------

bool Event_Output::In_Range (int step, Event_Type type, int mode)
{
	if (output_flag [type]) {
		Output_Data *output;

		for (output = (Output_Data *) output_data.First (); output != NULL; output = (Output_Data *) output_data.Next ()) {
			if (output->type [type] && output->time_range->In_Range (step)) {
				if (output->mode_range == NULL || output->mode_range->In_Range (mode)) {
					Event (type);
					return (true);
				}
			}
		}
	}
	return (false);
}

//---------------------------------------------------------
//	Output
//---------------------------------------------------------

void Event_Output::Output (void)
{
	if (!output_flag [type]) return; 

	Event_File *file;
	int diff;

	Output_Data *output = (Output_Data *) output_data.Record ();

	for (; output != NULL; output = (Output_Data *) output_data.Next ()) {
		if (output->type [type] && output->time_range->In_Range (schedule)) {
			if (output->mode_range == NULL || output->mode_range->In_Range (mode)) {
				diff = actual - schedule;

				if (abs (diff) >= output->filter) {
					file = (Event_File *) output->file;

					file->Household (hhold);
					file->Person (person);
					file->Mode (mode);
					file->Trip (trip);
					file->Leg (leg);
					file->Event (type);
					file->Schedule (output->time_range->Format_Step (schedule));
					file->Actual (output->time_range->Format_Step (actual));
					file->Link (link);
					file->Offset (offset);

					if (!file->Write ()) {
						exe->Error ("Writing Event Output File");
					}
				}
			}
		}
	}
}

//---------------------------------------------------------
//	Close
//---------------------------------------------------------

void Event_Output::Close (void)
{
	Output_Data *output;

	for (output = (Output_Data *) output_data.First (); output != NULL; output = (Output_Data *) output_data.Next ()) {
		if (output->file != NULL) {
			output->file->Close ();
			delete output->file;
			output->file = NULL;
		}
		if (output->time_range != NULL) {
			delete output->time_range;
			output->time_range = NULL;
		}
		if (output->mode_range != NULL) {
			delete output->mode_range;
			output->mode_range = NULL;
		}
	}
	memset (output_flag, '\0', sizeof (output_flag));
}

