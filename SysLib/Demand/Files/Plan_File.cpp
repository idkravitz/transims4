
//*********************************************************
//	Plan_File.cpp - Plan File Input/Output
//*********************************************************

#include "Plan_File.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Plan_File constructor
//---------------------------------------------------------

Plan_File::Plan_File (Access_Type access, Memory_Type memory, Sort_Type sort) : 
	Ext_File (access), Static_Scale ()
{
	Setup (memory, sort);
}

Plan_File::Plan_File (char *filename, Access_Type access, Memory_Type memory, Sort_Type sort) : 
	Ext_File (filename, access), Static_Scale ()
{
	Setup (memory, sort);
}

//---------------------------------------------------------
//	Plan_File destructor
//---------------------------------------------------------

Plan_File::~Plan_File (void)
{
	Close ();

	Delete_Plan ();
}

//---------------------------------------------------------
//	Setup
//---------------------------------------------------------

void Plan_File::Setup (Memory_Type memory, Sort_Type sort)
{
	num_record = num_plan = num_traveler = num_trip = max_data = 0;
	plan = NULL;

	File_Type ("Plan File");
	File_ID ("Plan");

	Setup_Record ();

	Plan_Memory (memory);
	Plan_Sort (sort);
	Node_Based_Flag (true);
}

//---------------------------------------------------------
//	Open
//---------------------------------------------------------

bool Plan_File::Open (int ext_number)
{
	return (Ext_File::Open (ext_number));
}

bool Plan_File::Open (char *filename)
{
	bool stat;

	if (filename != NULL) {
		Filename (filename);
	}
	filename = Filename ();

	if (*filename != '\0' && File_Access () != CREATE) {
		stat = Read_Plan_Def ();
	} else {
		stat = false;
	}

	//---- open the file ----

	if (Db_File::Open ()) {
		if (!stat || File_Access () == CREATE) {
			return (Write_Plan_Def ());
		}
		return (true);
	}
	return (false);
}

//---------------------------------------------------------
//	Read
//---------------------------------------------------------

bool Plan_File::Read (long offset)
{
	//---- check the file status ----

	if (!Check_File ()) return (false);
	if (plan == NULL) return (Status (RECORD_SIZE));
	if (File_Access () != READ) return (Status (ERROR));

	//---- move to a specified location in the file ----

	if (offset >= 0) {
		if (!Offset (offset)) return (false);
	}

	//---- allocate space ----

	if (allocate_memory) {
		if (!Setup_Record ()) return (false);
	}

	//---- read the next plan ----

	if (Record_Format () == BINARY) {
		int num_token;

		if (!Db_File::Read (plan, (sizeof (Plan_Data) - sizeof (int)))) return (false);

		if (time_sort) {
			int temp = plan->key1;
			plan->key1 = plan->key2;
			plan->key2 = temp;
		}
		num_record++;
		num_plan++;

		if (Leg () == 2) {
			num_trip++;
		} else if (Leg () == 1 && Trip () == 1) {
			num_traveler++;
		}
		num_token = Tokens ();
		if (num_token > 0) {
			if (!Check_Size (num_token)) return (false);
			if (!Db_File::Read (&(plan->data [0]), num_token * sizeof (int))) return (Status (PLAN_FIELDS));

			num_record++;
		}
		return (true);
	} else {

		int field, max_field, value;
		char buffer [40], *ptr;

		field = max_field = 0;

		while (Db_File::Read ()) {
			num_record++;

			ptr = Clean_Record ();

			//---- check for a blank record ----

			if (ptr == NULL || *ptr == '\0') continue;

			//---- process the plan record ----

			while (ptr != NULL) {
				ptr = Get_Token (ptr, buffer, sizeof (buffer));

				if (buffer [0] == '\0') break;
				field++;
				value = atol (buffer);

				switch (field) {
					case 1:		//---- traveler id ----
						Traveler (value);
						num_plan++;
						break;
					case 2:		//---- user field ----
						break;
					case 3:		//---- trip id ----
						Trip (value);
						break;
					case 4:		//---- leg id ----
						Leg (value);
						if (value == 2) {
							num_trip++;
						} else if (value == 1 && Trip () == 1) {
							num_traveler++;
						}
						break;
					case 5:		//---- time ----
						Time (value);
						break;
					case 6:		//---- start id ----
						Start_ID (value);
						break;
					case 7:		//---- start type ----
						Start_Type (value);
						break;
					case 8:		//---- end id ----
						End_ID (value);
						break;
					case 9:		//---- end type ----
						End_Type (value);
						break;
					case 10:	//---- duration ----
						Duration (value);
						break;
					case 11:	//---- stop time ----
						Stop_Time (value);
						break;
					case 12:	//---- max time flag ----
						break;
					case 13:	//---- cost ----
						Cost (value);
						break;
					case 14:	//---- gcf ----
						GCF (value);
						break;
					case 15:	//---- driver flag ----
						Driver_Flag (value);
						break;
					case 16:	//---- mode ----
						Mode (value);
						break;
					case 17:	//---- number of tokens ----
						if (value < 0) {
							Status (PLAN_FIELDS);
							return (false);
						}
						Tokens (value);
						max_field = value + 17;

						if (value == 0) return (true);
						if (!Check_Size (value)) return (false);
						break;

					default:	//---- token value ----

						if (field > max_field) {
							Status (PLAN_FIELDS);
							return (false);
						}
						plan->data [field - 18] = value;

						if (field == max_field) return (true);
						break;
				}
			}
		}
		if (field != 0) {
			return (Status (PLAN_FIELDS));
		}
		return (false);
	}
}

