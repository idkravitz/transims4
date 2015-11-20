//*********************************************************
//	Read_Fare.cpp - read the transit fare file
//*********************************************************

#include "Network_Service.hpp"

#include "Fare_File.hpp"
#include "Integer_Array.hpp"
#include "Data_Range.hpp"

#define DEFAULT_TIME_PERIOD	97200	//---- 27 hours ----

//---------------------------------------------------------
//	Read_Fare
//---------------------------------------------------------

void Network_Service::Read_Fare (void)
{
	int low, high, fare, transfer, count;
	int fz, tz, fm, tm, p, type, phase;
	char *str_ptr, *ptr, buffer [STRING_BUFFER], field [FIELD_BUFFER];
	bool old4_flag, scan_flag;

	Fare_Data *fare_ptr, fare_rec;
	Integer_Sort time_sort;
	Time_Range time_period;
	Data_Range from_zone, to_zone, from_mode, to_mode, type_range;
	Range_Data *fz_ptr, *tz_ptr, *fm_ptr, *tm_ptr, *p_ptr, *t_ptr;

	Fare_File *file = (Fare_File *) Network_Db_File (TRANSIT_FARE);

	//---- scane the transit fare data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress ();

	time_sort.Add (DEFAULT_TIME_PERIOD);
	old4_flag = file->Old4_Flag ();
	count = 1;

	//---- two phase process: scan and then write ----

	for (phase = 0; phase < 2; phase++) {
		scan_flag = (phase == 0);

		if (!scan_flag) {
			file->Rewind ();

			//---- set the time periods ----

			buffer [0] = '\0';

			for (low = time_sort.First_Key (); low > 0; low = time_sort.Next_Key ()) {
				str_fmt (field, sizeof (field), "%d ", low);
				str_cat (buffer, sizeof (buffer), field);
			}
			if (!fare_data.Periods (buffer)) {
				Error ("Insufficient Memory for Fare Time Periods");
			}
			count *= fare_data.Num_Periods ();

			//---- allocated maximuim memory ----

			if (!fare_data.Max_Records (count)) {
				Error ("Insufficient Memory for Fare Data");
			}
		}

		//---- read each fare record ----

		while (file->Read ()) {
			if (!scan_flag) Show_Progress ();

			//---- transit mode ----

			str_ptr = file->To_Mode ();
			if (*str_ptr == '\0') continue;

			//---- reset the range data ----

			from_zone.Reset ();
			to_zone.Reset ();
			from_mode.Reset ();
			to_mode.Reset ();
			time_period.Reset ();
			type_range.Reset ();

			//---- process the transit mode range ----

			str_ptr = str_cpy (buffer, sizeof (buffer), str_ptr);

			while (str_ptr != NULL) {
				str_ptr = Get_Token (str_ptr, field, sizeof (field));
				if (field [0] == '\0') break;

				ptr = strstr (field, "..");
				if (ptr != NULL) {
					*ptr++ = '\0';
					while (*ptr == '.') ptr++;
				}
				low = Transit_Code (field);
				if (low == NO_TRANSIT) {
					Check_Message ("Transit Fare Mode %s is Not Permitted", str_ptr);
					continue;
				}
				if (ptr != NULL) {
					high = Transit_Code (ptr);
					if (high < low) {
						Check_Message ("Transit Fare Mode %s is Out of Order", ptr);
						continue;
					}
				} else {
					if (low == ANY_TRANSIT) {
						low = 1;
						high = ANY_TRANSIT-1;
					} else {
						high = low;
					}
				}
				to_mode.Add_Range (low, high);
			}

			//---- from zone ----

			str_ptr = file->From_Zone ();
			if (*str_ptr == '\0') {
				Check_Message ("Transit Fare From Zone %s is Out of Range", str_ptr);
				continue;
			}
			from_zone.Add_Ranges (str_ptr);

			//---- to zone ----

			str_ptr = file->To_Zone ();
			if (*str_ptr == '\0') {
				Check_Message ("Transit Fare To Zone %s is Out of Range", str_ptr);
				continue;
			}
			to_zone.Add_Ranges (str_ptr);

			//---- from mode ----

			str_ptr = file->From_Mode ();

			if (old4_flag || *str_ptr == '\0') {
				from_mode.Add_Range (0, ANY_TRANSIT-1);
			} else {
				str_ptr = str_cpy (buffer, sizeof (buffer), str_ptr);

				while (str_ptr != NULL) {
					str_ptr = Get_Token (str_ptr, field, sizeof (field));
					if (field [0] == '\0') break;

					ptr = strstr (field, "..");
					if (ptr != NULL) {
						*ptr++ = '\0';
						while (*ptr == '.') ptr++;
					}
					low = Transit_Code (field);
					if (ptr != NULL) {
						high = Transit_Code (ptr);
						if (high < low) {
							Check_Message ("Transit Fare Mode %s is Out of Order", ptr);
							continue;
						}
					} else {
						if (low == ANY_TRANSIT) {
							low = 1;
							high = ANY_TRANSIT-1;
						} else {
							high = low;
						}
					}
					from_mode.Add_Range (low, high);
				}
			}

			//---- time period ----

			str_ptr = file->Period ();
			if (*str_ptr != '\0') {
				time_period.Add_Ranges (str_ptr);

				if (scan_flag) {
					for (p_ptr = time_period.First (); p_ptr; p_ptr = time_period.Next ()) {
						low = p_ptr->Low ();
						if (low > 0 && time_sort.Get_Index (low) == 0) {
							time_sort.Add (low);
						}
						high = p_ptr->High () + 1;
						if (high > 0 && time_sort.Get_Index (high) == 0) {
							time_sort.Add (high);
						}
					}
				}
			} else {
				time_period.Add_Range (0, DEFAULT_TIME_PERIOD-1);
			}

			//---- from mode ----

			str_ptr = file->Class ();

			if (old4_flag || *str_ptr == '\0') {
				type_range.Add_Range (CASH, SPECIAL);
			} else {
				str_ptr = str_cpy (buffer, sizeof (buffer), str_ptr);

				while (str_ptr != NULL) {
					str_ptr = Get_Token (str_ptr, field, sizeof (field));
					if (field [0] == '\0') break;

					ptr = strstr (field, "..");
					if (ptr != NULL) {
						*ptr++ = '\0';
						while (*ptr == '.') ptr++;
					}
					low = Class_Code (field);
					if (ptr != NULL) {
						high = Class_Code (ptr);
						if (high < low) {
							Check_Message ("Transit Fare Class %s is Out of Order", ptr);
							continue;
						}
					} else {
						high = low;
					}
					type_range.Add_Range (low, high);
				}
			}

			//---- get the fare data ----

			fare = file->Fare ();
			transfer = file->Transfer ();

			//---- process the range data ----

			for (fz_ptr = from_zone.First (); fz_ptr; fz_ptr = from_zone.Next ()) {
				for (fz = fz_ptr->Low (); fz <= fz_ptr->High (); fz++) {
					fare_rec.from_zone = (short) fz;
					if (fz > num_fare_zone) num_fare_zone = fz;

					for (tz_ptr = to_zone.First (); tz_ptr; tz_ptr = to_zone.Next ()) {
						for (tz = tz_ptr->Low (); tz <= tz_ptr->High (); tz++) {
							fare_rec.to_zone = (short) tz;
							if (tz > num_fare_zone) num_fare_zone = tz;

							for (fm_ptr = from_mode.First (); fm_ptr; fm_ptr = from_mode.Next ()) {
								for (fm = fm_ptr->Low (); fm <= fm_ptr->High (); fm++) {
									fare_rec.from_mode = (char) fm;

									for (tm_ptr = to_mode.First (); tm_ptr; tm_ptr = to_mode.Next ()) {
										for (tm = tm_ptr->Low (); tm <= tm_ptr->High (); tm++) {
											fare_rec.to_mode = (char) tm;

											if (scan_flag) {
												count += type_range.Num_Ranges ();
												continue;
											}
											for (p_ptr = time_period.First (); p_ptr; p_ptr = time_period.Next ()) {
												low = fare_data.Period (p_ptr->Low ());
												high = fare_data.Period (p_ptr->High ());

												for (p = low; p <= high; p++) {
													fare_rec.period = (char) p;

													for (t_ptr = type_range.First (); t_ptr; t_ptr = type_range.Next ()) {
														for (type = t_ptr->Low (); type <= t_ptr->High (); type++) {
															fare_rec.type = (char) type;

															//---- process the record ----
																
															fare_ptr = fare_data.Get (&fare_rec);

															if (!old4_flag || fm == 0) {
																if (fare_ptr == NULL) {
																	fare_rec.fare = fare;
																	if (!fare_data.Add (&fare_rec)) {
																		Error ("Adding Transit Fare Record");
																	}
																} else {
																	fare_ptr->fare = fare;
																}
															} else {
																if (fare_ptr == NULL) {
																	fare_rec.fare = transfer;
																	if (!fare_data.Add (&fare_rec)) {
																		Error ("Adding Transit Fare Record");
																	}
																} else {
																	fare_ptr->fare = transfer;
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	End_Progress ();

	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = fare_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Transit Fare Data Records = %d", num);
	}
	fare_data.Optimize ();

	if (num > 0) Network_Data_True (TRANSIT_FARE);
	return;
}
