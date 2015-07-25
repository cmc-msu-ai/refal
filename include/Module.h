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

#endif

//-----------------------------------------------------------------------------

} // end of namespace Refal2
