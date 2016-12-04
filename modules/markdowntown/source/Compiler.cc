#include <fstream>
#include "markdowntown.y.hh"
#include "markdowntown.l.hh"
#include <markdowntown/Compiler.hh>

void markdowntown_set_column (int  column_no , yyscan_t yyscanner);


namespace markdowntown {


Compiler::Compiler()
{

}


Compiler::~Compiler()
{

}


void Compiler::setContent(
	std::istream &in )
{
	std::stringstream ss;
	std::string line;

	while (std::getline(in, line))
	{
		// Notice: '\2' is a "begin of line" marker required by
		//         lexer to match some patterns
		ss << '\2';
		ss << line;
		ss << '\n';
	}

	content = ss.str();
}


void Compiler::printTokens(
	std::ostream &out )
{
	int tok = 0;
	int indent = 0;
	YYSTYPE temp;
	void *scanner = NULL;
	void *scanString = NULL;

	// initialize the lexer
	markdowntown_lex_init(&scanner);
	if (scanner == NULL) return;
	scanString = markdowntown__scan_string(content.c_str(), scanner);
	if (scanString == NULL) goto ESCAPE;
	markdowntown_set_lineno(1, scanner);
	markdowntown_set_column(1, scanner);
	markdowntown_set_debug(0, scanner);

	// print each token found
	temp.text = 0;
	while ((tok = markdowntown_lex(&temp, scanner)) != 0)
	{
		switch (tok)
		{
			case TOK_CLOSE_BLOCKQUOTE:
			case TOK_CLOSE_PARAGRAPH:
			case TOK_CLOSE_BOLD:
			case TOK_CLOSE_ITALIC:
			case TOK_CLOSE_STRONG:
			case TOK_CLOSE_HEADING:
			case TOK_CLOSE_UNORDERED_LIST:
			case TOK_CLOSE_ORDERED_LIST:
			case TOK_CLOSE_URL:
				indent--;
				break;
		}

		// prints the indentation
		for (int i = 0; i < indent; ++i)
			out << "   ";

		// prints the token information
		if (tok == TOK_TEXT || tok == TOK_RAW_TEXT)
		{
			out << "'" << temp.text << "'" << std::endl;
		}
		else
		{
			out << markdowntown_get_token_name(tok) << std::endl;
			temp.text = 0;
		}

		switch (tok)
		{
			case TOK_OPEN_BLOCKQUOTE:
			case TOK_OPEN_PARAGRAPH:
			case TOK_OPEN_BOLD:
			case TOK_OPEN_ITALIC:
			case TOK_OPEN_STRONG:
			case TOK_OPEN_HEADING:
			case TOK_OPEN_UNORDERED_LIST:
			case TOK_OPEN_ORDERED_LIST:
			case TOK_OPEN_URL:
				indent++;
				break;
		}
	}

	markdowntown__delete_buffer((YY_BUFFER_STATE)scanString, scanner);
ESCAPE:
	markdowntown_lex_destroy(scanner);
}


void Compiler::parse()
{
	void *scanner;
	void *scanString;
	parser_context_t context;

	// initializes the lexer
	markdowntown_lex_init(&scanner);
	if (scanner == NULL) return;
	scanString = markdowntown__scan_string(content.c_str(), scanner);
	if (scanString == NULL) goto ESCAPE;
	markdowntown_set_lineno(1, scanner);
	markdowntown_set_column(1, scanner);
	markdowntown_set_debug(1, scanner);

	// initializes the parser
	context.lexer = scanner;
	context.fileName = "bla";

	// parses the input text generating the AST
	markdowntown_parse(&context);

	if (!context.stack.empty())
	{
		root = context.stack[0];
		root->print(std::cout, markdowntown::Node::name);
	}
	else
		root = NULL;

	markdowntown__delete_buffer((YY_BUFFER_STATE)scanString, scanner);
ESCAPE:
	markdowntown_lex_destroy(scanner);
}


}
