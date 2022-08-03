/* HydHydraulicOptions 07/02/2021

 $$$$$$$$$£££$$$$$$$$$$$$$$$$$
 $   HydHydraulicOptions.h   $
 $$$$$$$$$$$£££$$$$$$$$$$$$$$$

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
#define _HYDHYDRAULICOPTIONS_H_

#include <string>

#ifndef __HYDTYPES_H__
#include "HydTypes.h"
#endif

// WBY - Ive added Hyd_ to prevent a clash with EPANET enums
enum  FlowUnits
{
	Hyd_CFS,
    Hyd_GPM,
    Hyd_MGD,
    Hyd_IMGD,
    Hyd_AFD,
    Hyd_LPS,
    Hyd_LPM,
    Hyd_MLD,
    Hyd_CMH,
    Hyd_CMD
};

enum class HeadlossFormula
{
	HazenWilliams,
	DarcyWeisbach,
	ChezyManning
};

enum class UnbalancedAction
{
	Stop,
	Continue
};

enum class StatusReportAction
{
	No,
	Yes,
	Full
};

enum class UnitParameter
{
	Demand,
	PipeDiameter,
	TankDiameter,
	Elevation,
	EmitterCoeff,
	Flow,
	HydraulicHead,
	Length,
	Power,
	Pressure,
	RoughnessCoeff,
	Velocity,
	Volume
};

class HydHydraulicOptions
{
public:
    HydHydraulicOptions(void)=default;


    FlowUnits 
    flowUnits(void) const { return m_flowUnits; }

    void 
    setFlowUnits(FlowUnits units) { m_flowUnits = units; }
    
    HeadlossFormula 
    headlossFormula(void) const { return m_headlossFormula; }

    void 
    setHeadlossFormula(HeadlossFormula formula) { m_headlossFormula = formula; }
    
    UnbalancedAction 
    unbalancedAction(void) const { return m_unbalancedAction; }

    void
    setUnbalancedAction(UnbalancedAction action) { m_unbalancedAction = action; }
    
    StatusReportAction 
    statusReport(void) const { return m_statusReport; }

    void 
    setStatusReport(StatusReportAction action) { m_statusReport = action; }
    
    HydFloat 
    specificGravity(void) const { return m_specificGravity; }

    void 
    setSpecificGravity(HydFloat specific_gravity) { m_specificGravity = specific_gravity; }
    
    HydFloat 
    relativeViscosity(void) const { return m_relativeViscosity; }

    void 
    setRelativeViscosity(HydFloat relative_viscosity) { m_relativeViscosity = relative_viscosity; }

    HydFloat 
    accuracy(void) const { return m_accuracy; }
    
    void 
    setAccuracy(HydFloat accuracy) {  m_accuracy = accuracy; }

    HydFloat 
    demandMultiplyer(void) const { return m_demandMultiplyer; }
    
    void 
    setDemandMultiplyer(HydFloat demand_multiplyer) { m_demandMultiplyer = demand_multiplyer; }
    
    HydFloat 
    emitterExponent(void) const { return m_emitterExponent; }

    void 
    setEmitterExponent(HydFloat emitter_exponent) { m_emitterExponent = emitter_exponent; }
    
    int
    maximumTrials(void) const { return m_maximumTrials; }

    void 
    setMaximumTrials(int maximum_trials) { m_maximumTrials = maximum_trials; }
    
    const std::string& 
    defaultPattern(void) const { return m_sDefaultPattern; }

    void 
    setDefaultPattern(const std::string& default_pattern) {  m_sDefaultPattern = default_pattern;  }

	std::string 
    unit(UnitParameter parameter) const;

private:
    
	FlowUnits m_flowUnits;
	HeadlossFormula m_headlossFormula;
	UnbalancedAction m_unbalancedAction;
	StatusReportAction m_statusReport;
	
	HydFloat m_specificGravity;
	HydFloat m_relativeViscosity;
	HydFloat m_accuracy;
	HydFloat m_demandMultiplyer;
	HydFloat m_emitterExponent;
	
	int m_maximumTrials;

	std::string m_sDefaultPattern;
};

#endif
