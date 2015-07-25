#pragma once

#include <Refal2.h>
#include <Dictionary.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

class CWord {
public:
	CWord( const char* text = 0);
	CWord( const CWord& word ) { SetWord( word.GetWord() ); }
	CWord& operator=( const CWord& newWord ) { SetWord( newWord.GetWord() ); }
	~CWord() { Empty(); }

	void Empty();
	int Length() const { return length( GetWord() ); }
	void SetWord( const char* newWord );
	const char* GetWord() const { return word; }

private:
	char* word;
	// auxiliary functions
	static int length( const char* text ) { return ::strlen( text ); }
};

inline CWord::CWord( const char* text ):
	word( 0 )
{
	SetWord( text );
}

inline void CWord::Empty()
{
	if( word != 0 ) {
		delete[] word;
		word = 0;
	}
}

inline void CWord::SetWord( const char* newWord )
{
	Empty();
	if( newWord != 0 ) {
		int size = length( newWord ) + 1;
		word = new char[size];
		for( int i = 0; i < size; i++ ) {
			word[i] = ::tolower( newWord[i] );
		}
		assert( word[size - 1] == '\0' );
	}
}

inline bool operator<( const CWord& word1, const CWord& word2 )
{
	const char* wordText1 = word1.GetWord();
	const char* wordText2 = word2.GetWord();
	if( wordText1 == wordText2 || ( wordText1 != 0 && wordText2 == 0 ) ) {
		return false;
	} else if( wordText1 == 0 ) {
		return true;
	} else {
		return ( ::strcmp( wordText1, wordText2 ) < 0 );
	}
}

inline bool operator==( const CWord& word1, const CWord& word2 )
{
	const char* wordText1 = word1.GetWord();
	const char* wordText2 = word2.GetWord();
	if( wordText1 == wordText2 || ( wordText1 != 0 && wordText2 == 0 ) ) {
		return false;
	} else if( wordText1 == 0 ) {
		return false;
	} else {
		return ( ::strcmp( wordText1, wordText2 ) == 0 );
	} 
}

//-----------------------------------------------------------------------------

typedef CDictionary<CScopedPtr<CModule>, std::string> CModules;

class CRefalFile {
public:
	const std::string& GetName() const { return name; }
	

private:
	std::string name;
};

class CRefalSourceFile : public CRefalFile {
};

class CRefalModuleFile : public CRefalFile {
};

class CProgram {
public:

private:
	CModules modules;
};

enum TProgramBuilderState {
	PBS_AddingModules,
	PBS_CheckedLinks,
	PBS_Compiled
};

class CProgramBuilder {
public:
	CProgramBuilder( IErrorHandler* errorHandler );

	void Reset();
	TProgramBuilderState GetState() const { return state; }
	bool AddModule( CScopedPtr<CModule> module );
	bool CheckExternalLinks();
	bool Compile();
	CScopedPtr<CProgram> Link();

private:
	TProgramBuilderState state;
	CScopedPtr<CProgram> program;
};

//----------------------------------------------------------------------------

class CModule {
public:
	virtual ~CModule() = 0 {};

	TLabel FindFunction( const std::string& name ) const;
	CFunction* GetFunction( TLabel function ) const;

private:
	CDictionary<CScopedPtr<CFunction>, std::string> functions;
};

CDictionary<CScopedPtr<CModule>, std::string> CModuleDictionary;

class CContext {
};

class CFunction {
public:
	virtual ~CFunction() = 0 {}
	virtual bool IsExternal() const = 0;
	virtual bool Run() = 0;
};

enum TModuleFunctionType {
	MFT_Empty,
	MFT_Ordinal,
	MFT_External
};

class CModuleFunction : public CFunction {
public:

private:
	TModuleFunctionType type;
	std::string& extrnalName;
	COperation* operation;
};

class CEmptyFunction : public CFunction {
public:
	virtual void Run() { /* TODO: error */ }
};

class COrdinaryFunction : public CFunction {
public:
	virtual void Run() { /* TODO: operation */ }

private:
	COperation* operation;
};

class CEntryFunction : public COrdinaryFunction {
};

