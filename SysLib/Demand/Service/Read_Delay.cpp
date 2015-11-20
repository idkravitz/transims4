//*********************************************************
//	Read_Delay.cpp - read the link delay file
//*********************************************************

#include "Demand_Service.hpp"
#include "Delay_File.hpp"
#include "Time_Step.hpp"

//---------------------------------------------------------
//	Read_Delay
//---------------------------------------------------------

void Demand_Service::Read_Delay (void)
{
	int i, num, nperiod, period, link, dir, volume, time, vol;
	bool flag, connect_flag;

	TTime_Data *ttime_ptr;
	Connect_Time *connect_ptr;

	Delay_File *file = (Delay_File *) Demand_Db_File (LINK_DELAY);

	//---- check for required data ----

	if (!Network_Data_Flag (LINK)) {
		Error (file_error, "Link", file->File_ID ());
	}
	connect_flag = (file->Dbase_Format () != VERSION3 && Network_Data_Flag (LANE_CONNECTIVITY));

	//connect_flag = false;

	//if (file->Dbase_Format () != VERSION3) {
	//	if (!Network_Data_Flag (LANE_CONNECTIVITY)) {
	//		Write (1, "Warning: Lane Connectivity is needed for Turn Delay processing");
	//		Show_Message (1);
	//	} else {
	//		connect_flag = true;
	//	}
	//}

	//---- store the link delay data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress (10000);

	First_Record (true);

	while (file->Read (false)) {
		Show_Progress ();

		flag = Delay_Processing (file);

		First_Record (false);

		num = file->Num_Connect ();

		for (i=1; i <= num; i++) {
			if (!file->Read (true)) {
				Error ("Number of Nested Records for Link %d", file->Link ());
			}
			Show_Progress ();

			if (flag) {
				Delay_Processing (file);
			}
		}
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d, Periods = %d", file->File_Type (), Progress_Count (), ttime_data.Periods ());

	if (Progress_Count () > 0) Demand_Data_True (LINK_DELAY);

	//---- calculate the travel times ----

	link = dir = volume = period  = 0;

	for (ttime_ptr = ttime_data.First (); ttime_ptr; ttime_ptr = ttime_data.Next ()) {

		//---- process each time period ----

		flag = false;
		nperiod = ttime_ptr->Periods ();

		for (i=1; i <= nperiod; i++) {
			vol = ttime_ptr->Volume (i);
            if (vol > 0) {
				time = ttime_ptr->TTime (i);

				time = (time + (vol >> 1)) / vol;
				if (time < 1) time = 1;

				ttime_ptr->TTime (i, time);

				volume++;
				flag = true;
			} else if (ttime_ptr->TTime (i) < (ttime_ptr->Time0 () >> 1)) {
				ttime_ptr->TTime (i, ttime_ptr->Time0 ());
			}
			period++;
		}
		if (flag) dir++;
		link++;
	}
	Print (1, "Percent of Link Directions with Travel Time Data = %.1lf%%", 
				((link) ? dir * 100.0 / link : 0.0));
	Print (1, "Percent of Link Time Periods with Travel Time Data = %.1lf%%", 
				((period) ? volume * 100.0 / period : 0.0));

	//---- adjust connection times ----

	if (connect_flag) {
		link = dir = volume = period = 0;

		for (connect_ptr = connect_time.First (); connect_ptr; connect_ptr = connect_time.Next ()) {
			flag = false;
			nperiod = connect_ptr->Periods ();

			ttime_ptr = ttime_data.Get (connect_ptr->In_Link_Dir ());
			if (ttime_ptr == NULL) {
				Error ("Link Connection");
			}

			for (i=1; i <= nperiod; i++) {
				vol = connect_ptr->Volume (i);
				if (vol > 0) {
					time = connect_ptr->TTime (i);
					if (time > 0) {
						time = (time + (vol >> 1)) / vol;
						if (time < 1) time = 1;

						time -= ttime_ptr->TTime (i);

						connect_ptr->TTime (i, time);

						volume++;
						flag = true;
					}
				}
				period++;
			}
			if (flag) dir++;
			link++;
		}
		Print (1, "Percent of Link Connections with Travel Time Data = %.1lf%%", 
					((link) ? dir * 100.0 / link : 0.0));
		Print (1, "Percent of Connection Periods with Travel Time Data = %.1lf%%", 
					((period) ? volume * 100.0 / period : 0.0));
	}
	Flush ();
}

//---------------------------------------------------------
//	Delay_Processing
//---------------------------------------------------------

bool Demand_Service::Delay_Processing (Db_File *fh)
{
	int start, end, link, dir, volume, seconds, out_link_dir;
	double ttim, max_time;

	static int nconnect, connect_num, in_link_dir, period, nerror; 
	static Time_Step trip_time (1, "SECONDS");

	Link_Data *link_ptr;
	TTime_Data *ttime_ptr;
	Connect_Time *connect_ptr;

	Delay_File *file = (Delay_File *) fh;

	if (First_Record ()) nerror = 0;

	//---- process a header line ----

	if (!file->Nest ()) {
		in_link_dir = 0;

		//---- get the time period and travel time ----

		ttim = file->TTime ();
		volume = file->Volume ();

		if (file->Dbase_Format () == VERSION3) {
			end = file->Time ();
			if (end == 0) return (false);
		} else {
			start = trip_time.Step (file->Start_Time ());
			end = trip_time.Step (file->End_Time ());
			if (end == 0) return (false);

			if (ttime_data.Period_Size () == 0) {
				if (ttime_data.Period_Size (end - start, ttime_data.Min_Size (), ttime_data.Max_Size ())) {
					if (((start - ttime_data.Min_Size ()) % ttime_data.Period_Size ()) != 0) {
						Warning ("Link Delay Period Size %d and Start Time %d do not Align", end - start, start);
					}
				}
			}
			connect_num = 1;
			nconnect = file->Num_Connect ();

			if (volume > 0) {
				ttim *= volume;
			}
		}
		period = Time_Period (end - 1);		//---- subtract 1 to adjust end time ---

		if (period == 0) return (false);

		if (volume > 0) {
			max_time = volume * MIDNIGHT;
		} else {
			max_time = MIDNIGHT;
		}
		if (ttim < 0.0 || ttim > max_time) {
			ttim = max_time;
		}		
		seconds = Round (ttim);
		if (seconds == 0) return (false);

		//---- get the link record ----

		link = file->Link ();
		dir = file->Dir ();

		if (file->LinkDir_Type () == LINK_SIGN) {
			if (link < 0) {
				link = -link;
				dir = 1;
			}
		}
		link_ptr = link_data.Get (link);
		if (link_ptr == NULL) goto error_check;

		//---- get the link direction ----

		if (file->LinkDir_Type () == LINK_NODE) {
			if (Renumber ()) {
				dir = node_data.Get_Index (dir);
				if (dir == 0) goto error_check;
			}

			//---- from node!!! ----

			if (dir == link_ptr->Anode ()) {
				dir = 0;
			} else if (dir == link_ptr->Bnode ()) {
				dir = 1;
			} else {
				goto error_check;
			}
		}
		if (Renumber ()) link = link_data.Record_Index ();

		if (dir == 0) {
			dir = link_ptr->AB_Dir ();
		} else {
			dir = link_ptr->BA_Dir ();
		}
		if (dir == 0) goto error_check;

		ttime_ptr = ttime_data [dir];
		if (ttime_ptr == NULL) goto error_check;

		//---- store the data ----

		in_link_dir = ttime_ptr->Link_Dir ();

		if (ttime_ptr->Periods () == 0) {
			if (!ttime_ptr->Periods (ttime_data.Periods (), ttime_data.Layers ())) {
				Error ("Insufficient Memory for Link Delay Data");
			}
		}
		if (ttime_ptr->Volume (period) == 0 && ttime_ptr->TTime (period) == ttime_ptr->Time0 ()) {
			ttime_ptr->TTime (period, seconds);
			ttime_ptr->Volume (period, volume);
		} else {
			ttime_ptr->Add_TTime (period, seconds);
			ttime_ptr->Add_Volume (period, volume);
		}

	} else if (Network_Data_Flag (LANE_CONNECTIVITY)) {

		//---- process a nested record ----

		if (connect_num++ > nconnect) return (false);
		if (in_link_dir == 0) return (false);

		ttim = file->Out_Time ();
		volume = file->Out_Turn ();
		if (volume <= 0) volume = 1;

		ttim *= volume;
		max_time = volume * MIDNIGHT;

		if (ttim < 0.0 || ttim > max_time) {
			ttim = max_time;
		}
		seconds = Round (ttim);
		if (seconds == 0) return (false);

		Dir_Data dir_rec;

		//---- process a connection record ----

		link = file->Out_Link ();
		dir = file->Out_Dir ();

		if (file->LinkDir_Type () == LINK_SIGN) {
			if (link < 0) {
				link = -link;
				dir = 1;
			}
		}
		link_ptr = link_data.Get (link);
		if (link_ptr == NULL) goto error_check;

		if (file->LinkDir_Type () == LINK_NODE) {
			if (Renumber ()) {
				dir = node_data.Get_Index (dir);
				if (dir == 0) goto error_check;
			}

			//---- from node!!!! ----

			if (dir == link_ptr->Anode ()) {
				dir = 0;
			} else if (dir == link_ptr->Bnode ()) {
				dir = 1;
			} else {
				goto error_check;
			}
		}
		if (Renumber ()) link = link_data.Record_Index ();

		dir_rec.Link (link);
		dir_rec.Dir (dir);

		out_link_dir = dir_rec.Link_Dir ();

		connect_ptr = connect_time.Get (in_link_dir, out_link_dir);
		if (connect_ptr == NULL) goto error_check;

		if (connect_time.Period_Size () == 0) {
			connect_time.Period_Size (ttime_data.Period_Size (), ttime_data.Min_Size (), ttime_data.Max_Size ());
		}
		if (connect_ptr->Periods () == 0) {
			if (!connect_ptr->Periods (connect_time.Periods (), connect_time.Layers ())) {
				Error ("Insufficient Memory for Connection Delay Data");
			}
			if (dir == 0) {
				connect_ptr->Dir_Index (link_ptr->AB_Dir ());
			} else {
				connect_ptr->Dir_Index (link_ptr->BA_Dir ());
			}
		}
		if (connect_ptr->TTime (period) == 0) {
			connect_ptr->TTime (period, seconds);
			connect_ptr->Volume (period, volume);
		} else {
			connect_ptr->Add_TTime (period, seconds);
			connect_ptr->Add_Volume (period, volume);
		}
	}
	return (true);

error_check:
	if (++nerror > Max_Delay_Errors ()) {
		Error ("Number of Link Delay Errors Exceeded (%d > %d)", nerror, Max_Delay_Errors ());
	}
	return (false);
}
