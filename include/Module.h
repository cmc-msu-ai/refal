#pragma once

#include <Refal2.h>

namespace Refal2 {

//-----------------------------------------------------------------------------

struct CRuntimeModule {
	CRuntimeFunctions Functions;
};

typedef int TRuntimeModuleId;

const TRuntimeModuleId InvalidRuntimeModuleId = -1;

class CProgram {
public:
	CProgram( int numberOfModules );
	~CProgram();

	int NumberOfModules() const { return modulesSize; }
	CRuntimeModule& Module( TRuntimeModuleId moduleId );
	const CRuntimeModule& Module( const TRuntimeModuleId moduleId ) const
		{ return const_cast<CProgram*>( this )->Module( moduleId ); }

private:
	const int modulesSize;
	CRuntimeModule* const modules;

	CProgram( const CProgram& );
	CProgram& operator=( const CProgram& );
};

typedef std::unique_ptr<CProgram> CProgramPtr;

//-----------------------------------------------------------------------------

struct CModuleData {
	CToken NameToken;
	CToken StartToken;
	CToken EndToken;
	CPreparatoryFunctions Functions;
};

typedef std::unique_ptr<CModuleData> CModuleDataPtr;
typedef std::vector<CModuleDataPtr> CModuleDataVector;

//-----------------------------------------------------------------------------

class CProgramBuilder : public CFunctionBuilder {
public:
	void BuildProgram();

protected:
	CProgramBuilder( IErrorHandler* errorHandler = 0 );

	void Reset();
	void AddModule( CModuleDataPtr& module );

private:
	CModuleDataVector modules;
};

//-----------------------------------------------------------------------------

} // end of namespace Refal2
