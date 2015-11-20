//*********************************************************
//	Grid_Data.hpp - network data grid class
//*********************************************************

#ifndef GRID_DATA_HPP
#define GRID_DATA_HPP

#include "Dir_Data.hpp"

//---------------------------------------------------------
//	Grid_Data class definition
//---------------------------------------------------------

class Grid_Data : public Dir_Data
{
public:
	Grid_Data (void);
	virtual ~Grid_Data (void)                    { Clear (); }
	
	int  Lanes (void)                            { return (lanes); }
	int  Cells (void)                            { return (cells); }

	int  Type (void)                             { return (type); }
	int  Cell_In (void)                          { return (cell_in); }
	int  Cell_Out (void)                         { return (cell_out); }

	int  Data (int cell, int lane)               { return (Index (cell, lane) ? data [index] : 0); }
	int  To_Data (int cell, int lane)            { return (Index_To (cell, lane) ? data [index] : 0); }

	int  Connect_Dir (int lane)                  { return (Index_Dir (lane) ? data [index] : 0); }
	int  Connect_Lane (int lane)                 { return (Index_Lane (lane) ? data [index] : 0); }
	int  Lane_Use (int lane)                     { return (Index_Use (lane) ? data [index] : 0); }
	int  TOD_Use (int lane)                      { return (Index_TOD (lane) ? data [index] : 0); }

	void Type (int value)                        { type = (char) value; }
	void Cell_In (int value)                     { cell_in = (short) value; }
	void Cell_Out (int value)                    { cell_out = (short) value; }

	void Data (int cell, int lane, int value)    { if (Index (cell, lane)) data [index] = value; }
	void To_Data (int cell, int lane, int value) { if (Index_To (cell, lane)) data [index] = value; }

	void Connect_Dir (int lane, int value)       { if (Index_Dir (lane)) data [index] = value; }
	void Connect_Lane (int lane, int value)      { if (Index_Lane (lane)) data [index] = value; }
	void Lane_Use (int lane, int value)          { if (Index_Use (lane)) data [index] = value; }
	void TOD_Use (int lane, int value)           { if (Index_TOD (lane)) data [index] = value; }

	bool Data_Size (int cells, int lanes);
	void Clear (void);

	void Zero_Grid (void);
	static void Switch_Grid (void)               { grid = !grid; }

private:
	short cells;
	char  lanes;
	char  type;
	short cell_in;
	short cell_out;
	int   *data;

	bool Index (int cell, int lane);
	bool Index_To (int cell, int lane);
	bool Index_Dir (int lane);
	bool Index_Lane (int lane);
	bool Index_Use (int lane);
	bool Index_TOD (int lane);

	static int  index;
	static bool grid;
};

//---------------------------------------------------------
//	Grid_Array class definition
//---------------------------------------------------------

class Grid_Array : public Dir_Array
{
public:
	Grid_Array (int max_records = 0);
	virtual ~Grid_Array (void)              { Clear (); }

	bool Add (Grid_Data *data = NULL)       { return (Dir_Array::Add (data)); }
	
	Grid_Data * New_Record (bool clear = false, int number = 1)
	                                        { return ((Grid_Data *) Dir_Array::New_Record (clear, number)); }

	Grid_Data * Get (int link)              { return ((Grid_Data *) Dir_Array::Get (link)); }
	Grid_Data * Get_GE (int link)           { return ((Grid_Data *) Dir_Array::Get_GE (link)); }
	Grid_Data * Get_LE (int link)           { return ((Grid_Data *) Dir_Array::Get_LE (link)); }

	Grid_Data * Record (int index)          { return ((Grid_Data *) Dir_Array::Record (index)); }
	Grid_Data * Record (void)               { return ((Grid_Data *) Dir_Array::Record ()); }

	Grid_Data * First (void)                { return ((Grid_Data *) Dir_Array::First ()); }
	Grid_Data * Next (void)                 { return ((Grid_Data *) Dir_Array::Next ()); }
	
	Grid_Data * Last (void)                 { return ((Grid_Data *) Dir_Array::Last ()); }
	Grid_Data * Previous (void)             { return ((Grid_Data *) Dir_Array::Previous ()); }
	
	Grid_Data * First_Key (void)            { return ((Grid_Data *) Dir_Array::First_Key ()); }
	Grid_Data * Next_Key (void)             { return ((Grid_Data *) Dir_Array::Next_Key ()); }

	Grid_Data * operator[] (int index)      { return (Record (index)); }

	virtual void Clear (void);
	void Zero_Grids (void);

	void Switch_Grids (void)                { Zero_Grids (); Grid_Data::Switch_Grid (); }
};

#endif
