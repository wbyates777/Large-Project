/* BaumWelch 03/06/2017
 
 $$$$$$$$$$$$$$$$$$$
 $   BaumWelch.h   $
 $$$$$$$$$$$$$$$$$$$
 
 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:
 
 
 Baum-Welch learning of multiple sequences (see 1) for discrete time, discrete space, hidden Markov model (see 2).
 The auxiliary functions seqProb() (see 2), klDistance() (see 4), and viterbi() (see 5) are also provided.
 
 (1) https://en.wikipedia.org/wiki/Baum_Welch_algorithm
 
 (2) L. R. Rabiner. A tutorial on hidden Markov models and selected applications in speech recognition.
 Proceddings of the IEEE, 77(2), pages 257–286, 1989. 
 http://www.cs.cmu.edu/~cga/behavior/rabiner1.pdf
 
 (3) Contains clarification of scaling and multi-sequence recursion equations defined in Rabiner paper
 http://alumni.media.mit.edu/~rahimi/rabiner/rabiner-errata/rabiner-errata.html
 
 (4) B.-H. Juang and L. R. Rabiner. A Probabilistic Distance Measure for Hidden Markov Models.
 AT&T Technical Journal, 64(2), pages 391-408, 1985
 https://ieeexplore.ieee.org/document/6770944
 https://en.wikipedia.org/wiki/Kullback–Leibler_divergence
 
 (5) https://en.wikipedia.org/wiki/Viterbi_algorithm
 
 This implementation of the Baum-Welch algorithm is based on a C version: 
 
 *
 *  written by Aurelien Garivier, CNRS & Telecom Paristech
 *  January 2012
 * (small bug corrected by Julius Su, Caltech)
 *
 * Baum-Welch algorithm for discrete Hidden Markov models
 * see https://www.math.univ-toulouse.fr/~agarivie/Telecom/code/index.php
 * 
 
 I have converted the code to C++, renamed variables and addded LaTeX descriptions 
 of the calculations using the notation employed by Rabiner.
 
 I have also added learning over multiple sequences.
 
 */


#ifndef __BAUMWELCH_H__
#define __BAUMWELCH_H__

#ifndef __HMM_H__
#include "HMM.h"
#endif

// defined in HMM.h
// typedef std::vector<std::string> HMMSequence;
// typedef std::vector<std::string> HMMAlphabet;

class BaumWelch
{
public:
    BaumWelch( void ): m_logger() { m_logger.getLogLevel( "BW" ); } 
    
    ~BaumWelch( void )=default;
    
    /// train model on single sequences of observations; 
    /// returns l = log-likelihood of seq for parameters A and B
    double 
    baumwelch( HMM& model, const HMMSequence& seq, int iters = 100 ) const;
    
    /// train model on mutiple sequences of observations
    /// returns l = log-likelihood of seqs for parameters A and B
    double 
    baumwelch( HMM& model, const std::vector<HMMSequence>& seq, int iters = 100 ) const;
    
    /// returns p = probability (or log-likelihood if logarithm = true) of single seq for parameters A and B
    double 
    seqProb( const HMM& model, const HMMSequence& seq, bool logarithm = false ) const;
    
    /// returns p = probabilies (or log-likelihood if logarithm = true) of multiple seqs for parameters A and B
    std::vector<double> 
    seqProb( const HMM& model, const std::vector<HMMSequence>& seq, bool logarithm = false ) const;
    
    /// returns d = symmetric Kullback-Leibler distance between two HMMs over S Monte Carlo trials of seqs of length T
    double 
    klDistance( HMM& model1,  HMM& model2, int T, int S = 1000 ) const;
    
    /// returns the length of a probability vector; shortest -> minimum certainty, longest -> maximum certainty
    /// do not confuse with geometric length ( see https://en.wikipedia.org/wiki/Probability_vector )
    double
    length( const std::vector<double>& prob_vector ) const;
    
    /// returns the most likely sequence of hidden states, called the Viterbi path, 
    /// that produces the given sequence of observations
    std::vector<int> 
    viterbi(const HMM& model, const HMMSequence& seq) const;
    
    Logger*
    getLogger( void ) const  { return &m_logger; }
    
 
private:
    
    static constexpr double LARGE_VALUE =  1.0E10;
    static constexpr double BAD_VALUE   = -1.0E10;
    static constexpr double NEAR_ZERO   =  1.0E-10; 
    
    void
    errorLog( const std::string& msg, int it ) const;
    
    std::vector<std::vector<int>>
    toIntegerRep( const std::vector<HMMSequence>& seq, const HMMAlphabet& alphabet ) const;
    
    void 
    forward( const std::vector<int>& y, 
           const std::vector<double>& pi, 
           const Matrix<double>& A, 
           const Matrix<double>& B, 
           Matrix<double>& alpha, 
           std::vector<double>& cinv ) const;
    
    void
    backward( const std::vector<int>& y, 
             const Matrix<double>& A, 
             const Matrix<double>& B, 
             const std::vector<double>& cinv, 
             Matrix<double>& beta ) const;

    double 
    baumwelch(const std::vector<std::vector<int>>& yy, 
              std::vector<double>& pi, 
              Matrix<double>& A, 
              Matrix<double>& B, 
              const int iters) const;
    
    double
    seqProb(const std::vector<int>& y, 
             const std::vector<double>& pi, 
             const Matrix<double>& A, 
             const Matrix<double>& B, bool logarithm ) const;
    
    double
    klMeasure( HMM& m1, HMM& m2, int T, int N ) const;
    
    std::vector<int> 
    viterbi(const std::vector<int>& y, 
            const std::vector<double>& pi, 
            const Matrix<double>& A, 
            const Matrix<double>& B) const;

    mutable Logger m_logger;
};

#endif


