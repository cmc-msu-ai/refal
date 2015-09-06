#include <Refal2.h>

using namespace Refal2;

class CErrorHandler : public IErrorHandler {
public:
	virtual void Error( const std::string& errorText );
	virtual void Warning( const std::string& warningText );
};

void CErrorHandler::Error( const std::string& errorText )
{
	std::cerr << errorText << std::endl;
}
void CErrorHandler::Warning( const std::string& warningText )
{
	std::cerr << warningText << std::endl;
}

const std::string SeparatorLine( 79, '-' );

bool ParseFile( CScanner& scanner, const std::string& fileName )
{
	std::ifstream fileStream( fileName );
	if( !fileStream.good() ) {
		std::cerr << "Cannot open file: `" << fileName << "`." << std::endl;
		std::cerr << SeparatorLine << std::endl << std::endl;
		return false;
	}
	std::cout << "Source file: `" << fileName << "`." << std::endl;
	std::cerr << "Source file: `" << fileName << "`." << std::endl;

	for( char c; fileStream.get( c ); ) {
		scanner.AddChar( c );
	}
	scanner.AddEndOfFile();

	std::cout << SeparatorLine << std::endl << std::endl;
	std::cerr << SeparatorLine << std::endl << std::endl;

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

	CUnitList fieldOfView;
	CUnitNode* errorNode;
	TExecutionResult result =
		COperationsExecuter::Run( program, fieldOfView, errorNode );

	std::cout << SeparatorLine << std::endl;
	std::cout << "Execution result: "
		<< ExecutionResultStrings[result] << "." << std::endl;
	std::cout << "Field of view: " << std::endl;
	CProgramPrintHelper programPrintHelper( program );
	//programPrintHelper.SetPrintLabelWithModule();
	fieldOfView.HandyPrint( std::cout, programPrintHelper );
	std::cout << std::endl << SeparatorLine << std::endl;
	std::cout << "Receptacle: " << std::endl;
	CUnitList receptacle;
	program->Receptacle().DigOutAll( receptacle );
	receptacle.HandyPrint( std::cout, programPrintHelper );
	std::cout << std::endl;
	return 0;
}
