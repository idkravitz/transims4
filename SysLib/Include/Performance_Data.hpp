//*********************************************************
//	Performance_Data.hpp - network performance data class
//*********************************************************

#ifndef PERFORMANCE_DATA_HPP
#define PERFORMANCE_DATA_HPP

#include "Dir_Data.hpp"

#include "Lookup_Table.hpp"
#include "Time_Range.hpp"

//---------------------------------------------------------
//	Performance_Data class definition
//---------------------------------------------------------

class Performance_Data : public Dir_Data, public Lookup_Table
{
public:
	Performance_Data (void);
	virtual ~Performance_Data (void)                   { Clear (); }

	int    Periods (void)                              { return (Rows ()); }
	bool   Periods (int periods, int layers = 1, bool ratio_flag = false);

	double Length (void)                               { return (length / 10.0); }
	void   Length (double value)                       { length = MAX ((int) (value * 10.0 + 0.5), 1); }

	int    Enter (int period)                          { return (Data (period, 1)); }
	int    Exit (int period)                           { return (Data (period, 2)); }
	double Time (int period)                           { return (Data (period, 3) / 10.0); }
	double Distance (int period)                       { return (Data (period, 4) / 10.0); }
	double Density (int period)                        { return (Average (period, 5)); }
	double Max_Density (int period)                    { return (Data (period, 6) / 100.0); }
	double Queue (int period)                          { return (Average (period, 7)); }
	int    Max_Queue (int period)                      { return (Data (period, 8)); }
	int    Failure (int period)                        { return (Data (period, 9)); }
	int    Count (int period)                          { return (Data (period, 10)); }
	int    Ratio_Count (int period)                    { return (Data (period, 11)); }
	double Ratio_VMT (int period)                      { return (Data (period, 12) / 10.0); }
	double Ratio_VHT (int period)                      { return (Data (period, 13) / 10.0); }              

	int    Volume (int period);
	double TTime (int period);
	double Avg_Speed (int period);
	double Delay (int period);
	double Time_Ratio (int period);

	void   Enter (int period, int value)               { Data (period, 1, value); }
	void   Exit (int period, int value)                { Data (period, 2, value); }
	void   Time (int period, double value)             { Data (period, 3, (int) (value * 10.0 + 0.5)); }
	void   Distance (int period, double value)         { Data (period, 4, (int) (value * 10.0 + 0.5)); }
	void   Density (int period, double value)          { Data (period, 5, (int) (value * 100.0 + 0.5)); }
	void   Max_Density (int period, double value)      { Data (period, 6, (int) (value * 100.0 + 0.5)); }
	void   Queue (int period, double value)            { Data (period, 7, (int) (value * 100.0 + 0.5)); }
	void   Max_Queue (int period, int value)           { Data (period, 8, value); }
	void   Failure (int period, int value)             { Data (period, 9, value); }
	void   Count (int period, int value)               { Data (period, 10, value); }
	void   Ratio_Count (int period, int value)         { Data (period, 11, value); }
	void   Ratio_VMT (int period, double value)        { Data (period, 12, (int) (value * 10.0 + 0.5)); }
	void   Ratio_VHT (int period, double value)        { Data (period, 13, (int) (value * 10.0 + 0.5)); }

