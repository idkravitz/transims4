//*********************************************************
//	Execution_Service.cpp - general execution class
//*********************************************************

#include "Execution_Service.hpp"
#include "Static_Service.hpp"

#include "Db_Code.hpp"
#include "Utility.hpp"

char * Execution_Service::TITLE						= "TITLE";
char * Execution_Service::REPORT_FILE				= "REPORT_FILE";
char * Execution_Service::REPORT_FLAG				= "REPORT_FLAG";
char * Execution_Service::PROJECT_DIRECTORY			= "PROJECT_DIRECTORY";
char * Execution_Service::DEFAULT_FILE_FORMAT		= "DEFAULT_FILE_FORMAT";
char * Execution_Service::MAX_WARNING_MESSAGES		= "MAX_WARNING_MESSAGES";
char * Execution_Service::MAX_WARNING_EXIT_FLAG		= "MAX_WARNING_EXIT_FLAG";
char * Execution_Service::TRAVELER_SCALING_FACTOR	= "TRAVELER_SCALING_FACTOR";

char Execution_Service::filename [] = "";

//---------------------------------------------------------
//	Execution_Service - constructor
//---------------------------------------------------------

Execution_Service::Execution_Service (void) : Report_Service (), Control_List ()
{
	char *keys [] = {TITLE, REPORT_FILE, REPORT_FLAG, PROJECT_DIRECTORY, DEFAULT_FILE_FORMAT, 
					 MAX_WARNING_MESSAGES, MAX_WARNING_EXIT_FLAG, TRAVELER_SCALING_FACTOR, NULL};

	Key_List (keys);

	report_key [0] = '\0';
	report_flag = false;

	report_name.Data_Size (sizeof (Report_Name));
	report_number.Data_Size (sizeof (Report_Number));

	project_directory = new char [1];
	project_directory [0] = '\0';

	Max_Records (50);

	Static_Service service (this);

	service.Default_Format (Db_Code::VERSION3);
}

//---------------------------------------------------------
//	Execution_Service - destructor
//---------------------------------------------------------

Execution_Service::~Execution_Service (void)
{
	if (project_directory != NULL) {
		delete [] project_directory;
	}
}

//-----------------------------------------------------------
//	Key_List - set the control key list
//-----------------------------------------------------------

void Execution_Service::Key_List (char *keys []) 
{
	for (int i=0; keys [i] != NULL; i++) {
		Add_Control_Key (keys [i]);
	}
}

//-----------------------------------------------------------
//	Start_Execution - run the program 
//-----------------------------------------------------------

int Execution_Service::Start_Execution (int commands, char *command []) 
{
	Program_Status (RUNNING);

	if (Command_Line (commands, command)) {
		if (report_flag) Read_Reports ();

		Program_Control ();

		if (report_flag) {
			List_Reports ();
		} else if (!Highest_Control_Group (report_key, 0)) {
			Print (1);
			Flush ();
		}
        Execute ();
	}
	return (Return_Code ());
}

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Execution_Service::Program_Control (void) 
{
}

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Execution_Service::Execute (void) 
{
	Show_Error ("%s is Not Yet Implemented", Program ());
}

//---------------------------------------------------------
//	Command_Line
//---------------------------------------------------------

