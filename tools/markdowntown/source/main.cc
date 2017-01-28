#include <markdowntown/Compiler.hh>
#include <markdowntown/HtmlExporter.hh>
#include <fstream>
#include <iostream>


using markdowntown::Compiler;
using markdowntown::HtmlExporter;
using std::ifstream;


int main( int argc, char **argv )
{
	if (argc != 2) return 1;

	ifstream input(argv[1]);
	if (input.good())
	{
		Compiler compiler;
		compiler.setContent(input);
		//compiler.printTokens(std::cout);
		compiler.parse();
		input.close();

		if (compiler.getTree() != NULL)
		{
			HtmlExporter exporter;
			exporter.write( std::cout, *compiler.getTree() );
		}
	}

	return 1;
}