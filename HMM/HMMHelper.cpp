/* HMMHelper 19/05/2017

 $$$$$$$$$$$$$$$$$$$$$
 $   HMMHelper.cpp   $
 $$$$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

*/


#ifndef __HMMHELPER_H__
#include "HMMHelper.h"
#endif


 
bool
HMMHelper::addInitialState( HMM& model )
{
    HMMStates states    = model.states();
    HMMEmissions emissions = model.emissions();
    
    std::vector<double> nu   = model.initial_probs();         
    Matrix<double> trans     = model.trans_probs();
    
    int state_num            = (int) states.size();

    states      = addInitialState(states);
    emissions   = addInitialState(emissions);
 
    nu          = initialState_nu(state_num + 1); // initial state probs
    trans       = addInitialState(trans);
    
    std::vector<Matrix<double>> emiss;
    for (int i = 0; i < model.emiss_probs().size(); ++i)
    {
        Matrix<double> e = model.emiss_probs(i); 
        
        e = addInitialEmission(e);
        
        emiss.push_back( e );
        
    }
    
    int old = model.getLogger()->level();
    model.getLogger()->level(0);
    

    bool ok = model.set(states, emissions, nu, trans, emiss);  
    
    model.getLogger()->level(old);
    return ok;
}

bool
HMMHelper::delInitialState( HMM& model )
{
    HMMStates states    = model.states();
    HMMEmissions emissions = model.emissions();
    
    std::vector<double> initial_probs  = model.initial_probs();          
    Matrix<double> trans = model.trans_probs();

    int state_num = (int) states.size();
    
    states        = delInitialState(states);
    emissions     = delInitialState(emissions);
    initial_probs = equi_nu(state_num - 1); // equi state probs
    trans         = delInitialState(trans);

    std::vector<Matrix<double>> emiss;
    for (int i = 0; i < model.emiss_probs().size(); ++i)
    {
        Matrix<double> e = model.emiss_probs(i); 
        
        e = delInitialEmission(e);
        
        emiss.push_back( e );

    }

    int old = model.getLogger()->level();
    model.getLogger()->level(0);
    
    bool ok = model.set(states, emissions, initial_probs, trans, emiss);
    
    model.getLogger()->level(old);
    return ok;   
}

std::vector<double>
HMMHelper::equi_nu( int size )
{
    return std::vector<double>(size, 1.0 / double(size));
}

std::vector<double>
HMMHelper::initialState_nu( int size )
{
    std::vector<double> retVal(size, 0.0);
    retVal[size-1] = 1.0;
    return retVal;
}


//
Matrix<double>
HMMHelper::identity( int size, double diag, double offdiag  )
{
    Matrix<double> retVal(size, size, offdiag);
    for (int i = 0; i < size; ++i)
        retVal[i][i] = diag;
    return retVal;
}

Matrix<double>
HMMHelper::equi( int rows, int cols )
{
    Matrix<double> retVal(rows, cols, 0.0);
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            retVal[i][j] = 1.0 / double(cols);
    return retVal;
}

Matrix<double>
HMMHelper::addInitialState( const Matrix<double>& arg )
{
    Matrix<double> retVal(arg.rows() + 1, arg.cols() + 1, 0.0);
    
    for (int i = 0; i < arg.rows(); ++i)
        for (int j = 0; j < arg.cols(); ++j)
            retVal[i][j] = arg[i][j];
    
    retVal[arg.rows()][arg.cols()] = 0.0;
    for (int j = 0; j < arg.cols(); ++j)
        retVal[arg.rows()][j] = 1.0 / double(arg.cols());
    
    return retVal;
}

Matrix<double>
HMMHelper::delZeros( const Matrix<double>& arg )
{
    Matrix<double> m(arg);
    
    const double percent =  0.025; // 0.025;
    const double treatAsZero = 1.0E-4;
    
    for (int i = 0; i < m.rows(); ++i)
    {
        double prob = 0.0;
        int active = 0;
        for (int j = 0; j < m.cols(); ++j)
        { 
            if (m[i][j] > treatAsZero)
            {
                prob += percent * m[i][j];
                ++active;
            }
            else 
            {
                prob += m[i][j];
                m[i][j] = 0.0;   
            }
        }
        double divsor = (double) (m.cols() - active);
        if (m.cols() != active) 
        {
            for (int j = 0; j < m.cols(); ++j)
            { 
                if (m[i][j] > treatAsZero)
                    m[i][j] -= percent * m[i][j];
                else m[i][j] = prob / divsor;
            }
        }
    }
    
    return m;
}

Matrix<double>
HMMHelper::delInitialState( const Matrix<double>& arg )
{
    Matrix<double> retVal(arg.rows() - 1, arg.cols() - 1, 0.0);
    for (int i = 0; i < retVal.rows(); ++i)
    {
        double delProb = arg[i][arg.cols() - 1] / (arg.cols() - 1);
        for (int j = 0; j < retVal.cols(); ++j)
            retVal[i][j] = arg[i][j] + delProb;
    }
    return retVal;
}

Matrix<double>
HMMHelper::addInitialEmission( const Matrix<double>& arg )
{
    Matrix<double> retVal(arg.rows() + 1, arg.cols() + 1, 0.0);

    for (int i = 0; i < arg.rows(); ++i)
        for (int j = 0; j < arg.cols(); ++j)
            retVal[i][j] = arg[i][j];
    
    retVal[arg.rows()][arg.cols()] = 1.0;
    
    return retVal;
}

Matrix<double>
HMMHelper::delInitialEmission( const Matrix<double>& arg )
{
    Matrix<double> retVal(arg.rows() - 1, arg.cols() -1, 0.0);
    for (int i = 0; i < retVal.rows(); ++i)
    {
        double delProb = arg[i][arg.cols() - 1] / (arg.cols() - 1);
        for (int j = 0; j < retVal.cols(); ++j)
            retVal[i][j] = arg[i][j] + delProb;
    }
    
    return retVal;
}

std::vector<std::string>
HMMHelper::addInitialState( const std::vector<std::string>& train )
{
    std::vector<std::string> retVal(train.size() + 1);

    retVal[train.size()] = "X";
    for (int j = 0; j < train.size(); ++j) 
    {
        retVal[j] = train[j];
    }

    return retVal;
}

std::vector<std::string>
HMMHelper::delInitialState( const std::vector<std::string>& train )
{
    std::vector<std::string> retVal(train.size() - 1);
    
    for (int j = 0; j < retVal.size(); ++j) 
    {
        retVal[j] = train[j];
    }
    
    return retVal;
}

std::vector<std::vector<std::string>>
HMMHelper::addInitialState( const std::vector<std::vector<std::string>>& train )
{
    std::vector<std::vector<std::string>> retVal(train.size());
    for (int i = 0; i < train.size(); ++i)
    {
        retVal[i].resize(train[i].size() + 1);
        retVal[i][0] = "X";
        for (int j = 0; j < train[i].size(); ++j) 
        {
            retVal[i][j+1] = train[i][j];
        }
       // retVal[i].push_back("X");
    }
    return retVal;
}
  
std::vector<std::vector<std::string>>
HMMHelper::delInitialState( const std::vector<std::vector<std::string>>& train )
{
    std::vector<std::vector<std::string>> retVal(train.size());
    for (int i = 0; i < train.size(); ++i)
    {
        retVal[i].resize(train[i].size() - 1);
        for (int j = 1; j < train[i].size(); ++j) 
        {
            retVal[i][j-1] = train[i][j];
        }
    }
    return retVal;
}


//