	void   Add_Enter (int period, int value)           { Add_Data (period, 1, value); }
	void   Add_Exit (int period, int value)            { Add_Data (period, 2, value); }
	void   Add_Time (int period, double value)         { Add_Data (period, 3, (int) (value * 10.0 + 0.5)); }
	void   Add_Distance (int period, double value)     { Add_Data (period, 4, (int) (value * 10.0 + 0.5)); }
	void   Add_Density (int period, double value)      { Add_Data (period, 5, (int) (value * 100.0 + 0.5)); }
	void   Add_Max_Density (int period, double value)  { Max_Data (period, 6, (int) (value * 100.0 + 0.5)); }
	void   Add_Queue (int period, double value)        { Add_Data (period, 7, (int) (value * 100.0 + 0.5)); }
	void   Add_Max_Queue (int period, int value)       { Max_Data (period, 8, value); }
	void   Add_Failure (int period, int value)         { Add_Data (period, 9, value); }
	void   Add_Count (int period, int value)           { Add_Data (period, 10, value); }
	void   Add_Ratio_Count (int period, int value)     { Add_Data (period, 11, value); }
	void   Add_Ratio_VMT (int period, double value)    { Add_Data (period, 12, (int) (value * 10.0 + 0.5)); }
	void   Add_Ratio_VHT (int period, double value)    { Add_Data (period, 13, (int) (value * 10.0 + 0.5)); }

	int    Enter2 (int period)                         { return (Data (period, C2 (1))); }
	int    Exit2 (int period)                          { return (Data (period, C2 (2))); }
	double Time2 (int period)                          { return (Data (period, C2 (3)) / 10.0); }
	double Distance2 (int period)                      { return (Data (period, C2 (4)) / 10.0); }
	double Density2 (int period)                       { return (Average (period, C2 (5))); }
	double Max_Density2 (int period)                   { return (Data (period, C2 (6)) / 100.0); }
	double Queue2 (int period)                         { return (Average (period, C2 (7))); }
	int    Max_Queue2 (int period)                     { return (Data (period, C2 (8))); }
	int    Failure2 (int period)                       { return (Data (period, C2 (9))); }
	int    Count2 (int period)                         { return (Data (period, C2 (10))); }
	int    Ratio_Count2 (int period)                   { return (Data (period, C2 (11))); }
	double Ratio_VMT2 (int period)                     { return (Data (period, C2 (12)) / 10.0); }
	double Ratio_VHT2 (int period)                     { return (Data (period, C2 (13)) / 10.0); }  

	int    Volume2 (int period);
	double TTime2 (int period);
	double Avg_Speed2 (int period);
	double Delay2 (int period);
	double Time_Ratio2 (int period);
	
	void   Enter2 (int period, int value)              { Data (period, C2 (1), value); }
	void   Exit2 (int period, int value)               { Data (period, C2 (2), value); }
	void   Time2 (int period, double value)            { Data (period, C2 (3), (int) (value * 10.0 + 0.5)); }
	void   Distance2 (int period, double value)        { Data (period, C2 (4), (int) (value * 10.0 + 0.5)); }
	void   Density2 (int period, double value)         { Data (period, C2 (5), (int) (value * 100.0 + 0.5)); }
	void   Max_Density2 (int period, double value)     { Data (period, C2 (6), (int) (value * 100.0 + 0.5)); }
	void   Queue2 (int period, double value)           { Data (period, C2 (7), (int) (value * 100.0 + 0.5)); }
	void   Max_Queue2 (int period, int value)          { Data (period, C2 (8), value); }
	void   Failure2 (int period, int value)            { Data (period, C2 (9), value); }
	void   Count2 (int period, int value)              { Data (period, C2 (10), value); }
	void   Ratio_Count2 (int period, int value)        { Data (period, C2 (11), value); }
	void   Ratio_VMT2 (int period, double value)       { Data (period, C2 (12), (int) (value * 10.0 + 0.5)); }
	void   Ratio_VHT2 (int period, double value)       { Data (period, C2 (13), (int) (value * 10.0 + 0.5)); }
	
