//*********************************************************
//	Lookup_Table.cpp - fixed dimension lookup tables
//*********************************************************

#include "Lookup_Table.hpp"

#include <string.h>

//---------------------------------------------------------
//	Lookup_Table constructor
//---------------------------------------------------------

Lookup_Table::Lookup_Table (int rows, int columns)
{
	data = NULL;
	Size (rows, columns);
}

void Lookup_Table::Clear (void)
{
	if (rows > 0) {
		delete [] data;
	}
	rows = columns = 0;
}

int  Lookup_Table::index = 0;

bool Lookup_Table::Index (int row, int column)
{ 
	if (row > 0 && row <= rows && column > 0 && column <= columns) {
		index = row + (column - 1) * rows - 1;
		return (true);
	} else {
		return (false);
	}
}

bool Lookup_Table::Size (int _rows, int _columns)
{
	Clear ();

	if (_rows > 0 && _rows < 32500 && _columns > 0 && _columns < 120) {
		int size = _rows * _columns;

		data = new int [size];
		if (data != NULL) {
			memset (data, '\0', size * sizeof (int));
			rows = (short) _rows;
			columns = (short) _columns;
			return (true);
		}
	}
	return (false);
}

//---------------------------------------------------------
//	Lookup_Size constructor
//---------------------------------------------------------

Lookup_Size::Lookup_Size (int rows, int columns)
{
	row_range = NULL;
	row_size = 0;
	wrap_flag = true;
	min_size = 0;
	max_size = MIDNIGHT;

	Rows (rows);
	Columns (columns);
}

int Lookup_Size::Row (int value)
{
	if (row_range == NULL) {
		if (row_size && rows) {
			if (wrap_flag) {
				value %= max_size;
			}
			if (value >= min_size && value <= max_size) {
				return (MIN (((value - min_size) / row_size + 1), rows));
			}
		}
		return (0);
	} else {
		return (row_range->In_Index (value)); 
	}
}

bool Lookup_Size::Row_Size (int size, int _min_size, int _max_size, bool _wrap_flag)
{
	if (row_range != NULL) return (false);
	if (_min_size < 0 || _max_size <= _min_size) return (false);

	row_size = MAX (size, 1);
	min_size = _min_size;
	max_size = _max_size;
	wrap_flag = _wrap_flag;
	rows = (max_size - min_size + row_size - 1) / row_size;
	return (true);
}

void Lookup_Size::Row_Range (Range_Array *range) 
{
	row_range = range;
	row_size = 1;
	if (row_range != NULL) {
		rows = row_range->Num_Ranges ();
	}
}
