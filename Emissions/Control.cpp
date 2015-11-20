//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "Emissions.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Emissions::Program_Control (void)
{
	int i, num, num_file, vehicle, subtype, box_len, bins, size, increment;
	char *str_ptr, *format_ptr, buffer [FIELD_BUFFER];
	double low, high;
	bool first, year_rate_flag, month_rate_flag;

	Label_Type type_rec;
	Db_File label_file;
	Speed_Bin_File *file;

	Data_Group *data;
	Rate_Group *rate;
	Category_Data *number;

	char *len_units [] = { "MILES", "KILOMETERS", "METERS", "FEET", NULL };
	char *spd_units [] = { "MPH", "KPH", "MPS", "FPS", NULL };
	char *time_units [] = { "HOURS", "DAYS", "MINUTES", "SECONDS", NULL };
	char *ems_units [] = { "KILOGRAMS", "GRAMS", NULL };

	//---- open network and demand files ----

	Demand_Service::Program_Control ();

	zone_flag = Network_File_Flag (ZONE);
	location_flag = Network_File_Flag (ACTIVITY_LOCATION);

	if ((zone_flag || location_flag) && !Network_File_Flag (NODE)) {
		Error ("A Node File is Required for Area Processing");
	}

	//---- get the number of speed data files ----

	num_file = Highest_Control_Group (SPEED_BIN_FILE_x, 0);

	if (num_file == 0) {
		if (Get_Control_String (SPEED_BIN_FILE) != NULL) {
			num_file = 1;
		}
	} else {
		num_file++;
	}

	if (num_file > 0) {
		if (!Network_File_Flag (LINK)) {
			Error ("A Link File is Required for Speed Bin Processing");
		}
		if (!data_group.Max_Records (num_file)) goto mem_error;

		//---- open each speed bin file ----

		for (i=0; i < num_file; i++) {
			if (i == 0) {
				str_ptr = Get_Control_String (SPEED_BIN_FILE);
				format_ptr = Get_Control_String (SPEED_BIN_FORMAT);
			} else {
				str_ptr = Get_Control_String (SPEED_BIN_FILE_x, i);
				format_ptr = Get_Control_String (SPEED_BIN_FORMAT_x, i);
			}
			if (str_ptr == NULL) continue;

			data = data_group.New_Record (true);
			if (data == NULL) goto mem_error;

			data->Group (i);
			data->Factor (1.0);

			Print (1);
			if (!data->Open_File (Project_Filename (str_ptr), format_ptr)) {
				Error ("Opening Speed Bin File");
			}

			//---- check the vehicle type, box size, and speed bins ----

			file = data->Speed_File ();

			vehicle = file->Vehicle_Type ();
			subtype = file->Veh_SubType ();
			Print (1, "\tVehicle Type = %d:%d", vehicle, subtype);

			size = Round (file->Cell_Length ());
			Print (1, "\tCell Length = %.1lf meters", UnRound (size));

			if (cell_size == 0) {
				cell_size = size;
			} else if (size != cell_size) {
				Error ("Cell Size %.1lf and %.1lf do not Match", UnRound (size), UnRound (cell_size));
			}
			box_len = Round (file->Box_Length ());
			Print (1, "\tBox Length = %.1lf meters", UnRound (box_len));

			if (box_length == 0) {
				box_length = box_len;
			} else if (box_len != box_length) {
				Error ("Box Length %.llf and %.1lf do not Match", UnRound (box_len), UnRound (box_length));
			}
			Print (1, "\tSampling Time = %d seconds", file->Sample_Time ());

			bins = file->Num_Bins ();
			Print (1, "\tNumber of Speed Bins = %d", bins);

			if (num_bins == 0) {
				num_bins = bins;
			} else if (bins != num_bins) {
				Error ("Number of Speed Bins %d and %d do not Match", bins, num_bins);
			}

			//---- get the speed bin factor ----

			if (i == 0) {
				str_ptr = NULL;
			} else {
				str_ptr = Get_Control_String (SPEED_BIN_FACTOR_x, i);
			}
			if (str_ptr == NULL) {
				str_ptr = Get_Control_String (SPEED_BIN_FACTOR);
			}
			if (str_ptr != NULL) {
				data->Factor (atof (str_ptr));

				if (data->Factor () < 0.1 || data->Factor () > 400) {
					Error ("Speed Bin Factor %s is Out of Range (0.1..400)", str_ptr);
				}
				Print (1, "\tSpeed Bin Factor = %s", str_ptr);
				volume_factor = data->Factor ();
			}

			//---- save the group ----

			if (!data_group.Add ()) goto mem_error;
		}

	} else {

		//---- check for a traveler file ----

		str_ptr = Get_Control_String (TRAVELER_FILE);

		if (str_ptr != NULL) {
			format_ptr = Get_Control_String (TRAVELER_FORMAT);

			if (format_ptr != NULL) {
				traveler_file.Dbase_Format (format_ptr);
			}
			Print (1);
			traveler_file.Open (Project_Filename (str_ptr));
			traveler_flag = true;

		} else {

			//---- open the volume speed file ----

			str_ptr = Get_Control_String (VOLUME_SPEED_FILE);

			if (str_ptr == NULL) {
				Error ("Speed Bin, Traveler, or Volume-Speed files were Not Found");
			}
			vol_spd_file.File_Type ("Volume Speed File");

			format_ptr = Get_Control_String (VOLUME_SPEED_FORMAT);

			if (format_ptr != NULL) {
				vol_spd_file.Dbase_Format (format_ptr);
			}
			Print (1);
			vol_spd_file.Open (Project_Filename (str_ptr));
			vol_spd_flag = true;

			//---- volume speed factor ----

			str_ptr = Get_Control_String (VOLUME_SPEED_FACTOR);

			if (str_ptr != NULL) {
				volume_factor = atof (str_ptr);

				if (volume_factor < 0.1 || volume_factor > 400.0) {
					Error ("Volume Speed Factor %s is Out of Range (0.1..400)", volume_factor);
				}
				Print (1, "\tVolume Speed Factor = %s", str_ptr);
			}

			//---- get the length field ----

			str_ptr = Get_Control_String (VOLSPD_LENGTH_FIELD);

			if (str_ptr == NULL) goto control_error;

			length_field = vol_spd_file.Field_Number (str_ptr);

			if (length_field == 0) {
				Error ("Length Field %s was Not Found in the Volume Speed File", str_ptr);
			}
			Print (1, "\tLength Field = %s, Number = %d", str_ptr, length_field);

			//---- get the length units ----

			str_ptr = Get_Control_String (VOLSPD_LENGTH_UNITS);

			if (str_ptr != NULL) {
				for (i=0; len_units [i] != NULL; i++) {
					if (!str_cmp (str_ptr, len_units [i])) {
						length_units = i;
						break;
					}
				}
				if (len_units [i] == NULL) {
					Error ("Length Units %s were Not Recognized", str_ptr);
				}
			}
			Print (0, "  Units = %s", len_units [length_units]);

			//---- get the facility field ----

			str_ptr = Get_Control_String (VOLSPD_FACILITY_FIELD);

			if (str_ptr == NULL) goto control_error;

			facility_field = vol_spd_file.Field_Number (str_ptr);

			if (facility_field == 0) {
				Error ("Facility Field %s was Not Found in the Volume Speed File", str_ptr);
			}
			Print (1, "\tFacility Field = %s, Number = %d", str_ptr, facility_field);

			//---- get the ramp facility ----

			str_ptr = Get_Control_String (VOLSPD_RAMP_FACILITY);

			if (str_ptr != NULL) {
				ramp_facility = atoi (str_ptr);

				if (ramp_facility < 0 || ramp_facility > 100) {
					Error ("Ramp Facility %d is Out of Range (0..100)", ramp_facility);
				}
				Print (1, "\tRamp Facility = %d", ramp_facility);
			}

			//---- get the area type field ----

			str_ptr = Get_Control_String (VOLSPD_AREATYPE_FIELD);

			if (str_ptr != NULL) {
				areatype_field = vol_spd_file.Field_Number (str_ptr);

				if (areatype_field == 0) {
					Error ("Area Type Field %s was Not Found in the Volume Speed File", str_ptr);
				}
				Print (1, "\tArea Type Field = %s, Number = %d", str_ptr, areatype_field);
			}

			//---- get the time period field ----

			str_ptr = Get_Control_String (VOLSPD_PERIOD_FIELD);

			if (str_ptr != NULL) {
				period_field = vol_spd_file.Field_Number (str_ptr);

				if (period_field == 0) {
					Error ("Time Period Field %s was Not Found in the Volume Speed File", str_ptr);
				}
				Print (1, "\tTime Period Field = %s, Number = %d", str_ptr, period_field);

				//---- get the time period units ----

				str_ptr = Get_Control_String (VOLSPD_PERIOD_UNITS);

				if (str_ptr != NULL) {
					for (i=0; time_units [i] != NULL; i++) {
						if (!str_cmp (str_ptr, time_units [i])) {
							period_units = i;
							break;
						}
					}
					if (time_units [i] == NULL) {
						Error ("Period Units %s were Not Recognized", str_ptr);
					}
					Print (0, "  Units = %s", str_ptr);
				}
			}

			//---- get the speed field ----

			str_ptr = Get_Control_String (VOLSPD_SPEED_FIELD);

			if (str_ptr == NULL) goto control_error;

			speed_field = vol_spd_file.Field_Number (str_ptr);

			if (speed_field == 0) {
				Error ("Speed Field %s was Not Found in the Volume Speed File", str_ptr);
			}
			Print (1, "\tSpeed Field = %s, Number = %d", str_ptr, speed_field);

			//---- get the speed units ----

			str_ptr = Get_Control_String (VOLSPD_SPEED_UNITS);

			if (str_ptr != NULL) {
				for (i=0; spd_units [i] != NULL; i++) {
					if (!str_cmp (str_ptr, spd_units [i])) {
						speed_units = i;
						break;
					}
				}
				if (spd_units [i] == NULL) {
					Error ("Speed Units %s were Not Recognized", str_ptr);
				}
				Print (0, "  Units = %s", str_ptr);
			}

			//---- get the volume fields ----
		
			num_vol_fld = Highest_Control_Group (VOLSPD_VOLUME_FIELD_x, 0);

			if (num_vol_fld == 0) {
				Error ("No Volume Field Names");
			}
			num = num_vol_fld + 1;

			volume_field = new int [num];
			memset (volume_field, '\0', num * sizeof (int));

			vol_spd_data.Num_Volume (num_vol_fld);

			for (i=1; i <= num_vol_fld; i++) {
				str_ptr = Get_Control_String (VOLSPD_VOLUME_FIELD_x, i);
				if (str_ptr == NULL) continue;

				volume_field [i] = vol_spd_file.Field_Number (str_ptr);

				if (volume_field [i] == 0) {
					Error ("Volume Field #%d %s was Not Found in the Volume Speed File", i, str_ptr);
				}
				Print (1, "\tVolume Field #%d = %s, Number = %d", i, str_ptr, volume_field [i]);
			}

			//---- get the interpolation flag ----

			str_ptr = Get_Control_String (USE_SIMPLE_INTERPOLATION);

			if (str_ptr != NULL) {
				Print (1, "Use Simple Interpolation = %s", str_ptr);

				interpolate_flag = Get_Control_Flag (USE_SIMPLE_INTERPOLATION);
			}
		}
	}

	//---- set the processing options ----

	bin_sum_flag = (Get_Control_String (NEW_SPEED_SUMMARY_FILE) != NULL);
	hpms_flag = (Get_Control_String (NEW_MOVES_HPMS_VMT_FILE) != NULL);
	ramp_flag = (Get_Control_String (NEW_MOVES_RAMP_FRACTION_FILE) != NULL);
	spd_flag = (Get_Control_String (NEW_MOVES_SPEED_HOUR_FILE) != NULL);
	vmt_flag = (Get_Control_String (NEW_MOVES_VMT_HOUR_FILE) != NULL);
	road_flag = (Get_Control_String (NEW_MOVES_VMT_ROAD_FILE) != NULL);

	input_flag = bin_sum_flag || hpms_flag || ramp_flag || spd_flag || vmt_flag || road_flag;

	link_sum_flag = (Get_Control_String (NEW_MOVES_LINK_SUMMARY_FILE) != NULL);
	link_veh_flag = (Get_Control_String (NEW_MOVES_LINK_VEHICLE_FILE) != NULL);
	link_spd_flag = (Get_Control_String (NEW_MOVES_LINK_SPEED_FILE) != NULL);
	link_ops_flag = (Get_Control_String (NEW_MOVES_LINK_OPERATIONS_FILE) != NULL);

	link_flag = link_sum_flag || link_veh_flag || link_spd_flag || link_ops_flag;

	if (link_flag) {
		if (!vol_spd_flag && !traveler_flag) {
			speed_bin_data.Num_Bins (num_bins * data_group.Num_Records ());
		}
	}
	if (!traveler_flag || !link_veh_flag) {
		Demand_File_False (VEHICLE);
	}
	
	//---- get the number of emission rate files ----

	num_file = Highest_Control_Group (EMISSION_RATE_FILE_x, 0);

	if (num_file == 0) {
		if (Get_Control_String (EMISSION_RATE_FILE) != NULL) {
			num_file = 1;
		}
	} else {
		num_file++;
	}
	rate_flag = (num_file > 0);
	year_rate_flag = month_rate_flag = false;

	if (rate_flag) {

		if (!rate_group.Max_Records (num_file)) goto mem_error;

		//---- open each emission rate file ----

		for (i=0; i < num_file; i++) {
			if (i == 0) {
				str_ptr = Get_Control_String (EMISSION_RATE_FILE);
				format_ptr = Get_Control_String (EMISSION_RATE_FORMAT);
			} else {
				str_ptr = Get_Control_String (EMISSION_RATE_FILE_x, i);
				format_ptr = Get_Control_String (EMISSION_RATE_FORMAT_x, i);
			}
			if (str_ptr == NULL) continue;

			rate = rate_group.New_Record (true);
			if (rate == NULL) goto mem_error;

			rate->Group (i);

			Print (1);
			if (!rate->Open_File (Project_Filename (str_ptr), format_ptr)) {
				Error ("Opening Emission Rate File");
			}

			//---- get the year field ----

			if (i == 0) {
				str_ptr = NULL;
			} else {
				str_ptr = Get_Control_String (RATE_YEAR_FIELD_x, i);
			}
			if (str_ptr == NULL) {
				str_ptr = Get_Control_String (RATE_YEAR_FIELD);
			}
			if (str_ptr != NULL) {
				if (!rate->Year_Field (str_ptr)) goto field_error;
				Print (1, "\tRate Year Field = %s, Number = %d", str_ptr, rate->Year_Field ());
				year_rate_flag = true;

				//---- initialize year categories ----

				if (category_data.Get (YEAR) == NULL) {
					number = new Category_Data (YEAR, true);
					number->Name ("Year");
					category_data.Add (number);
				}
			}

			//---- get the month field ----

			if (i == 0) {
				str_ptr = NULL;
			} else {
				str_ptr = Get_Control_String (RATE_MONTH_FIELD_x, i);
			}
			if (str_ptr == NULL) {
				str_ptr = Get_Control_String (RATE_MONTH_FIELD);
			}
			if (str_ptr != NULL) {
				if (!rate->Month_Field (str_ptr)) goto field_error;
				Print (1, "\tRate Month Field = %s, Number = %d", str_ptr, rate->Month_Field ());
				month_rate_flag = true;

				//---- initialize month categories ----

				if (category_data.Get (MONTH) == NULL) {
					number = new Category_Data (MONTH, true);
					number->Name ("Month");
					category_data.Add (number);
				}
			}

			//---- get the period field ----

			if (i == 0) {
				str_ptr = NULL;
			} else {
				str_ptr = Get_Control_String (RATE_PERIOD_FIELD_x, i);
			}
			if (str_ptr == NULL) {
				str_ptr = Get_Control_String (RATE_PERIOD_FIELD);
			}
			if (str_ptr != NULL) {
				if (!rate->Period_Field (str_ptr)) goto field_error;
				Print (1, "\tRate Period Field = %s, Number = %d", str_ptr, rate->Period_Field ());

				//---- initialize period categories ----

				if (category_data.Get (PERIOD) == NULL) {
					number = new Category_Data (PERIOD, true);
					number->Name ("Period");
					category_data.Add (number);
				}
			}

			//---- get the region field ----

			if (i == 0) {
				str_ptr = NULL;
			} else {
				str_ptr = Get_Control_String (RATE_REGION_FIELD_x, i);
			}
			if (str_ptr == NULL) {
				str_ptr = Get_Control_String (RATE_REGION_FIELD);
			}
			if (str_ptr != NULL) {
				if (!rate->Region_Field (str_ptr)) goto field_error;
				Print (1, "\tRate Region Field = %s, Number = %d", str_ptr, rate->Region_Field ());

				//---- initialize region categories ----

				if (category_data.Get (REGION) == NULL) {
					number = new Category_Data (REGION, true);
					number->Name ("Region");
					category_data.Add (number);
				}
			}

			//---- get the area type field ----

			if (i == 0) {
				str_ptr = NULL;
			} else {
				str_ptr = Get_Control_String (RATE_AREA_TYPE_FIELD_x, i);
			}
			if (str_ptr == NULL) {
				str_ptr = Get_Control_String (RATE_AREA_TYPE_FIELD);
			}
			if (str_ptr != NULL) {
				if (!rate->Area_Type_Field (str_ptr)) goto field_error;
				Print (1, "\tRate Area Type Field = %s, Number = %d", str_ptr, rate->Area_Type_Field ());

				//---- initialize region categories ----

				if (category_data.Get (AREATYPE) == NULL) {
					number = new Category_Data (AREATYPE, true);
					number->Name ("Area Type");
					category_data.Add (number);
				}
			}

			//---- get the facility field ----

			if (i == 0) {
				str_ptr = NULL;
			} else {
				str_ptr = Get_Control_String (RATE_FACILITY_FIELD_x, i);
			}
			if (str_ptr == NULL) {
				str_ptr = Get_Control_String (RATE_FACILITY_FIELD);
			}
			if (str_ptr != NULL) {
				if (!rate->Facility_Field (str_ptr)) goto field_error;
				Print (1, "\tRate Facility Field = %s, Number = %d", str_ptr, rate->Facility_Field ());

				//---- initialize facility categories ----

				if (category_data.Get (FACILITY) == NULL) {
					number = new Category_Data (FACILITY, true);
					number->Name ("Facility");
					category_data.Add (number);
				}
			}

			//---- get the road type field ----

			if (i == 0) {
				str_ptr = NULL;
			} else {
				str_ptr = Get_Control_String (RATE_ROAD_TYPE_FIELD_x, i);
			}
			if (str_ptr == NULL) {
				str_ptr = Get_Control_String (RATE_ROAD_TYPE_FIELD);
			}
			if (str_ptr != NULL) {
				if (!rate->Road_Type_Field (str_ptr)) goto field_error;
				Print (1, "\tRate Road Type Field = %s, Number = %d", str_ptr, rate->Road_Type_Field ());

				//---- initialize facility categories ----

				if (category_data.Get (ROADTYPE) == NULL) {
					number = new Category_Data (ROADTYPE, true);
					number->Name ("Road Type");
					category_data.Add (number);
				}
			}

			//---- get the vehicle field ----

			if (i == 0) {
				str_ptr = NULL;
			} else {
				str_ptr = Get_Control_String (RATE_VEHICLE_FIELD_x, i);
			}
			if (str_ptr == NULL) {
				str_ptr = Get_Control_String (RATE_VEHICLE_FIELD);
			}
			if (str_ptr != NULL) {
				if (!rate->Vehicle_Field (str_ptr)) goto field_error;
				Print (1, "\tRate Vehicle Field = %s, Number = %d", str_ptr, rate->Vehicle_Field ());

				//---- initialize vehicle categories ----

				if (category_data.Get (VEHTYPE) == NULL) {
					number = new Category_Data (VEHTYPE, true);
					number->Name ("Vehicle");
					category_data.Add (number);
				}
			}

			//---- get the pollutant field ----

			if (i == 0) {
				str_ptr = NULL;
			} else {
				str_ptr = Get_Control_String (RATE_POLLUTANT_FIELD_x, i);
			}
			if (str_ptr == NULL) {
				str_ptr = Get_Control_String (RATE_POLLUTANT_FIELD);
			}
			if (str_ptr == NULL) {

				//---- get the pollutant name ----

				if (i == 0) {
					str_ptr = Get_Control_String (EMISSION_RATE_POLLUTANT);
				} else {
					str_ptr = Get_Control_String (EMISSION_RATE_POLLUTANT_x, i);
				}
				if (str_ptr == NULL) goto control_error;

				Print (1, "\tEmission Rate Pollutant = %s", str_ptr);

				//---- set the pollutant number ----

				number = category_data.Get (POLLUTANT);
				if (number == NULL) {
					number = new Category_Data (POLLUTANT, true);
					number->Name ("Pollutant");
					number->Add_Member (100 + i, str_ptr);
					category_data.Add (number);
				} else {
					number->Add_Member (100 + i, str_ptr);
				}
			} else {
				if (!rate->Pollutant_Field (str_ptr)) goto field_error;
				Print (1, "\tRate Pollutant Field = %s, Number = %d", str_ptr, rate->Pollutant_Field ());

				//---- initialize pollutant categories ----

				number = category_data.Get (POLLUTANT);

				if (number == NULL) {
					number = new Category_Data (POLLUTANT, true);
					number->Name ("Pollutant");
					category_data.Add (number);
				}
			}

			//---- get the speed bin field ----

			if (i == 0) {
				str_ptr = NULL;
			} else {
				str_ptr = Get_Control_String (RATE_SPEED_BIN_FIELD_x, i);
			}
			if (str_ptr == NULL) {
				str_ptr = Get_Control_String (RATE_SPEED_BIN_FIELD);
			}
			if (str_ptr == NULL) {
				goto control_error;
			} else {
				if (!rate->Speed_Bin_Field (str_ptr)) goto field_error;
				Print (1, "\tRate Speed Bin Field = %s, Number = %d", str_ptr, rate->Speed_Bin_Field ());

				//---- initialize speed bin categories ----

				if (category_data.Get (SPEED_BIN) == NULL) {
					number = new Category_Data (SPEED_BIN, true);
					number->Name ("Speed Bin");
					category_data.Add (number);
				}
			}

			//---- get the emission rate field ----

			if (i == 0) {
				str_ptr = NULL;
			} else {
				str_ptr = Get_Control_String (EMISSION_RATE_FIELD_x, i);
			}
			if (str_ptr == NULL) {
				str_ptr = Get_Control_String (EMISSION_RATE_FIELD);
			}
			if (str_ptr == NULL) {
				goto control_error;
			} else {
				if (!rate->Rate_Field (str_ptr)) goto field_error;
				Print (1, "\tEmission Rate Field = %s, Number = %d", str_ptr, rate->Rate_Field ());
			}

			//---- get the emission rate units ----

			if (i == 0) {
				str_ptr = NULL;
			} else {
				str_ptr = Get_Control_String (EMISSION_RATE_UNITS_x, i);
			}
			if (str_ptr == NULL) {
				str_ptr = Get_Control_String (EMISSION_RATE_UNITS);
			}
			if (str_ptr == NULL) {
				str_ptr = "GRAMS_PER_KILOMETER";
			}
			Print (1, "\tEmission Rate Units = %s", str_ptr);

			rate->Rate_Units (Units_Code (str_ptr));

			//---- save the record ----

			if (!rate_group.Add ()) goto mem_error;
		}
	} else if (!input_flag) {
		Error ("No Emission Rate Files were Found");
	}

	//---- check the category data ----

	if (input_flag) {

		//---- initialize vehicle categories ----

		if (category_data.Get (VEHTYPE) == NULL) {
			number = new Category_Data (VEHTYPE);
			number->Name ("Vehicle");
			category_data.Add (number);
		}

		//---- initialize road type categories ----

		if (category_data.Get (ROADTYPE) == NULL) {
			number = new Category_Data (ROADTYPE);
			number->Name ("Road Type");
			category_data.Add (number);
		}

		//---- initialize period categories ----

		if (category_data.Get (PERIOD) == NULL) {
			number = new Category_Data (PERIOD);
			number->Name ("Period");
			category_data.Add (number);
		}

		//---- initialize speed bin categories ----

		if (category_data.Get (SPEED_BIN) == NULL) {
			number = new Category_Data (SPEED_BIN);
			number->Name ("Speed Bin");
			category_data.Add (number);
		}

		//---- initialize hpms categories ----

		if (hpms_flag && category_data.Get (HPMS) == NULL) {
			number = new Category_Data (HPMS);
			number->Name ("HPMS");
			category_data.Add (number);
		}
	}

	//---- get year numbers ----

	number = category_data.Get (YEAR);

	if (number != NULL) {
		num = Highest_Control_Group (YEAR_NUMBER_x, 0);

		if (num > 0) {
			first = true;

			for (i=1; i <= num; i++) {
				str_ptr = Get_Control_String (YEAR_NUMBER_x, i);

				if (str_ptr != NULL) {
					if (first) {
						Print (1);
						first = false;
					}
					Print (1, "Year Number #%d = %s", i, str_ptr);
					number->Add_Member (i, str_ptr);
				}
			}
		} else if (number->Key_Flag ()) {
			scan_flag = true;
		}
	}

	//---- get month numbers ----

	number = category_data.Get (MONTH);

	if (number != NULL) {
		num = Highest_Control_Group (MONTH_NUMBER_x, 0);

		if (num > 0) {
			first = true;

			for (i=1; i <= num; i++) {
				str_ptr = Get_Control_String (MONTH_NUMBER_x, i);

				if (str_ptr != NULL) {
					if (first) {
						Print (1);
						first = false;
					}
					Print (1, "Month Number #%d = %s", i, str_ptr);
					number->Add_Member (i, str_ptr);
				}
			}
		} else if (number->Key_Flag ()) {
			scan_flag = true;
		}
	}

	//---- get period numbers ----

	number = category_data.Get (PERIOD);

	if (number != NULL) {
		num = Highest_Control_Group (PERIOD_NUMBER_x, 0);

		if (num > 0) {
			first = true;

			for (i=1; i <= num; i++) {
				str_ptr = Get_Control_String (PERIOD_NUMBER_x, i);

				if (str_ptr != NULL) {
					if (first) {
						Print (1);
						first = false;
					}
					Print (1, "Period Number #%d = %s", i, str_ptr);
					number->Add_Member (i, str_ptr);
				}
			}
		} else if (number->Key_Flag ()) {
			scan_flag = true;
		}
	}

	//---- get region numbers ----

	number = category_data.Get (REGION);

	if (number != NULL) {
		num = Highest_Control_Group (REGION_NUMBER_x, 0);

		if (num > 0) {
			first = true;

			for (i=1; i <= num; i++) {
				str_ptr = Get_Control_String (REGION_NUMBER_x, i);

				if (str_ptr != NULL) {
					if (first) {
						Print (1);
						first = false;
					}
					Print (1, "Region Number #%d = %s", i, str_ptr);
					number->Add_Member (i, str_ptr);
				}
			}
		} else if (number->Key_Flag ()) {
			scan_flag = true;
		}
	}

	//---- get area type numbers ----

	number = category_data.Get (AREATYPE);

	if (number != NULL) {
		num = Highest_Control_Group (AREA_TYPE_NUMBER_x, 0);

		if (num > 0) {
			first = true;

			for (i=1; i <= num; i++) {
				str_ptr = Get_Control_String (AREA_TYPE_NUMBER_x, i);

				if (str_ptr != NULL) {
					if (first) {
						Print (1);
						first = false;
					}
					Print (1, "Area Type Number #%d = %s", i, str_ptr);
					number->Add_Member (i, str_ptr);
				}
			}
		} else if (number->Key_Flag ()) {
			scan_flag = true;
		}
	}

	//---- get facility numbers ----

	number = category_data.Get (FACILITY);

	if (number != NULL) {
		num = Highest_Control_Group (FACILITY_NUMBER_x, 0);

		if (num > 0) {
			first = true;

			for (i=1; i <= num; i++) {
				str_ptr = Get_Control_String (FACILITY_NUMBER_x, i);

				if (str_ptr != NULL) {
					if (first) {
						Print (1);
						first = false;
					}
					Print (1, "Facility Number #%d = %s", i, str_ptr);
					number->Add_Member (i, str_ptr);
				}
			}
		} else if (number->Key_Flag ()) {
			scan_flag = true;
		}
	}

	//---- get road type numbers ----

	number = category_data.Get (ROADTYPE);

	if (number != NULL) {
		num = Highest_Control_Group (ROAD_TYPE_NUMBER_x, 0);

		if (num > 0) {
			first = true;

			for (i=1; i <= num; i++) {
				str_ptr = Get_Control_String (ROAD_TYPE_NUMBER_x, i);

				if (str_ptr != NULL) {
					if (first) {
						Print (1);
						first = false;
					}
					Print (1, "Road Type Number #%d = %s", i, str_ptr);
					number->Add_Member (i, str_ptr);
				}
			}
		} else if (number->Key_Flag ()) {
			scan_flag = true;
		}
	}

	//---- get vehicle numbers ----

	number = category_data.Get (VEHTYPE);

	if (number != NULL) {
		num = Highest_Control_Group (VEHICLE_NUMBER_x, 0);

		if (num > 0) {
			first = true;

			for (i=1; i <= num; i++) {
				str_ptr = Get_Control_String (VEHICLE_NUMBER_x, i);

				if (str_ptr != NULL) {
					if (first) {
						Print (1);
						first = false;
					}
					Print (1, "Vehicle Number #%d = %s", i, str_ptr);
					number->Add_Member (i, str_ptr);
				}
			}
		} else if (number->Key_Flag ()) {
			scan_flag = true;
		}
	}

	//---- get speed bin numbers ----

	number = category_data.Get (SPEED_BIN);

	if (number != NULL) {
		num = Highest_Control_Group (SPEED_BIN_NUMBER_x, 0);

		if (num > 0) {
			first = true;

			for (i=1; i <= num; i++) {
				str_ptr = Get_Control_String (SPEED_BIN_NUMBER_x, i);

				if (str_ptr != NULL) {
					if (first) {
						Print (1);
						first = false;
					}
					Print (1, "Speed Bin Number #%d = %s", i, str_ptr);
					number->Add_Member (i, str_ptr);
				}
			}
		} else if (number->Key_Flag ()) {
			scan_flag = true;
		}
	}

	//---- get pollutant numbers ----

	number = category_data.Get (POLLUTANT);

	if (number != NULL) {
		num = Highest_Control_Group (POLLUTANT_NUMBER_x, 0);

		if (num > 0) {
			first = true;

			for (i=1; i <= num; i++) {
				str_ptr = Get_Control_String (POLLUTANT_NUMBER_x, i);

				if (str_ptr != NULL) {
					if (first) {
						Print (1);
						first = false;
					}
					Print (1, "Pollutant Number #%d = %s", i, str_ptr);
					number->Add_Member (i, str_ptr);
				}
			}
		} else if (number->Key_Flag () && number->Num_Members () == 0) {
			scan_flag = true;
		}
	}

	//---- get pollutant units ----

	num = Highest_Control_Group (POLLUTANT_UNITS_x, 0);

	if (num > 0) {
		int units;
		first = true;
		pollutant_units.Num_Records (num);

		for (i=1; i <= num; i++) {
			str_ptr = Get_Control_String (POLLUTANT_UNITS_x, i);

			if (str_ptr != NULL) {
				if (first) {
					Print (1);
					first = false;
				}
				Print (1, "Pollutant Units #%d = %s", i, str_ptr);
				units = Units_Code (str_ptr);
				pollutant_units.Set (i, units);
			}
		}
	}

	//---- get hpms numbers ----

	if (hpms_flag) {
		number = category_data.Get (HPMS);

		if (number != NULL) {
			num = Highest_Control_Group (HPMS_NUMBER_x, 0);

			if (num > 0) {
				first = true;
				number->Key_Flag (false);

				for (i=1; i <= num; i++) {
					str_ptr = Get_Control_String (HPMS_NUMBER_x, i);

					if (str_ptr != NULL) {
						if (first) {
							Print (1);
							first = false;
						}
						Print (1, "HPMS Number #%d = %s", i, str_ptr);
						number->Add_Member (i, str_ptr);
					}
				}
			} else {
				Error ("HPMS Vehicle Factors are Required for HPMS Output");
			}
		}
	}

	//---- summary years ----

	str_ptr = Get_Control_String (SUMMARY_YEARS);

	if (str_ptr != NULL) {
		Print (2, "Summary Years = %s", str_ptr);
		year_flag = true;

		while (str_ptr != NULL) {
			str_ptr = Get_Range (str_ptr, &low, &high);

			if (low == 0 && high == 0) continue;

			if (low > high || low < 1980 || high > 2100) {
				Error ("Summary Year Range %.0lf..%.0lf is Illogical", low, high);
			}
			for (num = (int) low; num <= high; num++) {
				if (!years.Add (&num)) goto mem_error;
			}
		}
	} else if (year_rate_flag) {
		Warning ("Summary Years were Not Defined for Year-based Rates");
	}

	//---- summary months ----

	str_ptr = Get_Control_String (SUMMARY_MONTHS);

	if (str_ptr != NULL) {
		if (!year_flag) Print (1);

		Print (1, "Summary Months = %s", str_ptr);
		month_flag = true;

		while (str_ptr != NULL) {
			str_ptr = Get_Range (str_ptr, &low, &high);

			if (low == 0 && high == 0) continue;

			if (low > high || low < 1 || high > 12) {
				Error ("Summary Month Range %.0lf..%.0lf is Illogical", low, high);
			}
			for (num = (int) low; num <= high; num++) {
				if (!months.Add (&num)) goto mem_error;
			}
		}
	} else if (month_rate_flag) {
		Warning ("Summary Months were Not Defined for Month-based Rates");
	}

	//---- get summary increment ----
	
	str_ptr = Get_Control_String (SUMMARY_TIME_INCREMENT);

	if (str_ptr != NULL) {
		increment = atol (str_ptr);

		if (increment < 0 || increment > 240) {
			Error ("Summary Time Increment %d is Out of Range (0-240)", increment);
		}
		increment *= 60;

		if (increment != 0) {
			time_periods.Period_Flag (true);
		}
	} else {
		increment = 0;
	}
	time_periods.Increment (increment);
	time_periods.Format ("24_HOUR_CLOCK");

	//---- get the summary time periods ----
	
	str_ptr = Get_Control_String (SUMMARY_TIME_PERIODS);

	if (str_ptr == NULL) {
		str_ptr = (char *) "All";
	}
	Print (2, "Summary Time Periods = %s", str_ptr);

	if (!time_periods.Add_Ranges (str_ptr)) {
		File_Error ("Time Period Range", str_ptr);
	}
	if (increment > 0) {
		Print (1, "Summary Time Increment = %d minutes", (increment / 60));
	} else {
		Print (1, "Summary Time Increment = Daily");
	}

	//---- weekend travel factor ----

	str_ptr = Get_Control_String (WEEKEND_TRAVEL_FACTOR);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &weekend_factor);

		if (weekend_factor < 0.0 || weekend_factor > 1.5) {
			Error ("Weekend Travel Factor %.2lf is Out of Range (0.0..1.5)", weekend_factor);
		}
		Print (1, "Weekend Travel Factor = %.2lf", weekend_factor);
	}

	//---- get the select locations ----
	
	str_ptr = Get_Control_String (SELECT_ZONE_RANGE);

	if (str_ptr != NULL) {
		Print (2, "Select Zone Range = %s", str_ptr);

		if (!select_zone.Add_Ranges (str_ptr)) {
			File_Error ("Select Zone Range", str_ptr);
		}
		select_flag = true;
		if (!zone_flag && !location_flag) {
			Error ("Zone or Location files are Required for Zone Range Selection");
		}
	}

	//---- get the select subarea polygon ----

	str_ptr = Get_Control_String (SELECT_SUBAREA_POLYGON);

	if (str_ptr != NULL) {
		if (!Network_File_Flag (NODE)) {
			Error ("Node file is Required for Subarea Polygon Selection");
		}
		Print (1);
		select_subarea.File_Type ("Select Subarea Polygon");

		select_subarea.Open (Project_Filename (str_ptr));
		
		if (!select_subarea.Read_Record ()) {
			Error ("Reading %s", select_subarea.File_Type ());
		}
		subarea_flag = true;
	}

	//---- open the link equivalence ----

	if (link_flag) {
		str_ptr = Get_Control_String (LINK_EQUIVALENCE_FILE);

		if (str_ptr != NULL) {
			Print (1);
			link_equiv.Open (Project_Filename (str_ptr));
			link_eq_flag = true;
		}
	}

	//---- open the zone equivalence ----

	str_ptr = Get_Control_String (ZONE_EQUIVALENCE_FILE);

	if (str_ptr != NULL) {
		if (!zone_flag && !location_flag) {
			Error ("A Zone Equivalence File is Required for Area Type Reports");
		}
		at_equiv_flag = true;
		Print (1);
		zone_equiv.Open (Project_Filename (str_ptr));
	}

	//---- open the region equivalence ----

	str_ptr = Get_Control_String (REGION_EQUIVALENCE_FILE);

	if (str_ptr != NULL) {
		if (!zone_flag && !location_flag) {
			Error ("A Region Equivalence File is Required for Region Processing");
		}
		reg_equiv_flag = true;
		Print (1);
		region_equiv.Open (Project_Filename (str_ptr));
	} else if (category_data.Get (REGION) != NULL) {
		Error ("Region Processing Requires a Region Equivalance File");
	}

	//---- open the area type labels ----

	str_ptr = Get_Control_String (AREA_TYPE_LABELS);

	if (str_ptr != NULL) {
		label_file.File_Type ("Area Type Label File");
		Print (1);

		label_file.Open (Project_Filename (str_ptr));

		while (label_file.Read ()) {

			str_ptr = label_file.Clean_Record ();

			if (*str_ptr == '\0') continue;

			str_ptr = Get_Integer (str_ptr, &i);
			if (i == 0 || str_ptr == NULL) continue;

			type_rec.type = i;

			while (*str_ptr == ' ' || *str_ptr == '\t' || *str_ptr == ',') str_ptr++;

			i = (int) (strlen (str_ptr)) + 1;

			type_rec.label = new char [i];

			if (type_rec.label == NULL) goto mem_error;

			str_cpy (type_rec.label, i, str_ptr);

			if (!area_type.Add (&type_rec)) goto mem_error;
		}
		label_file.Close ();
	}

	//---- open the facility type labels ----

	str_ptr = Get_Control_String (FACILITY_TYPE_LABELS);

	if (str_ptr != NULL) {
		label_file.File_Type ("Facility Type Label File");
		Print (1);

		label_file.Open (Project_Filename (str_ptr));

		while (label_file.Read ()) {

			str_ptr = label_file.Clean_Record ();

			if (*str_ptr == '\0') continue;

			str_ptr = Get_Integer (str_ptr, &i);
			if (i == 0 || str_ptr == NULL) continue;

			type_rec.type = i;

			while (*str_ptr == ' ' || *str_ptr == '\t' || *str_ptr == ',') str_ptr++;

			i = (int) (strlen (str_ptr)) + 1;

			type_rec.label = new char [i];

			if (type_rec.label == NULL) goto mem_error;

			str_cpy (type_rec.label, i, str_ptr);

			if (!facility_type.Add (&type_rec)) goto mem_error;
		}
		label_file.Close ();
	}

	//---- open the vehicle type labels ----

	str_ptr = Get_Control_String (VEHICLE_TYPE_LABELS);

	if (str_ptr != NULL) {
		label_file.File_Type ("Vehicle Type Label File");
		Print (1);

		label_file.Open (Project_Filename (str_ptr));

		while (label_file.Read ()) {

			str_ptr = label_file.Clean_Record ();

			if (*str_ptr == '\0') continue;

			str_ptr = Get_Integer (str_ptr, &i);
			if (i == 0 || str_ptr == NULL) continue;

			type_rec.type = i;

			while (*str_ptr == ' ' || *str_ptr == '\t' || *str_ptr == ',') str_ptr++;

			i = (int) (strlen (str_ptr)) + 1;

			type_rec.label = new char [i];

			if (type_rec.label == NULL) goto mem_error;

			str_cpy (type_rec.label, i, str_ptr);

			if (!vehicle_type.Add (&type_rec)) goto mem_error;
		}
		label_file.Close ();
	}

	//---- open the road type labels ----

	str_ptr = Get_Control_String (ROAD_TYPE_LABELS);

	if (str_ptr != NULL) {
		label_file.File_Type ("Road Type Label File");
		Print (1);

		label_file.Open (Project_Filename (str_ptr));

		while (label_file.Read ()) {

			str_ptr = label_file.Clean_Record ();

			if (*str_ptr == '\0') continue;

			str_ptr = Get_Integer (str_ptr, &i);
			if (i == 0 || str_ptr == NULL) continue;

			type_rec.type = i;

			while (*str_ptr == ' ' || *str_ptr == '\t' || *str_ptr == ',') str_ptr++;

			i = (int) (strlen (str_ptr)) + 1;

			type_rec.label = new char [i];

			if (type_rec.label == NULL) goto mem_error;

			str_cpy (type_rec.label, i, str_ptr);

			if (!road_type.Add (&type_rec)) goto mem_error;
		}
		label_file.Close ();
	}

	//---- area type map ----
	
	str_ptr = Get_Control_String (AREA_TYPE_MAP);

	if (str_ptr != NULL) {
		at_map_flag = true;
		at_map_file.File_Type ("Area Type Map");

		Print (1);
		if (!at_map_file.Open (Project_Filename (str_ptr))) {
			Error ("Opening %s", at_map_file.File_Type ());
		}
	}

	//---- facility type map ----
	
	str_ptr = Get_Control_String (FACILITY_TYPE_MAP);

	if (str_ptr != NULL) {
		fac_map_flag = true;
		fac_map_file.File_Type ("Facility Type Map");

		Print (1);
		if (!fac_map_file.Open (Project_Filename (str_ptr))) {
			Error ("Opening %s", fac_map_file.File_Type ());
		}
	}

	//---- vehicle type map ----

	str_ptr = Get_Control_String (VEHICLE_TYPE_MAP);

	if (str_ptr != NULL) {
		veh_map_flag = true;
		veh_map_file.File_Type ("Vehicle Type Map");

		Print (1);
		if (!veh_map_file.Open (Project_Filename (str_ptr))) {
			Error ("Opening %s", veh_map_file.File_Type ());
		}
	}

	//---- road type map ----
	
	str_ptr = Get_Control_String (ROAD_TYPE_MAP);

	if (str_ptr != NULL) {
		road_map_flag = true;
		road_map_file.File_Type ("Road Type Map");

		Print (1);
		if (!road_map_file.Open (Project_Filename (str_ptr))) {
			Error ("Opening %s", road_map_file.File_Type ());
		}
	}

	//---- hpms type map ----
	
	str_ptr = Get_Control_String (HPMS_TYPE_MAP);

	if (str_ptr != NULL) {
		hpms_map_file.File_Type ("HPMS Type Map");

		Print (1);
		if (!hpms_map_file.Open (Project_Filename (str_ptr))) {
			Error ("Opening %s", hpms_map_file.File_Type ());
		}
	} else if (hpms_flag) {
		Error ("HPMS Type Map is Required for HPMS Output");
	}

	//---- open the emission rate file ----

	str_ptr = Get_Control_String (NEW_EMISSION_RATE_FILE);

	if (str_ptr != NULL) {
		if (!rate_flag) goto sum_error;

		new_rate_file.File_Type ("Emission Rate File");
		new_rate_flag = true;

		Print (1);
		format_ptr = Get_Control_String (NEW_EMISSION_RATE_FORMAT);

		if (format_ptr != NULL) {
			new_rate_file.Dbase_Format (format_ptr);
		}
		if (!new_rate_file.Create (Project_Filename (str_ptr))) {
			File_Error ("Creating Emission Rate File", output_file.Filename ());
		}
	}

	//---- open the emission summary file ----

	str_ptr = Get_Control_String (NEW_EMISSION_SUMMARY_FILE);

	if (str_ptr != NULL) {
		if (!rate_flag) goto sum_error;

		output_file.File_Type ("Emission Summary File");
		output_flag = true;

		Print (1);
		format_ptr = Get_Control_String (NEW_EMISSION_SUMMARY_FORMAT);

		if (format_ptr != NULL) {
			output_file.Dbase_Format (format_ptr);
		}
		if (!output_file.Create (Project_Filename (str_ptr))) {
			File_Error ("Creating Emission Summary File", output_file.Filename ());
		}
	}

	//---- open the speed summary file ----

	if (bin_sum_flag) {
		str_ptr = Get_Control_String (NEW_SPEED_SUMMARY_FILE);

		bin_sum_file.File_Type ("Speed Summary File");

		Print (1);
		format_ptr = Get_Control_String (NEW_SPEED_SUMMARY_FORMAT);

		if (format_ptr != NULL) {
			bin_sum_file.Dbase_Format (format_ptr);
		}
		if (!bin_sum_file.Create (Project_Filename (str_ptr))) {
			File_Error ("Creating Speed Summary File", bin_sum_file.Filename ());
		}
	}

	//---- open the hpms vmt file ----

	if (hpms_flag) {
		str_ptr = Get_Control_String (NEW_MOVES_HPMS_VMT_FILE);

		hpms_file.File_Type ("MOVES HPMS VMT File");

		Print (1);
		format_ptr = Get_Control_String (NEW_MOVES_HPMS_VMT_FORMAT);

		if (format_ptr != NULL) {
			hpms_file.Dbase_Format (format_ptr);
		}
		if (!hpms_file.Create (Project_Filename (str_ptr))) {
			File_Error ("Creating HPMS VMT File", hpms_file.Filename ());
		}
	}

	//---- open the ramp fraction file ----

	if (ramp_flag) {
		str_ptr = Get_Control_String (NEW_MOVES_RAMP_FRACTION_FILE);

		ramp_file.File_Type ("MOVES Ramp Fraction File");

		Print (1);
		format_ptr = Get_Control_String (NEW_MOVES_RAMP_FRACTION_FORMAT);

		if (format_ptr != NULL) {
			ramp_file.Dbase_Format (format_ptr);
		}
		if (!ramp_file.Create (Project_Filename (str_ptr))) {
			File_Error ("Creating Ramp Fraction File", ramp_file.Filename ());
		}
	}

	//---- open the speed hour file ----

	if (spd_flag) {
		str_ptr = Get_Control_String (NEW_MOVES_SPEED_HOUR_FILE);

		spd_file.File_Type ("MOVES Speed Hour File");

		Print (1);
		format_ptr = Get_Control_String (NEW_MOVES_SPEED_HOUR_FORMAT);

		if (format_ptr != NULL) {
			spd_file.Dbase_Format (format_ptr);
		}
		if (!spd_file.Create (Project_Filename (str_ptr))) {
			File_Error ("Creating Speed Hour File", spd_file.Filename ());
		}
	}

	//---- open the vmt hour file ----

	if (vmt_flag) {
		str_ptr = Get_Control_String (NEW_MOVES_VMT_HOUR_FILE);

		vmt_file.File_Type ("MOVES VMT Hour File");

		Print (1);
		format_ptr = Get_Control_String (NEW_MOVES_VMT_HOUR_FORMAT);

		if (format_ptr != NULL) {
			vmt_file.Dbase_Format (format_ptr);
		}
		if (!vmt_file.Create (Project_Filename (str_ptr))) {
			File_Error ("Creating VMT Hour File", vmt_file.Filename ());
		}
	}

	//---- open the vmt road file ----

	if (road_flag) {
		str_ptr = Get_Control_String (NEW_MOVES_VMT_ROAD_FILE);

		road_file.File_Type ("MOVES VMT Road Type File");

		Print (1);
		format_ptr = Get_Control_String (NEW_MOVES_VMT_ROAD_FORMAT);

		if (format_ptr != NULL) {
			road_file.Dbase_Format (format_ptr);
		}
		if (!road_file.Create (Project_Filename (str_ptr))) {
			File_Error ("Creating VMT Road Type File", road_file.Filename ());
		}
	}

	//---- MOVES project level files ----

	//---- open the link summary file ----

	if (link_sum_flag) {
		str_ptr = Get_Control_String (NEW_MOVES_LINK_SUMMARY_FILE);

		link_sum_file.File_Type ("MOVES Link Summary File");

		Print (1);
		format_ptr = Get_Control_String (NEW_MOVES_LINK_SUMMARY_FORMAT);

		if (format_ptr != NULL) {
			link_sum_file.Dbase_Format (format_ptr);
		}
		if (!link_sum_file.Create (Project_Filename (str_ptr))) {
			File_Error ("Creating Link Summary File", link_sum_file.Filename ());
		}
	}

	//---- open the link vehicle file ----

	if (link_veh_flag) {
		str_ptr = Get_Control_String (NEW_MOVES_LINK_VEHICLE_FILE);

		link_veh_file.File_Type ("MOVES Link Vehicle File");

		Print (1);
		format_ptr = Get_Control_String (NEW_MOVES_LINK_VEHICLE_FORMAT);

		if (format_ptr != NULL) {
			link_veh_file.Dbase_Format (format_ptr);
		}
		if (!link_veh_file.Create (Project_Filename (str_ptr))) {
			File_Error ("Creating Link Speed File", link_veh_file.Filename ());
		}
	}

	//---- open the link speed file ----

	if (link_spd_flag) {
		str_ptr = Get_Control_String (NEW_MOVES_LINK_SPEED_FILE);

		link_spd_file.File_Type ("MOVES Link Speed File");

		Print (1);
		format_ptr = Get_Control_String (NEW_MOVES_LINK_SPEED_FORMAT);

		if (format_ptr != NULL) {
			link_spd_file.Dbase_Format (format_ptr);
		}
		if (!link_spd_file.Create (Project_Filename (str_ptr))) {
			File_Error ("Creating Link Speed File", link_spd_file.Filename ());
		}

		//---- check the speed flag ----

		str_ptr = Get_Control_String (USE_AVERAGE_SEGMENT_SPEEDS);

		if (str_ptr != NULL) {
			Print (1, "Use Average Segment Speeds = %s", str_ptr);

			speed_flag = Get_Control_Flag (USE_AVERAGE_SEGMENT_SPEEDS);
		}
	}

	//---- open the link operations file ----

	if (link_ops_flag) {
		str_ptr = Get_Control_String (NEW_MOVES_LINK_OPERATIONS_FILE);

		link_ops_file.File_Type ("MOVES Link Operations File");

		Print (1);
		format_ptr = Get_Control_String (NEW_MOVES_LINK_OPERATIONS_FORMAT);

		if (format_ptr != NULL) {
			link_ops_file.Dbase_Format (format_ptr);
		}
		if (!link_ops_file.Create (Project_Filename (str_ptr))) {
			File_Error ("Creating Link Operations File", link_ops_file.Filename ());
		}
	}

	//---- read the number smooth records ----

	if (vol_spd_flag) {
		moves_bin_flag = true;
	} else {
		moves_bin_flag = Get_Control_Flag (SMOOTH_TO_MOVES_SPEED_BINS);

		str_ptr = Get_Control_String (SMOOTH_GROUP_SIZE);

		if (str_ptr != NULL) {
			Get_Integer (str_ptr, &i);

			if ((moves_bin_flag && i < 3) || (!moves_bin_flag && i != 0 && i < 3)) {
				Error ("Smooth Group Size %d is Out of Range (>= 3)", i);
			}
			naverage = i;
		} else if (!moves_bin_flag) {
			naverage = 0;
		}

		if (naverage == 0) {
			Print (2, "Speed Bin Smoothing is Disabled");
		} else {
			smooth_flag = true;

			str_ptr = Get_Control_String (SMOOTH_TO_MOVES_SPEED_BINS);

			if (str_ptr != NULL) {
				Print (2, "Smooth to MOVES Speed Bins = %s", str_ptr);
			} else {
				Print (1);
			}
			Print (1, "Smooth Group Size = %d", naverage);

			if (!moves_bin_flag && !traveler_flag) {
				str_ptr = Get_Control_String (SMOOTH_SPEED_INCREMENT);

				if (str_ptr != NULL) {
					Get_Double (str_ptr, &bin_size);

					if (bin_size < 0.1 || bin_size > 10.0) {
						Error ("Smooth Speed Increments %.2lf is Out of Range (0.1..10.0)", bin_size);
					}
				}
				Print (1, "Smooth Speed Increment = %.2lf meters per second", bin_size);
			} else {
				bin_size = 2.5 * MPHTOMPS;
			}

			//---- read the percent distributed forward ----

			str_ptr = Get_Control_String (PERCENT_MOVED_FORWARD);

			if (str_ptr != NULL) {
				Get_Double (str_ptr, &forward);
			}
			Print (1, "Percent Distributed Forward = %.1lf percent", forward);

			//---- read the percent distributed backwarde ----
			
			str_ptr = Get_Control_String (PERCENT_MOVED_BACKWARD);

			if (str_ptr != NULL) {
				Get_Double (str_ptr, &backward);
			}
			Print (1, "Percent Distributed Backward = %.1lf percent", backward);

			//---- number of iterations ----

			str_ptr = Get_Control_String (NUMBER_OF_ITERATIONS);

			if (str_ptr != NULL) {
				Get_Integer (str_ptr, &niter);
			}
			Print (1, "Number of Iterations = %d", niter);

			//---- set the smoothing parameters ----

			if (traveler_flag) {
				if (!smooth_data.Setup (1000, niter, false, naverage, forward, backward)) {
					Error ("Smooth Parameters are Illogical");
				}
			} else {
				high = UnRound (cell_size) / bin_size;

				if (!smooth_data.Interpolate (num_bins, high, niter, naverage, forward, backward)) {
					Error ("Smooth Parameters are Illogical");
				}
			}
		}
	}
	bin_ptr = category_data.Get (SPEED_BIN);
	max_bin = bin_ptr->Max_Member ();
	if (max_bin == 0 && moves_bin_flag) max_bin = 16;

	if (smooth_flag) {

		//---- open the smooth sample file ----

		str_ptr = Get_Control_String (NEW_SMOOTH_SAMPLE_FILE);

		if (str_ptr != NULL) {
			sample_file.File_Type ("Smooth Sample File");

			Print (1);
			format_ptr = Get_Control_String (NEW_SMOOTH_SAMPLE_FORMAT);

			if (format_ptr != NULL) {
				sample_file.Dbase_Format (format_ptr);
			}
			if (!sample_file.Create (Project_Filename (str_ptr))) {
				File_Error ("Creating Smooth Sample File", sample_file.Filename ());
			}
			sample_flag = true;

			//---- create the bin fields ----

			for (i=0; i < num_bins; i++) {
				str_fmt (buffer, sizeof (buffer), "IN_BIN%d", i);
				sample_file.Add_Field (buffer, DOUBLE, 14, 2);
			}
			for (i=1; i <= max_bin; i++) {
				str_fmt (buffer, sizeof (buffer), "OUT_BIN%d", i);
				sample_file.Add_Field (buffer, DOUBLE, 14, 2);
			}
			sample_file.Write_Header ();

			//---- get the percent sample size ----

			str_ptr = Get_Control_String (PERCENT_SAMPLE_SIZE);

			if (str_ptr != NULL) {
				sample_size = atof (str_ptr);

				if (sample_size < 0.0001 || sample_size > 100.0) {
					Error ("Percent Sample Size %s is Out of Range (0.0001..100.0)", str_ptr);
				}
				sample_size /= 100.0;
			}
			Print (1, "Percent Sample Size = %.2lf percent", sample_size * 100.0);

			//---- random number seed ----

			str_ptr = Get_Control_String (RANDOM_NUMBER_SEED);

			if (str_ptr != NULL) {
				random.Seed (atoi (str_ptr));
			}
			Print (1, "Random Number Seed = %d", random.Seed ());
		}
	}

	if (traveler_flag) {

		//---- open the new traveler file ----

		str_ptr = Get_Control_String (NEW_TRAVELER_FILE);

		if (str_ptr != NULL) {
			Print (1);
			format_ptr = Get_Control_String (NEW_TRAVELER_FORMAT);

			if (format_ptr != NULL) {
				new_traveler_file.Dbase_Format (format_ptr);
			}
			if (!new_traveler_file.Create (Project_Filename (str_ptr))) {
				Error ("Creating %s", new_traveler_file.File_Type ());
			}
			new_traveler_flag = true;
		}
	}

	//---- check the report types ----

	if (Report_Flag (AT_REPORT) || Report_Flag (FT_REPORT) || Report_Flag (VT_REPORT) ||
		Report_Flag (RT_REPORT) || Report_Flag (AT_FT_REPORT) || Report_Flag (AT_VT_REPORT) ||
		Report_Flag (FT_VT_REPORT) || Report_Flag (RT_VT_REPORT) || Report_Flag (SUMMARY_REPORT)) {

		//---- get the travel units ----

		str_ptr = Get_Control_String (REPORT_TRAVEL_UNITS);

		if (str_ptr != NULL) {
			for (i=0; len_units [i] != NULL; i++) {
				if (!str_cmp (str_ptr, len_units [i])) {
					travel_units = i;
					break;
				}
			}
			if (len_units [i] == NULL) {
				Error ("Report Travel Units %s were Not Recognized", str_ptr);
			}
			Print (2, "Report Travel Units = %s", len_units [length_units]);
		}

		//---- get the emissions units ----

		str_ptr = Get_Control_String (REPORT_EMISSIONS_UNITS);

		if (str_ptr != NULL) {
			for (i=0; ems_units [i] != NULL; i++) {
				if (!str_cmp (str_ptr, ems_units [i])) {
					emission_units = i;
					break;
				}
			}
			if (ems_units [i] == NULL) {
				Error ("Report Emissions Units %s were Not Recognized", str_ptr);
			}
			Print (1, "Report Emissions Units = %s", ems_units [emission_units]);
		}
	}

	//---- write the report names ----

	List_Reports ();

	if (Report_Flag (AT_REPORT) || Report_Flag (AT_FT_REPORT) || 
		Report_Flag (AT_VT_REPORT) || Report_Flag (SUMMARY_REPORT)) {

		if (!vol_spd_flag && !zone_flag && (!location_flag || !at_equiv_flag)) {
			if (location_flag) {
				Error ("A Zone Equivalance File is Required for Area Type Reports");
			} else {
				Error ("A Zone or Location File is Required for Area Type Reports");
			}
		}
	}
	if (!rate_flag && (Report_Flag (AT_REPORT) || Report_Flag (FT_REPORT) ||
		Report_Flag (VT_REPORT) || Report_Flag (RT_REPORT) || Report_Flag (AT_FT_REPORT) || 
		Report_Flag (AT_VT_REPORT) || Report_Flag (FT_VT_REPORT) || Report_Flag (RT_VT_REPORT) ||
		Report_Flag (SUMMARY_REPORT))) {
		goto sum_error;
	}
	
	//---- process support data ----

	if (link_eq_flag) {
		link_equiv.Read (Report_Flag (LINK_EQUIV));
	}

	//---- read the zone equiv ----

	if (at_equiv_flag) {
		zone_equiv.Read (Report_Flag (ZONE_EQUIV));
	}

	//---- read the region equiv ----

	if (reg_equiv_flag) {
		region_equiv.Read (Report_Flag (REGION_EQUIV));
	}

	if (Warnings () > 0) Show_Message (1);
	return;

	//---- error message ----

control_error:
	Error ("Missing Control Key = %s", Current_Key ());

mem_error:
	Error ("Insufficient Memory for Control Data");

field_error:
	Error ("Field %s was Not Found", str_ptr);

sum_error:
	Error ("Emission Rates are needed to Summarize Emissions");
} 
