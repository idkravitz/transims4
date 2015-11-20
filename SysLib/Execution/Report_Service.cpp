//*********************************************************
//	Report_Service.cpp - general report class
//*********************************************************

#include "Report_Service.hpp"

#include "Utility.hpp"
#include "Date_Time.hpp"

#include <stdarg.h>

char Report_Service::extension [] = "";

//---------------------------------------------------------
//	Report_Service - constructor
//---------------------------------------------------------

Report_Service::Report_Service (void) : Message_Service ()
{
	page_width = 95;
	page_lines = 65;
	lines = 0;
	page = 1;
	title = NULL;
	control_file = NULL;
	report = NULL;
	header = 0;
	extension [0] = '\0';
	parameter = partition = enable = false;
	partition_number = 0;
}

//---------------------------------------------------------
//	Report_Service - destructor
//---------------------------------------------------------

Report_Service::~Report_Service (void)
{
	if (title != NULL) {
		delete [] title;
		title = NULL;
	}
}

//---------------------------------------------------------
//	Open_Report
//---------------------------------------------------------

void Report_Service::Open_Report (char *report_name, bool report_flag) 
{
	int len;
	char *result, buf [40], buffer [STRING_BUFFER], *str_ptr;

	//---- construct defaule report file name ----

	if (report_name == NULL) {
		report_name = str_cpy (buffer, sizeof (buffer), (char *) Control_File ());

		len = (int) strlen (buffer);
		len = (len > 4) ? len - 4 : 0;

		if ((result = strchr (buffer + len, '.')) != NULL) {
			*result = '\0';
		}
		if (partition) {
			str_cat (buffer, sizeof (buffer), str_fmt (buf, sizeof (buf), "_%d", partition_number));
		} else if (parameter && extension [0] != '\0') {
			str_cpy (buf, sizeof (buf), extension);

			str_ptr = strchr (buf, '.');

			if (str_ptr != NULL) {
				*str_ptr++ = '\0';
				while (*str_ptr == '.') str_ptr++;

				if (*str_ptr != '\0') {
					str_cat (buf, sizeof (buf), "-");
					str_cat (buf, sizeof (buf), str_ptr);
				}
			}
			str_cat (buffer, sizeof (buffer), "_");
			str_cat (buffer, sizeof (buffer), buf);
		} 
		str_cat (buffer, sizeof (buffer), ".prn");

	} else if (partition) {
		report_name = str_cpy (buffer, sizeof (buffer), report_name);

		len = (int) strlen (buffer);
		len = (len > 4) ? len - 4 : 0;

		if ((result = strchr (buffer + len, '.')) != NULL) {
			str_fmt (buf, sizeof (buf), "_%d%s", partition_number, result);
			*result = '\0';
		}
		str_cat (buffer, sizeof (buffer), buf);
	} else if (parameter && extension [0] != '\0') {
		report_name = str_cpy (buffer, sizeof (buffer), report_name);

		len = (int) strlen (buffer);
		len = (len > 4) ? len - 4 : 0;

		if ((result = strchr (buffer + len, '.')) != NULL) {
			str_cpy (buf, sizeof (buf), extension);

			str_ptr = strchr (buf, '.');

			if (str_ptr != NULL) {
				*str_ptr++ = '\0';
				while (*str_ptr == '.') str_ptr++;

				if (*str_ptr != '\0') {
					str_cat (buf, sizeof (buf), "-");
					str_cat (buf, sizeof (buf), str_ptr);
				}
			}
			str_cat (buf, sizeof (buf), result);
			*result = '\0';
		}
		str_cat (buffer, sizeof (buffer), "_");
		str_cat (buffer, sizeof (buffer), buf);
	}

	//---- open report file ----

	if (report_flag) {
		report = f_open (report_name, "at");
	} else {
		report = f_open (report_name, "wt");
	}
	if (report == NULL) {
		File_Error ("Opening Report File", report_name);
	}

	//---- print the banner ----

	lines = 12;

	Banner (report);

	fprintf (report, "\n\tControl File = %s", Control_File ());
	fprintf (report, "\n\tReport_File  = %s (%s)", report_name, ((report_flag) ? "Append" : "Create")); 

	fprintf (report, "\n\n\t%s", Title ());
	fflush (report);
}

//---------------------------------------------------------
//	Exit_Stat
//---------------------------------------------------------

