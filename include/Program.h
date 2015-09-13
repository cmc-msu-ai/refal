#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------
// CProgram

struct CRuntimeModule {
	CRuntimeFunctions Functions;
};

const TRuntimeModuleId InvalidRuntimeModuleId = -1;

class CProgram : public CCompilationContext {
public:
	CProgram( int numberOfModules );
	~CProgram();

	int NumberOfModules() const { return modulesSize; }
	CRuntimeModule& Module( TRuntimeModuleId moduleId );
	const CRuntimeModule& Module( const TRuntimeModuleId moduleId ) const
	{
		return const_cast<CProgram*>( this )->Module( moduleId );
	}

	void SetProgramStartFunction( const TLabel _programStartFunction )
	{
		assert( _programStartFunction != InvalidDictionaryIndex );
		programStartFunction = _programStartFunction;
	}

	TLabel GetProgramStartFunction() const { return programStartFunction; }

	CRuntimeFunctionPtr EmptyFunction() const { return emptyFunction; }

private:
	const int modulesSize;
	CRuntimeModule* const modules;
	TLabel programStartFunction;
	const CRuntimeFunctionPtr emptyFunction;

	CProgram( const CProgram& );
	CProgram& operator=( const CProgram& );
};

typedef std::shared_ptr<CProgram> CProgramPtr;

//-----------------------------------------------------------------------------
// CProgramPrintHelper

class CProgramPrintHelper : public CPrintHelper {
public:
	CProgramPrintHelper( const CProgramPtr _program ) :
		program( _program )
	{
		assert( static_cast<bool>( program ) );
	}

	virtual void Label( std::ostream& outputStream,
		const TLabel label ) const;

private:
	CProgramPtr program;
};

//-----------------------------------------------------------------------------
// CModuleData

struct CModuleData {
	CToken NameToken;
	CToken StartToken;
	CToken EndToken;
	CPreparatoryFunctions Functions;
};

typedef std::unique_ptr<CModuleData> CModuleDataPtr;
typedef std::vector<CModuleDataPtr> CModuleDataVector;

//-----------------------------------------------------------------------------
// CProgramBuilder

class CProgramBuilder : public CFunctionBuilder {
public:
	CProgramPtr BuildProgram();
	bool CheckProgram();

protected:
	CProgramBuilder( IErrorHandler* errorHandler = nullptr );

	void Reset();
	void AddModule( CModuleDataPtr& module );

private:
	CModuleDataVector modules;
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
