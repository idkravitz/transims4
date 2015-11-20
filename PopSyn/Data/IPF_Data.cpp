//*********************************************************
//	IPF_Data.cpp - iterative proportional fit
//*********************************************************

#include "IPF_Data.hpp"

#include <string.h>
#include <math.h>
#include <stdio.h>

//---------------------------------------------------------
//	IPF_Data constructor
//---------------------------------------------------------

IPF_Data::IPF_Data (void)
{
	share = previous = NULL;
	max_iter = 10000;
	max_diff = 0.0000001;
	num_cells = num_iter = 0;
	difference = 0.0;
}

//---- free attribute memory ----

void IPF_Data::Clear ()
{
	Clear_Cells ();

	for (Attribute_Data *ptr = attribute.First (); ptr; ptr = attribute.Next ()) {
		ptr->Clear ();
	}
	attribute.Clear ();
}

//---- clear memory ----

void IPF_Data::Clear_Cells (void)
{
	if (share != NULL) {
		delete share;
		share = NULL;
	}
	if (previous != NULL) {
		delete previous;
		previous = NULL;
	}
	num_cells = 0;
}

//---- allocate cell memory ----

bool IPF_Data::Num_Cells (int cells)
{
	Clear_Cells ();

	share = new double [cells];
	previous = new double [cells];

	if (share == NULL || previous == NULL) return (false);

	memset (share, '\0', cells * sizeof (double));
	num_cells = cells;
	return (true);
}

//---- add attribute ----

bool IPF_Data::Add_Attribute (int types) 
{
	Attribute_Data *at_ptr = attribute.New_Record (true);
	if (!at_ptr) return (false);
	if (!at_ptr->Num_Types (types)) return (false);
	if (!attribute.Add ()) return(false);
	return (true);
}

//---- define cells ----

bool IPF_Data::Set_Cells (void) 
{
	Attribute_Data *at_ptr;
	int cell, index, num, base, cells, types;

	//---- calculate total number of cells ----

	cells = 1;

	for (at_ptr = attribute.First (); at_ptr; at_ptr = attribute.Next ()) {
		types = at_ptr->Num_Types ();
		if (types <= 0) return (false);
		cells *= types;
	}

	//---- calculate cell indices ----

	num = base = cells;

	for (at_ptr = attribute.First (); at_ptr; at_ptr = attribute.Next ()) {
		if (!at_ptr->Num_Cells (cells)) return (false);

		types = at_ptr->Num_Types ();

		base /= types;

		for (cell=1; cell <= cells; cell++) {
			index = ((cell - 1) % num) / base + 1;
			at_ptr->Index (cell, index);
		}
		num = base;
	}
	return (Num_Cells (cells));
}

//---- zero share data ----

void IPF_Data::Zero_Share (void)
{
	if (num_cells > 0) {
		memset (share, '\0', num_cells * sizeof (double));
	}
}

//---- iterative proportional fit ----

bool IPF_Data::IPF (void) 
{
	int cell;
	double *share_ptr, total;
	bool flag;

	Attribute_Data *at_ptr;

	if (num_cells == 0) return (false);

	//---- normalize the data ----

	for (at_ptr = attribute.First (); at_ptr; at_ptr = attribute.Next ()) {
		at_ptr->Normalize ();
	}
	total = 0.0;

	for (cell=1, share_ptr=share; cell <= num_cells; cell++, share_ptr++) {
		total += *share_ptr;
	}
	if (total != 1.0) {
		flag = (total == 0.0);
		if (flag) total = 1.0 / num_cells;

		for (cell=1, share_ptr=share; cell <= num_cells; cell++, share_ptr++) {
			if (flag) {
				*share_ptr = total;
			} else {
				*share_ptr /= total;
			}
		}
	}

	//---- iterate until converence ----

	for (num_iter=1; num_iter <= max_iter; num_iter++) {

		//---- copy the current shares ----

		memcpy (previous, share, num_cells * sizeof (double));

		//---- process each attribute ----

		for (at_ptr = attribute.First (); at_ptr; at_ptr = attribute.Next ()) {
			at_ptr->Zero_Total ();

			//---- sum the attribute totals ----

			for (cell=1, share_ptr=share; cell <= num_cells; cell++, share_ptr++) {
				at_ptr->Total_Cell (cell, *share_ptr);
			}

			//---- calculate the correction factors ----

			if (at_ptr->Factor_Total () != 0.0) {

				//---- apply the correction factors ----

				for (cell=1, share_ptr=share; cell <= num_cells; cell++, share_ptr++) {
					*share_ptr *= at_ptr->Cell_Factor (cell);
				}
			}
		}

		//---- check the convergence criteria ----

		difference = 0.0;

		for (cell=0, share_ptr=share; cell < num_cells; cell++, share_ptr++) {
			difference += fabs (previous [cell] - *share_ptr);
		}
		if (difference <= max_diff) return (true);
	}
	return (false);
}

