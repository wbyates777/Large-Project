/* HydLookAhead 21/05/2021

 $$$$$$$$$$$$$$$$$$$$$$
 $   HydLookAhead.h   $
 $$$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:
 
 This contains all the info required by a look ahead request for a particular pipe
 Stored as a member of HydNetwork

*/


#ifndef __HYDLOOKAHEAD_H__
#define __HYDLOOKAHEAD_H__

#ifndef __HYDTYPES_H__
#include "HydTypes.h" 
#endif

#include <string>
#include <vector>

typedef std::vector<HydFloat> HydObjectives;

class HydLookAhead
{
public:

    HydLookAhead( void )=default;
    ~HydLookAhead( void )=default;
    
    const std::string& 
    currency(void) const { return m_currency; }  
    
    void 
    setCurrency(const std::string& currency) { m_currency = currency; }  
    
    
    const std::string&  
    selectedPipeId(void) const { return m_selectedPipeId; } 
    
    void 
    setSelectedPipeId(const std::string& pipe_id)  { m_selectedPipeId = pipe_id; } 

    
    const std::string&  
    diamUnits(void) const { return m_diamUnits; } 
    
    void 
    setDiamUnits(const std::string& diameter_unit) { m_diamUnits = diameter_unit; } 

    
    int 
    selectedDiamIdx(void) const { return m_selectedDiamIdx; } 
    
    void 
    setSelectedDiamIdx(int index) { m_selectedDiamIdx = index; } 

    
    const std::vector<HydFloat>& 
    diameters(void) const { return m_vDiameters; } 
    
    void
    setDiameters(const std::vector<HydFloat>& d) { m_vDiameters = d; } 
    

    const std::vector<HydFloat>& 
    diametersCost(void) const { return m_vDiametersCost; } 
    
    void 
    setDiametersCost(const std::vector<HydFloat>&  dc) { m_vDiametersCost = dc; } 
    
    
    const std::vector<std::string>& 
    objNames(void) const { return m_vObjectiveNames; } 
    
    void
    setObjNames(const std::vector<std::string>& on) { m_vObjectiveNames = on; } 
    
    
    const std::vector<std::string>& 
    objUnits(void) const { return m_vObjectiveUnits; } 
    
    void
    setObjUnits(const std::vector<std::string>& ou) { m_vObjectiveUnits = ou; } 
    
    
    const std::vector<std::vector<HydObjectives>>& 
    objValues(void) const { return m_vObjectiveValues; } 
    
    void 
    setObjValues(const std::vector<std::vector<HydObjectives>>& ov) { m_vObjectiveValues = ov; } 
    
    const std::vector<HydObjectives>& 
    objValuesCurrent(void) const { return m_vObjectiveValuesCurrent; } 

    void 
    setObjValuesCurrent(const std::vector<HydObjectives>& obj_cur) { m_vObjectiveValuesCurrent = obj_cur; } 
    
    
    const std::vector<HydObjectives>& 
    objMin(void) const { return m_vObjectiveMin; } 
    
    void 
    setObjMin(const std::vector<HydObjectives>& omin) { m_vObjectiveMin = omin; } 
    
    
    const std::vector<HydObjectives>& 
    objMax(void) const { return m_vObjectiveMax; } 
    
    void
    setObjMax(const std::vector<HydObjectives>& omax) { m_vObjectiveMax = omax; } 
    
    
    
    const std::vector<bool>& 
    objMinimise(void) const { return m_vObjectiveMinimise; } 
    
    void 
    setObjMinimise(const std::vector<bool>& omin ) { m_vObjectiveMinimise = omin; }

private:

    int m_selectedDiamIdx;
    
    std::string m_currency;
    std::string m_selectedPipeId;
    std::string m_diamUnits;
    
    std::vector<std::string> m_vObjectiveNames;
    std::vector<std::string> m_vObjectiveUnits;
    
    std::vector<HydFloat> m_vDiameters;
    std::vector<HydFloat> m_vDiametersCost;
    
    
    
    std::vector<HydObjectives> m_vObjectiveMin;
    std::vector<HydObjectives> m_vObjectiveMax;
    std::vector<HydObjectives> m_vObjectiveValuesCurrent;

    std::vector< std::vector<HydObjectives> > m_vObjectiveValues;

    std::vector<bool> m_vObjectiveMinimise;

};

#endif