bool Execution_Service::Command_Line (int commands, char *command [])
{
	bool banner_flag, syntax_flag, help_flag, command_flag, key_flag, param_flag;
	char *str_ptr, error [STRING_BUFFER];

	banner_flag = syntax_flag = help_flag = command_flag = key_flag = param_flag = false;

	if (commands < 2) {
		Show_Banner ();
		Show_Syntax ();
		banner_flag = syntax_flag = true;

		Control_File (Get_Control_File ());
		if (Control_File () == NULL) return (false);

	} else {

		//---- process the command line -----

		for (int i=1; i < commands; i++) {
			if (command [i][0] == '-') {

				if (command [i][1] == 'Q' || command [i][1] == 'q') {
					Quiet (true);
				} else if (command [i][1] == 'H' || command [i][1] == 'h') {
					Syntax_Help ();
					help_flag = syntax_flag = banner_flag = true;
				} else if (command [i][1] == 'P' || command [i][1] == 'p') {
					Pause (true);
				} else if (command [i][1] == 'N' || command [i][1] == 'n') {
					No_Pause (true);
				} else if (command [i][1] == 'B' || command [i][1] == 'b') {
					Batch (true);
				} else if (command [i][1] == 'K' || command [i][1] == 'k') {
					key_flag = true;
				} else {
					str_fmt (error, sizeof (error), "Unrecognized Command Line Option = %s", command [i]);
					goto error_message;
				}
			} else if (Control_File () == NULL) {
				Control_File (command [i]);
			} else if (Enable_Partitions ()) {
				if (Partition ()) {
					str_cpy (error, sizeof (error), "Too Many Partition Parameters");
					goto error_message;
				}
				int num = atoi (command [i]);

				if (num < 0 || num > 676) {
					str_fmt (error, sizeof (error), "Partition Number %d is Out of Range (0..676)", num);
					goto error_message;
				} else {
					Partition (true);
					Partition_Number (num); 
					str_fmt (error, sizeof (error), ".t%c%c", ('A' + (num / 26)), ('A' + (num % 26)));
					Extension (error);
				}
			} else if (Enable_Parameter ()) {
				if (param_flag) {
					str_cpy (error, sizeof (error), "Too Many Parameters");
					goto error_message;
				} else {
					Extension (command [i]);
					param_flag = true;
				}
			} else {
				command_flag = true;
			}
		}
		if (Control_File () == NULL) {
			if (help_flag) {
				if (command_flag) {
					Show_Warning ("%s has Too Many Parameters", Program ());
				}
				Pause (true);
				Exit_Stat (DONE);
				return (false);
			}
			str_cpy (error, sizeof (error), "No Control File");
			goto error_message;
		}
		if (!banner_flag) {
			Show_Banner ();
			banner_flag = true;
		}
	}

	//---- read the control file ----

	if (!Read_Control_File ((char *) Control_File ())) {
		str_fmt (error, sizeof (error), "Reading Control File: \"%s\"", Control_File ());
		goto error_message;
	}

	//---- open the output report ----

	Title (Get_Control_String (TITLE));

	Open_Report (Get_Control_String (REPORT_FILE), Get_Control_Flag (REPORT_FLAG));
	
	//---- show parameter warning ----

	if (command_flag) {
		Print (1);
		Warning ("%s has Too Many Parameters", Program ());
		Show_Message (1);
	}

	//---- show unrecognized control key warnings ----

	if (key_flag) {
		Key_Check ();
	}

	//---- print global keys ----

	str_ptr = Get_Control_String (PROJECT_DIRECTORY);

	if (str_ptr != NULL) {
		Print_Filename (2, "Project Directory", str_ptr);
		Project_Directory (str_ptr);
	}

	str_ptr = Get_Control_String (DEFAULT_FILE_FORMAT);

	if (str_ptr != NULL) {
		Db_Code code;
		Print (2, "Default File Format = %s", str_ptr);
		code.Default_Format (code.Format_Code (str_ptr));
	}

	//---- warning messages ----

	str_ptr = Get_Control_String (MAX_WARNING_MESSAGES);

	if (str_ptr != NULL) {
		Max_Warnings (atoi (str_ptr));
		if (Max_Warnings () < 0) {
			Error ("Maximum Warning Messages %s is Out of Range", str_ptr);
		}
		Print (2, "Maximum Warning Messages = %d", Max_Warnings ());
	}

	str_ptr = Get_Control_String (MAX_WARNING_EXIT_FLAG);

	if (str_ptr != NULL) {
		Warning_Flag (Get_Control_Flag (MAX_WARNING_EXIT_FLAG));

		Print (1, "Maximum Warning Exit Flag = %s", str_ptr);
	}

	//---- set the traveler scaling factor ----

	str_ptr = Get_Control_String (TRAVELER_SCALING_FACTOR);

	if (str_ptr != NULL) {
		int factor = atoi (str_ptr);
		Static_Scale scale (factor);
		Print (2, "Traveler Scaling Factor = %d", factor);
	}
	return (true);

error_message: 
	Quiet (false);
	if (!banner_flag) Show_Banner ();
	if (!syntax_flag) Show_Syntax ();
	Show_Error (error);
	return (false);
}

