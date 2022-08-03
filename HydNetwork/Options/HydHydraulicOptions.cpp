/* HydHydraulicOptions 07/02/2021

 $$$$$$$$$£££$$$$$$$$$$$$$$$$$$$
 $   HydHydraulicOptions.cpp   $
 $$$$$$$$$$$£££$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 This class is a rewrite of original code by
 
 Dr M. B. Johns
 Computer Science
 College of Engineering, Mathematics & Physical Sciences 
 University of Exeter
 
*/

#ifndef _HYDHYDRAULICOPTIONS_H_
#include "HydHydraulicOptions.h"
#endif

std::string 
HydHydraulicOptions::unit(UnitParameter parameter) const
{
	std::string sUnit = "parameter not implemented yet";

	switch (parameter)
	{
	case UnitParameter::Demand:
		break;
	case UnitParameter::Elevation:
		if(m_flowUnits < 5)
			sUnit = "ft";
		else
			sUnit = "m";
		break;
	case UnitParameter::EmitterCoeff:
		break;
	case UnitParameter::Flow:
		switch (m_flowUnits)
		{
		case FlowUnits::Hyd_AFD:
			break;
		case FlowUnits::Hyd_CFS:
			sUnit = "ft^3/s";
			break;
		case FlowUnits::Hyd_CMD:
			break;
		case FlowUnits::Hyd_CMH:
			sUnit = "m^3/hr";
			break;
		case FlowUnits::Hyd_GPM:
			sUnit = "US gal/min";
			break;
		case FlowUnits::Hyd_IMGD:
			break;
		case FlowUnits::Hyd_LPM:
			break;
		case FlowUnits::Hyd_LPS:
			break;
		case FlowUnits::Hyd_MGD:
			break;
		case FlowUnits::Hyd_MLD:
			break;
		default:
			break;
		}
		break;
	case UnitParameter::HydraulicHead:
		if (m_flowUnits < 5)
			sUnit = "ft";
		else
			sUnit = "m";
		break;
	case UnitParameter::Length:
		if (m_flowUnits < 5)
			sUnit = "ft";
		else
			sUnit = "m";
		break;
	case UnitParameter::PipeDiameter:
		if (m_flowUnits < 5)
			sUnit = "in";
		else
			sUnit = "mm";
		break;
	case UnitParameter::Power:
		if (m_flowUnits < 5)
			sUnit = "hp";
		else
			sUnit = "kW";
		break;
	case UnitParameter::Pressure:
		if (m_flowUnits < 5)
			sUnit = "psi";
		else
			sUnit = "m";
		break;
	case UnitParameter::RoughnessCoeff:
		if (m_headlossFormula == HeadlossFormula::DarcyWeisbach)
		{
			if (m_flowUnits < 5)
				sUnit = "x10^-3 ft";
			else
				sUnit = "mm";
		}
		else
		{
			sUnit = "";//unitless
		}
		break;
	case UnitParameter::TankDiameter:
		if (m_flowUnits < 5)
			sUnit = "ft";
		else
			sUnit = "m";
		break;
	case UnitParameter::Velocity:
		if (m_flowUnits < 5)
			sUnit = "ft/s";
		else
			sUnit = "m/s";
		break;
	case UnitParameter::Volume:
		if (m_flowUnits < 5)
			sUnit = "ft^3";
		else
			sUnit = "m^3";
		break;
	default:
		break;
	}

	return sUnit;
}
