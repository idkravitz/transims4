//*********************************************************
//	Equation.hpp - volume-delay equation array
//*********************************************************

#ifndef EQUATION_HPP
#define EQUATION_HPP

#include "Data_Array.hpp"
#include "Static_Service.hpp"

//---------------------------------------------------------
//	Equation_Array Class definition
//---------------------------------------------------------

class Equation_Array : public Data_Array, public Static_Service
{
public:
	Equation_Array (int max_records = 0);
	virtual ~Equation_Array (void);

	bool Num_Equations (int number)					{ return (Num_Records (number)); }
	int  Num_Equations (void)						{ return (Num_Records ()); }

	bool Add_Equation (int type, char *parameters);

	int  Apply_Equation (int type, int time0, int volume, int capacity, int length);

protected:


private:
	enum Equation_Type { BPR, BPR_PLUS, EXP, CONICAL, AKCELIK };

	typedef struct {
		Equation_Type type;
		double a;
		double b;
		double c;
		double d;
	} Equation;

	Equation * First (void)                { return ((Equation *) Data_Array::First ()); }
	Equation * Next (void)                 { return ((Equation *) Data_Array::Next ()); }

	Equation * operator[] (int index)      { return ((Equation *) Record (index)); }
};

#endif
