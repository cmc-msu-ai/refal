#pragma once

#include <set>
#include <iterator>
#include <algorithm>
#include <Refal2.h>

namespace Refal2 {

template<class T>
class CSetBuilder {
public:
	typedef std::set<T> CSet;

	CSetBuilder()
	{
		Reset();
	}

	void Reset();
	void Export(CFastSet<T>*, bool* include_all);

	void Include(const T&);
	void Include(const CSet&);
	void IncludeAll();
	void IncludeAllExcept(const CSet&);

	void Exclude(const T&);
	void Exclude(const CSet&);
	void ExcludeAll();
	void ExcludeAllExcept(const CSet&);
	
private:
	CSetBuilder(const CSetBuilder&);
	CSetBuilder& operator=(const CSetBuilder&);

	CSet yes, no, thru;
	enum TStatus {
		S_thru = 0,
		S_no = -1,
		S_yes = 1
	};
	TStatus full_set_status;
};

template<class T>
void CSetBuilder<T>::Reset()
{
	full_set_status = S_thru;
	yes.clear();
	no.clear();
	thru.clear();
}

template<class T>
void CSetBuilder<T>::Export(CFastSet<T>* set, bool* include_all)
{
	assert( full_set_status != S_thru && thru.empty() );
	
	if( full_set_status == S_yes ) {
		*include_all = true;
		*set = no;
	} else {
		*include_all = false;
		*set = yes;
	}
}

template<class T>
void CSetBuilder<T>::Include(const CSet& elements)
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
	if( full_set_status == S_thru ) {
		::std::set_difference(elements.begin(), elements.end(),
							  no.begin(), no.end(),
							  ::std::inserter(yes, yes.end()));
	} else {
		if( full_set_status == S_no ) {
			::std::set_intersection(elements.begin(), elements.end(),
									thru.begin(), thru.end(),
									::std::inserter(yes, yes.end()));
		}
		CSet tmp;
		::std::set_difference(thru.begin(), thru.end(),
							  elements.begin(), elements.end(),
							  ::std::inserter(tmp, tmp.end()));
		thru = tmp;
	}
}

template<class T>
void CSetBuilder<T>::Include(const T& element)
{
	CSet tmp;
	tmp.insert(element);
	Include(tmp);
}

template<class T>
void CSetBuilder<T>::IncludeAll()
{
	IncludeAllExcept(CSet());
}

template<class T>
void CSetBuilder<T>::IncludeAllExcept(const CSet& elements)
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
	if( full_set_status == S_thru ) {
		full_set_status = S_yes;
		thru = elements;
	} else {
		if( full_set_status == S_no ) {
			::std::set_difference(thru.begin(), thru.end(),
								  elements.begin(), elements.end(),
								  ::std::inserter(yes, yes.end()));
		}
		CSet tmp;
		::std::set_intersection(thru.begin(), thru.end(),
								elements.begin(), elements.end(),
								::std::inserter(tmp, tmp.end()));
		thru = tmp;
	}
}

template<class T>
void CSetBuilder<T>::Exclude(const CSet& elements)
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
	if( full_set_status == S_thru ) {
		::std::set_difference(elements.begin(), elements.end(),
							  yes.begin(), yes.end(),
							  ::std::inserter(no, no.end()));
	} else {
		if( full_set_status == S_yes ) {
			::std::set_intersection(elements.begin(), elements.end(),
									thru.begin(), thru.end(),
									::std::inserter(no, no.end()));
		}
		CSet tmp;
		::std::set_difference(thru.begin(), thru.end(),
							  elements.begin(), elements.end(),
							  ::std::inserter(tmp, tmp.end()));
		thru = tmp;
	}
}

template<class T>
void CSetBuilder<T>::Exclude(const T& element)
{
	CSet tmp;
	tmp.insert(element);
	Exclude(tmp);
}

template<class T>
void CSetBuilder<T>::ExcludeAll()
{
	ExcludeAllExcept(CSet());
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
	if( full_set_status == S_thru ) {
		full_set_status = S_no;
		thru = elements;
	} else {
		if( full_set_status == S_yes ) {
			::std::set_difference(thru.begin(), thru.end(),
								  elements.begin(), elements.end(),
								  ::std::inserter(no, no.end()));
		}
		CSet tmp;
		::std::set_intersection(thru.begin(), thru.end(),
								elements.begin(), elements.end(),
								::std::inserter(tmp, tmp.end()));
		thru = tmp;
	}
}

} // end of namespace refal2

