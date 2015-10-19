#include <Refal2.h>

using namespace Refal2;

//-----------------------------------------------------------------------------
// Constants

const char* const UtilityName = "refal2";

//-----------------------------------------------------------------------------
// CCommandLineOptions

typedef std::vector<std::string> CFileNames;

class CCommandLineOptions {
public:
	CCommandLineOptions() { ResetDefaults(); }

	void ResetDefaults();
	bool SetByCommandLineArguments( int numberOfArguments,
		const char* const arguments[] );

	bool IsCheckOnly() const { return isCheckOnly; }
	void SetCheckOnly( bool _isCheckOnly = true )
		{ isCheckOnly = _isCheckOnly; }

	bool IsInfoOnly() const { return isInfoOnly; }
	void SetInfoOnly( bool _isInfoOnly = true ) { isInfoOnly = _isInfoOnly; }

	void ResetSourceFiles() { sourceFiles.clear(); }
	bool HasSourceFiles() const { return !sourceFiles.empty(); }
	CFileNames SourceFiles() const { return sourceFiles; }
	void AddSourceFile( const std::string& fileName )
		{ sourceFiles.push_back( fileName ); }

private:
	bool isInfoOnly;
	bool optionsEnd;
	// if true program only parse and print found errors
	bool isCheckOnly;
	// *.ref files
	CFileNames sourceFiles;

	static bool isOption( const std::string& argument );
	bool parseArgument( const std::string& argument );

	static void printInvalidOption( const std::string& argument );
	static void printHelp();
	static void printVersion();
};

//-----------------------------------------------------------------------------

void CCommandLineOptions::ResetDefaults()
{
	SetInfoOnly( false );
	SetCheckOnly( false );
	ResetSourceFiles();
}

bool CCommandLineOptions::SetByCommandLineArguments( int numberOfArguments,
	const char* const arguments[] )
{
	ResetDefaults();
	optionsEnd = false;
	for( int i = 0; i < numberOfArguments; i++ ) {
		if( !parseArgument( arguments[i] ) ) {
			ResetDefaults();
			return false;
		}
		if( IsInfoOnly() ) {
			break;
		}
	}
	return true;
}

bool CCommandLineOptions::isOption( const std::string& argument )
{
	return ( argument.length() >= 2 && argument.front() == '-' );
}

bool CCommandLineOptions::parseArgument( const std::string& argument )
{
	if( !optionsEnd && isOption( argument ) ) {
		if( argument == "-c" || argument == "--check" ) {
			SetCheckOnly();
		} else if( argument == "--help" ) {
			SetInfoOnly();
			printHelp();
		} else if( argument == "--version" ) {
			SetInfoOnly();
			printVersion();
		} else if( argument == "--" ) {
			optionsEnd = true;
		} else {
			printInvalidOption( argument );
			return false;
		}
	} else if( !argument.empty() ) {
		AddSourceFile( argument );
	}
	return true;
}

void CCommandLineOptions::printInvalidOption( const std::string& argument )
{
	std::cerr
		<< UtilityName << ": invalid option `" << argument << "`" << std::endl
		<< "Try `" << UtilityName << " --help` for more information."
		<< std::endl;
}

void CCommandLineOptions::printHelp()
{
	std::cout
		<< "Usage: " << UtilityName << " [OPTION]... [FILE]..."
		<< std::endl << std::endl
		<< "  -c, --check      check source FILE(s) for errors and exit"
		<< std::endl
		<< "      --help       display this help and exit" << std::endl
		<< "      --version    output version information and exit"
		<< std::endl << std::endl << "Report bugs to <refal2@yandex.ru>."
		<< std::endl;
}

void CCommandLineOptions::printVersion()
{
	std::cout
		<< "Written by Anton Todua." << std::endl
		<< "Report bugs to <refal2@yandex.ru>" << std::endl;
}

//-----------------------------------------------------------------------------
// CSourceFilesProcessor

class CSourceFilesProcessor : private IErrorHandler {
public:
	static CProgramPtr Compile( const CFileNames& fileNames );
	static bool Check( const CFileNames& fileNames );

private:
	CScanner scanner;
	std::ifstream file;

	CSourceFilesProcessor( const CFileNames& fileNames );
	CSourceFilesProcessor( const CSourceFilesProcessor& );
	CSourceFilesProcessor& operator=( const CSourceFilesProcessor& );