	void   Add_Enter2 (int period, int value)          { Add_Data (period, C2 (1), value); }
	void   Add_Exit2 (int period, int value)           { Add_Data (period, C2 (2), value); }
	void   Add_Time2 (int period, double value)        { Add_Data (period, C2 (3), (int) (value * 10.0 + 0.5)); }
	void   Add_Distance2 (int period, double value)    { Add_Data (period, C2 (4), (int) (value * 10.0 + 0.5)); }
	void   Add_Density2 (int period, double value)     { Add_Data (period, C2 (5), (int) (value * 100.0 + 0.5)); }
	void   Add_Max_Density2 (int period, double value) { Max_Data (period, C2 (6), (int) (value * 100.0 + 0.5)); }
	void   Add_Queue2 (int period, double value)       { Add_Data (period, C2 (7), (int) (value * 100.0 + 0.5)); }
	void   Add_Max_Queue2 (int period, int value)      { Max_Data (period, C2 (8), value); }
	void   Add_Failure2 (int period, int value)        { Add_Data (period, C2 (9), value); }
	void   Add_Count2 (int period, int value)          { Add_Data (period, C2 (10), value); }
	void   Add_Ratio_Count2 (int period, int value)    { Add_Data (period, C2 (11), value); }
	void   Add_Ratio_VMT2 (int period, double value)   { Add_Data (period, C2 (12), (int) (value * 10.0 + 0.5)); }
	void   Add_Ratio_VHT2 (int period, double value)   { Add_Data (period, C2 (13), (int) (value * 10.0 + 0.5)); }

protected:
	double Average (int period, int column);

private:
	int length;

	int C2 (int num)                                   { return ((Columns () / 2) + num); }
};

//---------------------------------------------------------
//	Performance_Array class definition
//---------------------------------------------------------

class Performance_Array : public Dir_Array, public Lookup_Size
{
public:
	Performance_Array (int max_records = 0);
	virtual ~Performance_Array (void)          { Performance_Array::Clear (); }

	bool Add (Performance_Data *data = NULL)   { return (Dir_Array::Add (data)); }
	
	Performance_Data * New_Record (bool clear = false, int number = 1)
	                                           { return ((Performance_Data *) Dir_Array::New_Record (clear, number)); }

	Performance_Data * Get (int link)          { return ((Performance_Data *) Dir_Array::Get (link)); }
	Performance_Data * Get_GE (int link)       { return ((Performance_Data *) Dir_Array::Get_GE (link)); }
	Performance_Data * Get_LE (int link)       { return ((Performance_Data *) Dir_Array::Get_LE (link)); }

	Performance_Data * Record (int index)      { return ((Performance_Data *) Dir_Array::Record (index)); }
	Performance_Data * Record (void)           { return ((Performance_Data *) Dir_Array::Record ()); }

	Performance_Data * First (void)            { return ((Performance_Data *) Dir_Array::First ()); }
	Performance_Data * Next (void)             { return ((Performance_Data *) Dir_Array::Next ()); }
	
	Performance_Data * Last (void)             { return ((Performance_Data *) Dir_Array::Last ()); }
	Performance_Data * Previous (void)         { return ((Performance_Data *) Dir_Array::Previous ()); }
	
	Performance_Data * First_Key (void)        { return ((Performance_Data *) Dir_Array::First_Key ()); }
	Performance_Data * Next_Key (void)         { return ((Performance_Data *) Dir_Array::Next_Key ()); }

	Performance_Data * operator[] (int index)  { return (Record (index)); }

	virtual void Clear (void);

	int  Period (int tod)                      { return (Row (tod)); }
	void Time_Periods (Time_Range *range)      { Row_Range (range); }

	int  Period_Size (void)                    { return (Row_Size ()); }
	bool Period_Size (int size, int min_size = 0, int max_size = MIDNIGHT, bool wrap_flag = true) 
	                                           { return (Row_Size (size, min_size, max_size, wrap_flag)); }

	int  Periods (void)                        { return (Rows ()); }
	void Periods (int value)                   { Rows (value); }

	int  Layers (void)                         { return (layers); }
	void Layers (int value)                    { layers = value; }

	double Congested_Ratio (void)              { return (congested_ratio); }
	void   Congested_Ratio (double ratio)      { congested_ratio = ratio; }

	bool   Ratio_Flag (void)                   { return (congested_ratio > 1.0); }

private:
	int layers;
	double congested_ratio;
};

#endif
