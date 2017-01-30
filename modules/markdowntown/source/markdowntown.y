
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
#include "Token.hh"


#pragma GCC diagnostic ignored "-Wunused-function"


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
	std::cerr << context->fileName << ":"
		<< markdowntown_get_lineno(context->lexer) << ":"
		<< markdowntown_get_column(context->lexer) << ": error: "
		<< msg << std::endl;
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
		temp = new markdowntown::Node(tok);
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
	int type,
	const Token *token = NULL,
	uint32_t line = 1,
	uint32_t column = 1 )
{
	//std::cout << "PUSH " << value << std::endl;
	markdowntown::Node *node;
	if (token != NULL)
		node = new markdowntown::Node(type, token->value);
	else
		node = new markdowntown::Node(type);

	node->line = line;
	node->column = column;
	node->counter = (token != NULL) ? token->counter : 0;
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

#define PUSH_TOKEN(id, token)                                  \
	{                                                          \
		markdowntown_push(                                     \
			parserContext,                                     \
			(id),                                              \
			(token),                                           \
			markdowntown_get_lineno(parserContext->lexer),     \
			markdowntown_get_column(parserContext->lexer) );   \
	}

#define PUSH(id)                                               \
	{                                                          \
		markdowntown_push(                                     \
			parserContext,                                     \
			(id),                                              \
			NULL,                                              \
			markdowntown_get_lineno(parserContext->lexer),     \
			markdowntown_get_column(parserContext->lexer) );   \
	}

#define NPUSH(node)          markdowntown_push( parserContext, (node) )
#define POP()                markdowntown_pop(parserContext->stack)
#define TOP()                (parserContext->stack[ parserContext->stack.size() - 1 ])
#define COMBINE(tok,n)       markdowntown_combine(parserContext->stack, (tok), (n))
#define PSTACK()             markdowntown_printStack(parserContext->stack, parserContext->parser)

#pragma GCC diagnostic ignored "-Wconversion"

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
	Token *token;
}


%token <token> TOK_CODE
%token <token> TOK_TEXT
%token <token> TOK_HASHTAG
%token <token> TOK_SM
%token <token> TOK_COLON
%token <token> TOK_BOL
%token <token> TOK_OPEN_BLOCKQUOTE
%token <token> TOK_OPEN_PARAGRAPH
%token <token> TOK_OPEN_HEADING
%token <token> TOK_OPEN_BOLD
%token <token> TOK_OPEN_ITALIC
%token <token> TOK_OPEN_STRONG
%token <token> TOK_OPEN_UNORDERED_LIST
%token <token> TOK_OPEN_ORDERED_LIST
%token <token> TOK_OPEN_URL
%token <token> TOK_OPEN_CONTINUATION
%token <token> TOK_OPEN_MACRO
%token <token> TOK_CLOSE_BLOCKQUOTE
%token <token> TOK_CLOSE_PARAGRAPH
%token <token> TOK_CLOSE_HEADING
%token <token> TOK_CLOSE_BOLD
%token <token> TOK_CLOSE_ITALIC
%token <token> TOK_CLOSE_STRONG
%token <token> TOK_CLOSE_UNORDERED_LIST
%token <token> TOK_CLOSE_ORDERED_LIST
%token <token> TOK_CLOSE_URL
%token <token> TOK_CLOSE_CONTINUATION
%token <token> TOK_CLOSE_MACRO
%token <token> TOK_LINE
%token <token> TOK_RAW_TEXT
%token <token> TOK_MACRO
%token <token> TOK_MACRO_IDENTIFIER
%token <token> TOK_MACRO_VALUE
%token <token> TOK_MACRO_PIPE
%token <token> TOK_EMPTY


%start CompilationUnit

%type <node> Text TextEntry SimpleText BoldText ItalicText StrongText InlineCode RawText
%type <node> Identifier InlineUrl Macro MacroParameterList MacroParameter

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
	| Empty
	;

Empty:
	TOK_EMPTY
	{ PUSH(NTY_EMPTY); }
	;

Heading:
	TOK_OPEN_HEADING Text TOK_CLOSE_HEADING
	{
		TOP()->type = NTY_HEADING;
		TOP()->counter = $1->counter;
	}
	;

Paragraph:
	TOK_OPEN_PARAGRAPH Text TOK_CLOSE_PARAGRAPH
	{ TOP()->type = NTY_PARAGRAPH; }
	;

BlockQuote:
	TOK_OPEN_BLOCKQUOTE Block TOK_CLOSE_BLOCKQUOTE
	{ TOP()->type = NTY_BLOCKQUOTE; TOP()->counter = $1->counter; }
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
	{ PUSH_TOKEN(NTY_LINE, $1); }
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
	{ PUSH_TOKEN(NTY_TEXT, $1); }
	;

Identifier:
	TOK_MACRO_IDENTIFIER
	{ PUSH_TOKEN(NTY_MACRO_IDENTIFIER, $1); }
	;

RawText:
	TOK_RAW_TEXT
	{ PUSH_TOKEN(NTY_RAW_TEXT, $1); }
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
	{
		PUSH_TOKEN(NTY_CODE, $1);
	}
	;

InlineUrl:
	TOK_OPEN_URL SimpleText SimpleText TOK_CLOSE_URL
	{ COMBINE(NTY_INLINE_URL, 2); }
	;

Macro:
	TOK_OPEN_MACRO Identifier TOK_CLOSE_MACRO
	{
		PUSH(NTY_MACRO_PARAM_LIST);
		COMBINE(NTY_MACRO, 2);
	}
	| TOK_OPEN_MACRO Identifier TOK_MACRO_PIPE MacroParameterList TOK_CLOSE_MACRO
	{ COMBINE(NTY_MACRO, 2); }
	;

MacroParameterList:
	MacroParameter
	{ COMBINE(NTY_MACRO_PARAM_LIST, 1); }
	| MacroParameterList TOK_MACRO_PIPE MacroParameter
	{ COMBINE(0, 1); }
	;

MacroParameter:
	TOK_MACRO_VALUE
	{
		PUSH(NTY_MACRO_IDENTIFIER);
		PUSH_TOKEN(NTY_MACRO_VALUE, $1);
		COMBINE(NTY_MACRO_PARAM, 2);
	}
	| TOK_MACRO_IDENTIFIER TOK_MACRO_VALUE
		{
		PUSH_TOKEN(NTY_MACRO_IDENTIFIER, $1);
		PUSH_TOKEN(NTY_MACRO_VALUE, $2);
		COMBINE(NTY_MACRO_PARAM, 2);
	}
	;

%%

const char *markdowntown_get_token_name( int tok )
{
	return yytname[tok - 255];
}