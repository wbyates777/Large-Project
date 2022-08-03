/* HMM 13/05/2017

 $$$$$$$$$$$$$$$
 $   HMM.cpp   $
 $$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

 A simple, vanilla, discrete time, discrete space, hidden Markov model (see 1)
 
 Inputs:
 
 states    = text labels for the N hidden states of the model
 emissions = text labels for the M emissions of the hidden states.
 pi        = initial state probability distribution as vector of size N
 A         = transition probability matrix of size N x N
 B         = emission probability matrix with N rows, and M cols (M emissions for each of the N states)
 
 Outputs
 
 seq       = a sequence of observations - a vector of emission text labels 
 
 F
 
 (1) L. R. Rabiner. A tutorial on hidden Markov models and selected applications in speech recognition.
 Proceddings of the IEEE, 77(2):257–286, 1989.
 http://www.cs.cmu.edu/~cga/behavior/rabiner1.pdf
 
 (2) https://cran.r-project.org/web/packages/HMM/HMM.pdf
 
 =====================================================================================
 
 Copyright stuff
 
 1) Random number generator code is taken from "Numerical Recipes in C, 2nd Edition"
 
 Copyright (C) 1998 by Numerical Recipes Software.  Permission is
 granted for unlimited copying and redistribution, provided that this
 copyright notice is displayed, together with the two following links
 
 http://www.nr.com/news.html
 
 http://www.nr.com/index.html
 
 */


#ifndef __HMM_H__
#include "HMM.h"
#endif

#ifndef __UTILS_H__
#include "AUtils.h"
#endif

#include <fstream>
#include <assert.h>

const int DEFAULT_SEED = 22;

HMM::HMM( void ): m_ran(DEFAULT_SEED), m_pi(), m_A(), m_B(), m_states(), m_emissions(), m_logger() 
{
    m_logger.getLogLevel( "HMM" );
}

HMM::HMM( const HMMStates& states, 
          const HMMEmissions& emissions,
          const std::vector<double>& pi, 
          const Matrix<double>& A, 
          const std::vector<Matrix<double>>& B ) :  m_ran(DEFAULT_SEED), 
                                                    m_pi(pi), 
                                                    m_A(A), 
                                                    m_B(B), 
                                                    m_states(states), 
                                                    m_emissions(emissions), 
                                                    m_logger()
{
    m_logger.getLogLevel( "HMM" );  
}

HMM::HMM( const HMMStates& states, 
          const HMMEmissions& emissions ): m_ran(DEFAULT_SEED), 
                                       m_pi(), 
                                       m_A(), 
                                       m_B(), 
                                       m_states(states), 
                                       m_emissions(emissions), 
                                       m_logger()
{
    m_logger.getLogLevel( "HMM" );

    int state_num = (int) m_states.size();
   
    m_pi.resize(state_num, 1.0 / double(state_num)); 
    
    m_A.resize(state_num, state_num, 1.0 / double(state_num)); 
    
    m_B.resize(m_emissions.size());
    for (int i = 0; i < m_emissions.size(); ++i)
    {
        int emission_num = (int) m_emissions[i].size();
        m_B[i].resize(state_num, emission_num, 1.0 / double(emission_num));
    }
}

HMM::HMM( const HMM& h )
{
    m_ran = h.m_ran;
    m_pi  = h.m_pi;           
    m_A   = h.m_A;            
    m_B   = h.m_B;            
    m_states = h.m_states;       
    m_emissions = h.m_emissions;    
    m_y = h.m_y;  
}

HMM&
HMM::operator=( const HMM& rhs )
{
    if (this != &rhs)
    {
        m_ran = rhs.m_ran;
        m_pi  = rhs.m_pi;           
        m_A   = rhs.m_A;            
        m_B   = rhs.m_B;            
        m_states = rhs.m_states;       
        m_emissions = rhs.m_emissions;    
        m_y = rhs.m_y;            
    }
    
    return *this;
}


bool
HMM::set(  const HMMStates& states, 
           const HMMEmissions& emissions,
           const std::vector<double>& pi, 
           const Matrix<double>& A, 
           const std::vector<Matrix<double>>& B ) 
{
    m_ran.reset();
    //m_ran.seed(DEFAULT_SEED);
    m_pi = pi;
    m_A  = A;
    m_B  = B;
    
    m_states = states;
    m_emissions = emissions;
    
    return check();
}

bool
HMM::set( const HMMStates& states, 
          const HMMEmissions& emissions ) 
{
    m_ran.reset();
    //m_ran.seed(DEFAULT_SEED);
 
    m_states = states;
    m_emissions = emissions;
    
    int state_num = (int) m_states.size();

    m_pi.resize(state_num, 1.0 / double(state_num));
    m_A.resize(state_num, state_num, 1.0 / double(state_num)); 
    m_B.resize(m_emissions.size());
    for (int i = 0; i < m_emissions.size(); ++i)
    {
        int emission_num = (int) m_emissions[i].size();
        m_B[i].resize(state_num, emission_num, 1.0 / double(emission_num));
    }
    return true;
}

