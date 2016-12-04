#ifndef MARKDOWNTOWN_COMPILER_HH
#define MARKDOWNTOWN_COMPILER_HH


#include <vector>
#include <string>
#include <stdint.h>
#include <markdowntown/Node.hh>


namespace markdowntown {


class Compiler
{
	public:
		Compiler();

		virtual ~Compiler();

		void setContent(
			std::istream &in );

		void setContent(
			const std::string &text );

		void printTokens(
			std::ostream &out );

		void parse();

	private:
		std::string content;
		Node *root;
};

}


#endif // MARKDOWNTOWN_COMPILER_HH