//
//  special-conditions.hpp
//  BoE
//
//  Created by Celtic Minstrel on 2025-03-06.
//

#ifndef BOE_DATA_SPECIAL_COND_H
#define BOE_DATA_SPECIAL_COND_H

struct node_literal_t {
	using value_type = short;
	short operator()(const cSpecial&) const { return value; }
	node_literal_t(short v) : value(v) {}
private:
	short value;
};

struct node_value_t {
	using value_type = eSpecField;
	short operator()(const cSpecial& spec) const { return spec->*field; }
	node_value_t(eSpecField fld) : field(fld) {}
private:
	eSpecField field;
};

template<template<class> class P, typename A, typename B>
struct node_comparison_t {
	static const bool node_expr = true;
	bool operator()(const cSpecial& spec) const {
		short a = lhs(spec), b = rhs(spec);
		return pred(a, b);
	}
	node_comparison_t(A a, B b, P<short> p) : lhs(a), rhs(b), pred(p) {}
private:
	A lhs;
	B rhs;
	P<short> pred;
};

template<typename A, typename B>
struct node_logic_t {
	static const bool node_expr = true;
	bool operator()(const cSpecial& spec) const {
		return conjunct ? (a(spec) && b(spec)) : (a(spec) || b(spec));
	}
	node_logic_t(A a, B b, bool conj) : a(a), b(b), conjunct(conj) {}
private:
	A a;
	B b;
	bool conjunct;
};

template<typename T>
struct node_negate_t {
	static const bool node_expr = true;
	bool operator()(const cSpecial& spec) const {
		return !pred(spec);
	}
	node_negate_t(T p) : pred(p) {}
private:
	T pred;
};

#define DEFINE_CMP(LHS, RHS, OP, OP_NAME) \
	inline node_comparison_t<std::OP_NAME, LHS, RHS> operator OP(LHS::value_type a, RHS::value_type b) { \
		return node_comparison_t<std::OP_NAME, LHS, RHS>{ \
			LHS{a}, \
			RHS{b}, \
			std::OP_NAME<short>() \
		}; \
	}
#define DEFINE_CMPS(OP, OP_NAME) \
	DEFINE_CMP(node_value_t, node_literal_t, OP, OP_NAME); \
	DEFINE_CMP(node_literal_t, node_value_t, OP, OP_NAME);
DEFINE_CMPS(==, equal_to);
DEFINE_CMPS(<=, less_equal);
DEFINE_CMPS(>=, greater_equal);
DEFINE_CMPS(!=, not_equal_to);
DEFINE_CMPS(<, less);
DEFINE_CMPS(>, greater);
#undef DEFINE_CMP
#undef DEFINE_CMPS

template<typename A, typename B>
inline node_logic_t<A, B> operator&&(typename std::enable_if<A::node_expr, A>::type a, typename std::enable_if<B::node_expr, B>::type b) {
	return node_logic_t<A, B>{a, b, true};
}
template<typename A, typename B>
inline node_logic_t<A, B> operator||(typename std::enable_if<A::node_expr, A>::type a, typename std::enable_if<B::node_expr, B>::type b) {
	return node_logic_t<A, B>{a, b, false};
}
template<typename T>
inline node_negate_t<T> operator!(typename std::enable_if<T::node_expr, T>::type v) {
	return node_negate_t<T>{v};
};

#endif
