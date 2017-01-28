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
			case TOK_CLOSE_CONTINUATION:
			case TOK_CLOSE_MACRO:
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
		if (tok == TOK_MACRO_IDENTIFIER || tok == TOK_MACRO_VALUE)
		{
			out << markdowntown_get_token_name(tok) << " -> '" << temp.text << "'" << std::endl;
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
			case TOK_OPEN_CONTINUATION:
			case TOK_OPEN_MACRO:
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
	markdowntown_set_debug(0, scanner);

	// initializes the parser
	context.lexer = scanner;
	context.fileName = "bla";

	// parses the input text generating the AST
	markdowntown_parse(&context);

	if (!context.stack.empty())
	{
		root = context.stack[0];
		root->print(std::cerr, markdowntown::Node::name);
		prune(root);
		root->print(std::cerr, markdowntown::Node::name);
	}
	else
		root = NULL;

	markdowntown__delete_buffer((YY_BUFFER_STATE)scanString, scanner);
ESCAPE:
	markdowntown_lex_destroy(scanner);
}


const Node *Compiler::getTree() const
{
	return root;
}


void Compiler::prune(
	Node *root )
{
	Node *previous = NULL;
	Node *current = root->first();

	while (current != NULL)
	{
		prune(current);

		if (previous != NULL)
		{
			// merge contiguous paragraphs
			if ((current->type == NTY_PARAGRAPH && current->type == previous->type) ||
				current->type == NTY_CONTINUATION)
			{
				*previous << *current;
				root->remove(current);
				Node *temp = current;
				current = current->next();
				delete temp;
				continue;
			}
			else
			// merge lists if the current one only have list nodes
			if ((current->type == NTY_ORDERED_LIST ||  current->type == NTY_UNORDERED_LIST) &&
				current->type == previous->type)
			{
				// check if we the children are list nodes
				bool onlyList = true;
				Node *child = current->first();
				for (; onlyList && child != NULL; child = child->next())
					onlyList &= (child->type == NTY_ORDERED_LIST || child->type == NTY_UNORDERED_LIST);

				if (onlyList)
				{
					*previous << *current;
					root->remove(current);
					Node *temp = current;
					current = current->next();
					delete temp;
					continue;
				}
			}
		}
		// TODO: handle the case of a paragraph inside another paragraph (continuation with paragraphs)

		previous = current;
		current = current->next();
	}
}


}
