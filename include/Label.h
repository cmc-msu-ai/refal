#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

const int InvalidLabel = -1;

const int DefaultInitialLabelTableSize = 1024;

class CLabelTable {
public:
	CLabelTable( const int initialTableSize = DefaultInitialLabelTableSize );
	~CLabelTable();

	void Empty();

	TLabel AddLabel( const std::string& labelText );
	const std::string& GetLabelText( const TLabel label ) const;
	CFunction& GetLabelFunction( const TLabel label );
	const CFunction& GetLabelFunction( const TLabel label ) const;

	int GetNumberOfLabels() const { return tableFirstFree; }

	TLabel GetFirstLabel() const;
	TLabel GetNextLabel( const TLabel afterLabel ) const;

private:
	typedef std::map<std::string, TLabel> CLabelMap;
	struct CLabelInfo {
		CFunction function;
		CLabelMap::const_iterator labelPtr;

		CLabelInfo( const CLabelMap::const_iterator& _labelPtr ):
			labelPtr( _labelPtr ) {}
	};

	CLabelMap labelMap;
	int tableSize;
	int tableFirstFree;
	CLabelInfo* table;

	// auxiliary functions
	void alloc();
	void grow( const CLabelMap::const_iterator& labelPtr );

	CLabelTable( const CLabelTable& );
	CLabelTable& operator=( const CLabelTable& );
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
