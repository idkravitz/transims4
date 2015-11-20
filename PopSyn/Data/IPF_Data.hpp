//*********************************************************
//	IPF_Data.hpp - iterative proportional fix class
//*********************************************************

#ifndef IPF_DATA_HPP
#define IPF_DATA_HPP

#include "Attribute_Data.hpp"

//---------------------------------------------------------
//	IPF_Data class definition
//---------------------------------------------------------

class IPF_Data
{
public:
	IPF_Data (void);
	virtual ~IPF_Data (void)              { Clear (); }

	bool   Add_Attribute (int num_types);
	bool   Set_Cells (void);
	bool   IPF (void);

	double Share (int cell)               { return ((Check_Cell (cell)) ? share [cell-1] : 0); }
	void   Share (int cell, double value) { if (Check_Cell (cell)) share [cell-1] = value; }
	void   Zero_Share (void);

	void   Max_Iterations (int value)     { max_iter = value; }
	void   Max_Difference (double value)  { max_diff = value; }

	int    Num_Cells (void)               { return (num_cells); }
	int    Num_Iterations (void)          { return (num_iter); }
	double Difference (void)              { return (difference); }

	Attribute_Data * Attribute (int num)  { return (attribute [num]); }

	void   Clear (void);

protected:

	bool   Num_Cells (int cells);

	Attribute_Array attribute;

private:
	bool   Check_Cell (int cell)          { return (cell > 0 && cell <= num_cells); }

	void   Clear_Cells (void);

	int    num_cells, num_iter, max_iter;
	double *share, *previous, max_diff, difference;
};
#endif
