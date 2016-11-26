#include <fstream>
#include "markdowntown.y.hh"
#include "markdowntown.l.hh"
#include <markdowntown/Node.hh>


void markdowntown_set_column (int  column_no , yyscan_t yyscanner);


void *getScanString(
    void *scanner,
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

	return markdowntown__scan_string(ss.str().c_str(), scanner);
}


void tokens(
	std::istream &in,
	const std::string &fileName )
{
	YYSTYPE temp;
	void *scanner;
	void *scanString;

	// initialize the lexer
	markdowntown_lex_init(&scanner);
	scanString = getScanString(scanner, in);
	markdowntown_set_lineno(1, scanner);
	markdowntown_set_column(1, scanner);
	markdowntown_set_debug(0, scanner);

	temp.node = 0;
	int tok;
	int indent = 0;
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
				indent--;
				break;
		}

		for (int i = 0; i < indent; ++i)
			std::cout << "   ";

		if (tok == TOK_TEXT || tok == TOK_RAW_TEXT)
		{
			std::cout << "'" << temp.node << "'" << std::endl;
		}
		else
		{
			std::cout << markdowntown_get_token_name(tok) << std::endl;
			temp.node = 0;
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
				indent++;
				break;
		}
	}

	if (scanString != NULL)
		markdowntown__delete_buffer((YY_BUFFER_STATE)scanString, scanner);
	markdowntown_lex_destroy(scanner);
}


int main( int argc, char **argv )
{
#if (1)
	void *scanner;
	void *scanString;

	markdowntown_lex_init(&scanner);
	scanString = getScanString(scanner, std::cin);
	markdowntown_set_lineno(1, scanner);
	markdowntown_set_column(1, scanner);
	markdowntown_set_debug(1, scanner);

	parser_context_t context;
	context.lexer = scanner;
	context.fileName = "bla";
	context.rule = NULL;

	markdowntown_parse(&context);

	if (!context.stack.empty())
	{
		markdowntown::Node *root = context.stack[0];
		root->print(std::cout, markdowntown::Node::name);
	}

	if (scanString != NULL)
		markdowntown__delete_buffer((YY_BUFFER_STATE)scanString, scanner);
	markdowntown_lex_destroy(scanner);
#else
	tokens(std::cin, "");
#endif
	return 0;
}