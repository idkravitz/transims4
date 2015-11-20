//*********************************************************
//	Program_Data.hpp - Processes User Program Data
//*********************************************************

#ifndef PROGRAM_DATA_HPP
#define PROGRAM_DATA_HPP

#include "Data_Array.hpp"
#include "Index_Array.hpp"

#pragma pack (push, 4)

//---------------------------------------------------------
//	Stack_Data Array
//---------------------------------------------------------

typedef struct {
	int  type;
	int  length;
	union {
		int    lvalue;
		double fvalue;
		char * svalue;
	};
} Stack_Data;

class Stack_Array : public Data_Array
{
public:
	Stack_Array (int max_records = 0);

	bool Add (Stack_Data *data = NULL)       { return (Data_Array::Add (data)); }

	bool Set (int index, Stack_Data *data)   { return (Data_Array::Set (index, data)); }

	Stack_Data * Record (int index)          { return ((Stack_Data *) Data_Array::Record (index)); }
	Stack_Data * Record (void)               { return ((Stack_Data *) Data_Array::Record ()); }

	Stack_Data * First (void)                { return ((Stack_Data *) Data_Array::First ()); }
	Stack_Data * Next (void)                 { return ((Stack_Data *) Data_Array::Next ()); }
 
	Stack_Data * operator[] (int index)      { return (Record (index)); }
};

//---------------------------------------------------------
//	Command_Data Array
//---------------------------------------------------------

typedef struct {
	int  type;
	int  token;
	int  value;
} Command_Data;

class Command_Array : public Data_Array
{
public:
	Command_Array (int max_records = 0);

	bool Add (Command_Data *data = NULL)      { return (Data_Array::Add (data)); }

	bool Set (int index, Command_Data *data)  { return (Data_Array::Set (index, data)); }

	Command_Data * Record (int index)         { return ((Command_Data *) Data_Array::Record (index)); }
	Command_Data * Record (void)              { return ((Command_Data *) Data_Array::Record ()); }

	Command_Data * First (void)               { return ((Command_Data *) Data_Array::First ()); }
	Command_Data * Next (void)                { return ((Command_Data *) Data_Array::Next ()); }

	Command_Data * operator[] (int index)     { return (Record (index)); }
};

//---------------------------------------------------------
//	Token_Data Array
//---------------------------------------------------------

typedef struct {
	char * name;			//----	text string
	int	   type;			//----	token type
	int	   code;			//----	token code
} Token_Data;

class Token_Array : public Data_Array
{
public:
	Token_Array (int max_records = 0);

	bool Add (Token_Data *data = NULL)      { return (Data_Array::Add (data)); }

	bool Set (int index, Token_Data *data)  { return (Data_Array::Set (index, data)); }
	
	Token_Data * Record (int index)         { return ((Token_Data *) Data_Array::Record (index)); }
	Token_Data * Record (void)              { return ((Token_Data *) Data_Array::Record ()); }

	Token_Data * First (void)               { return ((Token_Data *) Data_Array::First ()); }
	Token_Data * Next (void)                { return ((Token_Data *) Data_Array::Next ()); }

	Token_Data * operator[] (int index)     { return (Record (index)); }

	virtual void Clear (void);
};

//---------------------------------------------------------
//	Integer Array
//---------------------------------------------------------

class Integer_Array : public Data_Array
{
public:
	Integer_Array (int max_records = 0);

	bool Add (int data)            { return (Data_Array::Add (&data)); }

	bool Set (int index, int data) { return (Data_Array::Set (index, &data)); }	

	int * Record (int index)       { return ((int *) Data_Array::Record (index)); }

	int * First (void)             { return ((int *) Data_Array::First ()); }
	int * Next (void)              { return ((int *) Data_Array::Next ()); }

	int operator[] (int index)     { return (Check_Pointer (Record (index))); }

	int Check_Pointer (void *ptr)  { return ((ptr) ? *((int *) ptr) : 0); }
};

//---------------------------------------------------------
//	Double_Array
//---------------------------------------------------------

class Double_Array : public Data_Array
{
public:
	Double_Array (int max_records = 0);

	bool Add (double data)            { return (Data_Array::Add (&data)); }
	
	bool Set (int index, double data) { return (Data_Array::Set (index, &data)); }	

	double * Record (int index)       { return ((double *) Data_Array::Record (index)); }

	double * First (void)             { return ((double *) Data_Array::First ()); }
	double * Next (void)              { return ((double *) Data_Array::Next ()); }

	double operator[] (int index)     { return (Check_Pointer (Record (index))); }

	double Check_Pointer (void *ptr)  { return ((ptr) ? *((double *) ptr) : 0); }
};

//---------------------------------------------------------
//	Svalue_Data Array
//---------------------------------------------------------

typedef struct {
	char * string;
	int    length;
} Svalue_Data;

class Svalue_Array : public Data_Array
{
public:
	Svalue_Array (int max_records = 0);

	bool Add (Svalue_Data *data)             { return (Data_Array::Add (data)); }
	
	bool Set (int index, Svalue_Data *data)  { return (Data_Array::Set (index, data)); }	

	Svalue_Data * Record (int index)         { return ((Svalue_Data *) Data_Array::Record (index)); }

	Svalue_Data * First (void)               { return ((Svalue_Data *) Data_Array::First ()); }
	Svalue_Data * Next (void)                { return ((Svalue_Data *) Data_Array::Next ()); }

	Svalue_Data * operator[] (int index)     { return (Record (index)); }

	virtual void Clear (void);
};

//---------------------------------------------------------
//	Svalue_Index Array
//---------------------------------------------------------

typedef struct {
	int  code;
	int  index;
} Svalue_Index;

class Svalue_Index_Array : public Index_Array
{
public:
	Svalue_Index_Array (int max_records = 0);

	bool Add (Svalue_Index *data)             { return (Index_Array::Add (data)); }
	
	bool Set (int index, Svalue_Index *data)  { return (Index_Array::Set (index, data)); }	

	Svalue_Index * Get (int code)             { return ((Svalue_Index *) Index_Array::Get (&code)); }

	Svalue_Index * Record (int index)         { return ((Svalue_Index *) Index_Array::Record (index)); }

	Svalue_Index * First (void)               { return ((Svalue_Index *) Index_Array::First ()); }
	Svalue_Index * Next (void)                { return ((Svalue_Index *) Index_Array::Next ()); }

	Svalue_Index * operator[] (int index)     { return (Record (index)); }
};
#pragma pack (pop)
#endif
