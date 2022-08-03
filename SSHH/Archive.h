/* Archive 18/07/2018

 $$$$$$$$$$$$$$$$$
 $   Archive.h   $
 $$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History: A solution archive/memory for multi-objective optimisation algorithms
 
 Based on a Python implementation by 
 
 Dr. David Walker
 Postdoctoral Research Fellow, 
 Computer Science & Centre for Water Systems 
 College of Engineering, Mathematics & Physical Sciences 
 University of Exeter

*/


#ifndef __ARCHIVE_H__
#define __ARCHIVE_H__

#ifndef __HHPROBLEM_H__
#include "HHProblem.h"
#endif

#ifndef __MATRIX_H__
#include "AMatrix.h"
#endif

#include <vector>



class Archive
{
public:
    
    Archive( void ): m_archive(), m_objVals() {}
    ~Archive( void )=default;
     
    bool
    update( const HHSolution& sol, const HHObjective& objVal, int iter );
    
    void
    clear( void );
    
    const HHSolution&
    operator[]( int idx ) const { return m_archive[idx]; }
    
    const HHObjective&
    operator()( int idx ) const { return m_objVals[idx]; }
    
    const std::vector<HHObjective>&
    values( void ) const { return m_objVals; }
    
    int
    size( void ) const { return (int) m_archive.size(); }
    
    // does obj1 dominate obj2? (assume less than as we are minimizing)
    bool
    dominates( const HHObjective& obj1, const HHObjective& obj2 ) const;
    
    
    std::vector<ObjType>
    max( void ) const;
    
    std::vector<double>
    crowding( void ) const;
    
private:
    
    std::vector<HHObjective> m_objVals; 
    std::vector<HHSolution>  m_archive;

};

#endif


