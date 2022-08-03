/* HydTimeOptions 07/02/2021

 $$$$$$$$$$$$$$$$$$$$$$$$
 $   HydTimeOptions.h   $
 $$$$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 This class is a rewrite of original code by
 
 Dr M. B. Johns
 Computer Science
 College of Engineering, Mathematics & Physical Sciences 
 University of Exeter
 
*/

#ifndef _HYDTIMEOPTIONS_H_
#define _HYDTIMEOPTIONS_H_

enum class StatisticOption
{
	None,
	Average,
	Minimum,
	Maximum,
	Range
};


class HydTimeHrMin
{
public:
    HydTimeHrMin(void) : hours(0), minutes(0) {} 
	int hours;
	int minutes;
};


class HydTimeOptions
{
public:
    HydTimeOptions(void)=default;

    HydTimeHrMin& 
    totalDuration(void)  { return m_totalDuration; }

    HydTimeHrMin& 
    hydraulicTimeStep(void) { return m_hydraulicTimeStep; }

    HydTimeHrMin& 
    qualityTimeStep(void) { return m_qualityTimeStep; }

    HydTimeHrMin& 
    patternTimeStep(void)  { return m_patternTimeStep; }

    HydTimeHrMin& 
    patternStartTime(void) { return m_patternStartTime; }

    HydTimeHrMin& 
    reportingTimeStep(void) { return m_reportingTimeStep; }

    HydTimeHrMin& 
    reportStartTime(void) { return m_reportStartTime; }

    HydTimeHrMin& 
    clockStartTime(void) { return m_clockStartTime; }

    StatisticOption 
    statisticOption(void) const { return m_statisticOption; }

    void 
    setStatisticOption(StatisticOption statistic_option) {  m_statisticOption = statistic_option; }

private:
    
	HydTimeHrMin m_totalDuration;
	HydTimeHrMin m_hydraulicTimeStep;
	HydTimeHrMin m_qualityTimeStep;
	HydTimeHrMin m_patternTimeStep;
	HydTimeHrMin m_patternStartTime;
	HydTimeHrMin m_reportingTimeStep;
	HydTimeHrMin m_reportStartTime;
	HydTimeHrMin m_clockStartTime;
	
	StatisticOption m_statisticOption;

};

#endif // !_HYDTIMEOPTIONS_H_

