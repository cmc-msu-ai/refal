#include <Refal2.h>

using namespace Refal2;

typedef std::vector<std::string> CFileNames;

class CSourceFilesProcessor : private IErrorHandler {
public:
	static CProgramPtr Process( const CFileNames& fileNames );

private:
	CScanner scanner;
	std::ifstream file;

	CSourceFilesProcessor( const CFileNames& fileNames );
	CSourceFilesProcessor( const CSourceFilesProcessor& );
	CSourceFilesProcessor& operator=( const CSourceFilesProcessor& );

	void processFile( const std::string& fileName );

	virtual void Error( const CError& error );
};

CProgramPtr CSourceFilesProcessor::Process( const CFileNames& fileNames )
{
	CSourceFilesProcessor sourceFilesProcessor( fileNames );
	return sourceFilesProcessor.scanner.BuildProgram();
}

CSourceFilesProcessor::CSourceFilesProcessor( const CFileNames& fileNames )
{
	scanner.SetErrorHandler( this );
	bool success = true;
	for( CFileNames::const_iterator fileName = fileNames.begin();
		fileName != fileNames.end(); ++fileName )
	{
		processFile( *fileName );
		if( scanner.ErrorSeverity() == ES_FatalError ) {
			break;
		}
	}
}

void CSourceFilesProcessor::processFile( const std::string& fileName )
{
	file.open( fileName );
	if( file.good() ) {
		scanner.SetFileName( fileName );
		for( char c; file.get( c ); ) {
			scanner.AddChar( c );
		}
		if( scanner.ErrorSeverity() != ES_FatalError ) {
			scanner.AddEndOfFile();
		}
	} else {
		std::cerr << "Cannot open file: `" << fileName << "`." << std::endl;
	}
}

void CSourceFilesProcessor::Error( const CError& error )
{
	std::cerr << error.UserMessage() << std::endl;
	if( error.Severity() == ES_FatalError ) {
		file.clear( std::ios_base::eofbit );
	}
}

const char* const ExecutionResultStrings[] = {
	"ok",
	"recognition impossible",
	"call empty function",
	"lost function label",
	"wrong argument of embedded function"
};

const std::string SeparatorLine( 80, '-' );

int main( int argc, const char* argv[] )
{
	std::ios::sync_with_stdio( false );
	if( argc < 2 ) {
		std::cerr << "There are no source files." << std::endl;
		return 1;
	}

	CFileNames fileNames;
	for( int i = 1; i < argc; i++ ) {
		fileNames.push_back( argv[i] );
	}

	CProgramPtr program = CSourceFilesProcessor::Process( fileNames );
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
