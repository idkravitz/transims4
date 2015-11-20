//*********************************************************
//	Write_Signal.cpp - convert the signal data
//*********************************************************

#include "VISSIMNet.hpp"

//---------------------------------------------------------
//	Write_Signal
//---------------------------------------------------------

void VISSIMNet::Write_Signal (void)
{
	int cycle;
	char buffer [FIELD_BUFFER], time1 [20], time2 [20], *control_type, *flag_type;

	Signal_Data *signal_ptr;
	Timing_Data *timing_ptr, timing_rec;
	
	fprintf (file, "\n<SIGNALCONTROLS>");

	for (signal_ptr = signal_data.First_Key (); signal_ptr; signal_ptr = signal_data.Next_Key ()) {

		timing_rec.Timing (signal_ptr->Timing ());
		timing_rec.Phase (1);

		cycle = 0;

		for (timing_ptr = timing_data.Get_GE (&timing_rec); timing_ptr; timing_ptr = timing_data.Next_Key ()) {
			if (timing_ptr->Timing () != signal_ptr->Timing ()) break;

			cycle += timing_ptr->Min_Green () + timing_ptr->Yellow () + timing_ptr->Red_Clear ();
		}

		buffer [0] = ' ';
		buffer [1] = '\0';
		str_fmt (time1, sizeof (time1), "PT%dS", cycle);
		str_fmt (time2, sizeof (time2), "PT%dS", signal_ptr->Offset ());

		//if (signal_ptr->Type () == ACTUATED) {
		//	control_type = "Nema";
		//	flag_type = "false";
		//} else {
			control_type = "FixedTime";
			flag_type = "true";
		//}

		fprintf (file, "\n<SIGNALCONTROL NO=\"%d\" NAME=\"%s\" CYCLETIME=\"%s\" TIMEOFFSET=\"%s\" SIGNALIZATIONTYPE=\"%s\" CYCLETIMEFIXED=\"%s\" >",
			signal_ptr->Timing (), buffer, time1, time2, control_type, flag_type);

		fprintf (file, "\n<SIGNALGROUPS>");

		cycle = 0;

		for (timing_ptr = timing_data.Get_GE (&timing_rec); timing_ptr; timing_ptr = timing_data.Next_Key ()) {
			if (timing_ptr->Timing () != signal_ptr->Timing ()) break;

			str_fmt (time1, sizeof (time1), "PT%dS", cycle);

			cycle += timing_ptr->Min_Green () + timing_ptr->Yellow () + timing_ptr->Red_Clear ();

			str_fmt (time2, sizeof (time2), "PT%dS", cycle);

			control_type = "Cycle";

			fprintf (file, "\n<SIGNALGROUP NO=\"%d\" NAME=\"\" GTSTART=\"%s\" GTEND=\"%s\" SIGNALGROUPTYPE=\"%s\" />",
				timing_ptr->Phase (), time1, time2, control_type);
		}

		fprintf (file, "\n</SIGNALGROUPS>");
		fprintf (file, "\n</SIGNALCONTROL>");
	}
	fprintf (file, "\n</SIGNALCONTROLS>");
}
