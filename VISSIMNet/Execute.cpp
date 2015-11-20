//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "VISSIMNet.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void VISSIMNet::Execute (void)
{
	int hour, minute, second;
	char start [TIME_BUFFER], end [TIME_BUFFER];

	//---- read the network ----

	Demand_Service::Execute ();

	//---- make a new network ----

	if (new_flag) {
		New_Network ();
	}

	//---- add node links ----

	Data_Setup ();

	//---- initialize the xml file ----

	file = xml_file.File ();

	if (time_range.Num_Ranges () > 0) {
		Range_Data *range_ptr = time_range.First ();

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
	} else {
		str_cpy (start, sizeof (start), "00:00:00");
		str_cpy (end, sizeof (end), "23:59:59");
	}

	fprintf (file, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>");
	fprintf (file, "\n<ABSTRACTNETWORKMODEL VERSNO=\"1.0\" FROMTIME=\"%s\" TOTIME=\"%s\" NAME=\"%s\">", start, end, Title ());
	fprintf (file, "\n<NETWORK LEFTHANDTRAFFIC=\"0\">");

	//---- write vehicle type records ----

	Write_Vehicle_Types ();

	//---- write node records ----

	Write_Node ();

	//---- write zone records ----

	Write_Zone ();

	//---- write link records ----

	Write_Link ();

	//---- write signal records ----

	Write_Signal ();

	//---- end the program ----

	fprintf (file, "\n</NETWORK>");
	fprintf (file, "\n</ABSTRACTNETWORKMODEL>");

	xml_file.Close ();

	Exit_Stat (DONE);
}
