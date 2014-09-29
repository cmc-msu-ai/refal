#include "Refal.h"

#if 0
namespace refal2
{
	Refal2::Refal2()
		: labels(), qualifiers(), entry(0), executer()
	{
	}

	Refal2::~Refal2()
	{
	}

	bool Refal2::run(std::ostream& output_stream, std::ostream& error_stream)
	{
		if( entry == 0 )
		{
			return false;
		}

		return executer.run(entry->second.operation,
							output_stream, error_stream);
	}

	bool Refal2::read_file(const std::string& filename,
						   std::ostream& error_stream)
	{
		std::ifstream file(filename);

		Reader reader(this, error_stream);

		if( !file.good() )
		{
			/* TODO: error */
			return false;
		}

		for(; ; )
		{
			char c = file.get();

			if( file.good() )
			{
				reader.push(c);
			}
			else
			{
				break;
			}
		}
		reader.push(Reader::END_OF_FILE);

		if( reader.good() )
		{
			return true;
		}

		return false;
	}

	bool Refal2::check_names(std::ostream& error_stream)
	{
		bool ok = true;

		for(LabelsTable::iterator i = labels.begin(); i != labels.end(); ++i)
		{
			if( i->second.rule == 0 )
			{
				ok = false;
				/* TODO: ERROR label wasn't defined */
			}
		}

		for(QualifiersTable::iterator i = qualifiers.begin();
			i != qualifiers.end(); ++i)
		{
			if( i->second == 0 )
			{
				ok = false;
				/* TODO: ERROR qualifier wasn't defined */
			}
		}

		if( entry == 0 )
		{
			ok = false;
			/* TODO: entry wasn't declarated */
		}

		return ok;
	}

	bool Refal2::loadFromFile(const std::string& filename,
							  std::ostream& error_stream)
	{
		if( !read_file(filename, error_stream) )
		{
			return false;
		}

		if( !check_names(error_stream) )
		{
			return false;
		}

		for(LabelsTable::iterator i = labels.begin(); i != labels.end(); ++i)
		{
			i->second.operation = Compiler::compile(i->second.rule);
		}

		return true;
	}
}
#endif
