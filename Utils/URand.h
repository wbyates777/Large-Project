/* Header file for uniform random number generator 09/02/06

                $$$$$$$$$$$$$$$$$$$$$$$
                $   URand.h - header  $ 
                $$$$$$$$$$$$$$$$$$$$$$$

   by W.B. Yates
   Uniform Distribution (0,1)
   from "Numerical Recipes in C, 2nd Edition", by
   Press, Fannery, Teukolsky and Vetterling
 
   Simple but portable
 
   TODO: replace with #include <random> and, for example std::mt19937_64
   TODO: its still here so I could reproduce earlier results that use this generator
*/

#ifndef __URAND_H__
#define __URAND_H__


#include <iostream>


class URand 
{
public:
    URand( void );
    
    explicit URand(unsigned int s);
    
	double 
	ran( void )
	{
		++m_count;
		return ran1( m_idum );
	}

    //!< set the seed and reinitialise the sequence
    void
    seed( unsigned int s );

    //!< what seed is being used
    unsigned int 
    seed( void ) const { return m_seed; }

    //!< at what position in random sequence seeded by s
    unsigned long int 
    count( void ) const { return m_count; }
    
    //!< return me to the point p in the sequence seeded by s 
    //!< reset(s,0) == seed(s)
    void
    reset( unsigned int s, unsigned long p );  
    
    //!< reset the sequence
    void
    reset( void ) { seed( m_seed ); }
	
private:
    
    double 
    ran1(long& idum);
    
    unsigned long int m_count;
    unsigned int m_seed;
	long m_idum;

	 //!< ran1 statics; defined as class variables for thread safety
    long m_iy;
    long m_iv[32];
};

 
std::ostream&
operator<<(std::ostream& ostr, const URand &r);

std::istream&
operator>>(std::istream& istr, URand &r);

#endif // __URAND_H__


