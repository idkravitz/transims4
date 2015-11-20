//*********************************************************
//	PUMA_Data.cpp - PUMA data class
//*********************************************************

#include "PUMA_Data.hpp"

//---------------------------------------------------------
//	PUMA_Data constructor
//---------------------------------------------------------

PUMA_Data::PUMA_Data (int puma, int num_data) : Class_Index (puma)
{
	data = NULL;
	Num_Data (num_data);
}

//---- clear memory ----

void PUMA_Data::Clear (void) 
{
	puma_zone.Clear ();

	if (data != NULL) {
		delete [] data;
		data = NULL;
	}
}

//---- set the data size ----

void PUMA_Data::Num_Data (int ndata)
{
	if (ndata > 1) {
		puma_zone.Data_Size (sizeof (PUMA_Zone) + (ndata - 1) * sizeof (double)); 
	}
	if (ndata > 0) {
		if (data != NULL) {
			delete [] data;
			data = NULL;
		}
		data = new double [ndata];
		memset (data, '\0', ndata * sizeof (double));
	}
}

//---- new zone ----

PUMA_Zone * PUMA_Data::New_Zone (int zone)
{
	PUMA_Zone *zone_ptr = puma_zone.New_Record (true);
	if (zone_ptr == NULL) return (NULL);

	zone_ptr->Zone (zone);

	if (!puma_zone.Add (zone_ptr)) return (NULL);

	return (puma_zone.Record ());
}

//---- new household ----

bool PUMA_Data::New_Household (int cell, int index)
{
	Household_Index *key = hh_index.New_Record ();
	if (key == NULL) return (false);

	key->cell = cell;
	key->index = index;

	return (hh_index.Add ());
}

//---------------------------------------------------------
//	PUMA_Data_Array constructor
//---------------------------------------------------------

PUMA_Data_Array::PUMA_Data_Array (int max_records) : 
	Class_Pointer (max_records)
{
	num_cells = num_data = 0;
	data_ptr = zone_ptr = NULL;
	puma_data = NULL;
	puma_zone = NULL;
}

//---- delete pointers ----

void PUMA_Data_Array::Delete_Pointers (void) 
{
	for (PUMA_Data *data_ptr = First (); data_ptr; data_ptr = Next ()) {
		delete data_ptr;
	}
	Data_Array::Reset ();
}

//---- clear memory ----

void PUMA_Data_Array::Clear (void) 
{
	for (Attribute_Type *ptr = attribute.First (); ptr; ptr = attribute.Next ()) {
		ptr->Clear ();
	}
	attribute.Clear ();

	Class_Pointer::Clear ();
}

//---- define data record ----

bool PUMA_Data_Array::Set_Record (void) 
{
	int cells, types, offset;
	Attribute_Type *at_ptr;

	//---- calculate total number of cells ----

	cells = 1;

	for (at_ptr = attribute.First (); at_ptr; at_ptr = attribute.Next ()) {
		types = at_ptr->Num_Types ();
		if (types <= 0) return (false);
		cells *= types;
	}
	if (cells == 1) return (false);

	//---- set the attribute offsets ----

	offset = cells + 1;

	for (at_ptr = attribute.First (); at_ptr; at_ptr = attribute.Next ()) {
		at_ptr->Offset (offset);
		offset += at_ptr->Num_Types ();
	}

	//---- set the data size ----

	num_data = offset;
	num_cells = cells;
	return (true);
}

//---- new puma ----

PUMA_Data * PUMA_Data_Array::New_PUMA (int puma)
{
	PUMA_Data *data_ptr = new PUMA_Data (puma, num_data);
	if (data_ptr == NULL) return (NULL);

	if (!Add (data_ptr)) return (NULL);

	return (Record ());
}

//---- set the current data point ----

void PUMA_Data_Array::Set_PUMA_Data (PUMA_Data *ptr)
{
	puma_data = ptr;
	if (puma_data != NULL) {
		data_ptr = puma_data->Data ();
	} else {
		data_ptr = NULL;
	}
	puma_zone = NULL;
	zone_ptr = NULL;
}

//---- set the current zone point ----

void PUMA_Data_Array::Set_PUMA_Zone (PUMA_Zone *ptr)
{
	puma_zone = ptr;
	if (puma_zone != NULL) {
		zone_ptr = puma_zone->Data ();
	} else {
		zone_ptr = NULL;
	}
}

//---- new attribute ----

Attribute_Type * PUMA_Data_Array::New_Attribute (void)
{
	if (!attribute.Add (attribute.New_Record (true))) return (NULL);
	return (attribute.Record ());
}

