/* AUtils 18/01/2021

 $$$$$$$$$$$$$$$$
 $   AUtils.h   $
 $$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:
 
 Template utilities for 
 
 i)   streaming std::map and std::vector, 
 ii)  converting between std::map and std::vector, and 
 iii) some basic statistics

*/


#ifndef __UTILS_H__
#define __UTILS_H__


#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <cmath>
#include <numeric>
#include <functional>
#include <algorithm>
#include <regex>

//
// stream  ooperators for std::vector, std::map, and std::pair
//

//
// begin of declarations
//

template <typename T>
std::ostream&
operator<<( std::ostream& ostr, const std::vector<T>& v );

template <typename T>
std::istream& 
operator>>( std::istream& istr, std::vector<T>& v );

template <typename K, typename T>
std::ostream& 
operator<<(std::ostream& ostr, const std::map<K,T>& m);

template <typename K, typename T>
std::istream&
operator>>(std::istream& istr, std::map<K,T>& m);

template <class T, class U>
std::ostream&
operator<<(std::ostream& ostr, const std::pair<T,U>& v);

template <class T, class U>
std::istream&
operator>>(std::istream& istr, std::pair<T,U>& v);


//
// end of declarations
//


//
// stream operators;
//

template <class T, class U>
inline std::ostream&
operator<<(std::ostream& ostr, const std::pair<T,U>& v)
{
    ostr << v.first << ' ' << v.second << ' ';
    return ostr;
}

template <class T, class U>
inline std::istream&
operator>>(std::istream& istr, std::pair<T,U>&v)
{
    istr >> v.first >> v.second;
    return istr;
}

template <typename T>
std::ostream&
operator<<( std::ostream& ostr, const std::vector<T>& v )
{
    ostr << v.size() << '\n';
    int count = 0;
    auto print = [&ostr, &count](const T& val) { ostr << val << (((++count % 20) == 0) ? '\n' : ' '); };
    std::for_each(v.begin(), v.end(), print);
    return ostr;
}

template <typename T>
std::istream& 
operator>>( std::istream& istr, std::vector<T>& v )
{
    int len = 0;    
    istr >> len;
    v.resize(len);
    auto read = [&istr](T& val) { istr >> val; };
    std::for_each(v.begin(), v.end(), read);
    return istr;
}

template <typename K, typename T>
std::ostream& 
operator<<(std::ostream& ostr, const std::map<K,T>& m)
{
    ostr << m.size() << '\n';
    auto print = [&ostr](const typename std::map<K,T>::value_type& val) { ostr << val.first << ' ' << val.second <<  '\n'; };
    std::for_each(m.begin(), m.end(), print);
    return ostr;
}

template <typename K, typename T>
std::istream&
operator>>(std::istream& istr, std::map<K,T>& m)
{
    m.clear();

    K key = K();
    T item = T();
    
    int len = 0;
    istr >> len;
    
    auto lastPos = m.begin();
    for (int i = 0; i < len; ++i)
    {
        istr >> key >> item;
        lastPos = m.insert( lastPos, typename std::map<K,T>::value_type( key, item ));
    }
    return istr;
}


//
// useful templates
//

namespace util {

    //
    // begin of declarations
    //