//---------------------------------------------------------
//	Write
//---------------------------------------------------------

bool Plan_File::Write (Plan_Data *data)
{
	int num_token;
	FILE *file;

	//---- check the file status ----

	if (!Check_File ()) return (false);
	if (File_Access () == READ) return (Status (ERROR));

	Plan_Data *backup = NULL;

	if (data != NULL) {
		backup = plan;
		plan = data;
	} else {
		if (plan == NULL) return (Status (RECORD_SIZE));
	}

	//---- write the plan data ----

	file = File ();
	num_token = Tokens ();

	if (Record_Format () == BINARY) {
		if (time_sort) {
			int size, temp;
			size = sizeof (Plan_Data) - sizeof (int);

			memcpy (backup, plan, size);
			
			temp = backup->key1;
			backup->key1 = backup->key2;
			backup->key2 = temp;

			if (!Db_File::Write (backup, size)) goto reset;
		} else {
			if (!Db_File::Write (plan, (sizeof (Plan_Data) - sizeof (int)))) goto reset;
		}

		num_record++;
		num_plan++;

		if (Leg () == 2) {
			num_trip++;
		} else if (Leg () == 1 && Trip () == 1) {
			num_traveler++;
		}
		if (num_token > 0) {
			if (!Db_File::Write (&(plan->data [0]), num_token * sizeof (int))) goto reset;
			num_record++;
		}

	} else {

		if (fprintf (file, "%d 0 %d %d\n%d %d %d %d %d\n%d %d %d %d %d\n%d %d\n%d\n",
			Traveler (), Trip (), Leg (), 
			Time (), Start_ID (), Start_Type (), End_ID (), End_Type (),
			Duration (), Stop_Time (), 1, Cost (), GCF (), 
			Driver_Flag (), Mode (), 
			num_token) < 0) goto reset;

		num_record += 5;
		num_plan++;

		if (Leg () == 2) {
			num_trip++;
		} else if (Leg () == 1 && Trip () == 1) {
			num_traveler++;
		}

		//---- write tokens ----

		if (num_token > 0) {
			int field;
			int i = 0;

			switch (Mode ()) {
				case AUTO_MODE:		//---- auto ----
					if (Driver_Flag ()) {

						//---- vehicle ID and number of passengers ----

						i = 2;
						if (fprintf (file, "%d %d\n", plan->data [0], plan->data [1]) < 0) goto reset;
						num_record++;
					}
					break;
				case TRANSIT_MODE:		//---- transit ----
					if (Driver_Flag ()) {

						//---- schedule pairs, vehicle ID, and route ID ----

						i = 3;
						if (fprintf (file, "%d %d %d\n", plan->data [0], plan->data [1], plan->data [2]) < 0) goto reset;
						num_record++;
					}
					break;
				default:
					break;
			}

			//---- print the rest of the fields in groups of 10 ----

			for (field=0; i < num_token; i++, field++) {
				if (!field) {
					if (fprintf (file, "%d", plan->data [i]) < 0) goto reset;
					num_record++;
				} else if (!(field % 10)) {
					if (fprintf (file, "\n%d", plan->data [i]) < 0) goto reset;
					num_record++;
				} else {
					if (fprintf (file, " %d", plan->data [i]) < 0) goto reset;
				}
			}
			if (field) {
				if (fprintf (file, "\n") < 0) goto reset;
			}
		}

		//---- add a blank line at the end of the plan ----

		if (fprintf (file, "\n") < 0) goto reset;
		num_record++;

		Flush ();
	}
	if (data != NULL) {
		plan = backup;
	}
	return (true);

reset:
	if (data != NULL) {
		plan = backup;
	}
	return (false);
}

//---------------------------------------------------------
//	Plan
//---------------------------------------------------------