//---- share data ----

double PUMA_Data_Array::PUMA_Share (int cell) 
{ 
	if (data_ptr && num_data > 1 && cell > 0 && cell <= num_cells) {
		return (data_ptr [cell]);
	} else {
		return (0.0);
	}
}

void PUMA_Data_Array::PUMA_Share (int cell, double value) 
{ 
	if (data_ptr && num_data > 1 && cell > 0 && cell <= num_cells) {
		data_ptr [cell] = value;
	}
}

double PUMA_Data_Array::Zone_Share (int cell) 
{ 
	if (zone_ptr && num_data > 1 && cell > 0 && cell <= num_cells) {
		return (zone_ptr [cell]);
	} else {
		return (0.0);
	}
}

void PUMA_Data_Array::Zone_Share (int cell, double value) 
{ 
	if (zone_ptr && num_data > 1 && cell > 0 && cell <= num_cells) {
		zone_ptr [cell] = value;
	}
}

//---- target data ----

double PUMA_Data_Array::PUMA_Target (int offset) 
{ 
	if (data_ptr && offset >= 0 && offset < num_data) {
		return (data_ptr [offset]);
	} else {
		return (0.0);
	}
}

void PUMA_Data_Array::PUMA_Target (int offset, double value) 
{ 
	if (data_ptr && offset >= 0 && offset < num_data) {
		data_ptr [offset] = value;
	}
}

double PUMA_Data_Array::Zone_Target (int offset) 
{ 
	if (zone_ptr && offset >= 0 && offset < num_data) {
		return (zone_ptr [offset]);
	} else {
		return (0.0);
	}
}

void PUMA_Data_Array::Zone_Target (int offset, double value) 
{ 
	if (zone_ptr && offset >= 0 && offset < num_data) {
		zone_ptr [offset] = value;
	}
}


//---- add share to cell ----

bool PUMA_Data_Array::Add_Share (int index, double value) 
{
	int type, types, cell, scaler;
	Attribute_Type *at_ptr;

	if (num_cells == 0 || num_data == 0) return (false);

	//---- calculate the cell index ----

	cell = 1;
	scaler = num_cells;

	for (at_ptr = attribute.First (); at_ptr; at_ptr = attribute.Next ()) {
		types = at_ptr->Num_Types ();
		type = at_ptr->Type ();

		if (type < 1 || type > types) return (false);

		scaler = scaler / types;
		cell += (type - 1) * scaler;
	}
	if (cell < 1 || cell > num_cells) return (false);

	data_ptr [cell] += value;
	
	return (puma_data->New_Household (cell, index));
}

//---- sum targets ----

void PUMA_Data_Array::Sum_Targets (void)
{
	int offset, offset1;
	double weight;

	//---- process each puma in the model ----

	offset1 = num_cells + 1;
	if (num_data <= offset1) return;

	for (puma_data = First (); puma_data; puma_data = Next ()) {
		data_ptr = puma_data->Data ();

		//---- initialize the puma targets ----

		*data_ptr = 0.0;
		memset (data_ptr + offset1, '\0', (num_data - offset1) * sizeof (double));

		for (puma_zone = puma_data->First (); puma_zone; puma_zone = puma_data->Next ()) {
			data_ptr = puma_data->Data ();
			zone_ptr = puma_zone->Data ();

			*data_ptr += weight = *zone_ptr;

			data_ptr += offset1;
			zone_ptr += offset1;

			for (offset=offset1; offset < num_data; offset++, data_ptr++, zone_ptr++) {
				*data_ptr += *zone_ptr * weight;
			}
		}
	}
}

//---- distribute households ----

void PUMA_Data_Array::Distribute_Households (void)
{
	int cell, hholds;
	double total, share, *bucket, *bucket1;

	//---- initialize the bucket rounding by cell ----

	bucket1 = data_ptr + 1;

	for (cell=1, bucket = bucket1; cell <= num_cells; cell++, bucket++) {
		*bucket = 0.5;
	}

	//---- process each zone ----

	for (puma_zone = puma_data->First (); puma_zone; puma_zone = puma_data->Next ()) {
		zone_ptr = puma_zone->Data ();

		total = *zone_ptr++;
		bucket = bucket1;

		for (cell=1; cell <= num_cells; cell++, zone_ptr++, bucket++) {
			share = *zone_ptr * total + *bucket;

			hholds = (int) share;
			*bucket = share - hholds;

			*zone_ptr = hholds;
		}
	}
}
