#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

#if 0
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

//-----------------------------------------------------------------------------

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

typedef int TModuleId;

class CFunction {
};

class CModule {
private:
	TModuleId moduleId;
	CDictionary<CFunction> functions;
};

class CProgram {
public:
private:
	CDictionary<CModule> modules;
};
#endif

//-----------------------------------------------------------------------------

struct CModuleInfo {
	CToken NameToken;
	CToken StartToken;
	CToken EndToken;
	CPreparatoryFunctions Functions;
};

typedef std::unique_ptr<CModuleInfo> CModuleInfoPtr;

//-----------------------------------------------------------------------------

typedef void (*TEmbeddedFunction)();

class CEntryFunctionInfo {
public:
	CEntryFunctionInfo():
		preparatoryFunction( 0 ),
		embeddedFunction( 0 )
	{
	}

	bool IsEmbeddedFunction() const
	{
		return ( embeddedFunction != 0 );
	}

	bool IsPreparatoryFunction() const
	{
		return ( preparatoryFunction != 0 );
	}

	bool IsDefined() const
	{
		return ( IsEmbeddedFunction() || IsPreparatoryFunction() ); 
	}

	void SetPreparatoryFunction(
		const CPreparatoryFunction* _preparatoryFunction )
	{
		assert( !IsDefined() );
		assert( _preparatoryFunction != 0 );
		preparatoryFunction = _preparatoryFunction;
	}

	const CPreparatoryFunction* PreparatoryFunction() const
	{
		assert( IsPreparatoryFunction() );
		return preparatoryFunction;
	}

	void SetEmbeddedFunction( TEmbeddedFunction _embeddedFunction )
	{
		assert( !IsDefined() );
		assert( _embeddedFunction != 0 );
		embeddedFunction = _embeddedFunction;
	}

	TEmbeddedFunction EmbeddedFunction() const
	{
		assert( IsEmbeddedFunction() );
		return embeddedFunction;
	}

private:
	const CPreparatoryFunction* preparatoryFunction;
	TEmbeddedFunction embeddedFunction;
};

//-----------------------------------------------------------------------------

class CExternalFunctionInfo {
public:
	CExternalFunctionInfo( int _externalFunctionIndex,
			const CPreparatoryFunction* _preparatoryFunction ):
		externalFunctionIndex( _externalFunctionIndex ),
		preparatoryFunction( _preparatoryFunction )
	{
		assert( externalFunctionIndex >= 0 );
		assert( preparatoryFunction != 0 );
	}

	int ExternalFunctionIndex() const { return externalFunctionIndex; }
	const CPreparatoryFunction* PreparatoryFunction() const
		{ return preparatoryFunction; }

private:
	int externalFunctionIndex;
	const CPreparatoryFunction* preparatoryFunction;
};

//-----------------------------------------------------------------------------

class CProgramBuilder : public CFunctionBuilder {
public:
	void BuildProgram();

protected:
	CProgramBuilder( IErrorHandler* errorHandler = 0 );

	void Reset();
	void AddModule( CModuleInfoPtr& module );

private:
	std::vector<CModuleInfoPtr> modules;
	std::vector<CExternalFunctionInfo> externalFunctions;
	CDictionary<CEntryFunctionInfo, std::string> entryFunctions;

	void addFunctions( const CPreparatoryFunctions& functions );
	void collect();
	void check();
	void compile();
	void link();
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