    inline std::string 
    toUpper(std::string s) 
    { 
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::toupper(c); });
        return s;
    }

    inline std::string 
    toLower(std::string s) 
    {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
        return s;
    }

    inline std::string  
    trim(const std::string& s)
    {
        size_t first = s.find_first_not_of(' ');
        if (first == std::string::npos)
            return s;
        size_t last = s.find_last_not_of(' ');
        return s.substr(first, (last - first + 1));
    }

    inline bool
    isFloat( const std::string& s ) 
    { 
        // we do not regard integers as floats
        const std::regex _float(R"((\s*)(?=.)([+-]?(\d*)(\.(\d+)?))(\s*))");
        const std::regex _float_sci(R"((\s*)([+-]?(\d*)(\.(\d+)?)?)(E|e)\d+(\s*))");
        return (std::regex_match(s, _float) || std::regex_match(s, _float_sci));
    }

    inline bool
    isInteger( const std::string& s ) 
    { 
        const std::regex _integer(R"((\s*)([+-]?)(\d+)(\s*))");
        return std::regex_match(s, _integer);
    }




    //template <typename V, typename U = int>
    //std::vector<U>
    //ordering( const std::vector<V>& values, std::function<bool(const V&, const V&)> cmp_op = std::less<V>() );

    template <typename K, typename T> 
    std::vector<K> 
    toKeyVector(const std::map<K,T>& m);

    template <typename K, typename T> 
    void
    toKeyVector(std::vector<K>& arg, const std::map<K,T>& m);

    template <typename K, typename T> 
    std::vector<T> 
    toValueVector(const std::map<K,T>& m);

    template <typename K,typename T> 
    std::map<K,T> 
    toKeyMap(const std::vector<K>& v);

    template <typename K,typename T> 
    std::map<K,T> 
    toValueMap(const std::vector<T>& v);



    template <typename T>
    T
    mean( const std::vector<T>& x );

    template <typename T>
    T
    sum( const std::vector<T>& x );

    template <typename T>
    T 
    var( const std::vector<T>& x );

    template <typename T>
    T
    std_dev( const std::vector<T>& x );

    template <typename T>
    T
    median( std::vector<T> x );

    template <typename T>
    T
    percentile( std::vector<T> x, int p );

    template <typename T>
    T
    mad( const std::vector<T>& x );

    //
    // Implementations
    //


    template <typename V, typename U = int>
    std::vector<U>
    ordering( const std::vector<V>& values, const std::function<bool(const V&, const V&)>& cmp_op = std::less<V>() )
    {
        typename std::vector<U> indexes(values.size());
        std::iota(indexes.begin(), indexes.end(), U(0));
        auto op = [&values, &cmp_op](const U& a, const U& b) { return cmp_op(values[a], values[b]); };
        std::sort( indexes.begin(), indexes.end(), op );
        return indexes;
    }



    template <typename K, typename T> 
    std::vector<K> 
    inline toKeyVector(const std::map<K,T>& m)
    // return a sorted vector of the unique keys in map
    {
        typename std::vector<K> v(m.size());
        std::transform( m.begin(), m.end(), v.begin(), [](const std::pair<K,T>& c) -> K { return c.first; } );
        return v;
    }

    template <typename K, typename T> 
    inline void
    toKeyVector(std::vector<K>& v, const std::map<K,T>& m)
    // return a sorted vector of the unique keys in map
    {
        v.resize(m.size());
        std::transform( m.begin(), m.end(), v.begin(), [](const std::pair<K,T>& c) -> K { return c.first; } );
    }

    template <typename K, typename T> 
    inline std::vector<T> 
    toValueVector(const std::map<K,T>& m)
    // return a sorted vector of the values in map
    {
        typename std::vector<T> v(m.size());
        std::transform( m.begin(), m.end(), v.begin(), [](const std::pair<K,T>& c) -> T { return c.second; } );
        return v;
    }

    template <typename K, typename T> 
    inline void 
    toValueVector(std::vector<T>& v, const std::map<K,T>& m)
    // return a sorted vector of the values in map
    {
        v.resize(m.size());
        std::transform( m.begin(), m.end(), v.begin(), [](const std::pair<K,T>& c) -> K { return c.second; } );
    }

    //

    template <typename K, typename T = int> 
    std::map<K,T> 
    toKeyMap(const std::vector<K>& v)
    // use the vector index as a value
    {
        typename std::map<K,T> m;
        
        T c = T();
        auto lastPos = m.begin();
        
        for (auto i = v.begin(); i != v.end(); ++i, ++c)
        {
            // this works more effieciently when the vector is sorted least value first 
            lastPos = m.insert( lastPos, typename std::map<K,T>::value_type(*i, c) );    
        } 
        return m;
    }

    template <typename K, typename T = int> 
    void 
    toKeyMap(std::map<K,T>& m, const std::vector<K>& v)
    // use the vector index as a value
    {
        T c = T();
        m.clear();
        auto lastPos = m.begin();
        
        for (auto i = v.begin(); i != v.end(); ++i, ++c)
        {
            // this works more effieciently when the vector is sorted least value first 
            lastPos = m.insert( lastPos, typename std::map<K,T>::value_type(*i, c) );    
        } 
    }

    template <typename K, typename T = int> 
    std::map<K,T> 
    toValueMap(const std::vector<T>& v)
    // use the vector index as a key
    {
        typename std::map<K,T> m;
        
        K c = K();
        auto lastPos = m.begin();
        
        for (auto i = v.begin(); i != v.end(); ++i, ++c)
        {
            // this works more effieciently when the vector is sorted least value first 
            lastPos = m.insert( lastPos, typename std::map<K,T>::value_type(c, *i) );    
        } 
        return m;
    }

    template <typename K, typename T = int> 
    void
    toValueMap(std::map<K,T>& m, const std::vector<T>& v)
    // use the vector index as a key
    {
        K c = K();
        m.clear();
        auto lastPos = m.begin();
        
        for (auto i = v.begin(); i != v.end(); ++i, ++c)
        {
            // this works more effieciently when the vector is sorted least value first 
            lastPos = m.insert( lastPos, typename std::map<K,T>::value_type(c, *i) );    
        } 
    }

    //
    // simple statistics
    //

    // WBY these should really be defined as iterator begin() and end()
    template <typename T>
    inline T
    mean( const std::vector<T> &x )
    {
        return std::accumulate(x.begin(), x.end(), T(), std::plus<T>()) / T(x.size());
    }

    template <typename T>
    inline T
    sum( const std::vector<T> &x )
    {
        return std::accumulate(x.begin(), x.end(), T(), std::plus<T>());
    }

    template <typename T>
    T 
    var( const std::vector<T> &x )
    {
        T ep = T(), var = T(), avg = mean(x);
        for (size_t i = 0; i < x.size(); ++i) 
        {
            T s = x[i] - avg;
            var += (s * s);
            ep  += s;
        }
        return (var - ((ep * ep) / T(x.size()))) / (T(x.size() - 1));
    }

    template <typename T>
    inline T
    std_dev( const std::vector<T> &x )
    {
        return std::sqrt( var( x ) );
    }

    template <typename T>
    inline T
    median( std::vector<T> x )
    {
        std::sort(x.begin(),x.end());
        return x[(x.size() / 2.0) - 1];
    }

    template <typename T>
    inline T
    percentile( std::vector<T> x, int p ) 
    // see https://en.wikipedia.org/wiki/Percentile
    {
        std::sort(x.begin(), x.end());
        return (p == 0) ? x[0] : x[int(x.size() * (p / 100.0)) -1];
    }

    template <typename T>
    T
    mad( const std::vector<T> &x )
    // median absolute deviation
    // https://en.wikipedia.org/wiki/Median_absolute_deviation
    {
        T med = median(x);
        std::vector<double> vals(x.size());
        
        for (int i = 0; i < x.size(); ++i)
        {
            T v = x[i] - med;
            vals[i] = (v >= 0) ? v : -v;
        }
        
        return median(vals);
    }

}

#endif




