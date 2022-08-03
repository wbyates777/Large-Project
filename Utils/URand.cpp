/* Code file for uniform distrubution random number generator 09/02/06

                $$$$$$$$$$$$$$$$$$$$$$$
                $   URand.cpp - code  $ 
                $$$$$$$$$$$$$$$$$$$$$$$

   by W.B. Yates

*/

#ifndef __URAND_H__
#include "URand.h"
#endif

#include <cmath>


const int DEFAULT_SEED = 13;

std::ostream&
operator<<(std::ostream& ostr, const URand &rn)
{
	ostr << rn.count() << ' ' << rn.seed() << '\n';
	return ostr;
}


std::istream&
operator>>(std::istream& istr, URand &rn)
{
    unsigned int seed = 0;
	unsigned long count = 0;

    istr >> count >> seed;
	rn.reset( seed, count );
	
	return istr;
}

URand::URand( void ): m_count(0), m_seed(0), m_idum(0), m_iy(0)
{
    seed(DEFAULT_SEED);
}

URand::URand( unsigned int s ): m_count(0) ,m_seed(0), m_idum(0), m_iy(0)
{
    seed(s);
}

void
URand::seed( unsigned int s )
{   
    assert(s != 0);
    m_idum = s;
	m_idum *= -1;
    m_seed = s;
    m_count = 0;
    ran1( m_idum );
}


void
URand::reset( unsigned int s, unsigned long p )
{
    seed( s );
    for (unsigned int i = 0; i < p; ++i)
        ran();
}


double 
URand::ran1( long &idum )
{
    constexpr long IA=16807,IM=2147483647,IQ=127773,IR=2836,NTAB=32;
    constexpr long NDIV=(1+(IM-1)/NTAB);
    constexpr double EPS=1.2e-7,AM=1.0/IM,RNMX=(1.0-EPS);

    long j;
	long k;
    double temp;

    if (idum <= 0 || !m_iy) 
    {
        if (-idum < 1) 
            idum=1;
        else idum = -idum;
        for (j=NTAB+7;j>=0;--j) 
		{
            k=idum/IQ;
            idum=IA*(idum-k*IQ)-IR*k;
            if (idum < 0) idum += IM;
            if (j < NTAB) m_iv[j] = idum;
        }
        m_iy=m_iv[0];
    }
    k=idum/IQ;
    idum=IA*(idum-k*IQ)-IR*k;
    if (idum < 0) 
        idum += IM;
    j=m_iy/NDIV;
    m_iy=m_iv[j];
    m_iv[j] = idum;

	if ((temp=AM*m_iy) > RNMX) 
        return RNMX;
    else return temp;	
}
