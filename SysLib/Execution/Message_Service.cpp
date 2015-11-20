//*********************************************************
//	Message_Service.cpp - general message class
//*********************************************************

#include "Message_Service.hpp"

#include "Utility.hpp"
#include "Date_Time.hpp"

#include <stdarg.h>

//---------------------------------------------------------
//	Message_Service - constructor
//---------------------------------------------------------

Message_Service::Message_Service (void)
{
	quiet = false;
	status = DONE;
	Program ("Program");
	Version ("1.0");
	CopyRight ("2009 by AECOM Consult");
	time (&starttime);
	progress = 0;
	progress_step = 100;
	warnings = 0;
	max_warnings = 100000;
	warning_flag = true;
	pause = false;
	no_pause = false;
	batch = false;
}

//---------------------------------------------------------
//	Message_Service - destructor
//---------------------------------------------------------

Message_Service::~Message_Service (void)
{
}

//-----------------------------------------------------------
//	Show_Banner
//-----------------------------------------------------------

void Message_Service::Show_Banner ()
{
	if (!Quiet ()) {
		Banner ();
		printf ("\n");
	}
}

//-----------------------------------------------------------
//	Banner
//-----------------------------------------------------------

void Message_Service::Banner (FILE *file)
{
	time_t ltime;
	int  pad1, pad2;
	char buffer [FIELD_BUFFER];

	char blank = ' ';
	char *bar = "********************************************";

	if (file == NULL) file = stdout;
	
	fprintf (file, "\n\t%s", bar);
	fprintf (file, "\n\t|%42c|", blank);

	str_fmt (buffer, sizeof (buffer), "%s - Version %s", Program (), Version ());

	pad1 = (int) (42 - strlen (buffer));
	pad2 = pad1 - pad1 / 2;
	pad1 = pad1 / 2;

	fprintf (file, "\n\t|%*c%s%*c|", pad1, blank, buffer, pad2, blank);

	str_fmt (buffer, sizeof (buffer), "Copyright (c) %s", CopyRight ());

	pad1 = (int) (42 - strlen (buffer));
	pad2 = pad1 - pad1 / 2;
	pad1 = pad1 / 2;

	fprintf (file, "\n\t|%*c%s%*c|", pad1, blank, buffer, pad2, blank);

	ltime = StartTime ();
	
	str_cpy (buffer, sizeof (buffer), c_time (&ltime));

	pad1 = (int) (42 - strlen (buffer));
	pad2 = pad1 - pad1 / 2;
	pad1 = pad1 / 2;

	fprintf (file, "\n\t|%*c%s%*c|", pad1, blank, buffer, pad2, blank);

	fprintf (file, "\n\t|%42c|", blank);
	fprintf (file, "\n\t%s\n", bar);
	fflush (file);
}

//---------------------------------------------------------
//	Exit_Stat
//---------------------------------------------------------

void Message_Service::Exit_Stat (Exit_Code stat) 
{
	if (stat != Program_Status ()) {
		Program_Status (stat);
	}
	if (Pause ()) Pause_Process ();

	Show_Message (1);

	int code = Return_Code ();

	if (stat != DONEX) {
		delete this;
	}
	exit (code);
}

//---------------------------------------------------------
//	Return_Code
//---------------------------------------------------------
	
int Message_Service::Return_Code (void)
{ 
	if (Program_Status () == FATAL) {
		return (1);
	} else if (Program_Status () == WARNING) {
		return (2);
	} else {
		return (0);
	}
}

//---------------------------------------------------------
//	Show_Message
//---------------------------------------------------------

void Message_Service::Show_Message (char *format, ...) 
{
	if (format == NULL) {
		Show_Error ("Show_Message is NULL");
	}
	if (!Quiet ()) {
		char text [STRING_BUFFER];

		va_list  args;
 		va_start (args, format);
		str_args (text, sizeof (text), format, args);
		va_end (args);

		fputs ("\n\t", stdout);
		fputs (text, stdout);
		fflush (stdout);
	}
}

void Message_Service::Show_Message (int num_lines, char *format, ...) 
{
	if (format == NULL) {
		Show_Error ("Show_Message is NULL");
	}
	if (!Quiet ()) {
		char text [STRING_BUFFER];

		va_list  args;
 		va_start (args, format);
		str_args (text, sizeof (text), format, args);
		va_end (args);

		if (num_lines > 0) {
			while (num_lines-- > 0) {
				fputs ("\n", stdout);
			}
			fputs ("\t", stdout);
		}
		fputs (text, stdout);
		fflush (stdout);
	}
}

void Message_Service::Show_Message (int num_lines) 
{
	if (!Quiet ()) {
		while (num_lines-- > 0) {
			fputs ("\n", stdout);
		}
		fflush (stdout);
	}
}

//---------------------------------------------------------
//	Show_Question
//---------------------------------------------------------

