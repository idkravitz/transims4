//*********************************************************
//	VehGen.hpp - generate vehicles for households
//*********************************************************

#ifndef VEHGEN_HPP
#define VEHGEN_HPP

#include "Demand_Service.hpp"
#include "Index_Array.hpp"

//---------------------------------------------------------
//	VehGen - execution class definition
//---------------------------------------------------------

class VehGen : public Demand_Service
{
public:

	VehGen (void);
	virtual ~VehGen (void);

	virtual void Execute (void);

	virtual bool Access_Processing (Db_File *file);
	virtual bool Household_Processing (Db_File *file);

protected:

	virtual void Program_Control (void);

private:

	bool vehicle_flag;

	typedef struct {
		int location;
		int parking;
	} Parking_Lot;

	Index_Array parking_lot;

	int num_vehicle, num_hhold, num_problem;

};
#endif
