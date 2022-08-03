/* Matrix Template Class
 
 $$$$$$$$$$$$$$$$$$$$$$$
 $   Matrix.h - defs   $
 $$$$$$$$$$$$$$$$$$$$$$$

 Copyright 2011 (c) W.B. Yates. All rights reserved. 
 
 History:
 
 Numerical i.e. float or double Matrix template

*/

#ifndef __MATRIX_H__
#define __MATRIX_H__


#include <iostream>
#include <vector>



template <typename T>
class Matrix 
{

public:
	
	enum VectorType  { ROW, COL };
	
	Matrix( void ): m_rows(0), m_cols(0), m_rawData() {}	
    Matrix( const int r, const int c, const T &x = T() ): m_rows(r), m_cols(c), m_rawData(r, std::vector<T>(c, x)) {}
    
    //Matrix( const int s, const T &x = T() ): m_rows(s), m_cols(s), m_rawData(s, std::vector<T>(s, x)) {}
    
	explicit Matrix( const std::vector< std::vector<T> >& values ): m_rows(0), m_cols(0), m_rawData() 
	{
		setMatrix( values );
	}
	
		
	Matrix( const std::vector<T>& v, VectorType vt = ROW ); // assume v is a row

	~Matrix( void ) { m_rows = 0; m_cols = 0; }
    
    Matrix( const Matrix& m ): m_rows(m.m_rows), m_cols(m.m_cols), m_rawData(m.m_rawData) {}
    
    Matrix( Matrix&& m ): m_rows(m.m_rows), m_cols(m.m_cols), m_rawData(std::move(m.m_rawData)) {}
    
    Matrix&
    operator=( const Matrix& m );
    
    Matrix&
    operator=( Matrix&& m );
    
    void 
    clear( void ) { m_rows = 0; m_cols = 0; m_rawData.clear(); } 
	
	Matrix<T>& 
	operator=( const T &a );		// assign a to every element
		
    void
    setMatrix( const int r, const int c, const T& v = T() )
    {
        m_rows = r;
        m_cols = c;
        m_rawData.resize( r, std::vector<T>(c, v) );	
    }
    
	void
	setMatrix( const std::vector< std::vector<T> >& values ) 
	{
		m_rows = (int) values.size();
        m_cols = (m_rows) ? (int) values[0].size() : 0;
		m_rawData = values;
	}
	
	std::vector<T>& 
    operator[]( const int i ) { return m_rawData[i]; }	// return row i
    
	const std::vector<T>& 
    operator[]( const int i ) const { return m_rawData[i]; }
	
	std::vector<T> 
    column( const int colIdx ) const; // extract column copy c as a (row) vector
    
    void
    column( const int colIdx, const std::vector<T>& c );
	
	int rows( void ) const { return m_rows; }
	int cols( void ) const { return m_cols; }

	void 
	resize( const int r, const int c, const T&); // will preserve/truncate existing data accordingly

    void 
    resize( const int r, const int c); // will preserve/truncate existing data accordingly
    
	const std::vector< std::vector<T> >&
	data( void ) const { return m_rawData; }
	
	std::vector< std::vector<T> >&
	data( void ) { return m_rawData; }
	
private:
		
	int m_rows;
	int m_cols;
	std::vector< std::vector<T> > m_rawData;
	
};


template <typename T> 
std::vector<std::vector<T>> 
toVector(const Matrix<T>& m)
{
    typename std::vector<std::vector<T>> v(m.rows(), std::vector<T>());
    
    for (int i = 0; i < m.rows(); ++i)
        v[i] = m[i];
    
    return v;
}

template <class T>
std::ostream&
operator<<( std::ostream& ostr, const Matrix<T>& m );

template <class T>
std::istream&
operator>>( std::istream& istr, Matrix<T>& m );


template <class T>
std::ostream&
operator<<( std::ostream& ostr, const Matrix<T>& m ) 
{
    ostr << m.rows() << ' ' << m.cols() << '\n';
    for (int i = 0; i < m.rows(); ++i)
    {
        for (int j = 0; j < m.cols(); ++j)
        {
            ostr << m[i][j] << ' ';
        }
        ostr << '\n';
    }
    ostr << '\n';
    return ostr;
}

template <class T>
std::istream&
operator>>( std::istream& istr, Matrix<T>& m )
{
    int r = 0, c = 0;
    istr >> r;
    istr >> c;
    
    m.resize(r,c);	
    for (int i = 0; i < m.rows(); ++i)
    {
        for (int j = 0; j < m.cols(); ++j)
        {
            istr >> m[i][j];
        }
    }
    return istr;
}

template <class T>
Matrix<T>&
Matrix<T>::operator=( const Matrix<T>& m )
{
    if (&m == this)
        return *this;
    
    m_rows = m.m_rows;
    m_cols = m.m_cols;
    m_rawData = m.m_rawData;
    
    return *this;
}

template <class T>
Matrix<T>&
Matrix<T>::operator=( Matrix<T>&& m )
{
    if (&m == this)
        return *this;
    
    m_rows = m.m_rows;
    m_cols = m.m_cols;
    m_rawData = std::move(m.m_rawData);
    
    return *this;
}


// v is a row vector by default
template <class T>
Matrix<T>::Matrix( const std::vector<T>& v, VectorType vt ) : m_rows(0), m_cols(0), m_rawData()
{
	if (vt == ROW)
	{
		m_rows = 1;
		m_cols = v.size();
		m_rawData.resize( 1, v );
	}
	else
	{
		m_cols = 1;
		m_rows = v.size();
		m_rawData.resize( v.size(), std::vector<T>(1) );
		for (int i = 0; i < v.size(); ++i)
		{
			m_rawData[i][0] = v[i];
		}
	}
}

template <class T>
void 
Matrix<T>::resize( const int r, const int c ) 
{        
    m_rows = r;
    m_cols = c;
    
    m_rawData.resize( r, std::vector<T>(c) );
}

template <class T>
void 
Matrix<T>::resize( const int r, const int c, const T& v ) 
{		
	m_rows = r;
	m_cols = c;
    m_rawData.clear();
	m_rawData.resize( r, std::vector<T>(c, v) );
}

template <class T>
Matrix<T>& 
Matrix<T>::operator=( const T &a )	
// assign a to every element
{
	for (int i = 0; i< m_rows; ++i)
	{
		for (int j = 0; j < m_cols; ++j)
		{
			m_rawData[i][j] = a;
		}
	}
	return *this;
}

// extract a copy of column c from matrix as a vector; use transpose(v) to construct a column vector (see below)
template <class T>
std::vector<T> 
Matrix<T>::column( const int colIdx ) const
{
	assert(colIdx < m_cols);
	
	std::vector<T> retVal(m_rows); 
	for (int j = 0; j < m_rows; ++j)
	{
		retVal[j] = m_rawData[j][colIdx];
	}
	return retVal;
}

template <class T>
void
Matrix<T>::column( const int colIdx, const std::vector<T>& col )
{
    assert(col.size() == m_rows && colIdx < m_cols);
    for (int j = 0; j < m_rows; ++j)
    {
        m_rawData[j][colIdx] = col[j];
    }
}



#endif // __MATRIX_H__ 


