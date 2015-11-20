//*********************************************************
//	Position_Data.hpp - vehicle position classes
//*********************************************************

#ifndef POSITION_DATA_HPP
#define POSITION_DATA_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Position class definition
//---------------------------------------------------------

class Position
{
public:
	Position (void);

	int  Dir (void)                     { return (dir); }
	int  Cell (void)                    { return (cell); }
	int  Lane (void)                    { return (lane); }

	void Dir (int value)                { dir = value; }
	void Cell (int value)               { cell = (short) value; }
	void Lane (int value)               { lane = (short) value; }

private:
	int   dir;
	short cell;
	short lane;
};

//---------------------------------------------------------
//	Position_Data class definition
//---------------------------------------------------------

class Position_Data
{
public:
	Position_Data (void);
	virtual ~Position_Data (void)       { Clear (); }

	int  Dir (void)                     { return (Dir_Num (1)); }
	int  Cell (void)                    { return (Cell_Num (1)); }
	int  Lane (void)                    { return (Lane_Num (1)); }

	int  To_Dir (void)                  { return (Dir_Num (0)); }
	int  To_Cell (void)                 { return (Cell_Num (0)); }
	int  To_Lane (void)                 { return (Lane_Num (0)); }

	int  Dir_Num (int num)              { return (Index (num) ? data [num].dir : 0); }
	int  Cell_Num (int num)             { return (Index (num) ? data [num].cell : 0); }
	int  Lane_Num (int num)             { return (Index (num) ? data [num].lane : 0); }

	void Dir (int value)                { Dir_Num (1, value); }
	void Cell (int value)               { Cell_Num (1, value); }
	void Lane (int value)               { Lane_Num (1, value); }

	void To_Dir (int value)             { Dir_Num (0, value); }
	void To_Cell (int value)            { Cell_Num (0, value); }
	void To_Lane (int value)            { Lane_Num (0, value); }

	void Dir_Num (int num, int value)   { if (Index (num)) data [num].dir = value; }
	void Cell_Num (int num, int value)  { if (Index (num)) data [num].cell = (short) value; }
	void Lane_Num (int num, int value)  { if (Index (num)) data [num].lane = (short) value; }

	int  Num_Cells (void)               { return (num_cells); }
	bool Num_Cells (int value);

	void Clear (void);

private:
	typedef struct {
		int   dir;
		short cell;
		short lane;
	} Position;

	int num_cells;

	Position *data;

	bool Index (int num)             { return (num >= 0 && num <= num_cells); }
};

//---------------------------------------------------------
//	Position_Data_Array class definition
//---------------------------------------------------------

class Position_Data_Array : public Data_Array
{
public:
	Position_Data_Array (int max_records = 0);

	bool Add (Position *data = NULL)    { return (Data_Array::Add (data)); }

	Position * New_Record (bool clear = false, int number = 1)
	                                    { return ((Position *) Data_Array::New_Record (clear, number)); }


	Position * Record (int index)       { return ((Position *) Data_Array::Record (index)); }
	Position * Record (void)            { return ((Position *) Data_Array::Record ()); }

	Position * First (void)             { return ((Position *) Data_Array::First ()); }
	Position * Next (void)              { return ((Position *) Data_Array::Next ()); }

	Position * Last (void)              { return ((Position *) Data_Array::Last ()); }
	Position * Previous (void)          { return ((Position *) Data_Array::Previous ()); }

	Position * operator[] (int index)   { return (Record (index)); }
};

#endif
