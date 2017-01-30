#include <markdowntown/Compiler.hh>
#include <markdowntown/HtmlExporter.hh>
#include <fstream>
#include <iostream>
#include <cstdlib>


using markdowntown::Compiler;
using markdowntown::HtmlExporter;
using std::ifstream;
using std::string;

void main_usage()
{
	std::cerr << "Usage: markdowntown -i <input> [ -o <output> ]" << std::endl;
	std::cerr << "       markdowntown -i <input> [ -o <output> ] -t" << std::endl << std::endl;
}


void main_error(
	const std::string &message )
{
	std::cerr << "Error: " << message << std::endl;
	main_usage();
	exit(1);
}


int main( int argc, char **argv )
{
	bool showTokens = false;
	string inputFileName;
	string outputFileName;
	bool result = false;

	for (int i = 1; i < argc; ++i)
	{
		string current = argv[i];
		string next;
		if (i + 1 < argc) next = argv[i + 1];

		if (current == "-t")
			showTokens = true;
		else
		if (current == "-i")
		{
			inputFileName = next;
			++i;
		}
		else
		if (current == "-o")
		{
			outputFileName = next;
			++i;
		}
		else
			main_error("Invalid argument: " + current);
	}

	if (inputFileName.empty())
		main_error("missing input file");

	std::ofstream outputFile;
	std::ostream *output = &std::cout;
	if (!outputFileName.empty())
	{
		outputFile.open(outputFileName.c_str(), std::ios_base::ate);
		if (outputFile.good()) output = &outputFile;
	}

	ifstream input(inputFileName.c_str());
	if (input.good())
	{
		Compiler compiler;
		compiler.setContent(input);
		if (showTokens)
		{
			compiler.printTokens(*output);
		}
		else
		{
			result = compiler.parse();
			if (result && compiler.getTree() != NULL)
			{
				HtmlExporter exporter;
				exporter.write( *output, *compiler.getTree() );
			}
		}
		input.close();
	}

	if (outputFile.is_open()) outputFile.close();

	return (result) ? 0 : 1;
}
