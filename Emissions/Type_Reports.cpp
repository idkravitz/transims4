//*********************************************************
//	Type_Reports.cpp - emissions by data types
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Type_Reports
//---------------------------------------------------------

void Emissions::Type_Reports (Emissions_Reports report, Report_Types type1, Report_Types type2)
{
	int i, len1, len2;
	double vmt, vht, spd, ems;
	char buffer [FIELD_BUFFER], label [FIELD_BUFFER];

	Summary_Data *ptr, *data_ptr, total (num_pollutants);
	Summary_Array data (num_pollutants);
	Label_Type *type_ptr;

	//---- gather the type data ----

	total.Zero ();

	for (ptr = summary_data.First (); ptr; ptr = summary_data.Next ()) {
		if (type1 == AREA_TYPE || type2 == AREA_TYPE) {
			total.Area_Type (ptr->Area_Type ());
		}
		if (type1 == FACILITY_TYPE || type2 == FACILITY_TYPE) {
			total.Facility_Type (ptr->Facility_Type ());
		}
		if (type1 == VEH_TYPE || type2 == VEH_TYPE) {
			total.Vehicle_Type (ptr->Vehicle_Type ());
		}
		if (type1 == ROAD_TYPE || type2 == ROAD_TYPE) {
			total.Road_Type (ptr->Road_Type ());
		}
		data_ptr = data.Get (&total);

		if (data_ptr == NULL) {
			data_ptr = new Summary_Data (num_pollutants);
			data_ptr->Zero (&total);

			if (!data.Add (data_ptr)) {
				Error ("Adding Type Record");
			}
			data_ptr = data.Record ();
		}
		data_ptr->Sum (ptr);
	}
	Header_Number (report);

	if (!Break_Check (data.Num_Records () + 9)) {
		Print (1);
		switch (report) {
			case AT_REPORT:				//---- Emissions by Area Type ----
				Type_Headers ("Area Type");
				break;
			case FT_REPORT:				//---- Emissions by Facility Type ----
				Type_Headers ("Facility Type");
				break;
			case VT_REPORT:				//---- Emissions by Vehicle Type ----
				Type_Headers ("Vehicle Type");
				break;
			case RT_REPORT:				//---- Emissions by Road Type ----
				Type_Headers ("Road Type");
				break;
			case AT_FT_REPORT:			//---- Emissions by Area and Facility ----
				Type_Headers ("Area and Facility Type");
				break;
			case AT_VT_REPORT:			//---- Emissions by Area and Vehicle ----
				Type_Headers ("Area and Vehicle Type");
				break;
			case FT_VT_REPORT:			//---- Emissions by Facility and Vehicle ----
				Type_Headers ("Facility and Vehicle Type");
				break;
			case RT_VT_REPORT:			//---- Emissions by Road and Vehicle ----
				Type_Headers ("Road and Vehicle Type");
				break;
			default:
				Type_Headers ("Unknown");
				break;
		}
	}

	for (data_ptr = data.First_Key (); data_ptr; data_ptr = data.Next_Key ()) {

		//---- construct the record label ----

		if (type1 == AREA_TYPE) {
			i = data_ptr->Area_Type ();
			type_ptr = (Label_Type *) area_type.Get (&i);
		} else if (type1 == FACILITY_TYPE) {
			i = data_ptr->Facility_Type ();
			type_ptr = (Label_Type *) facility_type.Get (&i);
		} else if (type1 == VEH_TYPE) {
			i = data_ptr->Vehicle_Type ();
			type_ptr = (Label_Type *) vehicle_type.Get (&i);
		} else if (type1 == ROAD_TYPE) {
			i = data_ptr->Road_Type ();
			type_ptr = (Label_Type *) road_type.Get (&i);
		}
		if (type_ptr != NULL) {
			str_cpy (label, sizeof (label), type_ptr->label);
		} else if (type1 == AREA_TYPE) {
			if (type2 == NO_TYPE) {
				str_fmt (label, sizeof (label), "Area Type %d", i);
			} else {
				str_fmt (label, sizeof (label), "Area %d", i);
			}
		} else if (type1 == FACILITY_TYPE) {
			if (type2 == NO_TYPE) {
				str_fmt (label, sizeof (label), "Facility Type %d", i);
			} else {
				str_fmt (label, sizeof (label), "Facility %d", i);
			}
		} else if (type1 == VEH_TYPE) {
			if (type2 == NO_TYPE) {
				str_fmt (label, sizeof (label), "Vehicle Type %d", i);
			} else {
				str_fmt (label, sizeof (label), "Vehicle %d", i);
			}
		} else {
			if (type2 == NO_TYPE) {
				str_fmt (label, sizeof (label), "Road Type %d", i);
			} else {
				str_fmt (label, sizeof (label), "Road %d", i);
			}
		}

		if (type2 != NO_TYPE) {
			if (type2 == AREA_TYPE) {
				i = data_ptr->Area_Type ();
				type_ptr = (Label_Type *) area_type.Get (&i);
			} else if (type2 == FACILITY_TYPE) {
				i = data_ptr->Facility_Type ();
				type_ptr = (Label_Type *) facility_type.Get (&i);
			} else if (type2 == VEH_TYPE) {
				i = data_ptr->Vehicle_Type ();
				type_ptr = (Label_Type *) vehicle_type.Get (&i);
			} else if (type2 == ROAD_TYPE) {
				i = data_ptr->Road_Type ();
				type_ptr = (Label_Type *) road_type.Get (&i);
			}
			if (type_ptr != NULL) {
				str_cpy (buffer, sizeof (buffer), type_ptr->label);
			} else if (type2 == AREA_TYPE) {
				str_fmt (buffer, sizeof (buffer), "Area %d", i);
			} else if (type2 == FACILITY_TYPE) {
				str_fmt (buffer, sizeof (buffer), "Facility %d", i);
			} else if (type2 == VEH_TYPE) {
				str_fmt (buffer, sizeof (buffer), "Vehicle %d", i);
			} else {
				str_fmt (buffer, sizeof (buffer), "Road %d", i);
			}

			//---- combine the two labels ----

			len1 = (int) strlen (label);
			len2 = (int) strlen (buffer);
			i = len1 + len2 + 1;
			if (i > 27) {
				if (len1 > len2) {
					if (len2 < 13) {
						label [24 - len2] = '\0';
					} else if (len1 < 2 * len2) {
						len1 = (len1 * 24 + (i / 2)) / i;
						label [len1] = '\0';
					} else {
						label [12] = '\0';
					}
				} else if (len1 > 12) {
					if (len1 * 2 < len2) {
						len1 = (len1 * 24 + (i / 2)) / i;
						label [len1] = '\0';
					} else {
						label [12] = '\0';
					}
				}
			}
			str_cat (label, sizeof (label), " ");
			str_cat (label, sizeof (label), buffer);
		}

		//---- write the emissions data ----

		vmt = data_ptr->Meters ();
		vht = data_ptr->Seconds ();

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
		Page_Check (1);
		Print (0, "\n%25.25s %11.0lf %11.0lf %6.1lf", label, vmt, vht, spd);

		for (i=1; i <= num_pollutants; i++) {
			ems = data_ptr->Emissions (i);
			if (emission_units == 0) ems /= 1000.0;

			Print (0, " %11.1lf", ems);
		}
		total.Sum (data_ptr);
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
	Page_Check (3);
	Print (0, "\n\n%25.25s %11.0lf %11.0lf %6.1lf", "Total", vmt, vht, spd);

	for (i=1; i <= num_pollutants; i++) {
		ems = total.Emissions (i);
		if (emission_units == 0) ems /= 1000.0;

		Print (0, " %11.1lf", ems);
	}
	Print (0, "\n");

	Header_Number (0);
}

//---------------------------------------------------------
//	Type_Headers
//---------------------------------------------------------

void Emissions::Type_Headers (char *label)
{
	int len;
	char *str_ptr;
	Category_Data *cat_ptr;

	//---- get the pollutant categories ----

	cat_ptr = category_data.Get (POLLUTANT);

	Page_Check (5);
	len = 60 - (((int) strlen (label) + 13) / 2);

	Print (0, "\n%*cEmissions by %s\n", len, BLANK, label);
	Print (0, "\n%30cVehicle     Vehicle  Speed", BLANK);

	for (str_ptr = cat_ptr->First_Label (); str_ptr; str_ptr = cat_ptr->Next_Label ()) {
		Print (0, "  %10.10s", str_ptr);
	}
	Print (0, "\n%25.25s  ", label, BLANK);

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
	Print (0, "\n");
}

/**********************************************************|***********************************************************

	                                      Emissions by Area and Facility Type

                              Vehicle     Vehicle  Speed  Pollutant1  Pollutant2  Pollutant3  Pollutant4  Pollutant5 
   Area and Facility Type  Kilometers       Hours  (m/s)   Kilograms   Kilograms   Kilograms   Kilograms   Kilograms

sssssssssssssssssssssssss  ffffffffff  ffffffffff  fff.f  ffffffff.f  ffffffff.f  ffffffff.f  ffffffff.f  ffffffff.f  
sssssssssssssssssssssssss  ffffffffff  ffffffffff  fff.f  ffffffff.f  ffffffff.f  ffffffff.f  ffffffff.f  ffffffff.f  
	
                    Total  ffffffffff  ffffffffff  fff.f  ffffffff.f  ffffffff.f  ffffffff.f  ffffffff.f  ffffffff.f  


**********************************************************|***********************************************************/ 
