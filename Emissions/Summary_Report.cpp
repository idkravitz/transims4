//*********************************************************
//	Summary_Report.cpp - emissions summary report
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Summary_Report
//---------------------------------------------------------

void Emissions::Summary_Report (void)
{
	int i;
	double vmt, vht, spd, ems;
	bool first;

	Summary_Data *ptr, area, facility, total;

	//---- get the pollutant categories ----

	area.Pollutants (num_pollutants);
	facility.Pollutants (num_pollutants);
	total.Pollutants (num_pollutants);

	Header_Number (SUMMARY_REPORT);
	New_Page ();

	first = true;
	total.Zero ();

	for (ptr = summary_data.First_Key (); ptr; ptr = summary_data.Next_Key ()) {
		if (first) {
			first = false;
			area.Zero (ptr);
			facility.Zero (ptr);

			Print (1, "%5d    %5d    %5d ", 
				ptr->Area_Type (), ptr->Facility_Type (), ptr->Vehicle_Type ());
		} else {
			if (ptr->Facility_Type () != facility.Facility_Type ()) {

				//---- write the facility type total ----

				vmt = facility.Meters ();
				vht = facility.Seconds ();

				if (vht > 0.0) {
					spd = vmt / vht;
				} else {
					spd = 0.0;
				}
				switch (travel_units) {
					case 0:		//---- miles ----
						vmt /= MILETOMETER;
						vht /= 3600.0;
						spd *= MPSTOMPH;
						break;
					default:
					case 1:		//---- kilometers ----
						vmt /= 1000.0;
						vht /= 3600.0;
						break;
					case 2:		//---- meters -----
						break;
					case 3:		//---- feet ----
						vmt *= METERTOFEET;
						spd *= MPSTOMPH;
						break;
				}
				Print (2, "%18cTotal  %11.0lf %11.0lf %6.1lf", 
					BLANK, vmt, vht, spd);

				for (i=1; i <= num_pollutants; i++) {
					ems = facility.Emissions (i);
					if (emission_units == 0) ems /= 1000.0;

					Print (0, " %11.1lf", ems);
				}				
			}
			if (ptr->Area_Type () != area.Area_Type ()) {

				//---- write the area type total ----

				vmt = area.Meters ();
				vht = area.Seconds ();

				if (vht > 0.0) {
					spd = vmt / vht;
				} else {
					spd = 0.0;
				}
				switch (travel_units) {
					case 0:		//---- miles ----
						vmt /= MILETOMETER;
						vht /= 3600.0;
						spd *= MPSTOMPH;
						break;
					default:
					case 1:		//---- kilometers ----
						vmt /= 1000.0;
						vht /= 3600.0;
						break;
					case 2:		//---- meters -----
						break;
					case 3:		//---- feet ----
						vmt *= METERTOFEET;
						spd *= MPSTOMPH;
						break;
				}

				Print (2, "%9cTotal%9c  %11.0lf %11.0lf %6.1lf", 
					BLANK, BLANK, vmt, vht, spd);
				
				for (i=1; i <= num_pollutants; i++) {
					ems = area.Emissions (i);
					if (emission_units == 0) ems /= 1000.0;

					Print (0, " %11.1lf", ems);
				}
				area.Zero (ptr);
				facility.Zero (ptr);

				Print (2, "%5d    %5d    %5d ", 
					ptr->Area_Type (), ptr->Facility_Type (), ptr->Vehicle_Type ());

			} else if (ptr->Facility_Type () != facility.Facility_Type ()) {
				facility.Zero (ptr);

				Print (2, "%9c%5d    %5d ", 
					BLANK, ptr->Facility_Type (), ptr->Vehicle_Type ());
			} else {
				Print (1, "%18c%5d ", 
					BLANK, ptr->Vehicle_Type ());
			}
		}

		//---- write the emissions data ----
		
		vmt = ptr->Meters ();
		vht = ptr->Seconds ();

		if (vht > 0.0) {
			spd = vmt / vht;
		} else {
			spd = 0.0;
		}
		switch (travel_units) {
			case 0:		//---- miles ----
				vmt /= MILETOMETER;
				vht /= 3600.0;
				spd *= MPSTOMPH;
				break;
			default:
			case 1:		//---- kilometers ----
				vmt /= 1000.0;
				vht /= 3600.0;
				break;
			case 2:		//---- meters -----
				break;
			case 3:		//---- feet ----
				vmt *= METERTOFEET;
				spd *= MPSTOMPH;
				break;
		}
		Print (0, " %11.0lf %11.0lf %6.1lf", vmt, vht, spd);
		
		for (i=1; i <= num_pollutants; i++) {
			ems = ptr->Emissions (i);
			if (emission_units == 0) ems /= 1000.0;

			Print (0, " %11.1lf", ems);
		}
		facility.Sum (ptr);
		area.Sum (ptr);
		total.Sum (ptr);
	}

	//---- write the facility type total ----

	vmt = facility.Meters ();
	vht = facility.Seconds ();

	if (vht > 0.0) {
		spd = vmt / vht;
	} else {
		spd = 0.0;
	}
	switch (travel_units) {
		case 0:		//---- miles ----
			vmt /= MILETOMETER;
			vht /= 3600.0;
			spd *= MPSTOMPH;
			break;
		default:
		case 1:		//---- kilometers ----
			vmt /= 1000.0;
			vht /= 3600.0;
			break;
		case 2:		//---- meters -----
			break;
		case 3:		//---- feet ----
			vmt *= METERTOFEET;
			spd *= MPSTOMPH;
			break;
	}
	Print (2, "%18cTotal  %11.0lf %11.0lf %6.1lf", 
		BLANK, vmt, vht, spd);
	
	for (i=1; i <= num_pollutants; i++) {
		ems = facility.Emissions (i);
		if (emission_units == 0) ems /= 1000.0;

		Print (0, " %11.1lf", ems);
	}

	//---- write the area type total ----

	vmt = area.Meters ();
	vht = area.Seconds ();

	if (vht > 0.0) {
		spd = vmt / vht;
	} else {
		spd = 0.0;
	}
	switch (travel_units) {
		case 0:		//---- miles ----
			vmt /= MILETOMETER;
			vht /= 3600.0;
			spd *= MPSTOMPH;
			break;
		default:
		case 1:		//---- kilometers ----
			vmt /= 1000.0;
			vht /= 3600.0;
			break;
		case 2:		//---- meters -----
			break;
		case 3:		//---- feet ----
			vmt *= METERTOFEET;
			spd *= MPSTOMPH;
			break;
	}
	Print (2, "%9cTotal%9c  %11.0lf %11.0lf %6.1lf", 
		BLANK, BLANK, vmt, vht, spd);
	
	for (i=1; i <= num_pollutants; i++) {
		ems = area.Emissions (i);
		if (emission_units == 0) ems /= 1000.0;

		Print (0, " %11.1lf", ems);
	}

	//---- write the total total ----

	vmt = total.Meters ();
	vht = total.Seconds ();

	if (vht > 0.0) {
		spd = vmt / vht;
	} else {
		spd = 0.0;
	}
	switch (travel_units) {
		case 0:		//---- miles ----
			vmt /= MILETOMETER;
			vht /= 3600.0;
			spd *= MPSTOMPH;
			break;
		default:
		case 1:		//---- kilometers ----
			vmt /= 1000.0;
			vht /= 3600.0;
			break;
		case 2:		//---- meters -----
			break;
		case 3:		//---- feet ----
			vmt *= METERTOFEET;
			spd *= MPSTOMPH;
			break;
	}
	Print (2, "Total%18c  %11.0lf %11.0lf %6.1lf", 
		BLANK, vmt, vht, spd);
	
	for (i=1; i <= num_pollutants; i++) {
		ems = total.Emissions (i);
		if (emission_units == 0) ems /= 1000.0;

		Print (0, " %11.1lf", ems);
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Summary_Header
//---------------------------------------------------------

void Emissions::Summary_Header (void)
{
	char *str_ptr;
	Category_Data *cat_ptr;

	//---- get the pollutant categories ----

	cat_ptr = category_data.Get (POLLUTANT);

	Print (1, "%46cEmissions Summary Report", BLANK);
	Print (2, " Area  Facility  Vehicle     Vehicle     Vehicle  Speed");

	for (str_ptr = cat_ptr->First_Label (); str_ptr; str_ptr = cat_ptr->Next_Label ()) {
		Print (0, "  %10.10s", str_ptr);
	}
	Print (1, " Type     Type     Type");

	switch (travel_units) {
		case 0:		//---- miles ----
			Print (0, "     Miles       Hours  (mph)");
			break;
		default:
		case 1:		//---- kilometers ----
			Print (0, "Kilometers       Hours  (m/s)");
			break;
		case 2:		//---- meters -----
			Print (0, "    Meters     Seconds  (m/s)");
			break;
		case 3:		//---- feet ----
			Print (0, "      Feet     Seconds  (mph)");
			break;
	}
	if (emission_units == 0) {
		for (int i=0; i < num_pollutants; i++) {
			Print (0, "   Kilograms");
		}
	} else {
		for (int i=0; i < num_pollutants; i++) {
			Print (0, "       Grams");
		}
	}
	Print (1);
}

/**********************************************************|***********************************************************

	                                             Emissions Summary Report

	 Area  Facility  Vehicle     Vehicle     Vehicle  Speed  Pollutant1  Pollutant2  Pollutant3  Pollutant4  Pollutant5
	 Type     Type     Type   Kilometers       Hours  (m/s)   Kilograms   Kilograms   Kilograms   Kilograms   Kilograms

	ddddd    ddddd    ddddd   ffffffffff  ffffffffff  fff.f  ffffffff.f  ffffffff.f  ffffffff.f  ffffffff.f  ffffffff.f
	                          ffffffffff  ffffffffff  fff.f  ffffffff.f  ffffffff.f  ffffffff.f  

	                  Total   ffffffffff  ffffffffff  fff.f  ffffffff.f  ffffffff.f  ffffffff.f

	         Total            ffffffffff  ffffffffff  fff.f  ffffffff.f  ffffffff.f  ffffffff.f
	
	Total                     ffffffffff  ffffffffff  fff.f  ffffffff.f  ffffffff.f  ffffffff.f


	Number of Records in the Report = dddddd

**********************************************************|***********************************************************/ 
