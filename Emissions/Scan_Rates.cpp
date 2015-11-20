//*********************************************************
//	Scan_Rates.cpp - scan the emission rate files
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Read_Rates
//---------------------------------------------------------

void Emissions::Scan_Rates (void)
{
	int year, month, value;
	bool pollutant_flag;

	Category_Data *cat_ptr;	
	Rate_Group *file_ptr;
	Db_Header *file;

	cat_ptr = category_data.Get (POLLUTANT, true);

	pollutant_flag = (cat_ptr != NULL && cat_ptr->Num_Members () == 0);

	//---- identify the number of members in each rate category ----

	for (file_ptr = rate_group.First (); file_ptr; file_ptr = rate_group.Next ()) {

		file = file_ptr->Rate_File ();

		Show_Message ("Scanning %s -- Record", file->File_Type ());
		Set_Progress ();

		while (file->Read ()) {
			Show_Progress ();

			//---- check the selection criteria ----
			
			if (year_flag) {
				year = file_ptr->Year ();
				if (year > 0) {
					if (!years.Get_Index (year)) continue;
				}
			}
			if (month_flag) {
				month = file_ptr->Month ();
				if (month > 0) {
					if (!months.Get_Index (month)) continue;
				}
			}

			//---- process each rate category ----

			for (cat_ptr = category_data.First (); cat_ptr; cat_ptr = category_data.Next ()) {
				if (!cat_ptr->Key_Flag ()) continue;

				switch (cat_ptr->Number ()) {
					case YEAR:
						value = file_ptr->Year ();
						break;
					case MONTH:
						value = file_ptr->Month ();
						break;
					case PERIOD:
						value = file_ptr->Period ();
						break;
					case REGION:
						value = file_ptr->Region ();
						break;
					case AREATYPE:
						value = file_ptr->Area_Type ();
						break;
					case FACILITY:
						value = file_ptr->Facility ();
						break;
					case ROADTYPE:
						value = file_ptr->Road_Type ();
						break;
					case VEHTYPE:
						value = file_ptr->Vehicle ();
						break;
					case POLLUTANT:
						if (!pollutant_flag) continue;
						value = file_ptr->Pollutant ();
						break;
					case SPEED_BIN:
						value = file_ptr->Speed_Bin ();
						break;
					default:
						value = 0;
						break;
				}
				if (value > 0 && cat_ptr->Member (value) == 0) {
					cat_ptr->Add_Member (value);
				}
			}
		}
		End_Progress ();
		file->Rewind ();
	}

	//---- set the key flag ----

	cat_ptr = category_data.Get (SPEED_BIN);
	max_bin = cat_ptr->Max_Member ();
}
