/* HMMHelper 19/05/2017

 $$$$$$$$$$$$$$$$$$$
 $   HMMHelper.h   $
 $$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

*/


#ifndef __HMMHELPER_H__
#define __HMMHELPER_H__

#include <string>

#ifndef __MATRIX_H__
#include "AMatrix.h"
#endif

#ifndef __HMM_H__
#include "HMM.h"
#endif

class HMMHelper 
{
public:
    
    HMMHelper( void ){}
    
    static bool
    addInitialState( HMM& model );
    
    static bool
    delInitialState( HMM& model );
    
    // state and emission alphabets
    static std::vector<std::string>
    addInitialState( const std::vector<std::string>& states );
    
    static std::vector<std::string>
    delInitialState( const std::vector<std::string>& states );
    
    // initial state probability vector
    static std::vector<double>
    equi_nu( int size );
    
    static std::vector<double>
    initialState_nu( int size );

    // useful matricies
    static Matrix<double>
    identity( int size, double diag = 1.0, double offdiag = 0.0 );
    
    static Matrix<double>
    equi( int rows, int cols );
    
    static Matrix<double>
    equi( int size ) { return equi( size, size ); }
    
    static Matrix<double>
    delZeros( const Matrix<double>& arg );
    
    // transition matricies
    static Matrix<double>
    addInitialState( const Matrix<double>& arg );
    
    static Matrix<double>
    delInitialState( const Matrix<double>& arg );

    // emission matricies
    static Matrix<double>
    addInitialEmission( const Matrix<double>& arg );
    
    static Matrix<double>
    delInitialEmission( const Matrix<double>& arg );
    
    // training observations
    static std::vector<std::vector<std::string>>
    addInitialState( const std::vector<std::vector<std::string>>& train );
    
    static std::vector<std::vector<std::string>>
    delInitialState( const std::vector<std::vector<std::string>>& train );

    
private:

};


#endif


