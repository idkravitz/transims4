//*********************************************************
//	Link_Delay.cpp - Create the Link Delay File
//*********************************************************

#include "PlanSum.hpp"

#include "Delay_File.hpp"

//---------------------------------------------------------
//	Link_Delay
//---------------------------------------------------------

void PlanSum::Link_Delay (void)
{
	int i, j, k, dir, nperiods, volume, nout, type, time0, cap, tod_cap, lanes, tod, list, len; 
	int num, index [20], duration;
	double ttime, length, speed, factor;
	
	Link_Data *link_ptr;
	TTime_Data *ttime_ptr;
	Link_Use_Data *use_ptr;
	Range_Data *range_ptr;
	Connect_Time *connect_ptr;

	Delay_File *delay_file = (Delay_File *) Demand_Db_Base (NEW_LINK_DELAY);

	Show_Message ("Writing %s -- Link", delay_file->File_Type ());
	Set_Progress (1000);
	nout = 0;

	//---- default lane and turn information ----

	delay_file->Lane (1);
	delay_file->Turn (0);

	for (link_ptr = link_data.First_Key (); link_ptr != NULL; link_ptr = link_data.Next_Key ()) {
		Show_Progress ();

		delay_file->Link (link_ptr->Link ());

		len = link_ptr->Length ();
		length = UnRound (len);
		type = link_ptr->Type ();

		//---- process each direction ----

		for (i=0; i < 2; i++) {
			dir = (i) ? link_ptr->BA_Dir () : link_ptr->AB_Dir ();
			if (dir == 0) continue;

			ttime_ptr = ttime_data [dir];

			nperiods = ttime_ptr->Periods ();
			if (nperiods == 0) continue;

			cap = (int) (ttime_ptr->Capacity () * cap_factor + 0.5);
			if (cap <= 0) continue;

			time0 = ttime_ptr->Time0 ();
			lanes = ttime_ptr->Thru ();
			if (lanes < 1) lanes = 1;

			list = ttime_ptr->TOD_List ();

			//---- save the link direction data ----

			if (delay_file->LinkDir_Type () == Db_Code::LINK_SIGN) {
				if (i) {
					delay_file->Link (-link_ptr->Link ());
				} else {
					delay_file->Link (link_ptr->Link ());
				}
			} else {
				if (delay_file->LinkDir_Type () == Db_Code::LINK_NODE) {

					//---- from node!!!! ----

					delay_file->Dir (((i) ? link_ptr->Bnode () : link_ptr->Anode ()));
				} else {
					delay_file->Dir (i);
				}
			}

			//---- process each time period ----

			num = 0;

			for (j=1; j <= num_inc; j++) {

				//---- get the volume ----

				volume = ttime_ptr->Data (j, layer);

				if (volume == 0) continue;

				//---- get the time period ----

				range_ptr = time_periods [j];

				//---- calculate travel time ----

				if (vc_flag) {

					//---- time-of-day capacity ----

					tod_cap = cap;

					if (list > 0) {
						tod = Round ((range_ptr->High () + range_ptr->Low () + 1) / 2);

						for (k = list; k; k = use_ptr->TOD_List ()) {
							use_ptr = link_use_data [k]; 

							if (use_ptr->Start () <= tod && tod < use_ptr->End ()) {
								tod_cap = (tod_cap * use_ptr->Lanes () + lanes / 2) / lanes;
								if (tod_cap == 0) tod_cap = cap / 2;
								break;
							}
						}
					}

					//---- apply volume delay function ----

					ttime = UnRound (equation.Apply_Equation (type, time0, volume, tod_cap, len));

				} else {

					//---- key the input travel time ----

					if (ttime_ptr->Volume (j) == 0) continue;

					ttime = UnRound (ttime_ptr->TTime (j));
				}

				if (ttime > 0.0) {
					speed = length / ttime;
				} else {
					speed = 0.1;
				}

				//---- save the Version 3 fields ----

				if (delay_file->Dbase_Format () == Db_Code::VERSION3) {

					delay_file->Time (range_ptr->High () + 1);
					delay_file->Volume (volume);
					delay_file->TTime (ttime * volume);
					delay_file->TTime2 (ttime * ttime * volume);
					delay_file->Vehicles (volume);
					delay_file->Speed (speed * volume);
					delay_file->Speed2 (speed * speed * volume);

					if (!delay_file->Write ()) goto write_error;
					nout++;

				} else {

					delay_file->Start_Time (time_periods.Format_Step (range_ptr->Low ()));
					delay_file->End_Time (time_periods.Format_Step (range_ptr->High () + 1));

					duration = range_ptr->High () - range_ptr->Low () + 1;
					if (duration > MIDNIGHT * 2) duration = MIDNIGHT;

					delay_file->Volume (volume);
					delay_file->Enter (volume);
					delay_file->Exit (volume);
					delay_file->Speed (speed);
					delay_file->TTime (ttime);

					factor = ttime_ptr->Thru ();
					if (factor < 1) factor = 1;

					factor = 1000.0 / (length * factor);

					factor = volume * factor / duration;
					if (factor > 0.0 && factor < 0.01) factor = 0.01;

					delay_file->Density (factor);
					delay_file->Max_Density (factor);

					factor = UnRound (ttime_ptr->Time0 ());
					if (factor < 0.1) factor = 0.1;

					delay_file->Delay (ttime - factor);
					delay_file->Time_Ratio (ttime / factor);

					delay_file->VMT (length * volume);
					delay_file->VHT (ttime * volume);

					if (!turn_delay) {
						delay_file->Num_Connect (0);
						if (!delay_file->Write (false)) goto write_error;
						nout++;
						continue;
					}

					//---- get the turn connections ----

					if (num == 0) {
						k = ttime_ptr->Link_Dir ();

						for (connect_ptr = connect_time.Get_GE (k, 0); connect_ptr; connect_ptr = connect_time.Next_Key ()) {
							if (connect_ptr->In_Link_Dir () != k || num == 20) break;

							index [num++] = connect_time.Record_Index ();
						}
					}
					if (num <= 1) {
						delay_file->Num_Connect (0);
						if (!delay_file->Write (false)) goto write_error;
						nout++;
						continue;
					}
					delay_file->Num_Connect (num);
					if (!delay_file->Write (false)) goto write_error;
					nout++;

					//---- write the turning movements ----

					for (k=0; k < num; k++) {
						connect_ptr = connect_time [index [k]];

						delay_file->Out_Link (connect_ptr->Out_Link ());
						delay_file->Out_Dir (connect_ptr->Out_Dir ());
						delay_file->Out_Turn (connect_ptr->Data (j, layer));
						delay_file->Out_Time (ttime);

						if (!delay_file->Write (true)) goto write_error;
						nout++;
					}
				}
			}
		}
	}
	End_Progress ();

	delay_file->Close ();

	Print (2, "Number of %s Records = %d", delay_file->File_Type (), nout);
	return;

write_error:
	exe->Error ("Writing Link Delay File");
}

