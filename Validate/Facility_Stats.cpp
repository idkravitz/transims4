//*********************************************************
//	Facility_Stats.cpp - Facility Type Statistics
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Facility_Stats
//---------------------------------------------------------

void Validate::Facility_Stats (void)
{
	int i, n, max_label;
	char *str_ptr, buffer [20];

	Group_Data *data;
	Statistics stats;
	Link_Data *link_ptr;
	Label_Type *type_ptr;

	char *type_label [] = {
		"Freeway", "Expressway", "Principal Arterial", "Major Arterial", "Minor Arterial", 
		"Collector", "Local Street", "Frontage Road", "Ramp", "Bridge/Other", "Walkway",
		"Bikeway", "Busway", "LightRail", "HeavyRail", "Ferry", "External", NULL
	};
	for (max_label=0; type_label [max_label] != NULL; max_label++);

	for (link_ptr = link_data.First (); link_ptr != NULL; link_ptr = link_data.Next ()) {
		if (link_ptr->AB_Dir () > 0) {
			Sum_Group (link_ptr->Type (), link_ptr, link_ptr->AB_Dir ());
		}
		if (link_ptr->BA_Dir () > 0) {
			Sum_Group (link_ptr->Type (), link_ptr, link_ptr->BA_Dir ());
		}
	}

	//---- print the page header ----

	Header_Number (FACILITY_TYPE);

	if (!Break_Check (group_data.Num_Records () + 9)) {
		Print (1);
		Facility_Header ();
	}

	//---- print each functional class ----

	for (data = (Group_Data *) group_data.First_Key (); data != NULL; data = (Group_Data *) group_data.Next_Key ()) {
		if (!(n = data->number)) continue;
	
		Calc_Stats (data, &stats);
		
		i = data->index;

		type_ptr = (Label_Type *) facility_type.Get (&i);

		if (type_ptr != NULL) {
			str_ptr = type_ptr->label;
		} else if (i > 0 && i <= max_label) {
			str_ptr = type_label [i-1];
		} else {
			str_ptr = str_fmt (buffer, sizeof (buffer), "Facility Type %d", i);
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
//	Facility_Header
//---------------------------------------------------------

void Validate::Facility_Header (void)
{
	Page_Check (5);
	Print (0, "\n%41cSummary Statistics by Facility Type\n", BLANK);
	Print (0, "\n%29cNum.  ------%s------ ---Difference---  ", BLANK, ((method) ? "--VMT-" : "Volume"));
	Print (0, "--Abs.Error--    Std.     %%     R   ----V/C----");
	Print (0, "\n            Facility Type    Obs.  Estimate  Observed   %6.6s", ((method) ? "VMT" : "Volume"));
	Print (0, "      %%     Avg.     %%     Dev.   RMSE   Sq.   Avg.  Max.\n");
}
	 
/**********************************************************|**********************************************************

                                         Summary Statistics by Facility Type   
									
                             Num.  ------Volume------ ---Difference---  --Abs.Error--    Std.     %     R   ----V/C----
            Facility Type    Obs.  Estimate  Observed   Volume     %      Avg.    %      Dev.   RMSE   Sq.   Avg.  Max.

sssssssssssssssssssssssss ddddddd lllllllll lllllllll llllllll fffff.f lllllll ffff.f lllllll ffff.f  f.fff ff.ff ff.ff

                    TOTAL ddddddd lllllllll lllllllll llllllll fffff.f lllllll ffff.f lllllll ffff.f  f.fff ff.ff ff.ff
    
***********************************************************|**********************************************************/ 
