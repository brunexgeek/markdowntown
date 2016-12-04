#include <markdowntown/Compiler.hh>
#include <fstream>


using markdowntown::Compiler;
using std::ifstream;


int main( int argc, char **argv )
{
	if (argc != 2) return 1;

	ifstream input(argv[1]);
	if (input.good())
	{
		Compiler compiler;
		compiler.setContent(input);
		compiler.parse();
		input.close();
	}

	return 1;
}