#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

enum TParsingElementState {
	PES_NotFinished,
	PES_Correct,
	PES_Wrong
};

class CParsingElementState {
public:
	CParsingElementState() { Reset(); }

	void Reset() { state = PES_NotFinished; }
	TParsingElementState State() const { return state; }
	bool IsFinished() const { return ( state != PES_NotFinished ); }
	void SetFinished( bool correct = false );
	bool IsCorrect() const { return ( state == PES_Correct ); }
	void SetCorrect() { SetFinished( true ); }
	bool IsWrong() const { return ( state == PES_Wrong ); }
	void SetWrong() { SetFinished(); }

private:
	TParsingElementState state;
};

inline void CParsingElementState::SetFinished( bool correct )
{
	assert( !IsFinished() );
	state = correct ? PES_Correct : PES_Wrong;
}

//-----------------------------------------------------------------------------

class CQualifierParser : public CModuleBuilder, public CParsingElementState {
protected:
	CToken token;

	CQualifierParser( IErrorHandler* errorHandler = nullptr );

	void Reset();
	void StartQualifer();
	void StartNamedQualifier();
	void AddToken();
	void GetQualifier( CQualifier& qualifier );

private:
	bool afterRightParen;
	CQualifierBuilder builder;
	CToken namedQualifier;
	// auxiliary functions
	void resetParser();
	void error( const std::string& message );
	void addWord();
	void addLabel();
	void addNumber();
	void addString();
	void addQualifier();
	void addLineFeed();
	void addLeftParen();
	void addRightParen();

	CQualifierParser( const CQualifierParser& );
	CQualifierParser& operator=( const CQualifierParser& );
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
