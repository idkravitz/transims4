//*********************************************************
//	Snapshot_Output.cpp - Output Interface Class
//*********************************************************

#include "Snapshot_Output.hpp"
#include "Network_Service.hpp"

#include "Utility.hpp"

char * Snapshot_Output::OUTPUT_SNAPSHOT_FILE_x			= "OUTPUT_SNAPSHOT_FILE_*";
char * Snapshot_Output::OUTPUT_SNAPSHOT_FORMAT_x		= "OUTPUT_SNAPSHOT_FORMAT_*";
char * Snapshot_Output::OUTPUT_SNAPSHOT_TIME_FORMAT_x	= "OUTPUT_SNAPSHOT_TIME_FORMAT_*";
char * Snapshot_Output::OUTPUT_SNAPSHOT_INCREMENT_x		= "OUTPUT_SNAPSHOT_INCREMENT_*";
char * Snapshot_Output::OUTPUT_SNAPSHOT_TIME_RANGE_x	= "OUTPUT_SNAPSHOT_TIME_RANGE_*";
char * Snapshot_Output::OUTPUT_SNAPSHOT_LINK_RANGE_x	= "OUTPUT_SNAPSHOT_LINK_RANGE_*";
char * Snapshot_Output::OUTPUT_SNAPSHOT_COORDINATES_x	= "OUTPUT_SNAPSHOT_COORDINATES_*";
char * Snapshot_Output::OUTPUT_SNAPSHOT_MAX_SIZE_x		= "OUTPUT_SNAPSHOT_MAX_SIZE_*";
char * Snapshot_Output::OUTPUT_SNAPSHOT_LOCATION_FLAG_x	= "OUTPUT_SNAPSHOT_LOCATION_FLAG_*";

//---------------------------------------------------------
//	Snapshot_Output constructor
//---------------------------------------------------------

Snapshot_Output::Snapshot_Output (void) : Static_Service ()
{
	output_data.Initialize (sizeof (Output_Data), 10);

	output_flag = false;
	vehicle = time = dir = lane = vehtype = driver = pass = 0;
	speed = offset = accel = 0.0;
}

//---------------------------------------------------------
//	Snapshot_Output destructor
//---------------------------------------------------------

Snapshot_Output::~Snapshot_Output (void)
{
	Close ();
}

//---------------------------------------------------------
//	Add_Keys
//---------------------------------------------------------

void Snapshot_Output::Add_Keys (void)
{
	char *keys [] = {
		OUTPUT_SNAPSHOT_FILE_x,
		OUTPUT_SNAPSHOT_FORMAT_x,
		OUTPUT_SNAPSHOT_TIME_FORMAT_x,
		OUTPUT_SNAPSHOT_INCREMENT_x,
		OUTPUT_SNAPSHOT_TIME_RANGE_x,
		OUTPUT_SNAPSHOT_LINK_RANGE_x,
		OUTPUT_SNAPSHOT_COORDINATES_x,
		OUTPUT_SNAPSHOT_MAX_SIZE_x,
		OUTPUT_SNAPSHOT_LOCATION_FLAG_x,
		NULL
	};

	if (exe != NULL) {
		exe->Key_List (keys);
	}
}

//---------------------------------------------------------
//	Read_Control
//---------------------------------------------------------

