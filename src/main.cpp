#include <Refal2.h>

using namespace Refal2;

namespace Refal2 {

class CStandartFunctionTable : std::map<std::string, void*> {
public:
	bool FindByName( const std::string& name,
		void*& standartFunction ) const;
};

static void strangePrint( const CUnitList& expression )
{
	for( const CUnitNode* i = expression.GetFirst(); i != 0; i = i->Next() ) {
		switch( i->GetType() ) {
			case UT_Char:
				std::cout << i->Char();
				break;
			case UT_Label:
				std::cout << "'" << LabelTable.GetLabelText( i->Label() ) << "'";
				break;
			case UT_Number:
				std::cout << "'" << i->Number() << "'";
				break;
			case UT_LeftParen:
				std::cout << "(";
				break;
			case UT_RightParen:
				std::cout << ")";
				break;
			default:
				assert( false );
				break;
		}
	}
	std::cout << std::endl;
}

bool ExtrnPrint( CUnitList& argument, std::string& errorText )
{
	strangePrint( argument );
	return true;
}

bool ExtrnPrintm( CUnitList& argument, std::string& errorText )
{
	HandyPrintFieldOfView( argument );
	return true;
}

bool ExtrnProut( CUnitList& argument, std::string& errorText )
{
	strangePrint( argument );
	argument.Empty();
	return true;
}

bool ExtrnProutm( CUnitList& argument, std::string& errorText )
{
	HandyPrintFieldOfView( argument );
	argument.Empty();
	return true;
}

}

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

bool ParseFile( std::istream& fileStream )
{
	LabelTable.Empty();
	CErrorHandler errorProcessor;
	CScanner scanner( &errorProcessor );

	for( char c; fileStream.get( c ); ) {
		scanner.AddChar( c );
	}

	return ( !scanner.HasErrors() );
}

int main( int argc, const char* argv[] )
{
	if( argc < 2 ) {
		std::cerr << "Wrong arguments: source files were not set." << std::endl;
		return 1;
	}

	bool success = true;
	for( int i = 1; i < argc; i++ ) {
		std::ifstream fileStream( argv[i] );
		if( !fileStream.good() ) {
			std::cerr << "Cannot open file: `" << argv[i] << "`." << std::endl;
			std::cerr << "------------------------" << std::endl << std::endl;
			success = false;
			continue;
		}
		std::cout << "Source file: `" << argv[i] << "`." << std::endl;
		ParseFile( fileStream );
		std::cout << "------------------------" << std::endl << std::endl;
		std::cerr << "------------------------" << std::endl << std::endl;
		fileStream.close();
	}

	return ( success ? 0 : 1 );
}
