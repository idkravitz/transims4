//*********************************************************
//	Link_Details.cpp - Link Group Details
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Link_Group_Details
//---------------------------------------------------------

void Validate::Link_Group_Details (void)
{
	int i, j, n, lnk;
	char *str_ptr, buffer [32], bearing [8];

	Group_Data *data;
	Statistics stats;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Integer_List *list;

	for (i = 1; i <= link_equiv.Max_Group (); i++) {
		list = link_equiv.Group_List (i);

		if (list == NULL) continue;

		for (j=1, lnk = list->First (); lnk != 0; lnk = list->Next ()) {

			link_ptr = link_data.Get (abs (lnk));

			if (link_ptr == NULL) continue;

			if (lnk > 0 && link_ptr->AB_Dir () > 0) {
				Sum_Group (j++, link_ptr, link_ptr->AB_Dir ());
			} else if (lnk < 0 && link_ptr->BA_Dir () > 0) {
				Sum_Group (j++, link_ptr, link_ptr->BA_Dir ());
			}
		}

		//---- get the link group label ----

		group_label = link_equiv.Group_Label (i);

		if (group_label == NULL) {
			str_fmt (buffer, sizeof (buffer), "Link Group %d", i);
			group_label = buffer;
		}

		//---- print the page header ----
		
		Header_Number (GROUP_DETAILS);

		if (!Break_Check (group_data.Num_Records () + 9)) {
			Print (1);
			Group_Detail_Header ();
		}

		//---- print each link ----

		for (j=1, lnk = list->First (); lnk != 0; lnk = list->Next ()) {

			link_ptr = link_data.Get (abs (lnk));
			if (link_ptr == NULL) continue;

			if ((lnk > 0 && link_ptr->AB_Dir () <= 0) || 
				(lnk < 0 && link_ptr->BA_Dir () <= 0)) {
				continue;
			}
			data = (Group_Data *) group_data.Get (&j);
			j++;

			if (data == NULL) continue;
			if (!(n = data->number)) continue;

			Calc_Stats (data, &stats);

			//---- get the link name ----

			if (street_name != NULL) {
				str_ptr = street_name [link_data.Record_Index () - 1];
			} else {
				str_ptr = NULL;
			}

			if (str_ptr == NULL) {
				str_ptr = str_fmt (buffer, sizeof (buffer), "Link %d %s", abs (lnk), ((lnk > 0) ? "AB" : "BA"));
			}

			//---- get the link direction ----
			
			if (lnk > 0) {
				dir_ptr = dir_data [link_ptr->AB_Dir ()];
			} else {
				dir_ptr = dir_data [link_ptr->BA_Dir ()];
			}
			str_cpy (bearing, sizeof (bearing), compass.Point_Text (dir_ptr->Out_Bearing ()));

			if (bearing [1] == '\0') {
				bearing [1] = 'B';
				bearing [2] = '\0';
			}

			//---- print the record ----

			Page_Check (1);
			Print (0, "\n%-25.25s %-2.2s ", str_ptr, bearing);
			Print (0, "%4d%10.0lf%10.0lf%9.0lf %7.1lf %7.0lf %6.1lf %7.0lf %6.1lf %6.3lf%6.2lf%6.2lf",
				n, data->volume, data->count, (data->volume - data->count), 
				stats.per_diff, stats.avg_error, stats.per_error, stats.std_dev, 
				stats.rmse, stats.r_sq, stats.avg_vc, data->max_vc);
		}
	
		//---- calculate the total statistics ----
		
		if ((n = total.number)) {
			Calc_Stats (&total, &stats);

			Page_Check (3);
			Print (0, "\n\n%-25.25s ", "TOTAL");
			Print (0, "%7d%10.0lf%10.0lf%9.0lf %7.1lf %7.0lf %6.1lf %7.0lf %6.1lf %6.3lf%6.2lf%6.2lf\n",
				n, total.volume, total.count, (total.volume - total.count), 
				stats.per_diff, stats.avg_error, stats.per_error, stats.std_dev, 
				stats.rmse, stats.r_sq, stats.avg_vc, total.max_vc);
		}
		Header_Number (0);

		group_data.Reset ();
		memset (&total, '\0', sizeof (total));
	}
}

//---------------------------------------------------------
//	Group_Detail_Header
//---------------------------------------------------------

void Validate::Group_Detail_Header (void)
{
	Page_Check (5);
	Print (0, "\n%35cDetailed Statistics for Link Group: %s\n", BLANK, group_label);
	Print (0, "\n%30cNum. ------%s------ ---Difference---  ", BLANK, ((method) ? "--VMT-" : "Volume"));
	Print (0, "--Abs.Error--    Std.     %%     R   ----V/C----");
	Print (0, "\nLink Name                Dir  Obs. Estimate  Observed   %6.6s", ((method) ? "VMT" : "Volume"));
	Print (0, "      %%     Avg.     %%     Dev.   RMSE   Sq.   Avg.  Max.\n");
}

/**********************************************************|***********************************************************

                                   Detailed Statistics for Link Group: sssssssssssssssssssssssss
									
                              Num. ------Volume------ ---Difference---  --Abs.Error--    Std.     %     R   ----V/C----
Link Name                Dir  Obs. Estimate  Observed   Volume     %      Avg.    %      Dev.   RMSE   Sq.   Avg.  Max.

sssssssssssssssssssssssss ss dddd lllllllll lllllllll llllllll fffff.f lllllll ffff.f lllllll ffff.f  f.fff ff.ff ff.ff

TOTAL                       ddddd lllllllll lllllllll llllllll fffff.f lllllll ffff.f lllllll ffff.f  f.fff ff.ff ff.ff
    
***********************************************************|***********************************************************/ 
