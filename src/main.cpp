#include <Refal2.h>

using namespace Refal2;

class CErrorHandler : public IErrorHandler {
public:
	virtual void Error( const CError& error );
};

void CErrorHandler::Error( const CError& error )
{
	std::cerr << error.UserMessage() << std::endl;
}

bool ParseFile( CScanner& scanner, const std::string& fileName )
{
	std::ifstream fileStream( fileName );
	if( !fileStream.good() ) {
		std::cerr << "Cannot open file: `" << fileName << "`." << std::endl;
		return false;
	}

	scanner.SetFileName( fileName );
	for( char c; fileStream.get( c ); ) {
		scanner.AddChar( c );
	}
	scanner.AddEndOfFile();

	return ( !scanner.HasErrors() );
}

typedef std::vector<std::string> CFileNames;

CProgramPtr ParseFiles( const CFileNames& fileNames )
{
	CErrorHandler errorHandler;
	CScanner scanner( &errorHandler );

	bool success = true;
	for( CFileNames::const_iterator i = fileNames.begin();
		i != fileNames.end(); ++i )
	{
		success &= ParseFile( scanner, *i );
	}

	if( success ) {
		return scanner.BuildProgram();
	}
	return CProgramPtr();
}

const char* const ExecutionResultStrings[] = {
	"ok",
	"recognition impossible",
	"call empty function",
	"lost function label",
	"wrong argument of external function"
};

const std::string SeparatorLine( 80, '-' );

int main( int argc, const char* argv[] )
{
	std::ios::sync_with_stdio( false );
	if( argc < 2 ) {
		std::cerr << "Wrong arguments: source files were not set." << std::endl;
		return 1;
	}

	CFileNames fileNames;
	for( int i = 1; i < argc; i++ ) {
		fileNames.push_back( argv[i] );
	}

	CProgramPtr program = ParseFiles( fileNames );

	if( !static_cast<bool>( program ) ) {
		return 1;
	}

    CReceptaclePtr receptacle( new CReceptacle );
	CUnitList fieldOfView;
	CUnitNode* errorNode;
	TExecutionResult result =
		COperationsExecuter::Run( program, receptacle, fieldOfView, errorNode );

	std::cout << SeparatorLine << std::endl;
	std::cout << "Execution result: "
		<< ExecutionResultStrings[result] << "." << std::endl;
	if( !fieldOfView.IsEmpty() ) {
		std::cout << "Field of view: " << std::endl;
		CProgramPrintHelper programPrintHelper( program );
		//programPrintHelper.SetPrintLabelWithModule();
		fieldOfView.HandyPrint( std::cout, programPrintHelper );
	}
	if( !receptacle->IsEmpty() ) {
		if( !fieldOfView.IsEmpty() ) {
			std::cout << SeparatorLine << std::endl;
		}
		std::cout << "Receptacle: " << std::endl;
		CUnitList receptacleUnitList;
		receptacle->DigOutAll( receptacleUnitList );
		CProgramPrintHelper programPrintHelper( program );
		receptacleUnitList.HandyPrint( std::cout, programPrintHelper );
		std::cout << std::endl;
	}
	return 0;
}
