//*********************************************************
//	Calculate.cpp - Calculate Emissions
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Calculate_Emissions
//---------------------------------------------------------

void Emissions::Calculate_Emissions (int type, int vehicle, double vmt, double vht, double speed)
{
	//int i, year, season, low, high;
	//double factor, units;

	//EMISSION_DATA *ptr;
	//EMISSION_RATE *rate, *rate2;

	//low = high = -1;

	//for (i=0; i < nclasses; i++) {
	//	if (classes [i].type == type) {
	//		if (classes [i].speed <= speed) {
	//			if (low < 0) {
	//				low = i;
	//			} else if (classes [i].speed > classes [low].speed) {
	//				low = i;
	//			}
	//		} else if (high < 0) {
	//			high = i;
	//		} else if (classes [i].speed < classes [high].speed) {
	//			high = i;
	//		}
	//	}
	//}
	//if (low < 0 && high < 0) return;
	//if (low < 0) low = high;
	//if (high < 0) high = low;
	//if (low != high) {
	//	factor = classes [high].speed - classes [low].speed;
	//	if (factor > 0.0) {
	//		factor = (speed - classes [low].speed) / factor;
	//	}
	//} else {
	//	factor = 0.0;
	//}
	//units = (vmt < vht) ? vht : vmt;

	//for (year=0; year < nyears; year++) {
	//	for (season=0; season < nseasons; season++) {

	//		ptr = data [year] [season] [vehicle] + type;
	//		rate = rates [year] [season] [vehicle] + low;

	//		ptr->vmt += vmt;
	//		ptr->vht += vht;

	//		ptr->voc += units * rate->voc;
	//		ptr->co += units * rate->co;
	//		ptr->nox += units * rate->nox;

	//		if (factor > 0.0) {
	//			rate2 = rates [year] [season] [vehicle] + high;

	//			ptr->voc += units * factor * (rate2->voc - rate->voc);
	//			ptr->co += units * factor * (rate2->co - rate->co);
	//			ptr->nox += units * factor * (rate2->nox - rate->nox);
	//		}
	//	}
	//}
	//return;
}
