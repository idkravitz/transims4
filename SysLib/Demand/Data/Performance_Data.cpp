//*********************************************************
//	Performance_Data.cpp - network performance data class
//*********************************************************

#include "Performance_Data.hpp"

//---------------------------------------------------------
//	Performance_Data constructor
//---------------------------------------------------------

Performance_Data::Performance_Data (void) : Dir_Data (0), Lookup_Table (0, 0)
{
	Length (100.0);
}

bool Performance_Data::Periods (int periods, int layers, bool ratio_flag)
{
	return (Size (periods, ((ratio_flag) ? 13 : 10) * layers)); 
}

double Performance_Data::TTime (int period)
{
	double time = Time (period);

	if (time <= 0.0) {
		time = Time0 () / 10.0;
	} else {
		double distance = Distance (period);
		double speed = distance / time;
		if (speed < 0.5) speed = 0.5;
		time = Length () / speed;
	}
	if (time < 0.1) time = 0.1;
	return (time);
}

double Performance_Data::Avg_Speed (int period)
{
	double time = Time (period);

	if (time <= 0.0) {
		double length = Length ();

		time = Time0 () / 10.0;
		if (time < 0.1) time = 0.1;

		return (length / time);
	} else {
		double distance = Distance (period);
		double speed = distance / time;

		if (speed < 0.5) speed = 0.5;
		return (speed);
	}
}

int Performance_Data::Volume (int period)
{
	//double distance = Distance (period);

	//if (distance > 0.0) {
	//	double length = Length ();

	//	if (length > 0.0) {
	//		int volume = (int) (distance / length + 0.5);
	//		if (volume == 0) {
	//			return (1);
	//		} else {
	//			return (volume);
	//		}
	//	} else {
	//		return ((int) (distance + 0.5));
	//	}
	//} else {
	//	return (0);
	//}

	if (Time (period) <= 0.0) {
		return (0);
	} else {
		int volume = (Enter (period) + Exit (period) + 1) / 2;
		if (volume < 1) volume = 1;
		return (volume);
	}
}

double Performance_Data::Delay (int period)
{
	double ttime = TTime (period);
	double time0 = Time0 () / 10.0;

	if (time0 < 0.1) time0 = 0.1;

	return (ttime - time0);
}

double Performance_Data::Time_Ratio (int period)
{
	double ttime = TTime (period);
	double time0 = Time0 () / 10.0;

	if (time0 < 0.1) time0 = 0.1;

	return (ttime / time0);
}

double Performance_Data::TTime2 (int period)
{
	double time = Time2 (period);

	if (time <= 0.0) {
		time = Time0 () / 10.0;
	} else {
		double distance = Distance2 (period);
		double speed = distance / time;
		if (speed < 0.5) speed = 0.5;
		time = Length () / speed;
	}
	if (time < 0.1) time = 0.1;
	return (time);
}

double Performance_Data::Avg_Speed2 (int period)
{
	double time = Time2 (period);

	if (time <= 0.0) {
		double length = Length ();

		time = Time0 () / 10.0;
		if (time < 0.1) time = 0.1;

		return (length / time);
	} else {
		double distance = Distance2 (period);
		double speed = distance / time;

		if (speed < 0.5) speed = 0.5;
		return (speed);
	}
}

int Performance_Data::Volume2 (int period)
{
	//double distance = Distance2 (period);

	//if (distance > 0.0) {
	//	double length = Length ();

	//	if (length > 0.0) {
	//		int volume = (int) (distance / length + 0.5);
	//		if (volume == 0) {
	//			return (1);
	//		} else {
	//			return (volume);
	//		}
	//	} else {
	//		return ((int) (distance + 0.5));
	//	}
	//} else {
	//	return (0);
	//}

	if (Time2 (period) <= 0.0) {
		return (0);
	} else {
		int volume = (Enter2 (period) + Exit2 (period) + 1) / 2;
		if (volume < 1) volume = 1;
		return (volume);
	}
}

double Performance_Data::Delay2 (int period)
{
	double ttime = TTime2 (period);
	double time0 = Time0 () / 10.0;

	if (time0 < 0.1) time0 = 0.1;

	return (ttime - time0);
}

double Performance_Data::Time_Ratio2 (int period)
{
	double ttime = TTime2 (period);
	double time0 = Time0 () / 10.0;

	if (time0 < 0.1) time0 = 0.1;

	return (ttime / time0);
}

double Performance_Data::Average (int period, int column)
{
	int count = Data (period, 10);
	double value = Data (period, column) / 100.0;

	if (count > 1) {
		value = value / count;
		if (value > 0.0 && value < 0.01) value = 0.01;
	}
	return (value);
}

//---------------------------------------------------------
//	Performance_Array constructor
//---------------------------------------------------------

Performance_Array::Performance_Array (int max_records) : 
	Dir_Array (max_records), Lookup_Size (0, 10)
{
	Layers (1);
	Congested_Ratio (0.0);

	Data_Size (sizeof (Performance_Data));
}

void Performance_Array::Clear (void)
{
	for (Performance_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		ptr->Clear ();
	}
	Dir_Array::Clear ();
}
