//*********************************************************
//	Program_Data.cpp - data arrays for user programs
//*********************************************************

#include "Program_Data.hpp"

//---------------------------------------------------------
//	Stack_Array
//---------------------------------------------------------

Stack_Array::Stack_Array (int max_records) :
	Data_Array (sizeof (Stack_Data), max_records)
{
	Stack_Data stack;

	Add (&stack);
}

//---------------------------------------------------------
//	Command_Array
//---------------------------------------------------------

Command_Array::Command_Array (int max_records) :
	Data_Array (sizeof (Command_Data), max_records)
{
}

//---------------------------------------------------------
//	Token_Array
//---------------------------------------------------------

Token_Array::Token_Array (int max_records) :
	Data_Array (sizeof (Token_Data), max_records)
{
}

void Token_Array::Clear (void)
{
	for (Token_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		if (ptr->name != NULL) {
			delete [] ptr->name;
			ptr->name = NULL;
		}
	}
	Data_Array::Clear ();
}

//---------------------------------------------------------
//	Integer_Array
//---------------------------------------------------------

Integer_Array::Integer_Array (int max_records) :
	Data_Array (sizeof (int), max_records)
{
	Add (0);
}

//---------------------------------------------------------
//	Double_Array
//---------------------------------------------------------

Double_Array::Double_Array (int max_records) :
	Data_Array (sizeof (double), max_records)
{
	Add (0.0);
}

//---------------------------------------------------------
//	Svalue_Array
//---------------------------------------------------------

Svalue_Array::Svalue_Array (int max_records) :
	Data_Array (sizeof (Svalue_Data), max_records)
{
	Svalue_Data rec;

	rec.string = new char [1];
	rec.string [0] = '\0';
	rec.length = 1;

	Add (&rec);
}

void Svalue_Array::Clear (void)
{
	for (Svalue_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		if (ptr->string != NULL) {
			delete [] ptr->string;
			ptr->string = NULL;
		}
	}
	Data_Array::Clear ();
}

//---------------------------------------------------------
//	Svalue_Index_Array constructor
//---------------------------------------------------------

Svalue_Index_Array::Svalue_Index_Array (int max_records) : 
	Index_Array (sizeof (Svalue_Index), max_records, false)
{
}