//-----------------------------------------------------------
//	Show_Syntax - show the program syntax
//-----------------------------------------------------------

void Execution_Service::Show_Syntax (void) 
{
	if (Enable_Partitions ()) {
		Show_Message ("Syntax is %s [-flag] [control_file] [partition]\n", Program ());
	} else if (Enable_Parameter ()) {
		Show_Message ("Syntax is %s [-flag] [control_file] [parameter]\n", Program ());
	} else {
		Show_Message ("Syntax is %s [-flag] [control_file]\n", Program ());
	}
}

//-----------------------------------------------------------
//	Syntax_Help - show the program syntax and keys
//-----------------------------------------------------------

void Execution_Service::Syntax_Help (void) 
{
	bool quiet_flag = Quiet ();
	char *str_ptr;

	Report_Name *name;

	Show_Banner ();
	Show_Syntax ();
	Show_Message (2, "Optional Flags:");
	Show_Message ("\t-Q[uiet]    = execute without screen messages");
	Show_Message ("\t-H[elp]     = show program syntax and control keys");
	Show_Message ("\t-K[eyCheck] = list unrecognized control file keys");
	Show_Message ("\t-P[ause]    = pause before exiting");
	Show_Message ("\t-N[oPause]  = never pause before exiting");
	Show_Message ("\t-B[atch]    = execute in batch processing mode");

	Show_Message (2, "Control File Keys:");

	for (str_ptr = First_Key (); str_ptr; str_ptr = Next_Key ()) {
		Show_Message ("\t%s", str_ptr);
	}

	//---- list the report options ----

	if (report_name.Num_Records () > 0) {
		Show_Message (2, "Report Options:");

		for (name = (Report_Name *) report_name.First (); name; name = (Report_Name *) report_name.Next ()) {
			Show_Message ("\t%s", name->name);
		}
	}
	Show_Message (1);
	Quiet (quiet_flag);
}

//-----------------------------------------------------------
//	Key_Check - list skipped keys from the control file
//-----------------------------------------------------------

void Execution_Service::Key_Check (void) 
{
	char *str_ptr = First_Skip_Key ();

	if (str_ptr == NULL) return;

	Print (1);
	for (; str_ptr; str_ptr = Next_Skip_Key ()) {
		Warning ("Control Key \"%s\" was Not Used", str_ptr);
	}
	Show_Message (1);
}

//-----------------------------------------------------------
//	Report_List - set the report list
//-----------------------------------------------------------

void Execution_Service::Report_List (char *names []) 
{
	int i;
	Report_Name *ptr;

	if (report_name.Max_Records () == 0) {
		report_name.Max_Records (10);
	}

	for (i=0; names [i] != NULL; i++) {
		ptr = (Report_Name *) report_name.New_Record (true);

		ptr->name = names [i];

		report_name.Add ();
	}

	//---- add the report key ----

	if (i > 0) {
		char *keys [] = {NULL, NULL};

		char *str_ptr;

		str_ptr = str_cpy (report_key, sizeof (report_key), (char *) Program ());

		while (*str_ptr != '\0') {
			if (*str_ptr >= 'a' && *str_ptr <= 'z') {
				*str_ptr = *str_ptr - 'a' + 'A';
			}
			str_ptr++;
		}
		str_cat (report_key, sizeof (report_key), "_REPORT_*");

		keys [0] = report_key;

		Key_List (keys);
		report_flag = true;
	}
}

//---------------------------------------------------------
//	Read_Reports
//---------------------------------------------------------

void Execution_Service::Read_Reports (void) 
{
	int i, len, num_reports;
	char *str_ptr;

	Report_Name *name;
	Report_Number *report;

	//---- get the number of selected reports ----

	num_reports = Highest_Control_Group (report_key, 0);

	if (num_reports <= 0) {
		report_flag = false;
		return;
	}

	report_number.Max_Records (num_reports);

	for (i=1; i <= num_reports; i++) {
		str_ptr = Get_Control_String (report_key, i);

		if (str_ptr == NULL) continue;

		report = (Report_Number *) report_number.New_Record (true);

		//---- identify the report ----

		for (name = (Report_Name *) report_name.First (); name; name = (Report_Name *) report_name.Next ()) {
			if (str_cmp (str_ptr, name->name) == 0) {
				name->flag = true;
				report->type = report_name.Record_Index ();
				break;
			} else {

				//---- check for a variable length report label ----

				len = (int) strlen (name->name) - 1;

				if (name->name [len] == '*') {
					if (str_cmp (str_ptr, name->name, len) == 0) {
						name->flag = true;
						report->type = report_name.Record_Index ();
						report->data = atof (str_ptr + len);
						break;
					}
				}
			}
		}

		//---- check for unrecognized report types ----

		if (report->type == 0) {
			Error ("Unrecognized Report Type = %s", str_ptr);
		} else {
			report_number.Add ();
		}
	}
}