bool Plan_File::Plan (Plan_Data *data)
{
	if (data == NULL) return (Status (NULL_POINTER));

	int size = data->tokens;

	if (!Check_Size (size)) return (false);

	if (size) size--;		

	size = sizeof (Plan_Data) + size * sizeof (int);

    memcpy (plan, data, size);
	return (true);
}

//---------------------------------------------------------
//	Delete_Plan
//---------------------------------------------------------

void Plan_File::Delete_Plan (Plan_Data *data)
{
	if (data == NULL) {
		data = plan;
		max_data = 0;
	}
	if (data != NULL) {
		free (data);
		data = NULL;
	}
}

//---------------------------------------------------------
//	Setup_Record
//---------------------------------------------------------

bool Plan_File::Setup_Record (void)
{
	max_data = 0;
	plan = (Plan_Data *) malloc (sizeof (Plan_Data));

	if (plan == NULL) return (Status (RECORD_SIZE));

	max_data = 1;
	memset (plan, '\0', sizeof (Plan_Data));
	return (true);
}

//---------------------------------------------------------
//	Check_Size
//---------------------------------------------------------

bool Plan_File::Check_Size (int size)
{
	if (size <= max_data) return (true);

	max_data = 0;
	if (size < 1) size = 1;

	size_t mem_size = sizeof (Plan_Data) + (size - 1) * sizeof (int);

	plan = (Plan_Data *) realloc (plan, mem_size);

	if (plan == NULL) return (Status (RECORD_SIZE));

	max_data = size;

	return (true);
}

//---------------------------------------------------------
//	Data
//---------------------------------------------------------

int * Plan_File::Data (int *max_token)
{
	if (plan != NULL) {
		*max_token = max_data;
		return (plan->data);
	} else {
		*max_token = 0;
		return (NULL);
	}
}

//---------------------------------------------------------
//	Path
//---------------------------------------------------------

int * Plan_File::Path (int *num_path)
{
	int offset = 0;
	*num_path = 0;

	//---- mode specific adjustments ----

	switch (Mode ()) {
		case AUTO_MODE:		//---- auto ----
			if (Driver_Flag ()) {	//---- driver ----
				offset = 2;
				//*num_path = Tokens () - offset - plan->data [1];
				*num_path = Tokens () - offset;
			}
			break;
		case TRANSIT_MODE:		//---- transit ----
			if (Driver_Flag ()) {	//---- driver ----
				offset = 3;
				*num_path = Tokens () - offset - 2 * plan->data [0];
			}
			break;
		case BIKE_MODE:		//---- bike ----
		case WALK_MODE:		//---- walk ----
			*num_path = Tokens ();
			break;
		default:	//---- other ----
			offset = 1;
			*num_path = Tokens () - offset;
			break;
	}
	return (plan->data + offset);
}

//---------------------------------------------------------
//	Num_Path
//---------------------------------------------------------

void Plan_File::Num_Path (int num)
{
	//---- mode specific adjustments ----

	switch (Mode ()) {
		case AUTO_MODE:		//---- auto ----
			if (Driver_Flag ()) {	//---- driver ----
				Tokens (num + 2);
			} else {				//---- passenger ----
				Tokens (1);
			}
			break;
		case TRANSIT_MODE:		//---- transit ----
			if (Driver_Flag ()) {	//---- driver ----
				Tokens (num + 3);
			} else {
				Tokens (1);
			}
			break;
		case BIKE_MODE:		//---- bike ----
		case WALK_MODE:		//---- walk ----
			Tokens (num);
			break;
		default:	//---- other ----
			Tokens (num + 1);
			break;
	}
}

//---------------------------------------------------------
//	Mode_ID
//---------------------------------------------------------

int Plan_File::Mode_ID (void)
{
	int mode_id;

	switch (Mode ()) {
		case AUTO_MODE:		//---- auto ----
			mode_id = plan->data [0];
			break;
		case TRANSIT_MODE:		//---- transit ----
			if (Driver_Flag ()) {	//---- driver ----
				mode_id = plan->data [2];	//---- route id ----
			} else {				//---- passenger ----
				mode_id = plan->data [0];		//---- route id ----
			}
			break;
		case BIKE_MODE:		//---- bike ----
		case MAGIC_MODE:		//---- magic move ----	
			mode_id = plan->data [0];		//---- type ----
			break;
		default:	//---- other ----
			mode_id = 0;	//---- not used ----
			break;
	}
	return (mode_id);
}

void Plan_File::Mode_ID (int id)
{
	switch (Mode ()) {
		case AUTO_MODE:		//---- auto ----
			plan->data [0] = id;
			if (Driver_Flag ()) {
				plan->data [1] = 0;		//---- passengers ----
			}
			break;
		case TRANSIT_MODE:		//---- transit ----
			if (Driver_Flag ()) {	//---- driver ----
				plan->data [0] = 0;		//---- schedule pairs ----
				plan->data [1] = 0;		//---- vehicle id ----
				plan->data [2] = id;	//---- route id ----
			} else {				//---- passenger ----
				plan->data [0] = id;		//---- route id ----
			}
			break;
		case BIKE_MODE:		//---- bike ----
		case MAGIC_MODE:		//---- magic move ----	
			plan->data [0] = id;		//---- type ----
			break;
		default:	//---- other ----
			break;
	}
}

