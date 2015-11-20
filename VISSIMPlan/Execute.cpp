//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "VISSIMPlan.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void VISSIMPlan::Execute (void)
{
	int nfile, hour, minute, second, period;
	char start [TIME_BUFFER], end [TIME_BUFFER];
	
	Range_Data *range_ptr;

	//---- read the network ----

	Demand_Service::Execute ();

	//---- initialize the xml file ----

	file = xml_file.File ();

	range_ptr = time_range.First ();

	second = range_ptr->Low ();
	hour = second / 3600;
	second = second - hour * 3600;
	minute = second / 60;
	second = second - minute * 60;

	str_fmt (start, sizeof (start), "%02d:%02d:%02d", hour, minute, second);

	range_ptr = time_range.Last ();

	second = range_ptr->High () + 1;
	hour = second / 3600;
	second = second - hour * 3600;
	minute = second / 60;
	second = second - minute * 60;

	str_fmt (end, sizeof (end), "%02d:%02d:%02d", hour, minute, second);

	fprintf (file, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>");
	fprintf (file, "\n<ABSTRACTNETWORKMODEL VERSNO=\"1.0\" FROMTIME=\"%s\" TOTIME=\"%s\" NAME=\"%s\">", start, end, Title ());
	fprintf (file, "\n<ROUTING>");
	fprintf (file, "\n<VEHTYPETIS>");

	period = 1;

	for (range_ptr = time_range.First (); range_ptr; range_ptr = time_range.Next ()) {
		second = range_ptr->Low ();
		hour = second / 3600;
		second = second - hour * 3600;
		minute = second / 60;
		second = second - minute * 60;

		str_fmt (start, sizeof (start), "%02d:%02d:%02d", hour, minute, second);

		second = range_ptr->High () + 1;
		hour = second / 3600;
		second = second - hour * 3600;
		minute = second / 60;
		second = second - minute * 60;

		str_fmt (end, sizeof (end), "%02d:%02d:%02d", hour, minute, second);

		fprintf (file, "\n<VEHTYPETI INDEX=\"%d\" FROMTIME=\"%s\" TOTIME=\"%s\" VEHTYPEID=\"10\"/>", period++, start, end);
	}
	fprintf (file, "\n</VEHTYPETIS>");
	fprintf (file, "\n<ROUTES>");

	//---- read the plan file ----

	for (nfile=0; ; nfile++) {
		if (!plan_file.Open (nfile)) break;

		//---- process the plan files ----

		Read_Plan ();
	}

	if (nfile == 0) {
		File_Error ("No Plan Files were Found", plan_file.Filename ());
	}

	//---- write summary statistics ----

	Break_Check (8);

	Write (2, "Number of Plan Files = %d", nfile);
	Write (1, "Number of Input Plans = %d", plan_file.Num_Plans ());
	Write (1, "Number of Input Records = %d", plan_file.Num_Records ());
	Write (1, "Number of Input Travelers = %d", plan_file.Num_Travelers ());
	Write (1, "Number of Input Trips = %d", plan_file.Num_Trips ());

	Write (2, "Number of Output Trips = %d", num_out);

	//---- end the program ----

	fprintf (file, "\n</ROUTES>");
	fprintf (file, "\n</ROUTING>");
	fprintf (file, "\n</ABSTRACTNETWORKMODEL>");

	xml_file.Close ();

	Exit_Stat (DONE);
}