void Report_Service::Exit_Stat (Exit_Code stat) 
{
	if (report != NULL) {
		if (stat == CANCEL) {
			Write_Time (2, "Process Terminated by the User");
		} else if (stat == FATAL) {
			Write_Time (2, "Process Terminated Due to Errors");
		} else if (stat == WARNING) {
			Write_Time (2, "Process Terminated with %d Warnin%s", Warnings (),
				((Warnings () > 1) ? "gs" : "g"));
		} else if (Warnings () > 0) {
			stat = WARNING;
			Write_Time (2, "Process Complete with %d Warnin%s", Warnings (),
				((Warnings () > 1) ? "gs" : "g"));
		} else {
			Write_Time (2, "Process Complete");	
		}
		fputs ("\n", report);
		fclose (report);
		report = NULL;
	}
	Message_Service::Exit_Stat (stat);
}

//---------------------------------------------------------
//	Write
//---------------------------------------------------------

void Report_Service::Write (int num_lines, char *format, ...) 
{ 
	if (format == NULL || num_lines > 10) return;

	char text [STRING_BUFFER];

	va_list  args;
 	va_start (args, format);
	str_args (text, sizeof (text), format, args);
	va_end (args);

	//---- send the message to the screen ----

	Show_Message (num_lines, "%s", text);

	//---- send the message to the print file ----

	if (report != NULL) {
		Page_Check (num_lines);

		//---- add the new line commands ----

		if (num_lines > 0) {
			while (num_lines-- > 0) {
				fputs ("\n", report);
			}
			fputs ("\t", report);
		}
		text [page_width] = '\0';

		fputs (text, report);
	}
}

void Report_Service::Write (int num_lines) 
{
	Print (num_lines);
	Show_Message (num_lines);
}

//---------------------------------------------------------
//	Print
//---------------------------------------------------------

void Report_Service::Print (int num_lines, char *format, ...) 
{ 
	if (format == NULL || report == NULL) return;

	char text [STRING_BUFFER];

	Page_Check (num_lines);

	//---- add the new line commands ----

	if (num_lines > 0) {
		while (num_lines-- > 0) {
			fputs ("\n", report);
		}
		fputs ("\t", report);
	}

	//---- expand the data fields ----

	va_list  args;
 	va_start (args, format);
	str_args (text, sizeof (text), format, args);
	va_end (args);

	//---- sent the message to the print file ----

	text [page_width] = '\0';

	fputs (text, report);
}

void Report_Service::Print (int num_lines) 
{
	if (report != NULL) {
		Page_Check (num_lines);

		while (num_lines-- > 0) {
			fputs ("\n", report);
		}
	}
}

//---------------------------------------------------------
//	Error
//---------------------------------------------------------

void Report_Service::Error (char *format, ...) 
{ 
	if (format == NULL) Exit_Stat (FATAL);

	char text [STRING_BUFFER];
	
	va_list  args;
 	va_start (args, format);
	str_args (text, sizeof (text), format, args);
	va_end (args);

	//---- send the message to the report file ----

	if (report != NULL) {
		Page_Check (3);
		time_t ltime;
		time (&ltime);

		fputs ("\n\n\t", report);
		fputs (c_time (&ltime), report);
		fputs ("\n\tError: ", report);
		fputs (text, report);
		Flush ();
	}

	//---- send the message to the screen ----

	Show_Error ("%s", text);
}

//---------------------------------------------------------
//	Warning
//---------------------------------------------------------

void Report_Service::Warning (char *format, ...) 
{ 
	if (format == NULL) Exit_Stat (FATAL);

	//---- check the warning message count ----

	if (Warnings () < Max_Warnings ()) {
		char text [STRING_BUFFER];
		
		va_list  args;
 		va_start (args, format);
		str_args (text, sizeof (text), format, args);
		va_end (args);

		//---- send the message to the report file ----

		if (report != NULL) {
			Page_Check (1);
			fputs ("\n\tWarning: ", report);
			fputs (text, report);
			Flush ();
		}

		//---- send the message to the screen ----

		Show_Warning ("%s", text);
	} else if (Warning_Flag ()) {
		Error ("Maximum Warning Messages %d Exceeded", Max_Warnings ());
	} else {
		Warnings (Warnings () + 1);
	}
}

