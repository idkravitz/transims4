//*********************************************************
//	Link_Report.cpp - Create a Link Volume Report
//*********************************************************

#include "LinkSum.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Link_Report
//---------------------------------------------------------

void LinkSum::Link_Report (double min_volume)
{
	bool flag;
	int i, num;
	char ab [12], ba [12];

	Link_Data *link_ptr;
	Performance_Data *ttime_ab, *ttime_ba;

	Show_Message ("Creating a Link Event Report -- Record");
	Set_Progress (10000);

	//---- find volumes greater than min_volume ----

	header_value = (int) (min_volume + 0.5);

	Header_Number (LINK_REPORT);
	New_Page ();

	for (link_ptr = link_data.First_Key (); link_ptr; link_ptr = link_data.Next_Key ()) {

		if (select_flag && link_ptr->Use () == 0) continue;

		//---- get the directional information ----

		ttime_ab = performance_data [link_ptr->AB_Dir ()];
		ttime_ba = performance_data [link_ptr->BA_Dir ()];

		num = 0;

		if (ttime_ab != NULL) {
			num = ttime_ab->Periods ();
		} else if (ttime_ba != NULL) {
			num = ttime_ba->Periods ();
		}
		if (num == 0) continue;

		//---- scan each period volume ----

		for (i=1; i <= num; i++) {

			ab [0] = ba [0] = '\0';
			flag = false;

			if (ttime_ab != NULL && link_ptr->Use () != -2) {
				if (ttime_ab->Volume (i) > header_value) {
					str_fmt (ab, sizeof (ab), "%d", ttime_ab->Volume (i));
					flag = true;
				}
			}
			if (ttime_ba != NULL && link_ptr->Use () != -1) {
				if (ttime_ba->Volume (i) > header_value) {
					str_fmt (ba, sizeof (ba), "%d", ttime_ba->Volume (i));
					flag = true;
				}
			}
			if (!flag) continue;

			Show_Progress ();

			//---- print the data record ----

			Print (1, "%10d%10d%10d  %12.12s%10.10s %10.10s",
				link_ptr->Link (), link_ptr->Anode (), link_ptr->Bnode (), 
				time_periods.Range_Format (i), ab, ba);
		}
	}
	End_Progress ();

	if (Progress_Count ()) {
		Print (2, "Number of Records in the Report = %d", Progress_Count ());
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Link_Report_Header
//---------------------------------------------------------

void LinkSum::Link_Report_Header (void)
{
	Print (1, "Link Volumes Greater Than %.0lf", header_value);
	Print (2, "      Link    A-Node    B-Node   Time-of-Day  Volume_AB  Volume_BA");
	Print (1);
}

/*********************************************|***********************************************

	Link Volume Greater Than dddddd

	      Link    A-Node    B-Node   Time-of-Day  Volume_AB  Volume_BA

	dddddddddd  dddddddd  dddddddd  dd:dd..dd:dd  dddddddd   dddddddd

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
