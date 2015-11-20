//******************************************************** 
//	Delay_File.hpp - Link Delay Input/Output
//********************************************************

#ifndef DELAY_FILE_HPP
#define DELAY_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Delay_File Class definition
//---------------------------------------------------------

class  Delay_File : public Db_Header
{
public:

	Delay_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Delay_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, int increment = 0);
	virtual ~Delay_File (void);

	int Link (void)					{ Get_Field (link, &lvalue); return (lvalue); }
	int Dir (void)					{ Get_Field (dir, &lvalue); return (lvalue); }
	int Time (void)					{ Get_Field (tod, &lvalue); return (lvalue); }
	char *Start_Time (void)			{ Get_Field (start, svalue); return (svalue); }
	char *End_Time (void)			{ Get_Field (end, svalue); return (svalue); }
	int Volume (void)				{ Get_Field (volume, &lvalue); return (lvalue); }
	int Enter (void)				{ Get_Field (enter, &lvalue); return (lvalue); }
	int Exit (void)					{ Get_Field (exit, &lvalue); return (lvalue); }
	double Speed (void)				{ Get_Field (speed, &dvalue); return (dvalue); }
	double Speed2 (void)			{ Get_Field (speed2, &dvalue); return (dvalue); }
	double TTime (void)				{ Get_Field (ttime, &dvalue); return (dvalue); }
	double TTime2 (void)			{ Get_Field (ttime2, &dvalue); return (dvalue); }
	double Delay (void)				{ Get_Field (delay, &dvalue); return (dvalue); }
	double Density (void)			{ Get_Field (density, &dvalue); return (dvalue); }
	double Max_Density (void)		{ Get_Field (max_den, &dvalue); return (dvalue); }
	double Time_Ratio (void)		{ Get_Field (ratio, &dvalue); return (dvalue); }
	double VMT (void)				{ Get_Field (vmt, &dvalue); return (dvalue); }
	double VHT (void)				{ Get_Field (vht, &dvalue); return (dvalue); }
	double Avg_Queue (void)			{ Get_Field (queue, &dvalue); return (dvalue); }
	int Max_Queue (void)			{ Get_Field (max_que, &lvalue); return (lvalue); }
	int Cycle_Failure (void)		{ Get_Field (fail, &lvalue); return (lvalue); }
	int Lane (void)					{ Get_Field (lane, &lvalue); return (lvalue); }
	int Turn (void)					{ Get_Field (turn, &lvalue); return (lvalue); }
	int Vehicles (void)				{ Get_Field (vehicles, &lvalue); return (lvalue); }
	int Num_Connect (void)			{ Get_Field (nconnect, &lvalue); return (lvalue); }
	int Out_Link (void)				{ Get_Field (out_link, &lvalue); return (lvalue); }
	int Out_Dir (void)				{ Get_Field (out_dir, &lvalue); return (lvalue); }
	int Out_Turn (void)				{ Get_Field (out_turn, &lvalue); return (lvalue); }
	double Out_Time (void)			{ Get_Field (out_time, &dvalue); return (dvalue); }

	void Link (int value)			{ Put_Field (link, value); }
	void Dir (int value)			{ Put_Field (dir, value); }
	void Time (int value)			{ Put_Field (tod, value); }
	void Start_Time (char *value)	{ Put_Field (start, value); }
	void End_Time (char *value)		{ Put_Field (end, value); }
	void Volume (int value)			{ Put_Field (volume, value); }
	void Enter (int value)			{ Put_Field (enter, value); }
	void Exit (int value)			{ Put_Field (exit, value); }
	void Speed (double value)		{ Put_Field (speed, value); }
	void Speed2 (double value)		{ Put_Field (speed2, value); }
	void TTime (double value)		{ Put_Field (ttime, value); }
	void TTime2 (double value)		{ Put_Field (ttime2, value); }
	void Delay (double value)		{ Put_Field (delay, value); }
	void Density (double value)		{ Put_Field (density, value); }
	void Max_Density (double value)	{ Put_Field (max_den, value); }
	void Time_Ratio (double value)	{ Put_Field (ratio, value); }
	void VMT (double value)			{ Put_Field (vmt, value); }
	void VHT (double value)			{ Put_Field (vht, value); }
	void Avg_Queue (double value)	{ Put_Field (queue, value); }
	void Max_Queue (int value)		{ Put_Field (max_que, value); }
	void Cycle_Failure (int value)	{ Put_Field (fail, value); }
	void Lane (int value)			{ Put_Field (lane, value); }
	void Turn (int value)			{ Put_Field (turn, value); }
	void Vehicles (int value)		{ Put_Field (vehicles, value); }
	void Num_Connect (int value)	{ Put_Field (nconnect, value); }
	void Out_Link (int value)		{ Put_Field (out_link, value); }
	void Out_Dir (int value)		{ Put_Field (out_dir, value); }
	void Out_Turn (int value)		{ Put_Field (out_turn, value); }
	void Out_Time (double value)	{ Put_Field (out_time, value); }

	bool Dir_Flag (void)			{ return (dir != 0); }

	int Increment (void)			{ return (increment); }
	int Iteration (void)			{ return (iteration); }
	int Period (void)				{ return ((Time () - 1) / increment); }

	void Increment (int value)		{ if (value > 0 && value <= MIDNIGHT) increment = value; }
	void Iteration (int value)		{ if (value > 0) iteration = value; }
	void Period (int value)			{ Put_Field (tod, value * increment); }

	virtual bool Create_Fields (void);

	bool Create_Header (void);

protected:
	virtual bool Read_Header (bool stat);

	virtual bool Set_Field_Numbers (void);
	virtual bool Default_Definition (void);

private:
	void Setup (void);

	int lvalue;
	double dvalue;

	int link, dir, tod, start, end, volume, enter, exit, speed, speed2;
	int ttime, ttime2, delay, density, max_den, ratio, vmt, vht, lane, turn, vehicles;
	int nconnect, out_link, out_dir, out_turn, out_time;
	int increment, iteration, queue, max_que, fail;
};

#endif
