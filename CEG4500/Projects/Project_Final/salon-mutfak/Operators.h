/*
 *  Operators.h
 *  
 *
 *  Created by Paul Fultz II on 12/8/08.
 *  Copyright 2008 KM Audio. All rights reserved.
 *
 */


#ifndef ___Operators_____
#define ___Operators_____

namespace km { namespace Core {

//#define KM_BINARY_OPERATOR_COMMUTATIVE( NAME, OP )                      \
//template <class T, class U >                                            \
//class NAME                                                              \
//{                                                                       \
//public:                                                                 \
//  friend T operator OP( T lhs, const U& rhs ) { return lhs OP##= rhs; } \
//  friend T operator OP( const U& lhs, T rhs ) { return rhs OP##= lhs; } \
//};                                                                      \
//\
//template <class T >                                                     \
//class NAME                                                              \
//{                                                                       \
//public:                                                                 \
//  friend T operator OP( T lhs, const T& rhs ) { return lhs OP##= rhs; } \
//};
//
//#define KM_BINARY_OPERATOR_NON_COMMUTATIVE( NAME, OP )                  \
//template <class T, class U >                                            \
//class NAME                                                              \
//{                                                                       \
//public:                                                                 \
//  friend T operator OP( T lhs, const U& rhs ) { return lhs OP##= rhs; } \
//};                                                                      \
//\
//template <class T, class U >                                            \
//class NAME##Left                                                        \
//{                                                                       \
//public:                                                                 \
//  friend T operator OP( const U& lhs, const T& rhs )                    \
//    { return T( lhs ) OP##= rhs; }                                      \
//};                                                                      \
//\
//template <class T >                                                     \
//class NAME                                                              \
//{                                                                       \
//public:                                                                 \
//  friend T operator OP( T lhs, const T& rhs ) { return lhs OP##= rhs; } \
//};
//
//
//KM_BINARY_OPERATOR_COMMUTATIVE( Multipliable, * )
//KM_BINARY_OPERATOR_COMMUTATIVE( Addable, + )
//KM_BINARY_OPERATOR_NON_COMMUTATIVE( Subtractable, - )
//KM_BINARY_OPERATOR_NON_COMMUTATIVE( Dividable, / )
//KM_BINARY_OPERATOR_NON_COMMUTATIVE( Modable, % )
//KM_BINARY_OPERATOR_COMMUTATIVE( Xorable, ^ )
//KM_BINARY_OPERATOR_COMMUTATIVE( Andable, & )
//KM_BINARY_OPERATOR_COMMUTATIVE( Orable, | )
//
//#undef KM_BINARY_OPERATOR_COMMUTATIVE
//#undef KM_BINARY_OPERATOR_NON_COMMUTATIVE

template <class T, class U > class Multipliable2 { public: friend T operator *( T lhs, const U& rhs ) { return lhs *= rhs; } friend T operator *( const U& lhs, T rhs ) { return rhs *= lhs; } }; template <class T > class Multipliable { public: friend T operator *( T lhs, const T& rhs ) { return lhs *= rhs; } };
template <class T, class U > class Addable2 { public: friend T operator +( T lhs, const U& rhs ) { return lhs += rhs; } friend T operator +( const U& lhs, T rhs ) { return rhs += lhs; } }; template <class T > class Addable { public: friend T operator +( T lhs, const T& rhs ) { return lhs += rhs; } };
template <class T, class U > class Subtractable2 { public: friend T operator -( T lhs, const U& rhs ) { return lhs -= rhs; } }; template <class T, class U > class SubtractableLeft { public: friend T operator -( const U& lhs, const T& rhs ) { return T( lhs ) -= rhs; } }; template <class T > class Subtractable { public: friend T operator -( T lhs, const T& rhs ) { return lhs -= rhs; } };
template <class T, class U > class Dividable2 { public: friend T operator /( T lhs, const U& rhs ) { return lhs /= rhs; } }; template <class T, class U > class DividableLeft { public: friend T operator /( const U& lhs, const T& rhs ) { return T( lhs ) /= rhs; } }; template <class T > class Dividable { public: friend T operator /( T lhs, const T& rhs ) { return lhs /= rhs; } };
template <class T, class U > class Modable2 { public: friend T operator %( T lhs, const U& rhs ) { return lhs %= rhs; } }; template <class T, class U > class ModableLeft { public: friend T operator %( const U& lhs, const T& rhs ) { return T( lhs ) %= rhs; } }; template <class T > class Modable { public: friend T operator %( T lhs, const T& rhs ) { return lhs %= rhs; } };
template <class T, class U > class Xorable2 { public: friend T operator ^( T lhs, const U& rhs ) { return lhs ^= rhs; } friend T operator ^( const U& lhs, T rhs ) { return rhs ^= lhs; } }; template <class T > class Xorable { public: friend T operator ^( T lhs, const T& rhs ) { return lhs ^= rhs; } };
template <class T, class U > class Andable2 { public: friend T operator &( T lhs, const U& rhs ) { return lhs &= rhs; } friend T operator &( const U& lhs, T rhs ) { return rhs &= lhs; } }; template <class T > class Andable { public: friend T operator &( T lhs, const T& rhs ) { return lhs &= rhs; } };
template <class T, class U > class Orable2 { public: friend T operator |( T lhs, const U& rhs ) { return lhs |= rhs; } friend T operator |( const U& lhs, T rhs ) { return rhs |= lhs; } }; template <class T > class Orable { public: friend T operator |( T lhs, const T& rhs ) { return lhs |= rhs; } };



template <class T, class U >
class Comparable2
{
public:
	friend bool operator<(const T& x, const U& y)  { return x.Compare(y) < 0; }
	friend bool operator>(const T& x, const U& y)  { return x.Compare(y) > 0; }
	friend bool operator<=(const T& x, const U& y) { return x.Compare(y) <= 0; }
	friend bool operator>=(const T& x, const U& y) { return x.Compare(y) >= 0; }
	friend bool operator==(const T& x, const U& y) { return (x.Compare(y) == 0); }
	friend bool operator!=(const T& x, const U& y) { return (x.Compare(y) != 0); }
	