void
HMM::randomise( void ) 
{ 
    /*double sum = 0.0;
    for(int i = 0; i < m_pi.size(); ++i)
        sum += (m_pi[i] = m_ran.ran());
    
    for(int i = 0; i < m_pi.size(); ++i)
        m_pi[i] /= sum;*/
    
    randomise(m_A); 
    for (int i = 0; i < m_B.size(); ++i) 
        randomise(m_B[i]); 
    
    // identity matrix
    //m_B[0].resize((int) m_pi.size(), (int) m_pi.size(), 0.0);
    //for (int i = 0; i < m_B[0].rows(); ++i)
   //     m_B[0][i][i] = 1.0;
}

void
HMM::setEquiprobable( void )
{
    int state_num = (int) m_states.size();
    
    m_pi.resize(state_num, 1.0 / double(state_num)); 
    
    m_A.resize(state_num, state_num, 1.0 / double(state_num)); 
    
    m_B.resize(m_emissions.size());
    
    // identity matrix
    m_B[0].resize(state_num, state_num, 0.0);
    for (int i = 0; i < state_num; ++i)
        m_B[0][i][i] = 1.0;
    
    for (int i = 1; i < m_emissions.size(); ++i)
    {
        int emission_num = (int) m_emissions[i].size();
        m_B[i].resize(state_num, emission_num, 1.0 / double(emission_num));
    }    
}

bool
HMM::checkMatrix( const Matrix<double>& mat ) const
{
    for (int i = 0; i < mat.rows(); ++i)
    {
        double total = 0.0; 
        for (int j = 0; j < mat.cols(); ++j)
        {
            // does it looks like a probability?
            if (mat[i][j] < 0.0 || mat[i][j] > 1.0 + NEAR_ZERO)
                return false;
            total += mat[i][j];
        }
        // sums to 1
        if (!nearZero(total - 1.0))
            return false;
    }
    return true;
}

bool
HMM::check( void ) const
{
    bool retVal = true;

    Message txt1(2);  
    
    // dimension check
    if (retVal && m_states.size() != m_pi.size())
    {
        txt1 << "Transition matrix and initial state probabilities don't match";
        retVal = false; 
    }
    
    if (retVal && m_states.size() != m_A.rows())
    {
        txt1 << "Transition matrix rows and number of states do not match";
        retVal = false; 
    }
    
    if (retVal && m_states.size() != m_A.cols())
    {
        txt1 << "Transition matrix columns and number of states do not match";
        retVal = false; 
    }   
 
    for (int i = 0; i < m_B.size(); ++i)
    {
        if (retVal && m_states.size() != m_B[i].rows())
        {
            txt1 << "Emission matrix " << i << " rows and number of states do not match";
            retVal = false; 
        }  
        
        if (retVal && m_emissions[i].size() != m_B[i].cols())
        {
            txt1 << "Emission matrix " << i << " columns and number of emissions " << i << " do not match";
            retVal = false; 
        }
    }
    
    // probability check
    if (retVal)
    {
        double total = 0.0;
        for (int i = 0; i < m_pi.size(); ++i)
        {
            // does it looks like a probability?
            if (m_pi[i] < 0.0 || m_pi[i] > 1.0)
            {
                txt1 << "Initial state vector contains non-probability at element " << i;
                retVal = false; 
            }
            
            total += m_pi[i];
        }
        if (retVal && !nearZero(total - 1.0))
        {
            txt1 << "Initial state vector does not sum to 1";
            retVal = false; 
        }
    }
    
    if (retVal && !checkMatrix(m_A))
    {
        txt1 << "Transition matrix is not a probability matrix";
        retVal = false; 
    }
    
    for (int i = 0; i < m_B.size(); ++i)
    {
        if (retVal && !checkMatrix(m_B[i]))
        {
            txt1 << "Emissions matrix " << i << " is not a probability matrix";
            retVal = false; 
        }
    }
    
    if (m_logger.level() >= 2 && !retVal)
    {
        m_logger.logMsg(txt1);
        m_logger.flush();
    }
    
    if (m_logger.level() >= 1)
    {
        Message txt2(1);
        if (retVal)
            txt2 << "Model parameter check PASSED";
        else txt2 << "Model parameter check FAILED";
        m_logger.logMsg(txt2);
        m_logger.flush();
    }
    return retVal;
}


bool
HMM::save( const std::string& fileName ) const
{
    std::ofstream to;
    to.precision(8);
    to.setf( std::ios::fixed, std::ios::floatfield );
    to.open( fileName );
    
    if (!to)
    {
        if (m_logger.level() >= 1)
        {
            Message txt(1);
            txt << "Could not open " << fileName << " for save";
            m_logger.logMsg(txt);
            m_logger.flush();
        }
        return false;
    }
    
    to << m_pi << '\n';
    to << m_A << '\n';
    to << m_B << '\n';
    to << m_ran << '\n';
    to << m_states << '\n';
    to << m_emissions << '\n';
    to << '\n';
    
    if (m_logger.level() >= 1)
    {
        Message txt(1);
        txt << "Model parameters saved to " << fileName;
        m_logger.logMsg(txt);
        m_logger.flush();
    }

    to.close();
    
    return true;
}