class CExternalFunction : public COrdinaryFunction {
public:
	virtual void Run() { /* TODO: set module */ COrdinaryFunction::Run(); }

private:
	int module;
};

typedef bool (*TEmbeddedFunction)( CUnitList& argument,
	IErrorHandler* errorHandler );

class CEmbeddedFunction : public CFunction {
public:
	virtual void Run() { /* TODO: call embedded function */ }

private:
	TEmbeddedFunction embeddedFunction;
};

class CRule {
private:
	CUnitList leftPart;
	CUnitList rightPart;
	CVariables variables;
	bool isRightDirection;
	CScopedPtr<CRule> nextRule;
};

enum TPreparatoryFunctionType {
	PFT_Declared,
	PFT_Defined,
	PFT_Ordinary,
	PFT_Empty,
	PFT_External
};

bool AreTokenWordsEqual( const CToken& word1, const CToken& word2 )
{
	std::string text1 = word1.word;
	std::string text2 = word2.word;
	MakeLower( text1 );
	MakeLower( text2 );
	return ( text1 == text2 );
}

class CPreparatoryFunction {
public:
	CPreparatoryFunction( const CToken& name );

	TPreparatoryFunctionType GetType() const { return type; }
	const CToken& GetName() const { return name; }
	const CToken& GetExternalName() const { return externalName; }

	bool IsEntry() const { return entry; }
	bool IsDeclared() const { return ( GetType() == PFT_Declared ); }
	bool IsDefined() const { return ( GetType() == PFT_Defined ); }
	bool IsOrdinary() const { return ( GetType() == PFT_Ordinary ); }
	bool IsEmpty() const { return ( GetType() == PFT_Empty ); }
	bool IsExternal() const { return ( GetType() == PFT_External ); }

	void SetDefined( const CToken& name );
	void SetOrdinary( CScopedPtr<CRule> firstRule );
	void SetEmpty();
	void SetEntry( const CToken& externalName );
	void SetExternal( const CToken& externalName );

private:
	TPreparatoryFunctionType type;
	bool entry;
	CToken name;
	CToken externalName;
	CScopedPtr<CRule> firstRule;
};

CPreparatoryFunction::CPreparatoryFunction( const CToken& _name ):
	type( PFT_Declared ),
	entry( false ),
	name( _name )
{
}

void CPreparatoryFunction::SetDefined( const CToken& _name )
{
	assert( AreTokenWordsEqual( name, _name ) );
	assert( type == PFT_Declared );
	type = PFT_Defined;
	name = _name;
}

void CPreparatoryFunction::SetOrdinary( CScopedPtr<CRule> _firstRule )
{
	assert( type == PFT_Defined );
	assert( !_firstRule.IsNull() );
	type = PFT_Ordinary;
	firstRule = _firstRule;
}

void CPreparatoryFunction::SetEmpty()
{
	assert( type == PFT_Defined );
	type = PFT_Empty;
}

void CPreparatoryFunction::SetEntry( const CToken& _externalName )
{
	assert( type != PFT_External );
	entry = true;
	externalName = _externalName;
}

void CPreparatoryFunction::SetExternal( const CToken& _externalName )
{
	assert( !entry );
	assert( type == PFT_Defined );
	type = PFT_External;
	externalName = _externalName;
}

struct CModuleInfo {
	CToken NameToken;
	CToken StartToken;
	CToken EndToken;
	CDictionary<CScopedPtr<CPreparatoryFunction>, std::string> Functions;
};

class CModuleBuilder {
protected:
	CModuleBuilder() { Reset(); }

	virtual void OnModuleReady( CScopedPtr<CModuleInfo> module ) = 0;

	void Reset();
	bool IsStarted() const { return started; }
	void StartModule();
	void StartModule( const CToken& startToken );
	void StartModule( const CToken& startToken, const CToken& nameToken );
	void EndModule();
	void EndModule( const CToken& endToken );
	TLabel Declare( const CToken& nameToken );
	void SetOrdinary( const CToken& nameToken );
	void SetOrdinary( const CToken& nameToken, CScopedPtr<CRule> firstRule );
	void SetEmpty( const CToken& nameToken );
	void SetEntry( const CToken& nameToken );
	void SetEntry( const CToken& nameToken, const CToken& externalNameToken );
	void SetExternal( const CToken& nameToken );
	void SetExternal( const CToken& nameToken,
		const CToken& externalNameToken );

private:
	bool started;
	CScopedPtr<CModuleInfo> module;
	// auxiliary functions
	TLabel declare( const CToken& nameToken );
	CPreparatoryFunction& addFunction( const CToken& nameToken );
	bool checkOnlyDeclared( CPreparatoryFunction& function,
		const CToken& nameToken );
	bool checkModule();
	void endModule();

