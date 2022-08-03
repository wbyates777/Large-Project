/* HMM 13/05/2017

 $$$$$$$$$$$$$
 $   HMM.h   $
 $$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:
 
 A simple, vanilla, discrete time, discrete space, multi-variate hidden Markov model (see 1) 
 
 Inputs:
 
 states    = text labels for the N hidden states of the model
 emissions = text labels for the M emissions of the hidden states.
 pi        = initial state probability distribution as vector of size N
 A         = transition probability matrix of size N x N
 B         = emission probability matrix with N rows, and M cols (M emissions for each of the N states)

 Outputs
 
 seq       = a sequence of observations - a vector of emission text labels 
 
 
 
(1) L. R. Rabiner. A tutorial on hidden Markov models and selected applications in speech recognition.
         Proceddings of the IEEE, 77(2), pages 257–286, 1989.
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
#define __HMM_H__


#include <vector>
#include <string>

#ifndef __MATRIX_H__
#include "AMatrix.h"
#endif

#ifndef __URAND_H__
#include "URand.h"
#endif

#ifndef __LOGGER_H__
#include "ALogger.h"
#endif


typedef std::vector<std::string> HMMSequence;
typedef std::vector<std::string> HMMStates;
typedef std::vector<std::string> HMMEmission;
typedef std::vector<HMMEmission> HMMEmissions;


typedef std::vector<std::string> HMMAlphabet;

class HMM
{
public:
        
    HMM( void );
    
    HMM( const HMMStates& states, 
         const HMMEmission& emission ): HMM(states, HMMEmissions(1, emission)) {}
    
    HMM( const HMMStates& states, 
         const HMMEmission& emission,
         const std::vector<double>& pi, 
         const Matrix<double>& A, 
         const Matrix<double>& B ): HMM(states, HMMEmissions(1, emission), pi, A, std::vector<Matrix<double>>(1, B)) {}
    
    HMM( const HMMStates& states, 
         const HMMEmissions& emissions );
    
    HMM( const HMMStates& states, 
         const HMMEmissions& emissions,
         const std::vector<double>& pi, 
         const Matrix<double>& A, 
         const std::vector<Matrix<double>>& B );
    
    ~HMM( void )=default;
    
    HMM( const HMM& h );
    
    HMM&
    operator=( const HMM& rhs );

    // set up using a single emission matrix
    bool
    set( const HMMStates& states, 
         const HMMEmission& emission ) { return set(states, HMMEmissions(1, emission)); }
    
    bool
    set( const HMMStates& states, 
         const HMMEmission& emission,
         const std::vector<double>& pi, 
         const Matrix<double>& A, 
         const Matrix<double>& B )
    {
        return set(states, HMMEmissions(1, emission), pi, A, std::vector<Matrix<double>>(1, B)); 
    }
    
    // set up using multiple emissions matrices
    bool
    set( const HMMStates& states, 
         const HMMEmissions& emissions );
    
    bool
    set( const HMMStates& states, 
         const HMMEmissions& emissions,
         const std::vector<double>& pi, 
         const Matrix<double>& A, 
         const std::vector<Matrix<double>>& B );
    
    bool
    check( void ) const;
    
    bool
    save( const std::string& fileName ) const;
    
    bool
    load( const std::string& fileName );
    
    std::vector<HMMSequence> 
    sample( int iter = 1 ) { return sample( iter, m_pi, m_A, m_B ); }
    
    /// Execute HMM for one iteration (at a time); 
    /// returns an HMM state where index 0 is hidden state, index 1 is emission 1, index 2 is emission 2,...
    const std::vector<int>& 
    run( void ) { return run( m_pi, m_A, m_B ); }
    
    /// Execute HMM for iter iterations; 
    /// returns vector of HMM states where index 0 is HMM state at t = 0, index 1 is HMM state at t = 1,...
    const std::vector<std::vector<int>>& 
    run( int iter ) { assert(iter > 1); return run( iter, m_pi, m_A, m_B ); }
    
    // data accessors
    const HMMStates& 
    states( void ) const { return m_states; }
    
    const HMMEmission& 
    emission( int i = 0 ) const { return m_emissions[i]; }
    
    const HMMEmissions& 
    emissions( void ) const { return m_emissions; }
    
    // the initial probabilities (one for each state)
    const std::vector<double>&
    initial_probs( void ) const { return m_pi; }
    
    std::vector<double>&
    initial_probs( void )  { return m_pi; }
    
    void
    initial_probs( const std::vector<double>& p ) { m_pi = p; }
    
    // the transition probabilities from one state to another
    const Matrix<double>&
    trans_probs( void ) const { return m_A; }
    
    Matrix<double>&
    trans_probs( void ) { return m_A; }
    
    void
    trans_probs( const Matrix<double>& tp )  { m_A = tp; }
    
    // the emission proablities for each state
    const Matrix<double>&
    emiss_probs( int i ) const { return m_B[i]; }
    
    Matrix<double>&
    emiss_probs( int i ) { return m_B[i]; }
    
    const std::vector<Matrix<double>>&
    emiss_probs( void ) const { return m_B; }
    
    std::vector<Matrix<double>>&
    emiss_probs( void ) { return m_B; }
    
    void
    emiss_probs( int i, const Matrix<double>& ep )  { m_B[i] = ep; }
    
    void
    emiss_probs( const std::vector<Matrix<double>>& ep )  { m_B = ep; }
    
    void
    randomise( void );
    
    void
    setEquiprobable( void );
    
    void
    flatten( void );
    
    int
    seed( void ) const { return m_ran.seed(); }
    
    void
    seed( int s ) { m_ran.seed(s); }
    
    void 
    randomise( Matrix<double>& K ); // used only in SSHH::settings; remove
    
    Logger*
    getLogger( void ) const  { return &m_logger; }
    
private:

    static constexpr int    STATE_MAX =  1024;
    static constexpr double NEAR_ZERO =  1.0E-4;
    
    bool nearZero( double p ) const { return ((p > -NEAR_ZERO) && (p < NEAR_ZERO)); }
    
    bool
    checkMatrix( const Matrix<double>& mat ) const;
    
    int 
    randm(const std::vector<double>& p );
    
    void
    flattenProbs( std::vector<double>& p, double amount = 0.05 );
    
    std::vector<HMMSequence> 
    sample(int iter,
           const std::vector<double>& pi, 
           const Matrix<double>& A, 
           const std::vector<Matrix<double>>& B);
    
    const std::vector<std::vector<int>>& 
    run(int iter, 
        const std::vector<double>& pi, 
        const Matrix<double>& A, 
        const std::vector<Matrix<double>>& B);
    
    
    const std::vector<int>& 
    run(const std::vector<double>& pi, 
        const Matrix<double>& A, 
        const std::vector<Matrix<double>>& B);
    
    
    URand                           m_ran;
    std::vector<double>             m_pi;           //!< initial state distribution
    Matrix<double>                  m_A;            //!< transition matrix
    std::vector<Matrix<double>>     m_B;            //!< emission(s) matrix
    HMMStates                       m_states;       //!< states
    HMMEmissions                    m_emissions;    //!< emissions
    std::vector<std::vector<int>>   m_y;            //!< output sequence
    mutable Logger                  m_logger;
};

#endif


