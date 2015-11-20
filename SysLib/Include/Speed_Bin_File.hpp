//******************************************************** 
//	Speed_Bin_File.hpp - Speed Bin File Input/Output
//********************************************************

#ifndef SPEED_BIN_FILE_HPP
#define SPEED_BIN_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Speed_Bin_File Class definition
//---------------------------------------------------------

class  Speed_Bin_File : public Db_Header
{
public:

	Speed_Bin_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Speed_Bin_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Speed_Bin_File (void);

	int Link (void)					{ Get_Field (link, &lvalue); return (lvalue); }
	int Dir (void)					{ Get_Field (dir, &lvalue); return (lvalue); }
	double Offset (void)			{ Get_Field (offset, &dvalue); return (dvalue); }
	char *End_Time (void)			{ Get_Field (end, svalue); return (svalue); }
	int Bin (int num)				{ if (Check_Num (num)) { Get_Field (bin [num], &lvalue); return (lvalue); } else return (0); }

	void Link (int value)			{ Put_Field (link, value); }
	void Dir (int value)			{ Put_Field (dir, value); }
	void Offset (double value)		{ Put_Field (offset, value); }
	void End_Time (char *value)		{ Put_Field (end, value); }
	void Bin (int num, int value)	{ if (Check_Num (num)) Put_Field (bin [num], value); }

	int  Num_Bins (void)			{ return (num_bins); }
	void Num_Bins (int value)		{ num_bins = value; }

	double Box_Length (void)		{ return (box_length); }
	void   Box_Length (double len)	{ box_length = len; }

	double Cell_Length (void)		{ return (cell_length); }
	void   Cell_Length (double len)	{ cell_length = len; }

	int  Sample_Time (void)			{ return (sample_time); }
	void Sample_Time (int time)		{ sample_time = time; }

	int  Increment (void)			{ return (increment); }
	void Increment (int time)		{ increment = time; }

	int  Vehicle_Type (void)		{ return (veh_type); }
	void Vehicle_Type (int type)	{ veh_type = type; }

	int  Veh_SubType (void)			{ return (subtype); }
	void Veh_SubType (int type)		{ subtype = type; }

	virtual bool Create_Fields (void);

	bool Create_Header (void);

protected:
	virtual bool Read_Header (bool stat);

	virtual bool Set_Field_Numbers (void);
	
private:
	void Setup (void);
	bool Check_Num (int num)		{ return (num >= 0 && num < num_bins); }

	int lvalue;
	double dvalue;

	int link, dir, offset, end, *bin;
	int num_bins, sample_time, increment, veh_type, subtype;
	double box_length, cell_length;

	static char *BOX_LENGTH, *CELL_LENGTH, *SAMPLE_TIME, *INCREMENT, *TIME_STEP,
		*VEHICLE_SUBTYPE, *VEHICLE_TYPE, *NUM_BINS, *VELOCITY_BINS, *VELOCITY_MAX;
};
#endif