//---------------------------------------------------------
//	Print_Filename
//---------------------------------------------------------

void Report_Service::Print_Filename (int num_lines, char *label, char *filename) 
{
	if (label == NULL || filename == NULL || report == NULL) return;

	int  len, max_len;
	char text [STRING_BUFFER];

	len = (int) strlen (filename);
	max_len = (int) (page_width - strlen (label) - 14);

	if (len > max_len && max_len > 0) {
		str_cpy (text, sizeof (text), (filename + len - max_len));

		text [0] = text [1] = text [2] = '.';
	} else {
		str_cpy (text, sizeof (text), filename);
	}
	Page_Check (num_lines);

	while (num_lines-- > 0) {
        fprintf (report, "\n");
	}
	fprintf (report, "\t%s = %s", label, text);
}

//---------------------------------------------------------
//	File_Error
//---------------------------------------------------------

void Report_Service::File_Error (char *label, char *format, ...) 
{ 
	if (label == NULL || format == NULL) {
		Exit_Stat (FATAL);
	}
	char text [STRING_BUFFER];
	
	va_list  args;
 	va_start (args, format);
	str_args (text, sizeof (text), format, args);
	va_end (args);

	Print (2, "Error: %s", label);
	Print (1, "\t\"%s\"", text);

	Show_Error ("%s\n\t\t\"%s\"", label, text);
}

//---------------------------------------------------------
//	Write_Time
//---------------------------------------------------------

void Report_Service::Write_Time (int num_lines, char *format, ...)
{
	if (format == NULL) Exit_Stat (FATAL);

	char text [STRING_BUFFER];
	
	va_list  args;
 	va_start (args, format);
	str_args (text, sizeof (text), format, args);
	va_end (args);

	time_t ltime;
	time (&ltime);

	int duration = (int) (ltime - StartTime ());
	int hour = duration / 3600;
	int second = duration % 3600;
	int minute = second / 60;
	second = second % 60;
	
	Write (num_lines, "%s -- %s (%d:%02d:%02d)", c_time (&ltime), 
		text, hour, minute, second);
	Flush ();
}

//---------------------------------------------------------
//	Break_Check
//---------------------------------------------------------

bool Report_Service::Break_Check (int num_lines)
{
	if (lines + num_lines > page_lines) {
		New_Page ();
		return true;
	} else {
		return false;
	}
}

//---------------------------------------------------------
//	New_Page
//---------------------------------------------------------

void Report_Service::New_Page () 
{
	page++;
	lines = 3;

	//---- print the system header ----

	if (report != NULL) {
		time_t ltime;
		ltime = time (&ltime);

		fprintf (report, "\f\t%s\n\t%s  %s  page %d\n", 
			Title (), c_time (&ltime), Program (), page);
	}

	//---- print the user header ----

	if (header != 0) Page_Header ();
}

//---------------------------------------------------------
//	Page_Size
//---------------------------------------------------------

void Report_Service::Page_Size (int width, int lines) 
{ 
	page_width = width; 
	page_lines = lines; 
}

//---------------------------------------------------------
//	Flush
//---------------------------------------------------------

void Report_Service::Flush () 
{ 
	if (report != NULL) fflush (report); 
}

//---------------------------------------------------------
//	Page_Check
//---------------------------------------------------------

bool Report_Service::Page_Check (int new_lines) 
{ 
	lines += new_lines;

	if (lines > page_lines) {
		New_Page ();
		lines += new_lines;
		return true;
	}
	return false;
}

//---------------------------------------------------------
//	Title
//---------------------------------------------------------

void Report_Service::Title (char *name) 
{ 
	if (name != NULL) {
		if (title != NULL) {
			delete [] title;
			title = NULL;
		}
		int len = (int) strlen (name) + 1;
		char *t = new char [len];
		str_cpy (t, len, name);
		title = (const char *) t;
	}
}

//---------------------------------------------------------
//      Fix_Directory
//---------------------------------------------------------

char * Report_Service::Fix_Directory (char dir [])
{
	int len = (int) strlen (dir) - 1;

	if (len > 0) {
#ifdef _WIN32
		if (dir [len] != '\\' && dir [len] != '/') {
			str_cat (dir, len + 3, "\\");
		}
#else
		if (dir [len] != '/') {
			strcat (dir, "/");
		}
#endif
	}
	return (dir);
}
