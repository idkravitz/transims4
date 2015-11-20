//*********************************************************
//	System_Event_Output.cpp - System Event Output Interface
//*********************************************************

#include "System_Event_Output.hpp"

#include "Network_Service.hpp"
#include "Utility.hpp"

char * System_Event_Output::OUTPUT_SYSTEM_EVENT_TYPE_x			= "OUTPUT_SYSTEM_EVENT_TYPE_*";
char * System_Event_Output::OUTPUT_SYSTEM_EVENT_FILE_x			= "OUTPUT_SYSTEM_EVENT_FILE_*";
char * System_Event_Output::OUTPUT_SYSTEM_EVENT_FORMAT_x		= "OUTPUT_SYSTEM_EVENT_FORMAT_*";
char * System_Event_Output::OUTPUT_SYSTEM_EVENT_TIME_FORMAT_x	= "OUTPUT_SYSTEM_EVENT_TIME_FORMAT_*";
char * System_Event_Output::OUTPUT_SYSTEM_EVENT_TIME_RANGE_x	= "OUTPUT_SYSTEM_EVENT_TIME_RANGE_*";
char * System_Event_Output::OUTPUT_SYSTEM_EVENT_NODE_RANGE_x	= "OUTPUT_SYSTEM_EVENT_NODE_RANGE_*";

//---- system event type codes ----

char * System_Event_Output::event_text [] = {
	"PHASE_CHANGE", "TIMING_CHANGE", NULL
};

System_Event_Output::Event_Type  System_Event_Output::event_code [] = {
	PHASE_CHANGE, TIMING_CHANGE
};

//---------------------------------------------------------
//	System_Event_Output constructor
//---------------------------------------------------------

System_Event_Output::System_Event_Output (void) : Static_Service ()
{
	output_data.Initialize (sizeof (Output_Data), 10);

	memset (output_flag, '\0', sizeof (output_flag));
	type = PHASE_CHANGE;
	time = node = plan = phase = ring = group = 0;
	status [0] = '\0';
}

//---------------------------------------------------------
//	System_Event_Output destructor
//---------------------------------------------------------

System_Event_Output::~System_Event_Output (void)
{
	Close ();
}

//---------------------------------------------------------
//	Add_Keys
//---------------------------------------------------------

void System_Event_Output::Add_Keys (void)
{
	char *keys [] = {
		OUTPUT_SYSTEM_EVENT_TYPE_x,
		OUTPUT_SYSTEM_EVENT_FILE_x,
		OUTPUT_SYSTEM_EVENT_FORMAT_x,
		OUTPUT_SYSTEM_EVENT_TIME_FORMAT_x,
		OUTPUT_SYSTEM_EVENT_TIME_RANGE_x,
		OUTPUT_SYSTEM_EVENT_NODE_RANGE_x,
		NULL
	};

	if (exe != NULL) {
		exe->Key_List (keys);
	}
}

//---------------------------------------------------------
//	Read_Control
//---------------------------------------------------------

