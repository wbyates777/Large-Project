/* Archive 18/07/2018

 $$$$$$$$$$$$$$$$$$$
 $   Archive.cpp   $
 $$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History: An solution container/memory for multi-objective optimisation
 
 Based on a Python implementation by 
 
 Dr. David Walker
 Postdoctoral Research Fellow, 
 Computer Science & Centre for Water Systems 
 College of Engineering, Mathematics & Physical Sciences 
 University of Exeter

*/


#ifndef __ARCHIVE_H__
#include "Archive.h"
#endif

#include <assert.h>

#ifndef __UTILS_H__
#include "AUtils.h"
#endif

void
Archive::clear( void )
{
    m_objVals.clear(); 
    m_archive.clear();
}

bool
Archive::dominates(const HHObjective& obj_x, const HHObjective& obj_y) const
// does obj_x dominate obj_y?
{
    // assert(obj_x.size() == obj_y.size());
    
    bool any = false;
    for (int i = 0; i < obj_x.size(); ++i)
    {
         // assume for positive vals and mimimization
         if (obj_x[i] > obj_y[i])
             return false;
         
         if (obj_x[i] < obj_y[i])
             any = true;
    }
    
    return any;
}


std::vector<ObjType>
Archive::max( void ) const
{
    std::vector<ObjType> retVal;
    
    if (m_objVals.empty())
        return retVal;
    
    retVal.resize(m_objVals[0].size(), std::numeric_limits<double>::min());
    
    for (int i = 0; i < m_objVals.size(); ++i)
    {
        for (int j = 0; j < m_objVals[i].size(); ++j)
        {
            retVal[j] = std::max(retVal[j], m_objVals[i][j]); 
        }
        
    }
    return retVal;
}

bool
Archive::update(const HHSolution& sol, const HHObjective& objVal, int iter )
{
    // Ensure that the solution does not already exist in the archive.
    for (int i = 0; i < m_archive.size(); ++i)
    {
        if (m_archive[i] == sol)
            return false;
    }
    
    // Identify any members of the archive which dominate the new solution.
    std::vector<int> indexes;
    for (int i = 0; i < m_objVals.size(); ++i)
    {
        if (dominates( m_objVals[i], objVal ))
            indexes.push_back(i);
    }
    
    if (indexes.empty())
    {
        // The solution is not dominated by the archive.  Remove any members of 
        // the archive which are dominated by the new solution.
        std::vector<HHSolution>  archive(1, sol);
        std::vector<HHObjective> objVals(1, objVal);
        
        for (int i = 0; i < m_objVals.size(); ++i)
        {
            if (!dominates( objVal, m_objVals[i] ))
            {
                archive.push_back(m_archive[i]);
                objVals.push_back(m_objVals[i]);
            }
        } 
        
        m_archive = archive;
        m_objVals = objVals;
        
        return true;
    }
    
    return false;
}



std::vector<double>
Archive::crowding( void ) const
// see for similar example http://gpbib.cs.ucl.ac.uk/gecco2005/docs/p257.pdf
/* 
 // tested with
 
 std::vector<HHObjective>   m_objVals = { 
     HHObjective({1.0,15.0}),
     HHObjective({2.0,14.5}),
     HHObjective({3.0,13.0}),
     HHObjective({4.0,11.5}),
     HHObjective({5.0,11.0})
 };

 */
{
    if (m_objVals.size() < 3)
        return std::vector<double>();
    
    int population_size =  (int) m_objVals.size();
    int number_of_objectives = (int) m_objVals[0].size();
    
    // normalise scores (ptp is max-min)
    std::vector<HHObjective> normed_scores(m_objVals);
    for (int i = 0; i < number_of_objectives; ++i)
    {
        ObjType min = m_objVals[0][i]; 
        ObjType max = m_objVals[0][i];
        for (int j = 0; j < population_size; ++j)
        {
            min = std::min(min, m_objVals[j][i]); 
            max = std::max(max, m_objVals[j][i]); 
        }
        if (max > min)
        {
            for (int j = 0; j < population_size; ++j)
            {
                normed_scores[j][i] = (m_objVals[j][i] - min) / (max - min);
            }
        }
    }
    
    // create crowding matrix of population (row) and score (column)
    Matrix<double> crowding_matrix(population_size, number_of_objectives, 0.0);
    
    // calculate crowding distance for each score in turn
    for (int i = 0; i < number_of_objectives; ++i)
    {
        std::vector<double> scores(population_size);
        for (int j = 0; j < population_size; ++j)
            scores[j] = normed_scores[j][i];
        
        // sort each score (to calculate crowding between adjacent scores)
        std::vector<int> order = util::ordering(scores);
        
        /// end points have max crowding distance
        std::vector<double> crowding(population_size, 0.0);
        crowding[0] = 1.0;
        crowding[population_size - 1] = 1.0;
        
        // calculate crowding distance for each individual
        for (int j = 1; j < population_size - 1; ++j)
            crowding[j] = scores[order[j+1]] - scores[order[j-1]];

        // record crowding distances
        for (int j = 0; j < population_size; ++j)
            crowding_matrix[j][i] = crowding[j];
    }
    
    // average crowding distances of each score
    std::vector<double> crowding_distances(population_size, 0.0);
    for (int i = 0; i < population_size; ++i)
    {
        for (int j = 0; j < number_of_objectives; ++j)
            crowding_distances[i] += crowding_matrix[i][j];
        
        crowding_distances[i] /= (number_of_objectives);
    }

    return crowding_distances;
}
