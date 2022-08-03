/* HydData 03/03/2021

 $$$$$$$$$$$$$$$$$$$
 $   HydData.cpp   $
 $$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

*/


#ifndef __HYDDATA_H__
#include "HydData.h"
#endif

#include "globals.h"
#include "ArgumentHandler.h"
#include "ForestClassification.h"
#include "ForestRegression.h"
#include "ForestSurvival.h"
#include "ForestProbability.h"
#include "utility.h"

using namespace ranger;

void
HydData::setData(  const std::vector<std::string>& names, const Matrix<double>& inputs, const Matrix<double>& outputs )
{
    variable_names = names;
    num_rows = inputs.rows();
    num_cols = inputs.cols();
    num_cols_no_snp = num_cols;

    reserveMemory(outputs.cols());
    
    std::vector<bool> is_ordered(inputs.cols() + outputs.cols(), true);
    setIsOrderedVariable(is_ordered);

    bool error = false;
    
    for (int i = 0; i < inputs.rows(); ++i)
        for (int j = 0; j < inputs.cols(); ++j)
            set_x(j, i, inputs[i][j], error);
    
    for (int i = 0; i < outputs.rows(); ++i)
        for (int j = 0; j < outputs.cols(); ++j)
            set_y(j, i, outputs[i][j], error);
}
