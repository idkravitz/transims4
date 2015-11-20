//*********************************************************
//	Lookup_Table.hpp - fixed dimension lookup tables
//*********************************************************

#ifndef LOOKUP_TABLE_HPP
#define LOOKUP_TABLE_HPP

#include "Range_Data.hpp"
#include "System_Defines.hpp"

//---------------------------------------------------------
//	Lookup_Table class definition
//---------------------------------------------------------

class Lookup_Table
{
public:
	Lookup_Table (int rows = 0, int columns = 0);
	virtual ~Lookup_Table (void)                   { Clear (); }

	int  Rows (void)                               { return (rows); }
	int  Columns (void)                            { return (columns); }
	int  Data (int row, int column)                { return (Index (row, column) ? data [index] : 0); }

	void Data (int row, int column, int value)     { if (Index (row, column)) data [index] = value; }
	void Add_Data (int row, int column, int value) { if (Index (row, column)) data [index] += value; }
	void Max_Data (int row, int column, int value) { if (Index (row, column)) data [index] = MAX (data [index], value); }

	bool Size (int rows, int columns = 1);

	void Clear (void);

private:
	short  rows;
	short  columns;
	int   *data;

	bool Index (int row, int column);

	static int index;
};

//---------------------------------------------------------
//	Lookup_Size class definition
//---------------------------------------------------------

class Lookup_Size
{
public:
	Lookup_Size (int rows = 0, int columns = 0);

	int  Rows (void)                     { return (rows); }
	int  Columns (void)                  { return (columns); }

	int  Row (int value);
	int  Row_Size (void)                 { return (row_size); }

	void Rows (int value)                { if (row_range == NULL) rows = MAX (value, 0); }
	void Columns (int value)             { columns = MAX (value, 0); }

	bool Row_Size (int size, int min_size = 0, int max_size = MIDNIGHT, bool wrap_flag = true);
	void Row_Range (Range_Array *range);

	int  Min_Size (void)                 { return (min_size); }
	int  Max_Size (void)                 { return (max_size); }

	void Min_Size (int value)            { min_size = value; }
	void Max_Size (int value)            { max_size = value; }

private:
	bool wrap_flag;
	int rows, columns, row_size, min_size, max_size;
	Range_Array *row_range;
};

#endif
