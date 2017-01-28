
%require "2.3"
%define api.pure full
%lex-param   { yyscan_t scanner }
%parse-param { parser_context_t *parserContext }


%code requires {


#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <markdowntown/Node.hh>


/*
 * The parser needs to know about the 'yyscan_t' type,
 * but the generated header by Flex don't provide this information.
 */
typedef void *yyscan_t;


typedef struct
{
	yyscan_t lexer;

	std::vector<markdowntown::Node*> stack;

	const char *fileName;

} parser_context_t;


/**
 * Get the current column in the lexer scanner.
 *
 * For some reason this prototype is not generated in the Flex
 * header file, so we include it here.
 */
int markdowntown_get_column  (yyscan_t yyscanner);


const char *markdowntown_get_token_name( int tok );


}


%code
{

#include "markdowntown.l.hh"

using namespace markdowntown;


static void markdowntown_error(parser_context_t *context, const char *msg)
{
	printf ("%s:%d:%d: error: %s\n",
		context->fileName,
		markdowntown_get_lineno(context->lexer),
		markdowntown_get_column(context->lexer),
		msg);
	return;
}


static markdowntown::Node *markdowntown_pop( std::vector<markdowntown::Node*> &stack )
{
	if (stack.size() == 0) return NULL;
	markdowntown::Node *node = stack[ stack.size() - 1 ];
	stack.pop_back();
	return node;
}


static markdowntown::Node* markdowntown_combine( std::vector<markdowntown::Node*> &stack, int tok, int n )
{
	markdowntown::Node *temp;

	if (n == 0 || (int) stack.size() < n) return NULL;

	if (tok > 0 )
		temp = new markdowntown::Node(tok, NULL);
	else
	{
		if ((int) stack.size() <= n) return NULL;
		temp = stack[ stack.size() - 1 - n ];
	}
	for (size_t i = stack.size() - n; i < stack.size(); ++i)
	{
		//std::cout << temp->getValue() << ": adding child " << p->getValue() << std::endl;;
		temp->append( stack[i] );
	}
	for (int i = 0; i < n; ++i)
		stack.pop_back();

	if (tok > 0) stack.push_back(temp);
	return temp;
}


static void markdowntown_printStack( std::vector<markdowntown::Node*> &stack  )
{
	std::cout << "Stack:" << std::endl;
	for (int i = 0; i < (int) stack.size(); ++i)
		stack[i]->print(std::cout, markdowntown::Node::name, 1, false);
}


static void markdowntown_push(
	parser_context_t *context,
	int token,
	const char *value,
	uint32_t line = 1,
	uint32_t column = 1 )
{
	//std::cout << "PUSH " << value << std::endl;
	markdowntown::Node *node;
	node = new markdowntown::Node(token,value);
	node->line = line;
	node->column = column;
	context->stack.push_back(node);
}


static void markdowntown_push(
	parser_context_t *context,
	markdowntown::Node *node )
{
	context->stack.push_back(node);
}


static void markdowntown_push(
	parser_context_t *context,
	markdowntown::Node &node )
{
	markdowntown_push(context, &node);
}


#define scanner              parserContext->lexer
#define PUSH(token,value)    markdowntown_push( parserContext, (token), (value), \
	markdowntown_get_lineno(parserContext->lexer), markdowntown_get_column(parserContext->lexer) )
#define NPUSH(node)          markdowntown_push( parserContext, (node) )
#define POP()                markdowntown_pop(parserContext->stack)
#define TOP()                (parserContext->stack[ parserContext->stack.size() - 1 ])
#define COMBINE(tok,n)       markdowntown_combine(parserContext->stack, (tok), (n))
#define PSTACK()             markdowntown_printStack(parserContext->stack, parserContext->parser)

}

/* namespace to enclose parser in */
%name-prefix "markdowntown_"


/* keep track of the current position within the input
%locations
%initial-action
{
	// initialize the initial location object
	//@$.begin.filename = @$.end.filename = &driver.streamname;
};*/


/* verbose error messages */
%error-verbose

/*
 * %union declares of what kinds of values appear on the value stack
 */
%union {
	char *text;
}


