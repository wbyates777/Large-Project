/* HydNetComp 07/02/2021

 $$$$$$$$$$$$$$$$$$$$
 $   HydNetComp.h   $
 $$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 This class is a rewrite of original code by
 
 Dr M. B. Johns
 Computer Science
 College of Engineering, Mathematics & Physical Sciences 
 University of Exeter
 
*/


#ifndef _HydNetComp_H_
#define _HydNetComp_H_


#include <string>
#include <vector>
#include <algorithm>

#ifndef __HYDTYPES_H__
#include "HydTypes.h" 
#endif

class HydNetComp
{
public:
	HydNetComp(void) :  m_pos(-1),
                        m_index(-1), 
                        m_hydType(),
                        m_ID(),
                        m_sDescription(),
                        m_sTag(),
                        m_group("default"),
                        m_quality(),
                        m_isDecisionVariable(false) {}

    HydNetComp(const std::string& hid, HydType compType) : m_pos(-1),
                                                          m_index(-1), 
                                                          m_hydType(compType),
                                                          m_ID(hid),
                                                          m_sDescription(),
                                                          m_sTag(),
                                                          m_group("default"),
                                                          m_quality(),
                                                          m_isDecisionVariable(false) {}
    
    virtual ~HydNetComp(void)=default;

    HydType hydType(void) const { return m_hydType; }
    
    const std::string& ID(void) const { return m_ID; }
    void setID(const std::string& hid) { m_ID = hid; }

    int index(void) const { return m_index; };
    void setIndex(int index) { m_index = index; }
    
    int pos(void) const { return m_pos; };
    void setPos(int p) { m_pos = p; }

    const std::string& description(void) const { return m_sDescription; }
    void setDescription(const std::string& description) { m_sDescription = description; }
    
    const std::string& tag(void) const { return m_sTag; }
    void setTag(const std::string& tag) { m_sTag = tag; }
    
    const std::string& group(void) const { return m_group; }
    void setGroup(const std::string& g) { m_group = g; }
	
    const std::vector<HydFloat>& quality(void) const { return m_quality; }
    void addQuality(HydFloat quality) { m_quality.push_back(quality); }
    void setQuality(HydFloat quality) { m_quality = std::vector<HydFloat>(1,quality); }
    void setQuality(const std::vector<HydFloat> & quality) { m_quality = quality; }

    bool isDecisionVariable(void) const { return m_isDecisionVariable; }
    void setIsDecisionVariable(bool is_dv) { m_isDecisionVariable = is_dv; }

    virtual void clear(void)=0;
    
protected:
    	
    int m_pos;
	int m_index;
    HydType m_hydType;
    std::string m_ID;
	std::string m_sDescription;
	std::string m_sTag;
    std::string m_group;
    std::vector<HydFloat> m_quality;
    bool m_isDecisionVariable;
	
};

#endif
