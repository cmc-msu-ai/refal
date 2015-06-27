#pragma once

#include <Refal2.h>

namespace Refal2 {

template<class T>
class CSetBuilder {
public:
	typedef std::set<T> CSet;

	CSetBuilder() { Reset(); }

	void Reset();
	// return true if includes all elements without contained set
	bool Export( CFastSet<T>& exportTo );

	void Include( const T& element );
	void Include( const CSet& elements );
	void IncludeAll();
	void IncludeAllExcept( const CSet& elements );

	void Exclude( const T& element );
	void Exclude( const CSet& elements );
	void ExcludeAll();
	void ExcludeAllExcept( const CSet& elements );
	
private:
	CSetBuilder( const CSetBuilder& );
	CSetBuilder& operator=( const CSetBuilder& );

	CSet yes;
	CSet no;
	CSet thru;

	enum TStatus {
		S_Thru = 0,
		S_No = -1,
		S_Yes = 1
	};
	TStatus fullSetStatus;
};

template<class T>
void CSetBuilder<T>::Reset()
{
	fullSetStatus = S_Thru;
	yes.clear();
	no.clear();
	thru.clear();
}

template<class T>
bool CSetBuilder<T>::Export( CFastSet<T>& exportTo )
{
	assert( fullSetStatus != S_Thru && thru.empty() );
	if( fullSetStatus == S_Yes ) {
		exportTo = no;
		return true;
	} else {
		exportTo = yes;
		return false;
	}
}

template<class T>
void CSetBuilder<T>::Include( const CSet& elements )
{
	/*
	IF full_set_status == S_thru THEN
		YES = YES + (elements \ NO);
	ELSE IF full_set_status == S_yes THEN
		THRU = THRU \ elements
	ELSE IF full_set_status == S_no THEN
		YES = YES + (elements * THRU)
		THRU = THRU \ elements
	*/
	if( fullSetStatus == S_Thru ) {
		std::set_difference( elements.begin(), elements.end(),
			no.begin(), no.end(), std::inserter( yes, yes.end() ) );
	} else {
		if( fullSetStatus == S_No ) {
			std::set_intersection( elements.begin(), elements.end(),
				thru.begin(), thru.end(), std::inserter( yes, yes.end() ) );
		}
		CSet tmp;
		std::set_difference( thru.begin(), thru.end(),
			elements.begin(), elements.end(), std::inserter( tmp, tmp.end() ) );
		thru = tmp;
	}
}

template<class T>
void CSetBuilder<T>::Include( const T& element )
{
	CSet tmp;
	tmp.insert( element );
	Include( tmp );
}

template<class T>
void CSetBuilder<T>::IncludeAll()
{
	IncludeAllExcept( CSet() );
}

template<class T>
void CSetBuilder<T>::IncludeAllExcept( const CSet& elements )
{
	/*
	IF full_set_status == S_thru THEN
		full_set_status = S_yes
		THRU = elements
	ELSE IF full_set_status == S_yes THEN
		THRU = THRU * elements
	ELSE IF full_set_status == S_no THEN
		YES = YES + (THRU \ elements)
		THRU = THRU * elements
	*/
	if( fullSetStatus == S_Thru ) {
		fullSetStatus = S_Yes;
		thru = elements;
	} else {
		if( fullSetStatus == S_No ) {
			std::set_difference( thru.begin(), thru.end(), elements.begin(),
				elements.end(), std::inserter( yes, yes.end() ) );
		}
		CSet tmp;
		std::set_intersection( thru.begin(), thru.end(),
			elements.begin(), elements.end(), std::inserter( tmp, tmp.end() ) );
		thru = tmp;
	}
}

template<class T>
void CSetBuilder<T>::Exclude( const CSet& elements )
{
	/*
	IF full_set_status == S_thru THEN
		NO = NO + (elements \ YES)
	ELSE IF full_set_status == S_yes THEN
		NO = NO + (THRU * elements)
		THRU = THRU \ elements
	ELSE IF full_set_status == S_no THEN
		THRU = THRU \ elements
	*/
	if( fullSetStatus == S_Thru ) {
		std::set_difference( elements.begin(), elements.end(),
			yes.begin(), yes.end(), std::inserter( no, no.end() ) );
	} else {
		if( fullSetStatus == S_Yes ) {
			std::set_intersection( elements.begin(), elements.end(),
				thru.begin(), thru.end(), std::inserter( no, no.end() ) );
		}
		CSet tmp;
		std::set_difference( thru.begin(), thru.end(),
			elements.begin(), elements.end(), std::inserter( tmp, tmp.end() ) );
		thru = tmp;
	}
}

template<class T>
void CSetBuilder<T>::Exclude( const T& element )
{
	CSet tmp;
	tmp.insert( element );
	Exclude( tmp );
}

template<class T>
void CSetBuilder<T>::ExcludeAll()
{
	ExcludeAllExcept( CSet() );
}

template<class T>
void CSetBuilder<T>::ExcludeAllExcept(const CSet& elements)
{
	/*
	IF full_set_status == S_thru THEN
		full_set_status = S_no
		THRU = elements
	ELSE IF full_set_status == S_yes THEN
		NO = NO + (THRU - elements)
		THRU = THRU * elements
	ELSE IF full_set_status == S_no THEN
		THRU = THRU * elements
	*/
	if( fullSetStatus == S_Thru ) {
		fullSetStatus = S_No;
		thru = elements;
	} else {
		if( fullSetStatus == S_Yes ) {
			std::set_difference( thru.begin(), thru.end(), elements.begin(),
				elements.end(), std::inserter( no, no.end() ) );
		}
		CSet tmp;
		std::set_intersection( thru.begin(), thru.end(), elements.begin(),
			elements.end(), std::inserter( tmp, tmp.end() ) );
		thru = tmp;
	}
}

} // end of namespace refal2

