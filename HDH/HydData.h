/* HydData 03/03/2021

 $$$$$$$$$$$$$$$$$
 $   HydData.h   $
 $$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 This class assumes all variables are ordered i.e they can be ordered using '<'
 
*/


#ifndef __HYDDATA_H__
#define __HYDDATA_H__



#include <string>
#include <vector>
#include <map>



#ifndef DATADOUBLE_H_
#include "DataDouble.h"
#endif


#ifndef __MATRIX_H__
#include "AMatrix.h"
#endif

class HydData : public ranger::DataDouble
{
public:

    HydData( void ): ranger::DataDouble() {}
    
    HydData( const std::vector<std::string>& names, 
             const Matrix<double>& inputs, 
             const Matrix<double>& outputs ): ranger::DataDouble() 
    {
        setData(names, inputs, outputs); 
    }
    
    ~HydData( void ) override=default;

    
    void
    setData( const std::vector<std::string>& names, 
             const Matrix<double>& inputs, 
             const Matrix<double>& outputs );
    

private:


};

#endif