	CModuleBuilder( const CModuleBuilder& );
	CModuleBuilder& operator=( const CModuleBuilder& );
};

void CModuleBuilder::Reset()
{
	started = false;
	module = new CModuleInfo;
}

void CModuleBuilder::StartModule()
{
}

void CModuleBuilder::StartModule( const CToken& startToken )
{
	module->StartToken = startToken;
}

void CModuleBuilder::StartModule( const CToken& startToken,
	const CToken& nameToken )
{
	module->StartToken = startToken;
	module->NameToken = nameToken;
}

void CModuleBuilder::EndModule()
{
	endModule();
}

void CModuleBuilder::EndModule( const CToken& endToken )
{
	module->EndToken = endToken;
	endModule();
}

TLabel CModuleBuilder::Declare( const CToken& nameToken )
{
	return ( declare( nameToken ) );
}

void CModuleBuilder::SetOrdinary( const CToken& nameToken )
{
	CPreparatoryFunction& function = addFunction( nameToken );
	if( checkOnlyDeclared( function, nameToken ) ) {
		function.SetDefined( nameToken );
	}
}

void CModuleBuilder::SetOrdinary( const CToken& nameToken,
	CScopedPtr<CRule> firstRule )
{
	CPreparatoryFunction& function = addFunction( nameToken );
	if( checkOnlyDeclared( function, nameToken ) ) {
		function.SetDefined( nameToken );
		function.SetOrdinary( firstRule );
	}
}

void CModuleBuilder::SetEmpty( const CToken& nameToken )
{
	CPreparatoryFunction& function = addFunction( nameToken );
	if( checkOnlyDeclared( function, nameToken ) ) {
		function.SetDefined( nameToken );
		function.SetEmpty();
	}
}

void CModuleBuilder::SetEntry( const CToken& nameToken )
{
	SetEntry( nameToken, nameToken );
}

void CModuleBuilder::SetEntry( const CToken& nameToken,
	const CToken& externalNameToken )
{
	CPreparatoryFunction& function = addFunction( nameToken );
	if( function.IsExternal() ) {
		// TODO: error
		return;
	}
	function.SetEntry( externalNameToken );
}

void CModuleBuilder::SetExternal( const CToken& nameToken )
{
	SetExternal( nameToken, nameToken );
}

void CModuleBuilder::SetExternal( const CToken& nameToken,
	const CToken& externalNameToken )
{
	CPreparatoryFunction& function = addFunction( nameToken );
	if( !checkOnlyDeclared( function, nameToken ) ) {
		if( function.IsEntry() ) {
			function.SetDefined( nameToken );
			function.SetExternal( externalNameToken );
		} else {
			// error
		}
	}
}

TLabel CModuleBuilder::declare( const CToken& nameToken )
{
	std::string name = nameToken.word;
	MakeLower( name );
	return ( module->Functions.AddKey( name ) );
}

CPreparatoryFunction& CModuleBuilder::addFunction( const CToken& nameToken )
{
	return *module->Functions.GetData( declare( nameToken ) );
}

bool CModuleBuilder::checkOnlyDeclared( CPreparatoryFunction& function,
	const CToken& nameToken )
{
	if( function.IsDeclared() ) {
		return true;
	}
	// TODO: error
	return false;
}

bool CModuleBuilder::checkModule()
{
	// TODO: check
	return true;
}

void CModuleBuilder::endModule()
{
	CScopedPtr<CModuleInfo> savedModule = module;
	Reset();
	if( checkModule() ) {
		OnModuleReady( savedModule );
	}
}

//-----------------------------------------------------------------------------

} // end of namespace Refal2