bool Message_Service::Show_Question (char *format, ...) 
{
	if (Batch ()) {
		Show_Error ("Show_Question in 'Batch' mode (-B)");
		return (false);
	}
	if (format == NULL) {
		Show_Error ("Show_Question is NULL");
	}
	char text [STRING_BUFFER];

	va_list  args;
 	va_start (args, format);
	str_args (text, sizeof (text), format, args);
	va_end (args);

	printf ("\n\t%s (Y/N): ", text);
	fflush (stdout);

	if (fgets (text, sizeof (text), stdin) != NULL) {
		return (*text == 'Y' || *text == 'y');
	} else {
		return (false);
	}
}

//---------------------------------------------------------
//	Show_Error
//---------------------------------------------------------

void Message_Service::Show_Error (char *format, ...) 
{
	if (format == NULL) {
		format = "Show_Error is NULL";
	}
	if (Quiet () && warnings == 0) {
		Quiet (false);
		Show_Banner ();
	}
	char text [STRING_BUFFER];

	va_list  args;
 	va_start (args, format);
	str_args (text, sizeof (text), format, args);
	va_end (args);

	if (Program_Status () == FATAL) {
		printf ("\n\t%s", text);
		fflush (stdout);
	} else {
		printf ("\n\n\tError: %s", text);
		fflush (stdout);

		Pause (true);

		Exit_Stat (FATAL);
	}
}

//---------------------------------------------------------
//	Show_Warning
//---------------------------------------------------------

void Message_Service::Show_Warning (char *format, ...) 
{
	if (format == NULL) {
		format = "Show_Warning is NULL";
	}
	if (Quiet () && warnings == 0) {
		Quiet (false);
		Show_Banner ();
		Quiet (true);
	}
	warnings++;
	if (warnings <= max_warnings) {
		char text [STRING_BUFFER];

		va_list  args;
 		va_start (args, format);
		str_args (text, sizeof (text), format, args);
		va_end (args);

		printf ("\n\tWarning: %s", text);
		fflush (stdout);
	} else if (warning_flag) {
		Show_Error ("Maximum Warning Messages %d Exceeded", max_warnings);
	}
}

//---------------------------------------------------------
//	Set_Progress
//---------------------------------------------------------

void Message_Service::Set_Progress (int step_size) 
{
	if (step_size < 1) step_size = 100;

	progress = 0;
	progress_step = step_size;
}

//---------------------------------------------------------
//	Show_Progress
//---------------------------------------------------------

void Message_Service::Show_Progress (int value) 
{
	if (value < 1) {
		value = ++progress;
	} else {
		progress = value;
	}
	if (!Quiet ()) {
		if ((value % progress_step) == 0) {
			if (Batch ()) {
				printf ("\n\tRecord: %d", value); 
				fflush (stdout);
			} else {
				printf (" %-10d\b\b\b\b\b\b\b\b\b\b\b", value);
			}
		}
	}
}

void Message_Service::Show_Progress (char *text) 
{
	progress++;

	if (!Quiet ()) {
		if ((progress % progress_step) == 1) {
			if (Batch ()) {
				printf ("\n\tRecord: %s", text); 
				fflush (stdout);
			} else {
				printf (" %-16.16s\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b", text);
			}
		}
	}
}

//---------------------------------------------------------
//	End_Progress
//---------------------------------------------------------

void Message_Service::End_Progress (void) 
{
    if (!Quiet ()) {
		if (Batch ()) {
			printf ("\n\tRecord: %d\n", progress); 
			fflush (stdout);
		} else {
			printf (" %d\n", progress);
		}
	}
}

void Message_Service::End_Progress (char *text) 
{
    if (!Quiet ()) {
		if (Batch ()) {
			printf ("\n\tRecord: %s\n", text); 
			fflush (stdout);
		} else {
			printf (" %-16.16s\n", text);
		}
	}
}

//---------------------------------------------------------
//	Pause_Process
//---------------------------------------------------------

void Message_Service::Pause_Process (void) 
{
    if (!Quiet () && !No_Pause () && !Batch ()) {
		char pause [10];

		printf ("\n\n\tPress Enter to Continue");
		fflush (stdout);

		fgets (pause, sizeof (pause), stdin);
	}
}

//---------------------------------------------------------
//	Get_Control_File
//---------------------------------------------------------

char * Message_Service::Get_Control_File () 
{
	int len;
	char text [STRING_BUFFER], *path;

	printf ("\n\tEnter Control File Name: ");

	if (fgets (text, sizeof (text), stdin) == NULL) {
		return (NULL);
	}
	len = (int) strlen (text);
	if (len == 0) return (NULL);

	if (text [len-1] == '\n') text [--len] = '\0';
	if (len == 0) return (NULL);

	path = new char [++len];
	if (path == NULL) return (NULL);

	return (str_cpy (path, len, text));
}