void Snapshot_Output::Read_Control (int steps_per_second)
{
	if (exe == NULL) return;

	//---- read the output snapshot data ----

	int i, num;

	num = exe->Highest_Control_Group (OUTPUT_SNAPSHOT_FILE_x, 0);

	if (num != 0) {
		Output_Data *output;
		char *str_ptr, buffer [FIELD_BUFFER];
		int increment;

		//---- allocate array space ----

		if (!output_data.Max_Records (num)) {
			exe->Error ("Insufficient Memory for %d Output Snapshot Groups", num);
		}

		//---- process each file ----

		for (i=1; i <= num; i++) {

			str_ptr = exe->Get_Control_String (OUTPUT_SNAPSHOT_FILE_x, i);

			if (str_ptr == NULL) continue;

			char *name_ptr = exe->Project_Filename (str_ptr);

			output = (Output_Data *) output_data.New_Record ();

			output->time_range = new Time_Range (steps_per_second);
			output->link_range = NULL;
			output->coord_flag = false;
			output->size_flag = false;
			output->max_size = 0;

			output->file = new Snapshot_File (Db_Code::CREATE);

			exe->Break_Check (7);
			exe->Print (1);
			str_fmt (buffer, sizeof (buffer), "Output Snapshot File #%d", i);

			output->file->File_Type (buffer);

			//---- set the output format ----

			str_ptr = exe->Get_Control_String (OUTPUT_SNAPSHOT_FORMAT_x, i);

			if (str_ptr != NULL) {
				output->file->Dbase_Format (str_ptr);
			}
			if (exe->Get_Control_Flag (OUTPUT_SNAPSHOT_LOCATION_FLAG_x, i) ||
				output->file->Dbase_Format () == Db_Code::VERSION3) {
				output->file->Location_Flag (true);
			}
			output->file->Open (name_ptr);

			//---- get the time format ----

			str_ptr = exe->Get_Control_String (OUTPUT_SNAPSHOT_TIME_FORMAT_x, i);

			if (str_ptr != NULL) {
				if (!output->time_range->Format (str_ptr)) {
					exe->Error ("Output Snapshot Time Format %s was Unrecognized", str_ptr);
				}
				exe->Print (1, "Output Snapshot Time Format #%d = %s", i, output->time_range->Time_Code ());
			}

			//---- get the increment ----

			str_ptr = exe->Get_Control_String (OUTPUT_SNAPSHOT_INCREMENT_x, i);

			if (str_ptr != NULL) {
				output->time_range->Increment (str_ptr);
				increment = output->time_range->Increment ();

				if (increment < 1 || increment > output->time_range->Duration ("1:00")) {
					exe->Error ("Output Snapshot Increment %s is Out of Range", 
						output->time_range->Format_Duration (increment));
				}
			} else {
				output->time_range->Increment ("0:15");
				increment = output->time_range->Increment ();
			}
			exe->Print (1, "Output Snapshot Increment #%d = %s", i, 
				output->time_range->Format_Duration (increment));

			if (output->time_range->Format () == Time_Step::SECONDS) {
				exe->Print (0, " second%c", (increment != output->time_range->Duration (1) ? 's' : ' '));
			} else if (output->time_range->Format () == Time_Step::HOURS) {
				exe->Print (0, " hour%c", (increment != output->time_range->Duration (1.0) ? 's' : ' '));
			}

			//---- get the time range ----

			str_ptr = exe->Get_Control_String (OUTPUT_SNAPSHOT_TIME_RANGE_x, i);

			if (str_ptr != NULL) {
				if (!output->time_range->Add_Ranges (str_ptr)) {
					exe->File_Error ("Output Snapshot Time Range", str_ptr);
				}
				exe->Print (1, "Output Snapshot Time Range #%d = ", i);

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

			str_ptr = exe->Get_Control_String (OUTPUT_SNAPSHOT_LINK_RANGE_x, i);

			if (str_ptr != NULL) {
				exe->Print (1, "Output Snapshot Link Range #%d = %s", i, str_ptr);

				output->link_range = new Data_Range ();

				if (!output->link_range->Add_Ranges (str_ptr)) {
					exe->File_Error ("Output Snapshot Link Range", str_ptr);
				}
				if (output->link_range->Num_Ranges () == 0) {
					delete output->link_range;
					output->link_range = NULL;
				}
			}

			//---- get the coordinates ----

			str_ptr = exe->Get_Control_String (OUTPUT_SNAPSHOT_COORDINATES_x, i);

			if (str_ptr != NULL) {
				double dvalue;
				Network_Service *net = (Network_Service *) exe;

				exe->Print (1, "Output Snapshot Coordinates #%d = %s", i, str_ptr);

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

			//---- get the max size ----

			str_ptr = exe->Get_Control_String (OUTPUT_SNAPSHOT_MAX_SIZE_x, i);

			if (str_ptr != NULL) {
				int size = atoi (str_ptr);

				if (size < 0 || size > 2048) {
					exe->Error ("Output Snapshot Maximum Size %d is Out of Range (0..2048)", size);
				}
				output->max_size = size * 1024 * 1024;

				exe->Print (1, "Output Snapshot Maximum Size #%d = %d megabytes", i, size);
			}

			//---- get the location flag ----

			str_ptr = exe->Get_Control_String (OUTPUT_SNAPSHOT_LOCATION_FLAG_x, i);

			if (str_ptr != NULL) {
				exe->Print (1, "Output Snapshot Location Flag #%d = %s", i, str_ptr);
			}
			if (output->file->Dbase_Format () == Db_Code::VERSION3) {
				output->time_range->Format (Time_Step::SECONDS);
			}

			if (!output_data.Add ()) {
				exe->Error ("Adding Snapshot Output Group");
			}
			output_flag = true;
		}
	}
	return;

coord_error:
	exe->Error ("Output Snapshot Coordinate Data #%d", i);
}

//---------------------------------------------------------
//	In_Range
//---------------------------------------------------------

bool Snapshot_Output::In_Range (int step, int dir)
{
	if (output_flag) {
		Output_Data *output;

		for (output = (Output_Data *) output_data.First (); output != NULL; output = (Output_Data *) output_data.Next ()) {
			if (output->size_flag) continue;
			if (output->time_range->In_Range (step) && output->time_range->At_Increment (step)) {
				if (dir > 0 && (output->coord_flag || output->link_range != NULL)) {
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

void Snapshot_Output::Output (void)
{
	if (output_flag) {
		Output_Data *output = (Output_Data *) output_data.Record ();

		for (; output != NULL; output = (Output_Data *) output_data.Next ()) {
			if (output->size_flag) continue;
			if (output->time_range->In_Range (time) && output->time_range->At_Increment (time)) {
				double length;				
				Dir_Data *dir_ptr;
				Link_Data *link_ptr;
				Network_Service *net = (Network_Service *) exe;

				//---- check the file size ----

				if (output->max_size > 0) {
					Db_File *file = output->file;

					if ((file->Offset () + file->Max_Size ()) > output->max_size) {
						output->size_flag = true;
						continue;
					}
				}
				dir_ptr = net->dir_array->Record (dir);
				if (dir_ptr == NULL) continue;

				link_ptr = net->link_array->Record (dir_ptr->Link ());
				if (link_ptr == NULL) continue;

				if (output->link_range != NULL) {
					if (!output->link_range->In_Range (link_ptr->Link ())) continue;
				}
				length = net->UnRound (link_ptr->Length ());

				if (offset > length) {
					offset = length;
				} else if (offset < 0) {
					offset = 0;
				}
				output->file->Vehicle (vehicle);
				output->file->Time (output->time_range->Format_Step (time));

				if (output->file->LinkDir_Type () == Db_Code::LINK_SIGN) {
					output->file->Link (-link_ptr->Link ());
				} else {
					output->file->Link (link_ptr->Link ());

					if (output->file->LinkDir_Type () == Db_Code::LINK_DIR) {
						output->file->Dir (dir_ptr->Dir ());
					}
				}
				output->file->Lane (lane);
				output->file->Offset (offset);
				output->file->Speed (speed);
				output->file->VehType (vehtype);
				output->file->Accel (accel);
				output->file->Driver (driver);
				output->file->Passengers (pass);

				if (output->coord_flag || output->file->Location_Flag () || 
					output->file->LinkDir_Type () == Db_Code::LINK_NODE) {

					int ax, ay, bx, by;
					Node_Data *node_ptr;

					if (dir_ptr->Dir () == 0) {
						node_ptr = net->node_array->Record (link_ptr->Bnode ());
						bx = node_ptr->X ();
						by = node_ptr->Y ();
						node_ptr = net->node_array->Record (link_ptr->Anode ());
					} else {
						node_ptr = net->node_array->Record (link_ptr->Anode ());
						bx = node_ptr->X ();
						by = node_ptr->Y ();
						node_ptr = net->node_array->Record (link_ptr->Bnode ());
					}
					bx -= ax = node_ptr->X ();
					by -= ay = node_ptr->Y ();

					ax += (int) (bx * offset / length + 0.5);
					ay += (int) (by * offset / length + 0.5);

					if (output->coord_flag) {
						if (ax < output->x1 || ax > output->x2 ||
							ay < output->y1 || ay > output->y2) continue;
					}
					if (output->file->Location_Flag ()) {
						bx = net->compass.Direction ((double) bx, (double) by);
						if (output->file->Dbase_Format () == Db_Code::VERSION3) {
							bx = 90 - bx;
							if (bx > 180) {
								bx = bx - 360;
							} else if (bx < -179) {
								bx = 360 + bx;
							}
						}
						output->file->X (net->UnRound (ax));
						output->file->Y (net->UnRound (ay));
						output->file->Z (0.0);
						output->file->Bearing ((double) bx);
						output->file->User (0);
					}
					if (output->file->LinkDir_Type () == Db_Code::LINK_NODE) {
						output->file->Dir (node_ptr->Node ());
					}
				}
				if (!output->file->Write ()) {
					exe->Error ("Writing Snapshot Output File");
				}
			}
		}
	}
}

//---------------------------------------------------------
//	Close
//---------------------------------------------------------

void Snapshot_Output::Close (void)
{
	if (!output_flag) return;

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
		if (output->link_range != NULL) {
			delete output->link_range;
			output->link_range = NULL;
		}
	}
	output_flag = false;
}
