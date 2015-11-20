//*********************************************************
//	Signal_Setup.cpp - set signal green phase movements
//*********************************************************

#include "Progression.hpp"

#define MAX_PHASE		24

//---------------------------------------------------------
//	Signal_Setup
//---------------------------------------------------------

void Progression::Signal_Setup (void)
{
	int p, phase, nphase, r, nring, cycle, sum;
	int green [MAX_PHASE], other [MAX_PHASE], ring [MAX_PHASE], next [MAX_PHASE];

	Signal_Data *signal_ptr;
	Timing_Data *timing_ptr, timing_rec;
	Green_Phase *green_ptr, green_rec;
	Signal_Offset *offset_ptr;

	//---- process the cycle and phase time for each signal ----

	Show_Message ("Calculating Phase Offsets -- Record");
	Set_Progress (100);

	for (offset_ptr = signal_offset.First (); offset_ptr; offset_ptr = signal_offset.Next ()) {
		Show_Progress ();
		signal_ptr = signal_data [offset_ptr->Signal ()];

		memset (green, '\0', sizeof (green));
		memset (other, '\0', sizeof (other));
		memset (ring, '\0', sizeof (ring));
		memset (next, '\0', sizeof (next));
		nphase = nring = 0;

		//---- collect the phase data ----

		timing_rec.Timing (signal_ptr->Timing ());
		timing_rec.Phase (1);

		for (timing_ptr = timing_data.Get_GE (&timing_rec); timing_ptr; timing_ptr = timing_data.Next_Key ()) {

			if (timing_ptr->Timing () != signal_ptr->Timing ()) break;

			phase = timing_ptr->Phase ();
			if (phase < 1 || phase >= MAX_PHASE) continue;

			green [phase] = MAX (timing_ptr->Min_Green (), timing_ptr->Max_Green ());
			other [phase] = timing_ptr->Yellow () + timing_ptr->Red_Clear ();
			ring [phase] = r = timing_ptr->Ring ();
			next [phase] = timing_ptr->Next_Phase ();
			if (phase > nphase) nphase = phase;
			if (r > nring) nring = r;
		}

		//---- calculate the cycle length for each ring ----

		offset_ptr->Cycle (0);

		for (r=1; r <= nring; r++) {
			for (phase=1; phase <= nphase; phase++) {
				if (ring [phase] == r) break;
			}
			if (phase > nphase) continue;

			cycle = green [phase] + other [phase];
			other [phase] = 0;

			for (p = next [phase]; p != phase; p = next [p]) {
				sum = green [p] + other [p];
				other [p] = cycle;
				cycle += sum;
			}
			if (cycle > offset_ptr->Cycle ()) offset_ptr->Cycle (cycle);
		}

		//---- apply the phase offset to each movement ----

		for (green_ptr = offset_ptr->First_Phase (); green_ptr; green_ptr = offset_ptr->Next_Phase ()) {
			phase = green_ptr->Protected ();

			if (next [phase] == green_ptr->Permitted ()) {
				green_ptr->Green_Time (green [phase] + green [next [phase]]);
			} else {
				green_ptr->Green_Time (green [phase]);
			}
			green_ptr->Phase_Offset (other [phase]);
		}
	}
	End_Progress ();
}