	friend bool operator<(const U& x, const T& y)  { return y.Compare(x) > 0; }
	friend bool operator>(const U& x, const T& y)  { return y.Compare(x) < 0; }
	friend bool operator<=(const U& x, const T& y) { return y.Compare(x) >= 0; }
	friend bool operator>=(const U& x, const T& y) { return y.Compare(x) <= 0; }
	friend bool operator==(const U& x, const T& y) { return (y.Compare(x) == 0); }
	friend bool operator!=(const U& x, const T& y) { return (y.Compare(x) != 0); }
};


template <class T >
class Comparable
{
public:
	friend bool operator<(const T& x, const T& y)  { return x.Compare(y) < 0; }
	friend bool operator>(const T& x, const T& y)  { return x.Compare(y) > 0; }
	friend bool operator<=(const T& x, const T& y) { return !(x.Compare(y) > 0); }
	friend bool operator>=(const T& x, const T& y) { return !(x.Compare(y) < 0); }
	friend bool operator==(const T& x, const T& y) { return (x.Compare(y) == 0); }
	friend bool operator!=(const T& x, const T& y) { return (x.Compare(y) != 0); }
};


template <class T, class U >
class LessThanComparable2
{
public:
  friend bool operator<=(const T& x, const U& y) { return !(x > y); }
  friend bool operator>=(const T& x, const U& y) { return !(x < y); }
  friend bool operator>(const U& x, const T& y)  { return y < x; }
  friend bool operator<(const U& x, const T& y)  { return y > x; }
  friend bool operator<=(const U& x, const T& y) { return !(y < x); }
  friend bool operator>=(const U& x, const T& y) { return !(y > x); }
};

template <class T >
class LessThanComparable
{
public:
  friend bool operator>(const T& x, const T& y)  { return y < x; }
  friend bool operator<=(const T& x, const T& y) { return !(y < x); }
  friend bool operator>=(const T& x, const T& y) { return !(x < y); }
};

template <class T, class U >
class Equalable2
{
public:
  friend bool operator==(const U& y, const T& x) { return x == y; }
  friend bool operator!=(const U& y, const T& x) { return !(x == y); }
  friend bool operator!=(const T& y, const U& x) { return !(y == x); }
};

template <class T >
class Equalable
{
public:
  friend bool operator!=(const T& x, const T& y) { return !(x == y); }
};
	
}}

#endif

