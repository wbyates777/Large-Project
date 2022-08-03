/* HydVertex 19/02/2021

 $$$$$$$$$$$$$$$$$$$
 $   HydVertex.h   $
 $$$$$$$$$$$$$$$$$$$

 by W.B. Yates
 Copyright (c) University of Exeter. All rights reserved.
 History:

*/

#ifndef __HYDVERTEX_H__
#define __HYDVERTEX_H__

#ifndef __HYDTYPES_H__
#include "HydTypes.h" 
#endif

class HydVertex
{
    public:
    
    HydVertex(void): x(m_data[0]), y(m_data[1]), z(m_data[2]) { x = y = z = 0.0; }
    explicit HydVertex(HydFloat v): x(m_data[0]), y(m_data[1]), z(m_data[2]) { x = y = z = v; }
    HydVertex(HydFloat xx, HydFloat yy): x(m_data[0]), y(m_data[1]), z(m_data[2]) { x = xx; y = yy; z = 0.0; }
    HydVertex(HydFloat xx, HydFloat yy, HydFloat zz): x(m_data[0]), y(m_data[1]), z(m_data[2]) {  x = xx; y = yy; z = zz; }
    
    HydVertex(const HydVertex& v): x(m_data[0]), y(m_data[1]), z(m_data[2]) 
    {
        m_data[0] = v.m_data[0];
        m_data[1] = v.m_data[1];
        m_data[2] = v.m_data[2];
    }
    
    HydVertex&
    operator=(const HydVertex& v) 
    {
        if (this == &v)
            return *this;
        
        m_data[0] = v.m_data[0];
        m_data[1] = v.m_data[1];
        m_data[2] = v.m_data[2];
        return *this;
    }
    
    HydVertex&
    operator=(HydFloat v) 
    {
        m_data[0] = v;
        m_data[1] = v;
        m_data[2] = v;
        return *this;
    }
    
    void 
    set(HydFloat xx, HydFloat yy, HydFloat zz) { x = xx; y = yy; z = zz; }
    
    void 
    set(const HydVertex& v) { x = v.x; y = v.y; z = v.z; }

    // arithmetic operations
    HydVertex& operator+=(const HydVertex& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
    HydVertex& operator-=(const HydVertex& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }        
    HydVertex& operator*=(const HydVertex& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
    HydVertex& operator/=(const HydVertex& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }    
    
    const HydVertex operator-( void ) { return HydVertex(-x,-y,-z); } 
    const HydVertex operator+(const HydVertex& rhs) const { HydVertex r(*this); r += rhs; return r; }
    const HydVertex operator-(const HydVertex& rhs) const { HydVertex r(*this); r -= rhs; return r; }        
    const HydVertex operator*(const HydVertex& rhs) const { HydVertex r(*this); r *= rhs; return r; }
    const HydVertex operator/(const HydVertex& rhs) const { HydVertex r(*this); r /= rhs; return r; }

    HydVertex& operator+=(const HydFloat rhs) { x += rhs; y += rhs; z += rhs; return *this; }
    HydVertex& operator-=(const HydFloat rhs) { x -= rhs; y -= rhs; z -= rhs; return *this; }        
    HydVertex& operator*=(const HydFloat rhs) { x *= rhs; y *= rhs; z *= rhs; return *this; }
    HydVertex& operator/=(const HydFloat rhs) { x /= rhs; y /= rhs; z /= rhs; return *this; }    
    
    const HydVertex operator+(const HydFloat rhs) const { HydVertex r(*this); r += rhs; return r; }
    const HydVertex operator-(const HydFloat rhs) const { HydVertex r(*this); r -= rhs; return r; }        
    const HydVertex operator*(const HydFloat rhs) const { HydVertex r(*this); r *= rhs; return r; }
    const HydVertex operator/(const HydFloat rhs) const { HydVertex r(*this); r /= rhs; return r; }
    
    // relational operators
    bool operator==(const HydVertex& rhs) const { return ((x == rhs.x) && (y == rhs.y) && (z == rhs.z)); }    
    bool operator!=(const HydVertex& rhs) const { return ((x != rhs.x) || (y != rhs.y) || (z != rhs.z)); }
    
    HydFloat&
    operator[](int i) { return m_data[i]; }
    
    HydFloat
    operator[](int i) const { return m_data[i]; }
    
    HydFloat& x;
    HydFloat& y;
    HydFloat& z;
    
    private:
    
    HydFloat m_data[3];
};

       
inline const HydVertex operator*(const HydFloat lhs, const HydVertex& v)  { return v * lhs; }

#endif /* HydVertex_h */
