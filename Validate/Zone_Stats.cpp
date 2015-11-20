//*********************************************************
//	Zone_Stats.cpp - Zone Group Statistics
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Zone_Group_Stats
//---------------------------------------------------------

void Validate::Zone_Group_Stats (void)
{
	int i, n;
	char *str_ptr, buffer [20];

	Group_Data *data;
	Statistics stats;
	Link_Data *link_ptr;

	for (link_ptr = link_data.First (); link_ptr != NULL; link_ptr = link_data.Next ()) {
		if (link_ptr->AB_Dir () > 0) {
			n = node_zone [node_data.Get_Index (link_ptr->Anode ())];

			Sum_Group (zone_equiv.Zone_Group (n), link_ptr, link_ptr->AB_Dir ());
		}
		if (link_ptr->BA_Dir () > 0) {
			n = node_zone [node_data.Get_Index (link_ptr->Bnode ())];

			Sum_Group (zone_equiv.Zone_Group (n), link_ptr, link_ptr->BA_Dir ());
		}
	}

	//---- print the page header ----
	
	Header_Number (ZONE_GROUP);

	if (!Break_Check (group_data.Num_Records () + 9)) {
		Print (1);
		Zone_Group_Header ();
	}

	//---- print each zone group ----

	for (data = (Group_Data *) group_data.First_Key (); data != NULL; data = (Group_Data *) group_data.Next_Key ()) {
	
		if (!(n = data->number)) continue;

		Calc_Stats (data, &stats);

		i = data->index;
		
		str_ptr = zone_equiv.Group_Label (i);

		if (str_ptr == NULL) {
			str_fmt (buffer, sizeof (buffer), "Zone Group %d", i);
            str_ptr = buffer;
		}

		Page_Check (1);
		Print (0, "\n%25.25s ", str_ptr);
		Print (0, "%7d%10.0lf%10.0lf%9.0lf %7.1lf %7.0lf %6.1lf %7.0lf %6.1lf %6.3lf%6.2lf%6.2lf",
			n, data->volume, data->count, (data->volume - data->count), 
			stats.per_diff, stats.avg_error, stats.per_error, stats.std_dev, 
			stats.rmse, stats.r_sq, stats.avg_vc, data->max_vc);
	}
		
	//---- calculate the total statistics ----
	
	if ((n = total.number)) {
		Calc_Stats (&total, &stats);
		
		Page_Check (3);
		Print (0, "\n\n%25.25s ", "TOTAL");
		Print (0, "%7d%10.0lf%10.0lf%9.0lf %7.1lf %7.0lf %6.1lf %7.0lf %6.1lf %6.3lf%6.2lf%6.2lf\n",
			n, total.volume, total.count, (total.volume - total.count), 
			stats.per_diff, stats.avg_error, stats.per_error, stats.std_dev, 
			stats.rmse, stats.r_sq, stats.avg_vc, total.max_vc);
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Zone_Group_Header
//---------------------------------------------------------

void Validate::Zone_Group_Header (void)
{
	Page_Check (5);
	Print (0, "\n%44cSummary Statistics by Zone Group\n", BLANK);
	Print (0, "\n%29cNum.  ------%s------ ---Difference---  ", BLANK, ((method) ? "--VMT-" : "Volume"));
	Print (0, "--Abs.Error--    Std.     %%     R   ----V/C----");
	Print (0, "\n               Zone Group    Obs.  Estimate  Observed   %6.6s", ((method) ? "VMT" : "Volume"));
	Print (0, "      %%     Avg.     %%     Dev.   RMSE   Sq.   Avg.  Max.\n");
}

/**********************************************************|**********************************************************

                                            Summary Statistics by Zone Group
									
                             Num.  ------Volume------ ---Difference---  --Abs.Error--    Std.     %     R   ----V/C----
               Zone Group    Obs.  Estimate  Observed   Volume     %      Avg.    %      Dev.   RMSE   Sq.   Avg.  Max.

sssssssssssssssssssssssss ddddddd lllllllll lllllllll llllllll fffff.f lllllll ffff.f lllllll ffff.f  f.fff ff.ff ff.ff

                    TOTAL ddddddd lllllllll lllllllll llllllll fffff.f lllllll ffff.f lllllll ffff.f  f.fff ff.ff ff.ff
    

***********************************************************|**********************************************************/ 
