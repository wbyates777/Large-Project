/* HydQualityOptions 07/02/2021

 $$$$$$$$$$$$$$$$$$$$$$$$$$$
 $   HydQualityOptions.h   $
 $$$$$$$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 This class is a rewrite of original code by
 
 Dr M. B. Johns
 Computer Science
 College of Engineering, Mathematics & Physical Sciences 
 University of Exeter
 
*/

#ifndef _HYDQUALITYOPTIONS_H_
#define _HYDQUALITYOPTIONS_H_


#ifndef __HYDTYPES_H__
#include "HydTypes.h"
#endif

enum class QualityParameter
{
	None,
	Chemical,
	Trace,
	Age
};

enum class MassUnit
{
	mgPerL,
	ugPerL
};

class HydQualityOptions
{
public:
    
    HydQualityOptions(void)=default;

    QualityParameter 
    parameter(void) const { return m_parameter; }

    void 
    setParameter(QualityParameter parameter) {  m_parameter = parameter; }
    
    MassUnit
    massUnits(void) const { return m_massUnits; }

    void 
    setMassUnits(MassUnit units) { m_massUnits = units;  }
    
    HydFloat 
    relativeDiffusivity(void) const  { return m_relativeDiffusivity;  }

    void 
    setRelativeDiffusivity(HydFloat relative_diffusivity) { m_relativeDiffusivity = relative_diffusivity; }

    HydFloat 
    qualityTolerance(void) const { return m_qualityTolerance; }

    void 
    setQualityTolerance(HydFloat quality_tolerance) { m_qualityTolerance = quality_tolerance; }
    
    const std::string& 
    traceNode(void) const { return m_sTraceNode; }

    void 
    setTraceNode(const std::string& trace_node) { m_sTraceNode = trace_node; }

private:
    
	QualityParameter m_parameter;
	MassUnit m_massUnits;

	HydFloat m_relativeDiffusivity;
	HydFloat m_qualityTolerance;
	
	std::string m_sTraceNode;
};

#endif // !_HYDQUALITYOPTIONS_H_

