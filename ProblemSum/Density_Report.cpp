//*********************************************************
//	Density_Report.cpp - Print the Problem Density Report
//*********************************************************

#include "ProblemSum.hpp"

#include "Index_Array.hpp"

//---------------------------------------------------------
//	Density_Report
//---------------------------------------------------------

void ProblemSum::Density_Report (double min_density)
{
	int i, total, problems;
	double factor, length, density;

	Link_Data *link_ptr;
	TTime_Data *ttime_ptr;
	Range_Data *range_ptr;

	typedef struct {
		int density;
		int link;
		int dir;
		int time;
		int problems;
		int length;
	} Density_Index;

	Index_Array index (sizeof (Density_Index));

	Density_Index *index_ptr, index_rec;

	//---- summarize the problem times ----

	for (ttime_ptr = ttime_data.First (); ttime_ptr; ttime_ptr = ttime_data.Next ()) {
		if (ttime_ptr->Periods () > 0) {
			index_rec.link = ttime_ptr->Link ();
			index_rec.dir = ttime_ptr->Dir ();

			link_ptr = link_data.Get (index_rec.link);

			index_rec.length = link_ptr->Length ();
			length = UnRound (index_rec.length) / 1000.0;

			for (i=1; i <= ttime_ptr->Periods (); i++) {
				range_ptr = times [i];
				factor = (range_ptr->High () - range_ptr->Low () + 1) / (3600.0 * length);

				problems = ttime_ptr->Count (i);
				density = problems * factor;

				if (density >= min_density) {
					index_rec.density = (int) (density * 1000.0 + 0.5);
					index_rec.time = i;
					index_rec.problems = problems;

					if (!index.Add (&index_rec)) {
						Error ("Adding Density Index Data");
					}
				}
			}
		}
	}
	index.Optimize (true);

	//---- print the report ----

	Header_Number (DENSITY_REPORT);

	header_code = (int) (min_density * 1000.0 + 0.5);

	total = 0;
	factor = 0.0;

	if (!Break_Check (index.Num_Records () + 6)) {
		Print (1);
		Density_Header ();
	}
	for (index_ptr = (Density_Index *) index.Last_Key (); index_ptr; index_ptr = (Density_Index *) index.Previous_Key ()) {
		total += index_ptr->problems;

		length = UnRound (index_ptr->length);

		density = index_ptr->density / 1000.0;
		factor += density;

		Print (1, "%-10d %d  %9.2lf  %-24.24s  %10d  %12.3lf", index_ptr->link, index_ptr->dir,
			length, times.Range_Format (index_ptr->time), index_ptr->problems, density);
	}
	i = index.Num_Records ();
	if (i > 0) {
		factor = factor / i;
	}
	Print (2, "Total/Average                                      %10d  %12.3lf", total, factor);

	Header_Number (0);
}

//---------------------------------------------------------
//	Density_Header
//---------------------------------------------------------

void ProblemSum::Density_Header (void)
{
	Print (1, "Problem Density Greater Than %.3lf (problems/kilometer/hour)", header_code / 1000.0);
	Print (2, "Link      Dir   Length   Time-of-Day                 Problems     Density");
	Print (1);
}
	 
/*********************************************|***********************************************

	Problem Density Greater Than fffffff.fff (problems/kilometer/hour)

	Link      Dir   Length   Time-of-Day                 Problems     Density

	dddddddddd d  ffffff.ff  ssssssssssssssssssssssss  dddddddddd  ffffffff.fff

	Total/Average                                      dddddddddd  ffffffff.fff

**********************************************|***********************************************/ 
