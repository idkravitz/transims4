//*********************************************************
//	Trip_Length_Data.hpp - trip length data class
//*********************************************************

#ifndef Trip_Length_DATA_HPP
#define Trip_Length_DATA_HPP

#include "Db_File.hpp"
#include "Class_Index.hpp"
#include "Class_Array.hpp"
#include "Static_Service.hpp"

//---------------------------------------------------------
//	Trip_Length_Data class definition
//---------------------------------------------------------

class Trip_Length_Data : public Class2_Index
{
public:
	Trip_Length_Data (void);

	int   Mode (void)                 { return ((Key1 () >> 16)); }
	int   Purpose1 (void)             { return ((Key1 () & 0xFFFF)); }
	int   Purpose2 (void)             { return ((Key2 () >> 16)); }
	int   Increment (void)            { return ((Key2 () & 0xFFFF)); }
	int   Count (void)                { return (count); }

	void  Mode (int value)            { Key1 ((value << 16) + Purpose1 ()); }
	void  Purpose1 (int value)        { Key1 ((Mode () << 16) + value); }
	void  Purpose2 (int value)        { Key2 ((value << 16) + Increment ()); }
	void  Increment (int value)       { Key2 ((Purpose2 () << 16) + value); }
	void  Count (int value)           { count = value; }

	void  Add_Count (int value = 1)   { count += value; }

private:
	int count;
};

//---------------------------------------------------------
//	Trip_Length_Array class definition
//---------------------------------------------------------

class Trip_Length_Array : public Class2_Array, public Static_Service
{
public:
	Trip_Length_Array (int max_records = 0);

	bool Add (Trip_Length_Data *data = NULL)  { return (Class2_Array::Add (data)); }
	
	Trip_Length_Data * New_Record (bool clear = false, int number = 1)
	                                          { return ((Trip_Length_Data *) Class2_Array::New_Record (clear, number)); }

	Trip_Length_Data * Record (int index)     { return ((Trip_Length_Data *) Class2_Array::Record (index)); }
	Trip_Length_Data * Record (void)          { return ((Trip_Length_Data *) Class2_Array::Record ()); }

	Trip_Length_Data * First (void)           { return ((Trip_Length_Data *) Class2_Array::First ()); }
	Trip_Length_Data * Next (void)            { return ((Trip_Length_Data *) Class2_Array::Next ()); }
	
	Trip_Length_Data * First_Key (void)       { return ((Trip_Length_Data *) Class2_Array::First_Key ()); }
	Trip_Length_Data * Next_Key (void)        { return ((Trip_Length_Data *) Class2_Array::Next_Key ()); }

	Trip_Length_Data * operator[] (int index) { return (Record (index)); }

	//---- standard processing methods -----

	int  Increment (void)                     { return (increment); }
	void Increment (int value)                { increment = value; }

	bool Add_Trip (int mode, int purpose1, int purpose2, int length, int count = 1);

	bool Open_Trip_Length_File (char *filename, char *label = NULL);
	void Write_Trip_Length_File (void);

	//void Report (int number, char *title, char *key1, char *key2 = NULL);
	//void Header ();

private:
	bool output_flag;
	int increment;

	Db_File length_file;
};

#endif
