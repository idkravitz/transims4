//*********************************************************
//	Stage2_IPF_Data.hpp - Two Stage IPF data class
//*********************************************************

#ifndef STAGE2_IPF_DATA_HPP
#define STAGE2_IPF_DATA_HPP

#include "IPF_Data.hpp"
#include "PUMA_Data.hpp"

//---------------------------------------------------------
//	Stage2_IPF_Data class definition
//---------------------------------------------------------

class Stage2_IPF_Data : public IPF_Data
{
public:
	Stage2_IPF_Data (void);

	bool Set_Stage_Two (IPF_Data *ptr);
	bool Copy_Stage_One (PUMA_Data_Array *ptr);
	bool Save_Shares (PUMA_Data_Array *ptr);

private:

};
#endif