void System_Event_Output::Read_Control (int steps_per_second)
{
	Event_Type type = TIMING_CHANGE;

	if (exe == NULL || steps_per_second < 1) return;

	//---- read the output delay data ----

	int num = exe->Highest_Control_Group (OUTPUT_SYSTEM_EVENT_FILE_x, 0);

	if (num != 0) {
		Output_Data *output;
		char *str_ptr, buffer [FIELD_BUFFER];

		//---- allocate array space ----

		if (!output_data.Max_Records (num)) {
			exe->Error ("Insufficient Memory for %d Output System Events", num);
		}

		//---- process each file ----

		for (int i=1; i <= num; i++) {

			str_ptr = exe->Get_Control_String (OUTPUT_SYSTEM_EVENT_FILE_x, i);

			if (str_ptr == NULL) continue;

			char *name_ptr = exe->Project_Filename (str_ptr);

			output = (Output_Data *) output_data.New_Record ();

			output->time_range = new Time_Range (steps_per_second);
			output->node_range = NULL;

			//---- get the file type ----

			str_ptr = exe->Get_Control_String (OUTPUT_SYSTEM_EVENT_TYPE_x, i);

			if (str_ptr == NULL) {
				exe->Error ("Missing Control Key = %s", exe->Current_Key ());
			}
			exe->Break_Check (7);
			exe->Print (2, "Output System Event Type #%d = %s", i, str_ptr);

			memset (output->type, '\0', sizeof (output->type));

			while (str_ptr != NULL) {
				str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

				if (buffer [0] != '\0') {
					type = Event_Code (buffer);
					output->type [type] = true;
					output_flag [type] = true;
				}
			}
			output->file = new System_Event_File (Db_Code::CREATE);

			str_fmt (buffer, sizeof (buffer), "Output System Event File #%d", i);

			output->file->File_Type (buffer);

			//---- set the output format ----

			str_ptr = exe->Get_Control_String (OUTPUT_SYSTEM_EVENT_FORMAT_x, i);

			if (str_ptr != NULL) {
				output->file->Dbase_Format (str_ptr);
			}
			output->file->Open (name_ptr);

			//---- get the time format ----

			str_ptr = exe->Get_Control_String (OUTPUT_SYSTEM_EVENT_TIME_FORMAT_x, i);

			if (str_ptr != NULL) {
				if (!output->time_range->Format (str_ptr)) {
					exe->Error ("Output System Event Time Format %s was Unrecognized", str_ptr);
				}
				exe->Print (1, "Output System Event Time Format #%d = %s", i, output->time_range->Time_Code ());
			}

			//---- get the time range ----

			str_ptr = exe->Get_Control_String (OUTPUT_SYSTEM_EVENT_TIME_RANGE_x, i);

			if (str_ptr != NULL) {
				if (!output->time_range->Add_Ranges (str_ptr)) {
					exe->File_Error ("Output System Event Time Range", str_ptr);
				}
				exe->Print (1, "Output System Event Time Range #%d = ", i);

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

			//---- get the node range ----

			str_ptr = exe->Get_Control_String (OUTPUT_SYSTEM_EVENT_NODE_RANGE_x, i);

			if (str_ptr != NULL) {
				exe->Print (1, "Output System Event Node Range #%d = %s", i, str_ptr);

				output->node_range = new Data_Range ();

				if (!output->node_range->Add_Ranges (str_ptr)) {
					exe->File_Error ("Output System Event Node Range", str_ptr);
				}
			}

			//---- save the output record ----

			if (!output_data.Add ()) {
				exe->Error ("Adding Output System Event Group");
			}
			output_flag [type] = true;
		}
	}
}

//---------------------------------------------------------
//	In_Range
//---------------------------------------------------------

bool System_Event_Output::In_Range (int step, Event_Type type, int node)
{
	if (output_flag [type]) {
		Output_Data *output;

		for (output = (Output_Data *) output_data.First (); output != NULL; output = (Output_Data *) output_data.Next ()) {
			if (output->type [type] && output->time_range->In_Range (step)) {
				Event (type);
				if (node > 0 && output->node_range != NULL && output->node_range->Num_Ranges () > 0) {
					Network_Service *net = (Network_Service *) exe;
					Node_Data *node_ptr = net->node_array->Record (node);
					if (node_ptr != NULL) {
						if (!output->node_range->In_Range (node_ptr->Node ())) continue;
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

void System_Event_Output::Output (void)
{
	if (!output_flag [type]) return; 

	System_Event_File *file;
	int ext_node = 0;

	Output_Data *output = (Output_Data *) output_data.Record ();

	for (; output != NULL; output = (Output_Data *) output_data.Next ()) {
		if (output->type [type] && output->time_range->In_Range (time)) {
			if (ext_node == 0) {
				Network_Service *net = (Network_Service *) exe;
				Node_Data *node_ptr = net->node_array->Record (node);
				if (node_ptr != NULL) {
					ext_node = node_ptr->Node ();
				}
			}
			if (output->node_range != NULL && output->node_range->Num_Ranges () > 0) {
				if (!output->node_range->In_Range (ext_node)) continue;
			}
			file = (System_Event_File *) output->file;

			file->Time (output->time_range->Format_Step (time));
			file->Event (type);
			file->Node (ext_node);
			file->Plan (plan);
			file->Phase (phase);
			file->Ring (ring);
			file->Group (group);
			file->Status (status);

			if (!file->Write ()) {
				exe->Error ("Writing Output System Event File");
			}
		}
	}
}

//---------------------------------------------------------
//	Close
//---------------------------------------------------------

void System_Event_Output::Close (void)
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
		if (output->node_range != NULL) {
			delete output->node_range;
			output->node_range = NULL;
		}
	}
	memset (output_flag, '\0', sizeof (output_flag));
}

//---------------------------------------------------------
//	Event_Code
//---------------------------------------------------------

System_Event_Output::Event_Type  System_Event_Output::Event_Code (char *text)
{
	for (int i=0; event_text [i] != NULL; i++) {
		if (str_cmp (text, event_text [i]) == 0) {
			return (event_code [i]);
		}
	}
	exe->Error ("Converting System Event Type %s", text);

	return (PHASE_CHANGE);
}

char * System_Event_Output::Event_Code (System_Event_Output::Event_Type code)
{
	for (int i=0; event_text [i] != NULL; i++) {
		if (code == event_code [i]) {
			return (event_text [i]);
		}
	}
	exe->Error ("Converting System Event Code %d", code);

	return (NULL);
}
