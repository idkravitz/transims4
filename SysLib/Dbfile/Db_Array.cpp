//********************************************************* 
//	Db_Array.cpp - database arrays
//*********************************************************

#include "Db_Array.hpp"

//-----------------------------------------------------------
//	Db_Data_Array
//-----------------------------------------------------------

Db_Data_Array::Db_Data_Array () : 
	Db_Base (MODIFY, BINARY), Data_Array (0, 0)
{
}

//-----------------------------------------------------------
//	Read_Record
//-----------------------------------------------------------

bool Db_Data_Array::Read_Record (int index)
{
	void *rec;

	if (index > 0) {
		rec = Data_Array::Record (index); 
	} else {
		rec = Next ();
	}
	if (rec != NULL) {
		Db_Record::Record ((char *) rec); 
	}
	return (rec != NULL); 
}

//-----------------------------------------------------------
//	Write_Record
//-----------------------------------------------------------

bool Db_Data_Array::Write_Record (int index)
{
	void *ptr;

	if (index > 0) {
		ptr = Data_Array::Record (index);
	} else {
		ptr = Data_Array::Record (Record_Index ()); 
	}
	if (ptr == NULL) return (false);

	char *rec = Db_Record::Record ();
	if (rec == NULL) return (false);

	memcpy (ptr, rec, Data_Size ());
	return (true); 
}

//-----------------------------------------------------------
//	Add_Record
//-----------------------------------------------------------

bool Db_Data_Array::Add_Record (void)
{
	char *rec = Db_Record::Record ();
	if (rec == NULL) return (false);

	return (Data_Array::Add (rec)); 
}

//-----------------------------------------------------------
//	Record_Size
//-----------------------------------------------------------

bool Db_Data_Array::Record_Size (int size)
{
	if (!Db_Record::Record_Size (size)) return (false);

	Data_Size (Db_Record::Max_Size ());

	return (true);
}

//-----------------------------------------------------------
//	Db_Index_Array
//-----------------------------------------------------------

Db_Index_Array::Db_Index_Array () : 
	Db_Base (MODIFY, BINARY), Index_Array (0, 0)
{
}

//-----------------------------------------------------------
//	Read_Record
//-----------------------------------------------------------

bool Db_Index_Array::Read_Record (int index)
{
	void *rec;

	if (index > 0) {
		rec = Index_Array::Record (index); 
	} else {
		rec = Next_Key ();
	}
	if (rec != NULL) {
		Db_Record::Record ((char *) rec); 
	}
	return (rec != NULL); 
}

//-----------------------------------------------------------
//	Write_Record
//-----------------------------------------------------------

bool Db_Index_Array::Write_Record (int index)
{
	void *ptr;

	if (index > 0) {
		ptr = Index_Array::Record (index);
	} else {
		ptr = Index_Array::Record (Record_Index ()); 
	}
	if (ptr == NULL) return (false);

	char *rec = Db_Record::Record ();
	if (rec == NULL) return (false);

	memcpy (ptr, rec, Data_Size ());
	return (true); 
}

//-----------------------------------------------------------
//	Add_Record
//-----------------------------------------------------------

bool Db_Index_Array::Add_Record (void)
{
	char *rec = Db_Record::Record ();
	if (rec == NULL) return (false);

	return (Index_Array::Add (rec)); 
}

//-----------------------------------------------------------
//	Record_Size
//-----------------------------------------------------------

bool Db_Index_Array::Record_Size (int size)
{
	if (!Db_Record::Record_Size (size)) return (false);

	Data_Size (Db_Record::Max_Size ());

	return (true);
}
