/* HydTank 07/02/2021

 $$$$$$$$$$$$$$$$$
 $   HydTank.h   $
 $$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 This class is a rewrite of original code by
 
 Dr M. B. Johns
 Computer Science
 College of Engineering, Mathematics & Physical Sciences 
 University of Exeter
 
*/

#ifndef _HYDTANK_H_
#define _HYDTANK_H_

#ifndef _HYDSTORAGENODE_H_
#include "HydStorageNode.h"
#endif


class HydTank : public HydStorageNode
{
public:
    HydTank(void) : HydStorageNode("NONE", HydType::Tank),
                    m_initLevel(0.0),
                    m_minLevel(0.0),
                    m_maxLevel(0.0),
                    m_diameter(0.0),
                    m_minVolume(0.0),
                    m_mixingFraction(0.0),
                    m_reactionCoeff(0.0),
                    m_baseElevation(0.0),
                    m_sVolumeCurveID(),
                    m_mixingModel() {}
    
	HydTank(const std::string& hid): HydStorageNode(hid, HydType::Tank),
                                    m_initLevel(0.0),
                                    m_minLevel(0.0),
                                    m_maxLevel(0.0),
                                    m_diameter(0.0),
                                    m_minVolume(0.0),
                                    m_mixingFraction(0.0),
                                    m_reactionCoeff(0.0),
                                    m_baseElevation(0.0),
                                    m_sVolumeCurveID(),
                                    m_mixingModel() {}

    HydFloat initLevel(void) const { return m_initLevel; }
    void setInitLevel(HydFloat init_level) { m_initLevel = init_level; }
    
    HydFloat minLevel(void) const { return m_minLevel; }
    void setMinLevel(HydFloat min_level) { m_minLevel = min_level; }
    
    HydFloat maxLevel(void) const { return m_maxLevel; }
    void setMaxLevel(HydFloat max_level) { m_maxLevel = max_level; }
    
    HydFloat diameter(void) const { return m_diameter; }
    void setDiameter(HydFloat diameter) { m_diameter = diameter; }
    
    HydFloat minVolume(void) const { return m_minVolume; }
    void setMinVolume(HydFloat min_volume) { m_minVolume = min_volume; }
    
    HydFloat mixingFraction(void) const { return m_mixingFraction; }
    void setMixingFraction(HydFloat mixing_fraction) { m_mixingFraction = mixing_fraction; }
    
    HydFloat reactionCoeff(void) const { return m_reactionCoeff; }
    void setReactionCoeff(HydFloat reaction_coeff) { m_reactionCoeff = reaction_coeff; }
    
    HydFloat baseElevation(void) const { return m_baseElevation; }
    void setBaseElevation(HydFloat base_elevation) { m_baseElevation = base_elevation; }
    
    const std::string& volumeCurveID(void) const { return m_sVolumeCurveID; }
    void setVolumeCurveID(const std::string& volume_curve_id) { m_sVolumeCurveID = volume_curve_id; }
    
    MixingModel mixingModel(void) const { return m_mixingModel; }
    void setMixingModel(MixingModel mixing_model) { m_mixingModel = mixing_model; }

    void
    clear(void) override 
    {  
        m_pressure.clear();
        m_netInflow.clear(); 
        m_quality.clear();
    }
    
private:
    
	HydFloat m_initLevel;
	HydFloat m_minLevel;
	HydFloat m_maxLevel;
	HydFloat m_diameter;
	HydFloat m_minVolume;
	HydFloat m_mixingFraction;
	HydFloat m_reactionCoeff;
	HydFloat m_baseElevation;
	std::string m_sVolumeCurveID;
	MixingModel m_mixingModel;

};




#endif
