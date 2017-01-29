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
	temp.token = 0;
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
			out << "'" << temp.token->value << "'" << std::endl;
		}
		else
		if (tok == TOK_MACRO_IDENTIFIER || tok == TOK_MACRO_VALUE)
		{
			out << markdowntown_get_token_name(tok) << " -> '" << temp.token->value << "'" << std::endl;
		}
		else
		{
			out << markdowntown_get_token_name(tok) << std::endl;
			temp.token = 0;
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


bool Compiler::parse()
{
	void *scanner;
	void *scanString;
	parser_context_t context;
	bool result = false;

	// initializes the lexer
	markdowntown_lex_init(&scanner);
	if (scanner == NULL) return false;
	scanString = markdowntown__scan_string(content.c_str(), scanner);
	if (scanString == NULL) goto ESCAPE1;
	markdowntown_set_lineno(1, scanner);
	markdowntown_set_column(1, scanner);
	markdowntown_set_debug(0, scanner);

	// initializes the parser
	context.lexer = scanner;
	context.fileName = "bla";

	// parses the input text generating the AST
	if (markdowntown_parse(&context) != 0)
		goto ESCAPE2;

	if (!context.stack.empty())
	{
		root = context.stack[0];
		root->print(std::cerr, markdowntown::Node::name);
		prune(root);
		root->print(std::cerr, markdowntown::Node::name);
		result = true;
	}
	else
		root = NULL;

ESCAPE2:
	markdowntown__delete_buffer((YY_BUFFER_STATE)scanString, scanner);
ESCAPE1:
	markdowntown_lex_destroy(scanner);

	if (!result) root = NULL;

	return result;
}


const Node *Compiler::getTree() const
{
	return root;
}


void Compiler::prune(
	Node *root )
{
	while ( pruneNode(root) )
	{
		std::cerr << "Pruned!\n";
	}
}


static void mergeNodes(
	Node *nodeA,
	Node *nodeB )
{
	*nodeA << *nodeB;
	if (nodeB->parent != NULL)
		nodeB->parent->remove(nodeB);
	delete nodeB;
}


bool Compiler::pruneNode(
	Node *root )
{
	bool hasChange = false;

	Node *previous = NULL;
	Node *current = root->first();

	while (current != NULL)
	{
		hasChange |= pruneNode(current);

		if (previous != NULL)
		{
			bool merge = false;

			// merge contiguous paragraphs
			if ((current->type == NTY_PARAGRAPH && current->type == previous->type) ||
				current->type == NTY_CONTINUATION)
			{
				merge = true;
			}
			else
			// merge lists if the current one only have list nodes
			if ((current->type == NTY_ORDERED_LIST ||  current->type == NTY_UNORDERED_LIST) &&
				current->type == previous->type)
			{
				// check if we the children are list nodes
				merge = true;
				Node *child = current->first();
				for (; merge && child != NULL; child = child->next())
					merge &= (child->type == NTY_ORDERED_LIST || child->type == NTY_UNORDERED_LIST);

			}
			else
			if (current->type == NTY_BLOCKQUOTE && current->type == previous->type)
				merge = true;

			if (merge)
			{
				Node *temp = current->next();
				mergeNodes(previous, current);
				hasChange = true;
				current = temp;
				continue;
			}
		}
		// TODO: handle the case of a paragraph inside another paragraph (continuation with paragraphs)

		previous = current;
		current = current->next();
	}

	return hasChange;
}


}