	void processFile( const std::string& fileName );

	virtual void Error( const CError& error );
};

//-----------------------------------------------------------------------------

CProgramPtr CSourceFilesProcessor::Compile( const CFileNames& fileNames )
{
	CSourceFilesProcessor sourceFilesProcessor( fileNames );
	return sourceFilesProcessor.scanner.BuildProgram();
}

bool CSourceFilesProcessor::Check( const CFileNames& fileNames )
{
	CSourceFilesProcessor sourceFilesProcessor( fileNames );
	return sourceFilesProcessor.scanner.CheckProgram();
}

CSourceFilesProcessor::CSourceFilesProcessor( const CFileNames& fileNames )
{
	scanner.SetErrorHandler( this );
	if( fileNames.empty() ) {
		for( char c; std::cin.get( c ); ) {
			scanner.AddChar( c );
		}
		if( scanner.ErrorSeverity() != ES_FatalError ) {
			scanner.AddEndOfFile();
		}
	} else {
		for( CFileNames::const_iterator fileName = fileNames.begin();
			fileName != fileNames.end(); ++fileName )
		{
			processFile( *fileName );
			if( scanner.ErrorSeverity() == ES_FatalError ) {
				break;
			}
		}
	}
}

void CSourceFilesProcessor::processFile( const std::string& fileName )
{
	file.open( fileName );
	scanner.SetFileName( fileName );
	if( file.good() ) {
		for( char c; file.get( c ); ) {
			scanner.AddChar( c );
		}
		if( scanner.ErrorSeverity() != ES_FatalError ) {
			scanner.AddEndOfFile();
		}
	} else {
		scanner.RaiseError( ES_FatalError, "no such file" );
	}
	scanner.ResetFileName();
	file.close();
}

void CSourceFilesProcessor::Error( const CError& error )
{
	std::cerr << error.UserMessage() << std::endl;
	if( error.Severity() == ES_FatalError ) {
		file.clear( std::ios_base::eofbit );
		std::cin.clear( std::ios_base::eofbit );
	}
}

//-----------------------------------------------------------------------------

const char* const ExecutionResultStrings[] = {
	"ok",
	"recognition impossible",
	"call empty function",
	"lost function label",
	"wrong argument of embedded function"
};

const std::string SeparatorLine( 80, '-' );

enum TReturnCode {
	RC_OK = 0,
	RC_InvalidOptions = 1,
	RC_ErrorsInSourceFiles = 2,
	RC_RuntimeError = 3
};

int main( int argc, const char* argv[] )
{
	std::ios::sync_with_stdio( false );

	CCommandLineOptions commandLineOptions;
	if( !commandLineOptions.SetByCommandLineArguments( argc - 1, argv + 1 ) ) {
		return RC_InvalidOptions;
	}

	if( commandLineOptions.IsInfoOnly() ) {
		return RC_OK;
	}

	if( commandLineOptions.IsCheckOnly() ) {
		if( CSourceFilesProcessor::Check( commandLineOptions.SourceFiles() ) ) {
			return RC_OK;
		}
		return RC_ErrorsInSourceFiles;
	}

	CProgramPtr program = CSourceFilesProcessor::Compile(
		commandLineOptions.SourceFiles() );
	if( !static_cast<bool>( program ) ) {
		return RC_ErrorsInSourceFiles;
	}

	CReceptaclePtr receptacle( new CReceptacle );
	CUnitList fieldOfView;
	CUnitNode* errorNode = nullptr;
	TExecutionResult result = ER_OK;
	try {
		result = COperationsExecuter::Run( program, receptacle, fieldOfView,
			errorNode );
	} catch( std::bad_alloc& e ) {
		std::cerr << "Not enough memory `" << e.what() << "`," << std::endl
			<< "Check your program for infinite recursion." << std::endl;
		return RC_RuntimeError;
	}

	std::cout << SeparatorLine << std::endl;
	std::cout << "Execution result: "
		<< ExecutionResultStrings[result] << "." << std::endl;
	if( !fieldOfView.IsEmpty() ) {
		std::cout << "Field of view: " << std::endl;
		CProgramPrintHelper programPrintHelper( program );
		//programPrintHelper.SetPrintLabelWithModule();
		fieldOfView.HandyPrint( std::cout, programPrintHelper );
		std::cout << std::endl;
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
	return RC_OK;
}