//-----------------------------------------------------------
//	List_Reports - print the report keys
//-----------------------------------------------------------

void Execution_Service::List_Reports (void) 
{
	int i, offset, num_reports;
	char *str_ptr;
	bool first;

	if (!report_flag) return;
	report_flag = false;

	//---- get the number of selected reports ----

	num_reports = Highest_Control_Group (report_key, 0);
	offset = 0;

	if (num_reports > 0) {
		first = true;

		for (i=1; i <= num_reports; i++) {
			str_ptr = Get_Control_String (report_key, i);

			if (str_ptr == NULL) continue;

			if (first) {
				Print (2, "%s Reports: ", Program ());
				first = false;
				offset = (int) strlen (Program ()) + 10;
			} else {
				Print (1, "%*c", offset, BLANK);
			}
			Print (0, "%2d. %s", i, str_ptr);
		}
	}
	Print (1);
	Flush ();
}

//-----------------------------------------------------------
//	Report_Flag - check report type flag
//-----------------------------------------------------------

bool Execution_Service::Report_Flag (int type) 
{
	Report_Name *ptr = (Report_Name *) report_name [type];

	if (ptr != NULL) {
		return (ptr->flag);
	} else {
		return (false);
	}
}

//-----------------------------------------------------------
//	Report_Type - get the report type
//-----------------------------------------------------------

int Execution_Service::Report_Type (int number) 
{
	Report_Number *ptr = (Report_Number *) report_number [number];

	if (ptr != NULL) {
		return (ptr->type);
	} else {
		return (0);
	}
}

//-----------------------------------------------------------
//	Report_Data - get the report data
//-----------------------------------------------------------

double Execution_Service::Report_Data (int number) 
{
	Report_Number *ptr;
	
	if (number == 0) {
		ptr = (Report_Number *) report_number.Record ();
	} else {
		ptr = (Report_Number *) report_number [number];
	}

	if (ptr != NULL) {
		return (ptr->data);
	} else {
		return (0.0);
	}
}

//-----------------------------------------------------------
//	First_Report - get the first report type
//-----------------------------------------------------------

int Execution_Service::First_Report (void) 
{
	Report_Number *ptr = (Report_Number *) report_number.First ();

	if (ptr != NULL) {
		return (ptr->type);
	} else {
		return (0);
	}
}

//-----------------------------------------------------------
//	Next_Report - get the next report type
//-----------------------------------------------------------

int Execution_Service::Next_Report (void) 
{
	Report_Number *ptr = (Report_Number *) report_number.Next ();

	if (ptr != NULL) {
		return (ptr->type);
	} else {
		return (0);
	}
}

//-----------------------------------------------------------
//	Project_Directory
//-----------------------------------------------------------

void Execution_Service::Project_Directory (char *dir) 
{ 
	if (dir != NULL) {
		delete [] project_directory;

		int len = (int) strlen (dir) + 2;

		project_directory = new char [len];

		if (project_directory == NULL) {
			Error ("Insufficient Memory for Project Directory");
		}
		str_cpy (project_directory, len, dir);

		Fix_Directory (project_directory);
	} else {
		project_directory [0] = '\0';
	}
}
//-----------------------------------------------------------
//	Filename
//-----------------------------------------------------------

char * Execution_Service::Filename (char *dir, char *name, char *ext) 
{ 
	str_cpy (filename, sizeof (filename), dir);
	str_cat (filename, sizeof (filename), name);

	if (ext != NULL) {
		str_cat (filename, sizeof (filename), ext);
	}
	return (filename);
}
