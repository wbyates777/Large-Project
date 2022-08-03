/* BaumWelch 03/06/2017
 
 $$$$$$$$$$$$$$$$$$$$$
 $   BaumWelch.cpp   $
 $$$$$$$$$$$$$$$$$$$$$
 
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
#include "BaumWelch.h"
#endif


#ifndef __UTILS_H__
#include "AUtils.h"
#endif


#include <cmath>

///
/// interface methods
///

double
BaumWelch::length( const std::vector<double>& prob_vector ) const
{
    double n = prob_vector.size();
    double sd = util::std_dev(prob_vector);
    return sqrt((n * sd * sd) + 1.0 / n);
}

std::vector<std::vector<int>>
BaumWelch::toIntegerRep( const std::vector<HMMSequence>& seqs, const HMMAlphabet& alphabet ) const 
{
    std::vector<std::vector<int>> seqs_int(seqs.size());
    std::map<std::string,int> map;  // map emission tokens to integers

    int c = 0;
    for (auto i = alphabet.begin(); i != alphabet.end(); ++i, ++c)
        map.insert( std::map<std::string, int>::value_type(*i, c) );    
    
    seqs_int.resize(seqs.size());
    
    for (int i = 0; i < seqs.size(); ++i)
    {
        seqs_int[i].resize(seqs[i].size()); 
        
        for (int j = 0; j < seqs[i].size(); ++j)
        {
            auto fi = map.find(seqs[i][j]);
            
            if (fi == map.end())
            {
                if (m_logger.level() >= 1)
                {
                    Message txt(1);
                    txt << "toIntegerRep::Unknown symbol \"" << seqs[i][j] << "\" in observation sequence";
                    m_logger.logMsg(txt);
                    m_logger.flush();
                }
                return std::vector<std::vector<int>>();
            }
                
            seqs_int[i][j] = fi->second;
        }
    }
    return seqs_int;
}

double 
BaumWelch::baumwelch( HMM& model, const HMMSequence& seq, int iters ) const 
{ 
    return baumwelch( model, std::vector<HMMSequence>(1, seq), iters ); 
}

double 
BaumWelch::baumwelch( HMM& model, const std::vector<HMMSequence>& seqs, int iters ) const
{
    if (seqs.empty())
    {
        if (m_logger.level() >= 1)
        {
            Message txt(1);
            txt << "No training observations";
            m_logger.logMsg(txt);
            m_logger.flush();
        }
        return BAD_VALUE;
    }
    
    std::vector<std::vector<int>> seqs_int = toIntegerRep(seqs, model.emission());

    return baumwelch( seqs_int, model.initial_probs(), model.trans_probs(), model.emiss_probs(0), iters );    
}

/// returns p = probability (or log-likelihood if logarithm = true) of single seq for parameters A and B
double 
BaumWelch::seqProb( const HMM& model, const HMMSequence& seq, bool logarithm ) const
{
    std::vector<double> p = seqProb( model, std::vector<HMMSequence>(1, seq), logarithm );
    return (!p.empty()) ? p[0] : BAD_VALUE;
}

std::vector<double>
BaumWelch::seqProb( const HMM& model, const std::vector<HMMSequence>& seq, bool logarithm ) const
{
    std::vector<std::vector<int>> seqs_int = toIntegerRep(seq, model.emission() );
    std::vector<double> probs;
    
    for (int i = 0; i < seqs_int.size(); ++i)
    {
        double prob = seqProb(seqs_int[i], model.initial_probs(), model.trans_probs(), model.emiss_probs(0), logarithm ); 
        probs.push_back(prob);
        
        if (m_logger.level() >= 2)
        {
            Message txt(2);
            txt << "Emission probability of ";
            for (int k = 0; k < seq[i].size(); ++k)
                txt << seq[i][k];
            txt << " is " << prob;
            
            m_logger.logMsg(txt);
            m_logger.flush();
        }
    }

    return probs;
}

//
// calculation methods
//

/// <summary>
///   The forward algorithm for calculating the $\hat{\alpha}$ and $c_t$.
/// </summary>
/// <remarks>
///   Given sequences of training observation  $O = \{ o_1, o_2, \dots, o_Y \}$
///   and a HMM $M = (\pi, A, B)$ recursively compute the scaled $\hat{\alpha}$ values.
///   Notice the difference between $\hat{\alpha}$ and $\bar{\alpha}$ (see Errata document), and that
///   the cinv variable defined here is the inverse of the $c_t$ variable used in Rabiner.
/// </remarks>
/// <param name="y" />
///   An array of observation sequences to be used to train the model.
///
/// <param name="pi" />
///   The hidden Markov model's initial state distribution.
///
/// <param name="A" />
///   The hidden Markov model's transition probability matrix.
///
/// <param name="B" />
///   The hidden Markov model's emission probability matrix.
///
/// <returns>
///  The scaled $\hat{\alpha}$ and $c_t$ values.
/// </returns>
void 
BaumWelch::forward(const std::vector<int>& y, 
                  const std::vector<double>& pi, 
                  const Matrix<double>& A, 
                  const Matrix<double>& B, 
                  Matrix<double>& alpha, 
                  std::vector<double>& cinv) const
// int y[T], double pi[N], double A[N][N], double B[N][M], double alpha[T][N], double c[T]
{
    const int N = (int) pi.size();
    const int T = (int) y.size();
    std::vector<double> z(N, 0.0);
    
    cinv[0] = 0.0;
    for (int i = 0; i < N; ++i) 
    {
        z[i] = pi[i] * B[i][y[0]];      /// $$\bar{\alpha}_1(i) = \alpha_1(i) = \pi_i b_i(O_1)$$ 
        cinv[0] += z[i];                /// $$c_1 = \frac{1}{\sum_i^N \bar{\alpha}_1(i)}$$
    }
    
    for (int i = 0; i < N; ++i) 
        alpha[0][i] = z[i] / cinv[0];   /// $$\hat{\alpha}_1(i) = \bar{\alpha}_1(i) c_1$$
  
    for (int t = 1; t < T; ++t)
    {
        cinv[t] = 0.0;
        for (int j = 0; j < N; ++j)
        {
            z[j] = 0.0;
            /// $$\bar{\alpha}_t(j) = \sum_{i=1}^N \hat{\alpha}_{t-1}(i) a_{ij} b_j(O_t)$$ 
            for (int i = 0; i < N; ++i) 
                z[j] += alpha[t-1][i] * A[i][j] * B[j][y[t]]; 
            
            /// $$c_t = \frac{1}{\sum_{i=1}^N \bar{\alpha}_{t}(i)}$$
            cinv[t] += z[j]; 
        }
        
        /// $$\hat{\alpha}_t(j) = \bar{\alpha}_t(j) c_t$$
        for (int j = 0; j < N; ++j) 
            alpha[t][j] = z[j] / cinv[t]; 
    }
}

/// <summary>
///   The backward algorithm for calculating the $\hat{\beta}$.
/// </summary>
/// <remarks>
///   Given sequences of training observation  $O = \{ o_1, o_2, \dots, o_Y \}$
///   and a HMM $M = (\pi, A, B)$ recursively compute the scaled $\hat{\beta}$ values.
///   Notice the difference between $\hat{\beta}$ and $\bar{\beta}$ (see Errata document), and that
///   the cinv variable defined here is the inverse of the $c_t$ variable used in Rabiner.
/// </remarks>
/// <param name="y" />
///   An array of observation sequences to be used to train the model.
///
/// <param name="pi" />
///   The hidden Markov model's initial state distribution.
///
/// <param name="A" />
///   The hidden Markov model's transition probability matrix.
///
/// <param name="B" />
///   The hidden Markov model's emission probability matrix.
///
/// <returns>
///  The scaled $\hat{\beta}$.
/// </returns>
void 
BaumWelch::backward(const std::vector<int>& y, 
                    const Matrix<double>& A, 
                    const Matrix<double>& B, 
                    const std::vector<double>& cinv, 
                    Matrix<double>& beta) const
// int y[T], double A[N][N], double B[N][M], double c[T], double beta[T][N]
{
    const int T = (int) y.size();
    const int N = (int) A.rows();
    
    /// $$\bar{\beta}_T(i) = \beta_T(i) = 1$$
    /// $$\hat{\alpha}_T(i) = \bar{\alpha}_T(i) c_T$$
    for (int j = 0; j < N; ++j) 
        beta[T-1][j] = 1.0 / cinv[T-1];
    
    for (int t = T-2; t >= 0; --t)
    {
        for (int i = 0; i < N; ++i)
        {
            double z = 0.0;
            
            /// $$\bar{\beta}_t(i) = \sum_{j=1}^N  a_{ij} b_j(O_{t+1}) \hat{\beta}_{t+1}(j)$$
            for (int j = 0; j < N; ++j)
                z += A[i][j] * B[j][y[t+1]] * beta[t+1][j];
            
            /// $$\hat{\beta}_t(i) = \bar{\beta}_t(i) c_t$$  
            beta[t][i] = z / cinv[t+1]; 
        }
    }
}

void
BaumWelch::errorLog( const std::string& msg, int it ) const
{
    if (m_logger.level() >= 1)
    {
        Message txt(1);
        txt << "Bad value detected at iteration " << it << " : " << msg;
        m_logger.logMsg(txt);
        m_logger.flush();
    }
    
    assert(0==1);
}

/// <summary>
///   Performs Baum-Welch learning for a hidden Markov model.
/// </summary>
/// <remarks>
///   Given sequences of training observation  $O = \{ o_1, o_2, \dots, o_Y \}$
///   and a HMM, determine the HMM parameters $M = (\pi, A, B)$ that best fit training data.
/// </remarks>
/// <param name="y" />
///   An array of observation sequences to be used to train the model.
///
/// <param name="pi" />
///   The hidden Markov model's initial state distribution.
///
/// <param name="A" />
///   The hidden Markov model's transition probability matrix.
///
/// <param name="B" />
///   The hidden Markov model's emission probability matrix.
///
/// <returns>
///   The log-likelihood for the observations after the model has been trained.
/// </returns>
double
BaumWelch::baumwelch(const std::vector<std::vector<int>>& y, 
                      std::vector<double>& pi, 
                      Matrix<double>& A, 
                      Matrix<double>& B,
                      const int maxIt) const
// int y[m][n], double pi[N], double A[N][N], double B[N][M]
{
    if (y.empty())
        return BAD_VALUE;
    
    const int N = (int) pi.size();         // number of states
    const int M = (int) B.cols();          // number of symbols
    const int S = (int) y.size();          // number of sequences
    
    Matrix<double>                              alpha;
    Matrix<double>                              beta;
    
    // one for each training sequence s
    std::vector<std::vector<Matrix<double>>>    xi(S);    
    std::vector<Matrix<double>>                 gamma(S); 
    std::vector<std::vector<double>>            cinv(S); /// cinv is the inverse of the $c_t$ variable defined in Rabiner
   
    // loop control parameters
    int it = 0; 
    const double tol = 1E-4;  
    double change = tol + 1;
    
    // return value
    double l = 0.0;
    
    for (; (change > tol) && (it < maxIt); ++it)
    {
        change = 0.0;
        l = 0.0;
        
        // for each training sequence s 
        for (int s = 0; s < S; ++s)
        {
            const int T = (int) y[s].size();
            
            cinv[s].resize( T, 0.0 );
            gamma[s].resize( T, N, 0.0 );
            alpha.resize( T, N, 0.0 );
            beta.resize( T, N, 0.0 );

            forward(y[s], pi, A, B, alpha, cinv[s]);
            backward(y[s], A, B, cinv[s], beta);
            
            for (int t = 0; t < T; ++t)
            {
                double sum = 0.0;
                for (int i = 0; i < N; ++i)
                {
                    /// $$\hat{\alpha}_t(i) \hat{\beta}_t(i) \frac{1}{c_t}$$
                    double z = alpha[t][i] * beta[t][i] * cinv[s][t]; 
                    gamma[s][t][i] =  z;
                    
                    /// $$\sum_{i=1}^N \hat{\alpha}_t(i) \hat{\beta}_t(i) \frac{1}{c_t}$$
                    sum += z;
                }
                
                if (sum == 0)
                {
                    errorLog("Division by zero updating gamma", it);
                    return BAD_VALUE;
                }
 
                /// $$\gamma_t(i) = \frac{\hat{\alpha}_t(i) \hat{\beta}_t(i) \frac{1}{c_t} }{\sum_{i=1}^N \hat{\alpha}_t(i) \hat{\beta}_t(i) \frac{1}{c_t}}$$
                for (int i = 0; i < N; ++i)
                    gamma[s][t][i] /= sum;
            }
            
            xi[s].resize( T, Matrix<double>(N, N, 0.0) );
            
            for (int t = 0; t < T - 1; ++t)
            {    
                double sum = 0.0;
                for (int i = 0; i < N; ++i)
                {
                    for (int j = 0; j < N; ++j)
                    {
                        /// $$\hat{\alpha}_t(i) a_{ij} b_j(O_{t+1}) \hat{\beta}_{t+1}(j)$$ 
                        double z = alpha[t][i]  * A[i][j] * B[j][y[s][t+1]] * beta[t+1][j]; 
                        
                        if (std::isnan(z))
                        {
                            errorLog("NaN detected updating xi", it);
                            return BAD_VALUE;
                        }
        
                        xi[s][t][i][j] = z;
                        
                        /// $$\sum_{i=1}^N \sum _{j=1}^N \hat{\alpha}_t(i) a_{ij} b_j(O_{t+1}) \hat{\beta}_{t+1}(j)$$
                        sum +=  z;
                    }
                }
                
                if (sum == 0)
                {
                    errorLog("Division by zero updating xi", it);
                    return BAD_VALUE;
                }  

                /// $$\xi_t(i,j) = \frac{\hat{\alpha}_t(i) a_{ij} b_j(O_{t+1}) \hat{\beta}_{t+1}(j)}{\sum_{i=1}^N \sum _{j=1}^N \hat{\alpha}_t(i) a_{ij} b_j(O_{t+1}) \hat{\beta}_{t+1}(j)}$$
                for (int i = 0; i < N; ++i)
                    for (int j = 0; j < N; ++j)
                        xi[s][t][i][j] /= sum;

            }
             
            for (int t = 0; t < cinv[s].size(); ++t) 
                l += log(cinv[s][t]);
        }
                
        // apply pi initial state updates
        for (int i = 0; i < N; ++i)
        {
            double sum = 0.0;
            
            for (int s = 0; s < S; ++s)
                sum += gamma[s][0][i];
            
            pi[i] = sum / S; 
        }
        
        // apply A transition updates
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                double den = 0.0, num = 0.0;
                
                for (int s = 0; s < S; ++s)
                {
                    const int T = (int) y[s].size();
                    
                    for (int t = 0; t < T - 1; ++t)
                    {
                        num += xi[s][t][i][j];  /// $$\sum_{t=1}^{T-1} \xi_t(i,j)$$ 
                        den += gamma[s][t][i];  /// $$\sum_{t=1}^{T-1} \gamma_t(i)$$
                    }
                }
    
                if (den == 0)
                {
                    errorLog("Division by zero updating A", it);
                    return BAD_VALUE;
                }
                
                double z = num / den;
                change = std::max(change, fabs(A[i][j] - z));
                
                A[i][j] = z; /// $$\bar{a}_{i,j} = \frac{\sum_{t=1}^{T-1} \xi_t(i,j)}{\sum_{t=1}^{T-1} \gamma_t(i)}$$
            }
        }
        
        // apply B emission updates
        for (int j = 0; j < N; ++j)
        {
            for (int k = 0; k < M; ++k)
            {
                double den = 0.0, num = 0.0;
                
                for (int s = 0; s < S; ++s)
                {
                    const int T = (int) y[s].size();
                    
                    for (int t = 0; t < T; ++t)
                    {
                        /// $$\sum_{t=1}^T \gamma^{*}_t(j)$$ is the probability of being in state $S_j$ while observing symbol $O_t = v_k$.
                        if (y[s][t] == k)
                            num += gamma[s][t][j]; 

                        den += gamma[s][t][j]; /// $$\sum_{t=1}^T \gamma_t(j)$$
                    }
                }
                
                // avoid locking a parameter at zero.
                double z = (num == 0) ? NEAR_ZERO : (num / den);
                change = std::max(change, fabs(B[j][k] - z));
                
                /// $$\bar{b}_i(k) = \frac{\sum_{t=1}^T \gamma^{*}_t(j)}{\sum_{t=1}^T \gamma_t(j)}$$
                B[j][k] = z;
            }
        }
    }
    
    if (m_logger.level() >= 1)
    {
        Message txt(1);
        txt << "BaumWelch log-likelihood of observations for params A and B is " << l << " after " << it << " iterations";
        m_logger.logMsg(txt);
        m_logger.flush();
    }
    
    return l;
}

//
/// <summary>
///   Calculates the probability that this model has generated the given sequence.
/// </summary>
/// <remarks>
///   Evaluation problem. Given the HMM  $\lambda = (\pi, A, B)$ and  the observation
///   sequence $O = {o_1, o_2, \ldots , o_T}$, calculate the probability that model
///   $\lambda$ has generated sequence $O$. This is computed using the forward algorithm.
/// </remarks>
/// <param name="y" />
///   An observation sequences.
///
/// <param name="pi" />
///   The hidden Markov model's initial state distribution.
///
/// <param name="A" />
///   The hidden Markov model's transition probability matrix.
///
/// <param name="B" />
///   The hidden Markov model's emission probability matrix.
///
/// <param name="logarithm" />
///   True to return the log-likelihood, false to return the likelihood. The default is false.
///
/// <returns>
///   The likelihood or log-likelihood that the given sequence has been generated by this model.
/// </returns>
double
BaumWelch::seqProb(const std::vector<int>& y, 
                    const std::vector<double>& pi, 
                    const Matrix<double>& A, 
                    const Matrix<double>& B, 
                    bool logarithm ) const
{
    if (y.empty())
        return BAD_VALUE;
    
    const int T = (int) y.size();
    const int N = (int) pi.size();            // number of states
    
    Matrix<double> alpha(T, N, 0.0);    
    std::vector<double> cinv(T, 0.0); 
    
    // compute scaled forward probabilities 
    forward(y, pi, A, B, alpha, cinv);
    
    /// as we are using scaled variables we cannot just sum up the $\hat{\alpha}$ terms (see Rabiner last paragraph, page 272)
    /// instead use eqn 102, page 273 which is
    /// $$ P(O | \lambda) = \frac{1}{\prod_{t=1}^T c_t} $$
    /// where $$c_t = \frac{1}{\sum_i^N \bar{\alpha}_{t}(i)}$$
    double lnProb = 0.0;
    for (int t = 0; t < T; ++t) 
    {
        if (cinv[t] > 0.0)
            lnProb += log(cinv[t]);
        else return (logarithm) ? BAD_VALUE : 0.0;
    }
    
    return (logarithm) ? (lnProb) : exp(lnProb);
}

double
BaumWelch::klMeasure( HMM& m1, HMM& m2, int T, int S ) const
/// define a distance measure $D(\lambda_0, \lambda)$ between two Markov models $\lambda_0$ and $\lambda$ by
/// $$D(\lambda_0, \lambda) = \lim_{T \rightarrow \infty} \frac{1}{T} \big [ \log P(O_T | \lambda_0) - \log P(O_T | \lambda) \big ]$$
/// see Juang and Rabiner, eqn 6, page 394
/// $D$ is estimated using a Monte Carlo approach
{ 
    // don't assume same alphabets; this need not be true
    std::vector<std::string> alphabet1 = m1.emission();
    std::vector<std::string> alphabet2 = m2.emission();
    
    // map emission of m2 to integers
    std::map<std::string,int> map;  
    for (int  i = 0; i < alphabet2.size(); ++i)
        map.insert( std::map<std::string, int>::value_type(alphabet2[i], i) ); 
    
    bool useLogs = true;
    double dist = 0.0;
    std::vector<int> trans_out(T,0);
    
    for (int s = 0; s < S; ++s)
    {
        // out[0] is states, out[1] is emissions
        const std::vector<std::vector<int>>& out = m1.run( T );
        
        // translate these m1 emissions to m2 emissions
        for (int t = 0; t < T; ++t)
        {
            auto findIdx = map.find( alphabet1[out[1][t]] );
            if (findIdx == map.end())
            {              
                if (m_logger.level() >= 1)
                {
                    Message txt(1);
                    txt << "klMeasure::Unknown symbol \"" << alphabet1[out[1][t]] << "\" in observation sequence";
                    m_logger.logMsg(txt);
                    m_logger.flush();
                }
                return LARGE_VALUE;
            }
            
            trans_out[t] = findIdx->second;
        }
        
        double log_p1 = seqProb(out[1], m1.initial_probs(), m1.trans_probs(), m1.emiss_probs(0), useLogs );
        double log_p2 = seqProb(trans_out, m2.initial_probs(), m2.trans_probs(), m2.emiss_probs(0), useLogs );
        
        // if either prob is zero; return an arbitrary large number/distance
        if ((log_p1 == BAD_VALUE) || (log_p2 == BAD_VALUE))
            return LARGE_VALUE;
        
        dist += (log_p1 - log_p2);
    }
    
    return dist / (T * S);
}

double 
BaumWelch::klDistance( HMM& m1,  HMM& m2, int T, int N ) const
/// define a symmetric distance measure $D_s(\lambda_0, \lambda)$ between two Markov models $\lambda_0$ and $\lambda$ by
/// $$D_s(\lambda_0, \lambda) = \frac{1}{2} \big [ D(\lambda_0, \lambda) + D(\lambda, \lambda_0) \big ]$$
/// see Juang and Rabiner, eqn 7, page 395
{
    double d1 = klMeasure(m1, m2, T, N);
    double d2 = klMeasure(m2, m1, T, N);
    
    double d = ((d1 + d2) / 2.0);
    
    if (m_logger.level() >= 2)
    {
        Message txt(2);
        txt << "Kullback-Leibler distance using seqs of length " << T << " over " << N << " trials is " << d;
        m_logger.logMsg(txt);
        m_logger.flush();
    }
    
    return d;
}


std::vector<int> 
BaumWelch::viterbi(const HMM& model, const HMMSequence& seq) const
{
    std::vector<std::vector<int>> seqs_int = toIntegerRep(std::vector<HMMSequence>(1,seq), model.emission() );
    return viterbi(seqs_int[0], model.initial_probs(), model.trans_probs(), model.emiss_probs(0));
}

std::vector<int> 
BaumWelch::viterbi(const std::vector<int>& y, 
                  const std::vector<double>& pi, 
                  const Matrix<double>& A, 
                  const Matrix<double>& B) const
{ 
    if (y.empty()) 
        return std::vector<int>(); 
    
    const int T = (int) y.size();
    const int N = (int) pi.size();            // number of states
    
    double delta[T][N]; 
    int psy[T][N]; 
    std::vector<int> stateSeq(T,0); 
    
    for (int i = 0; i < N; ++i) 
    { 
        delta[0][i] = -log(pi[i]) - log(B[i][y[0]]); 
        psy[0][i] = 0; 
    } 
    
    for (int t = 1; t < T; ++t) 
    { 
        for (int i = 0; i < N; ++i) 
        {
            double minDelta = LARGE_VALUE; 
            int min_psy = 0; 
            
            for (int j = 0; j < N; ++j) 
            { 
                double d = delta[t-1][j] - log(A[j][i]); 
                
                if (d < minDelta) 
                { 
                    minDelta = d; 
                    min_psy = j; 
                } 
            } 
            
            delta[t][i] = minDelta - log(B[i][y[t]]); 
            psy[t][i]   = min_psy; 
        }
    }
       
    double minLnProb = LARGE_VALUE; 
    for (int i = 0; i < N; ++i) 
    { 
        double p = delta[T-1][i]; 
        
        if (p < minLnProb) 
        { 
            minLnProb = p; 
            stateSeq[T - 1] = i; 
        } 
    } 
    minLnProb = -minLnProb; 
    
    for (int t = T - 2; t >= 0; --t) 
        stateSeq[t] = psy[t+1][stateSeq[t+1]]; 
    
    return stateSeq;
} 

///////////////////