bool
HMM::load( const std::string& fileName )
{
    std::ifstream from;
    from.open( fileName );
    
    if (!from)
    {
        if (m_logger.level() >= 1)
        {
            Message txt(1);
            txt << "Could not open " << fileName << " for load";
            m_logger.logMsg(txt);
            m_logger.flush();
        }
        return false;
    }

    from >> m_pi;
    from >> m_A;
    from >> m_B;
    from >> m_ran;
    from >> m_states;
    from >> m_emissions;
        
    if (m_logger.level() >= 1)
    {
        Message txt(1);
        txt << "Model paramters loaded from " << fileName;
        m_logger.logMsg(txt);
        m_logger.flush();
    }
    
    from.close();
    
    return true;
}

/*
 sample from discrete distribution
 in:   p = vector of probabilities,assumed to sum to 1
 */
int 
HMM::randm(const std::vector<double>& probs)
{
    int res = (int) probs.size();
    double sum = 0.0;
    double p = m_ran.ran();
    
    for (int i = 0; i < probs.size(); ++i)
    {
        sum += probs[i];
        if (sum >= p)
        {
            res = i;
            break;
        }
    }
    assert( res < probs.size() );
    return res;
}

/*
 sample a trajectory from a hidden markov chain
 in:   nu = initial distribution as vector of size k
 Q = transition matrix of size k
 n = positive integer
 out:  (x,y) = sample trajectory of size n of a HMM defined by (nu, Q, g):
 x = sample trajectory of size n of a Markov Chain with initial distribution nu and transition matrix Q
 y = observations such that the conditionnal distribution of y[k]
 given x[k] is g(x[k], :)
 */
std::vector<HMMSequence> 
HMM::sample(int iter, 
            const std::vector<double>& pi, 
            const Matrix<double>& A, 
            const std::vector<Matrix<double>>& B)
// double nu[N], double Q[N][N], double g[N][M], int x[T] and int y[T]
{
    int T = iter;
    std::vector<HMMSequence> retVal( 1 + B.size(), HMMSequence(T) );
    
    std::vector<int> x(T,0);
    m_y.clear();
    m_y.resize(B.size(), std::vector<int>(T,0)); 

    x[0] = randm(pi);
    for (int i = 0; i < B.size(); ++i)
        m_y[i][0] = randm(B[i][x[0]]);   
    
    for(int i = 1; i < T; ++i)
    {
        x[i] = randm(A[x[i-1]]);
        
        for (int j = 0; j < B.size(); ++j)
            m_y[j][i] = randm(B[j][x[i]]);
    }
    
    for (int i = 0; i < T; ++i)   
        retVal[0][i] =  m_states[x[i]];
    
    for (int i = 0; i < T; ++i) 
        for (int j = 0; j < B.size(); ++j)
            retVal[j+1][i] = m_emissions[j][m_y[j][i]];
    
    return retVal;
} 

const std::vector<std::vector<int>>& 
HMM::run(int iter, 
         const std::vector<double>& pi, 
         const Matrix<double>& A, 
         const std::vector<Matrix<double>>& B)
{
    int T = iter;
    const int STATE = 0;
    
    m_y.clear();
    m_y.resize(1 + B.size(), std::vector<int>(T,0)); 
    
    m_y[STATE][0] = randm(pi);
    for (int i = 0; i < B.size(); ++i)
        m_y[i+1][0] = randm(B[i][m_y[STATE][0]]);   
    
    
    for (int t = 1; t < T; ++t)
    {
        m_y[STATE][t] = randm(A[m_y[STATE][t-1]]);
        
        for (int i = 0; i < B.size(); ++i)
            m_y[i+1][t] = randm(B[i][m_y[STATE][t]]);
    }
    
    return m_y;
} 

const std::vector<int>& 
HMM::run(const std::vector<double>& pi, 
         const Matrix<double>& A, 
         const std::vector<Matrix<double>>& B)
{
    if (m_y.empty() || m_y.size() != 1)
    {
        m_y.resize(1);
        m_y[0].resize(1 + B.size(),0); 
        m_y[0][0] = randm(pi);
        for (int i = 0; i < B.size(); ++i)
            m_y[0][i+1] = randm(B[i][m_y[0][0]]); 
    }
    else
    {
        m_y[0][0] = randm(A[m_y[0][0]]);
        for (int i = 0; i < B.size(); ++i)
            m_y[0][i+1] = randm(B[i][m_y[0][0]]);
    }
    return m_y[0];
}


void 
HMM::randomise(Matrix<double>& K)
// double K[N][M]
{
    int N = K.rows();
    int M = K.cols();
    
    int i,j;
    double s;
    for(i = 0; i < N; ++i)
    {
        s = 0;
        for(j = 0; j < M; ++j)
            s += (K[i][j] = m_ran.ran());
        
        for(j = 0; j < M; ++j)
            K[i][j] /= s;
    }
}


///////////////////