%token <text> TOK_CODE
%token <text> TOK_TEXT
%token <text> TOK_HASHTAG
%token <text> TOK_SM
%token <text> TOK_COLON
%token <text> TOK_BOL
%token <text> TOK_OPEN_BLOCKQUOTE
%token <text> TOK_OPEN_PARAGRAPH
%token <text> TOK_OPEN_HEADING
%token <text> TOK_OPEN_BOLD
%token <text> TOK_OPEN_ITALIC
%token <text> TOK_OPEN_STRONG
%token <text> TOK_OPEN_UNORDERED_LIST
%token <text> TOK_OPEN_ORDERED_LIST
%token <text> TOK_OPEN_URL
%token <text> TOK_OPEN_CONTINUATION
%token <text> TOK_OPEN_MACRO
%token <text> TOK_CLOSE_BLOCKQUOTE
%token <text> TOK_CLOSE_PARAGRAPH
%token <text> TOK_CLOSE_HEADING
%token <text> TOK_CLOSE_BOLD
%token <text> TOK_CLOSE_ITALIC
%token <text> TOK_CLOSE_STRONG
%token <text> TOK_CLOSE_UNORDERED_LIST
%token <text> TOK_CLOSE_ORDERED_LIST
%token <text> TOK_CLOSE_URL
%token <text> TOK_CLOSE_CONTINUATION
%token <text> TOK_CLOSE_MACRO
%token <text> TOK_LINE
%token <text> TOK_RAW_TEXT
%token <text> TOK_MACRO
%token <text> TOK_IDENTIFIER


%start CompilationUnit

%type <node> Text TextEntry SimpleText BoldText ItalicText StrongText InlineCode RawText InlineUrl Macro

%%

CompilationUnit:
	Block
	;

Block:
	BlockEntry
	{ COMBINE(NTY_BLOCK, 1); }
	| Block BlockEntry
	{ COMBINE(0, 1); }
	;

BlockEntry:
	Heading
	| Paragraph
	| BlockQuote
	| HorizontalLine
	| UnorderedList
	| OrderedList
	| Continuation
	;

Heading:
	TOK_OPEN_HEADING Text TOK_CLOSE_HEADING
	{ TOP()->type = NTY_HEADING; std::cout << "@@@ HEADING\n"; }
	;

Paragraph:
	TOK_OPEN_PARAGRAPH Text TOK_CLOSE_PARAGRAPH
	{ TOP()->type = NTY_PARAGRAPH; }
	;

BlockQuote:
	TOK_OPEN_BLOCKQUOTE Block TOK_CLOSE_BLOCKQUOTE
	{ TOP()->type = NTY_BLOCKQUOTE; }
	;

UnorderedList:
	TOK_OPEN_UNORDERED_LIST Block TOK_CLOSE_UNORDERED_LIST
	{ TOP()->type = NTY_UNORDERED_LIST; }
	;

OrderedList:
	TOK_OPEN_ORDERED_LIST Block TOK_CLOSE_ORDERED_LIST
	{ TOP()->type = NTY_ORDERED_LIST; }
	;

Continuation:
	TOK_OPEN_CONTINUATION Block TOK_CLOSE_CONTINUATION
	{ TOP()->type = NTY_CONTINUATION; }
	;

HorizontalLine:
	TOK_LINE
	{ PUSH(NTY_LINE, $1); }
	;


Text:
	TextEntry
	{ COMBINE(NTY_BLOCK, 1); }
	| Text TextEntry
	{ COMBINE(0, 1); }
	;

TextEntry:
	SimpleText
	| RawText
	| BoldText
	| ItalicText
	| StrongText
	| InlineCode
	| InlineUrl
	| Macro
	;

SimpleText:
	TOK_TEXT
	{ PUSH(NTY_TEXT, $1); }
	;

Identifier:
	TOK_IDENTIFIER
	{ PUSH(NTY_IDENTIFIER, $1); }
	;

RawText:
	TOK_RAW_TEXT
	{ PUSH(NTY_RAW_TEXT, $1); }
	;

BoldText:
	TOK_OPEN_BOLD Text TOK_CLOSE_BOLD
	{ TOP()->type = NTY_BOLD; }
	;

ItalicText:
	TOK_OPEN_ITALIC Text TOK_CLOSE_ITALIC
	{ TOP()->type = NTY_ITALIC; }
	;

StrongText:
	TOK_OPEN_STRONG Text TOK_CLOSE_STRONG
	{ TOP()->type = NTY_STRONG; }
	;

InlineCode:
	TOK_CODE
	{ PUSH(NTY_TEXT, $1); COMBINE(NTY_CODE, 1); }
	;

InlineUrl:
	TOK_OPEN_URL SimpleText SimpleText TOK_CLOSE_URL
	{ COMBINE(NTY_INLINE_URL, 2); }
	;

Macro:
	TOK_OPEN_MACRO Identifier TOK_CLOSE_MACRO
	{ COMBINE(NTY_MACRO, 1); }
	;

%%

const char *markdowntown_get_token_name( int tok )
{
	return yytname[tok - 255];
}