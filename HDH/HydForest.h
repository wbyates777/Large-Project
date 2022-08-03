/* HydForest 04/03/2021

 $$$$$$$$$$$$$$$$$$$
 $   HydForest.h   $
 $$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 A wrapper for the random forest implementation 'ranger'
 
 https://github.com/imbs-hl/ranger
 
 https://cran.r-project.org/web/packages/ranger/ranger.pdf
 
 At the moment I can't find a way of clearing the ranger structure
 after learning/loading for loading/learning again. This is why there
 is lots of error handling and logging
 
 TODO: remove Matrix<double> and use std::vector<std::vector<double>>>
 TODO: work out how to .clear() forest structure
 
*/


#ifndef __HYDFOREST_H__
#define __HYDFOREST_H__

#ifndef FOREST_H_
#include "Forest.h"
#endif

#ifndef FORESTCLASSIFICATION_H_
#include "ForestClassification.h"
#endif

#ifndef __HDHCONFIG_H__
#include "HDHConfig.h"
#endif

#ifndef __MATRIX_H__
#include "AMatrix.h"
#endif

#ifndef __LOGGER_H__
#include "ALogger.h"
#endif

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <memory>



class HydForest : private ranger::ForestClassification
{
public:

    HydForest( void );
    HydForest( const HDHConfig& config );
    virtual ~HydForest( void ) override {}

    
    // the using directive here introduces init() 
    // into this scope so overloading between base *and* subclass works
    using ranger::ForestClassification::init;
    
    // this must be called before a load, learn, or predict
    void
    init( const std::string& forestName, 
          const std::vector<std::string>& in_names, 
          const std::vector<std::string>& out_names );
    
    void
    init( const HDHConfig& config );
   
    //
    
    const std::string&
    name( void ) const { return m_config.name(); }
    
    int
    inputs( void ) const { return (int) m_config.inputNames().size(); }
    
    int
    outputs( void ) const { return 1; } 
    
    unsigned int 
    seed(void) const { return m_seed; }
    
    void 
    seed(unsigned int s) { m_seed = s; }
    //
    
    bool 
    learn( const Matrix<double>& inputs, const Matrix<double>& outputs );
    
    std::vector<double>
    predict( std::vector<double>& inputs );
    
    bool 
    load( const std::string& name = "" );
    
    bool 
    save( const std::string& name = "" );
    
    
protected:

    HydForest( const HydForest& )=delete;

    HydForest&
    operator=( const HydForest& )=delete;
    
    unsigned int m_seed;
    int m_haveForest;

    HDHConfig m_config;
    
    std::ostringstream m_trace;
    
    mutable Logger m_logger;
};

#endif


