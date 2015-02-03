#include <map>
#include <string>
#include <Refal2.h>

namespace Refal2 {

CLabelTable::CLabelTable(int initialTableSize):
	tableSize(initialTableSize), tableFirstFree(0), table(0)
{
	alloc();
}

CLabelTable::~CLabelTable()
{
	for( int i = 0; i < tableFirstFree; i++ ) {
		table[i].~CLabelInfo();
	}

	::operator delete(table);
}

TLabel CLabelTable::AddLabel(const std::string& labelText)
{
	typedef std::pair<CLabelMap::iterator, bool> CPair;

	CPair pair = labelMap.insert( std::make_pair( labelText, tableFirstFree ) );

	if( pair.second ) {
		grow( pair.first );
	}

	return pair.first->second;
}

const std::string& CLabelTable::GetLabelText(TLabel label)
{
	assert( label >= 0 && label < tableFirstFree );

	return table[label].labelPtr->first;
}

CFunction* CLabelTable::GetLabelFunction(TLabel label)
{
	assert( label >= 0 && label < tableFirstFree );

	return &table[label].function;
}

TLabel CLabelTable::GetFirstLabel() const
{
	if( tableFirstFree > 0 ) {
		return 0;
	} else {
		return InvalidLabel;
	}
}

TLabel CLabelTable::GetNextLabel(TLabel afterLabel) const
{
	assert( afterLabel >= 0 );

	if( afterLabel < tableFirstFree - 1 ) {
		return ( afterLabel + 1 );
	} else {
		return InvalidLabel;
	}
}

void CLabelTable::alloc()
{
	table = static_cast<CLabelInfo*>(
		::operator new( tableSize * sizeof(CLabelInfo) ) );
}

void CLabelTable::grow(const CLabelMap::const_iterator& labelPtr)
{
	if( tableFirstFree == tableSize ) {
		CLabelInfo* tmp = table;
		tableSize *= 2;
		alloc();
		memcpy( table, tmp, tableFirstFree * sizeof(CLabelInfo) );
		delete tmp;
	}
	
	new(table + tableFirstFree)CLabelInfo(labelPtr);
	++tableFirstFree;
}

} // end of namespace Refal2