//---------------------------------------------------------
//	Vehicle
//---------------------------------------------------------

int Plan_File::Vehicle (void)
{
	switch (Mode ()) {
		case AUTO_MODE:		//---- auto ----
			return (plan->data [0]);
			break;
		case TRANSIT_MODE:		//---- transit ----
			if (Driver_Flag ()) {	//---- driver ----
				return (plan->data [1]);	//---- vehicle id ----
			}
			break;
		default:	//---- other ----
			break;
	}
	return (0);
}

void Plan_File::Vehicle (int veh)
{
	switch (Mode ()) {
		case AUTO_MODE:		//---- auto ----
			plan->data [0] = veh;
			break;
		case TRANSIT_MODE:		//---- transit ----
			if (Driver_Flag ()) {	//---- driver ----
				plan->data [1] = veh;	//---- vehicle id ----
			} else {				//---- passenger ----
			}
			break;
		case BIKE_MODE:		//---- bike ----
		case MAGIC_MODE:		//---- magic move ----	
			break;
		default:	//---- other ----
			break;
	}
}

//---------------------------------------------------------
//	Route
//---------------------------------------------------------

int Plan_File::Route (void)
{
	if (Mode () == TRANSIT_MODE) {
		if (Driver_Flag ()) {
			return (plan->data [2]);
		} else {
			return (plan->data [0]);
		}
	}
	return (0);
}

void Plan_File::Route (int id)
{
	if (Mode () == TRANSIT_MODE) {
		if (Driver_Flag ()) {
			plan->data [2] = id;
		} else {
			plan->data [0] = id;
		}
	}
}

//-----------------------------------------------------------
//	Read_Plan_Def
//-----------------------------------------------------------

bool Plan_File::Read_Plan_Def (void) 
{
	char *str_ptr, filename [FILE_NAME_SIZE], buffer [FIELD_BUFFER];
	FILE *def;

	//---- construct the definition filename ----

	str_cpy (filename, sizeof (filename)-5, Filename ());
	str_cat (filename, sizeof (filename), ".def");

	//---- open the definition file ----

	def = f_open (filename, "rt");

	if (def == NULL) return (false);

	//---- get the header line ----

	if (fgets (filename, sizeof (filename), def) == NULL) return (Status (DEF_FORMAT));

	if (str_cmp (filename, "AECOM HEADER", 12) != 0) return (Status (DEF_FORMAT));

	str_ptr = filename + 12;

	str_ptr = Get_Token (str_ptr, buffer);

	File_Format (buffer);

	if (str_cmp (buffer, "BINARY", 6) != 0 && str_cmp (buffer, "VERSION3", 8) != 0) {
		return (Status (DEF_FORMAT));
	}
	str_ptr = Get_Token (str_ptr, buffer);

	int scale = atoi (buffer);
	if (scale < 2 || scale > 100) return (Status (DEF_FORMAT));
	if (Traveler_Scale () < scale) {
		exe->Error ("Traveler Scaling Factor %d does not match Plan File Factor %d", Traveler_Scale (), scale);
	}
	Traveler_Scale (scale);

	str_ptr = Get_Token (str_ptr, buffer);

	if (str_cmp (buffer, "NODE_BASED", 10) == 0) {
		Node_Based_Flag (true);
	} else if (str_cmp (buffer, "LINK_BASED", 10) == 0) {
		Node_Based_Flag (false);
	} else {
		return (Status (DEF_FORMAT));
	}
	fclose (def);
	return (true);
}

//-----------------------------------------------------------
//	Write_Plan_Def
//-----------------------------------------------------------

bool Plan_File::Write_Plan_Def (void) 
{
	char filename [FILE_NAME_SIZE];
	FILE *def;

	//---- construct the definition filename ----

	str_cpy (filename, sizeof (filename)-5, Filename ());
	str_cat (filename, sizeof (filename), ".def");

	//---- create the definition file ----

	def = f_open (filename, "wt");

	if (def == NULL) return (Status (OPEN_DEF));

	if (fprintf (def, "AECOM HEADER, %s, %d, %s\n", ((Record_Format () == BINARY) ? "BINARY" : "VERSION3"), 
		Traveler_Scale (), ((Node_Based_Flag ()) ? "NODE_BASED" : "LINK_BASED")) < 0) {
		return (Status (DEF_FORMAT));
	}
	fclose (def);
	return (true);
}

