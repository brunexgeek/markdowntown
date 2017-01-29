%{

#include <string>
#include <cstdio>
#include <sstream>
#include <vector>
#include <fstream>
#include <iostream>
#include "markdowntown.y.hh"
#include "Token.hh"


#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-compare"

enum StatementType
{
	STATE_BOLD,
	STATE_ITALIC,
	STATE_STRONG,
	STATE_CODE
};


static bool statements[4] = { false };


class TokenList
{
	public:
		std::vector<Token> entries;

		int index;

		TokenList() : index(-1)
		{
		}

		const Token *getNext()
		{
			if (index < 0 || index >= (int) entries.size()) return NULL;
			return &entries[index++];
		}

		TokenList process( const TokenList &object )
		{
			TokenList output;

			int temp = index;
			index = 0;
			bool listMode = false;

			while (index < (int) entries.size())
			{
				if (index >= (int) object.entries.size())
				{
					output.add( entries[index++] );
					continue;
				}

				const Token &current  = entries[index];
				const Token &previous = object.entries[index];

//std::cerr << markdowntown_get_token_name(current.id) << " != " << markdowntown_get_token_name(previous.id) << "?\n";

				// always close the last level of a list
				if ((current.id == TOK_OPEN_ORDERED_LIST || current.id == TOK_OPEN_UNORDERED_LIST) &&
					previous.id == current.id)
					listMode = true;
				else
				if (listMode)
				{
					--index;
					break;
				}

				if (current.id != previous.id)
					break;

				// always close paragraphs
				if (current.id == TOK_OPEN_PARAGRAPH || previous.id == TOK_OPEN_PARAGRAPH)
					break;

				// always close headings
				if (current.id == TOK_OPEN_HEADING || previous.id == TOK_OPEN_HEADING)
					break;

				index++;
			}

			// close the remaining tokens from previous
			for (int i = (int)object.entries.size() - 1; i >= index; --i)
			{
				int id = closeToken( object.entries[i] );
				if (id >= 0) output.add( Token(id, "") );
			}

			// output the remaining tokens from current
			for (int i = index; i < (int)entries.size(); ++i)
				output.add( entries[i] );
			index = temp;

			return output;
		}

		void add( const Token &token )
		{
			entries.push_back( token );
			if (index < 0) index = 0;
		}

		void reset()
		{
			entries.clear();
			index = -1;
		}

		bool hasNext()
		{
			return (index >= 0 && index < (int) entries.size());
		}

		void swap( TokenList &object )
		{
			object.entries.swap(entries);
			int i = object.index;
			object.index = index;
			index = i;
		}

		Token * last()
		{
			if (entries.size() > 0)
				return &entries[ entries.size() - 1 ];
			else
				return NULL;
		}

		/*
		 * Returns the closing token for statements whose do not have
		 * an explicit closing representation.
		 */
		int closeToken( const Token &token )
		{
			switch (token.id)
			{
				case TOK_OPEN_BLOCKQUOTE:
					return TOK_CLOSE_BLOCKQUOTE;
				case TOK_OPEN_PARAGRAPH:
					return TOK_CLOSE_PARAGRAPH;
				case TOK_OPEN_HEADING:
					return TOK_CLOSE_HEADING;
				case TOK_OPEN_UNORDERED_LIST:
					return TOK_CLOSE_UNORDERED_LIST;
				case TOK_OPEN_ORDERED_LIST:
					return TOK_CLOSE_ORDERED_LIST;
				case TOK_OPEN_CONTINUATION:
					return TOK_CLOSE_CONTINUATION;
				default:
					return -1;
			}
		}

};


static TokenList pending;

static TokenList currentTokens;

static TokenList previousTokens;


static const std::string markdowntown_trim(
	const std::string &text )
{
	if (&text == NULL || text.empty()) return "";

	size_t start, end;

	for (start = 0; start < text.length(); ++start)
	{
		uint8_t current = (uint8_t) text[start];
		if (current > 0x20) break;
	}

	for (end = text.length() - 1; end > 0; --end)
	{
		uint8_t current = (uint8_t) text[end];
		if (current > 0x20) break;
	}

	if (end >= start)
		return text.substr(start, end - start + 1);
	else
		return "";
}


static const char *markdowntown_getTokenText( int tok, const char *text )
{
	switch (tok)
	{
		case TOK_BOL:
			return "<<BOL>>";
		default:
			return text;
	}
}


#define EMIT(id) \
	{                                                                                  \
		currentTokens.add( Token((id), markdowntown_getTokenText( (id), yytext ) ) );  \
	}

#define EMIT_TEXT(id, text)                            \
	{                                                  \
		currentTokens.add( Token( (id), (text) ) );    \
	}

#define TOP() ( currentTokens.last() )

#define EMIT_TOGGLE(variable, tokTrue, tokFalse) \
	{ \
		variable = !variable; \
		int id = (variable) ? tokTrue : tokFalse; \
		currentTokens.add( Token((id), "none" ) ); \
	}

#define ECHO

%}

%option reentrant
%option prefix="markdowntown_"
%option noyywrap
%option never-interactive
%option bison-bridge
%option debug
%option 8bit

 /*
  * We use the special character '\2' as begin of line marker.
  */
BOL                     "\2"
EOL						"\n"
WS                      [ \t]
CONTINUE                "\t"|"    "
OWS                     {WS}*

 /*
  * UNORDERED_LIST          "*"+" "
  * ORDERED_LIST            "#"+" "
  */
UNORDERED_LIST          "*"
ORDERED_LIST            "#"

HYPERLINK               [^\]\n\2]+
ADDRESS                 [^\)\n\2]+

IDENTIFIER              [A-Za-z_][A-Za-z_0-9]*
VALUE                   [^}|\n]+

ITALIC                  "''"
BOLD                    "'''"
STRONG                  "'''''"

%x PENDING
%x OPEN
%x CONTENT
%x URL
%x CONTINUATION
%x MACRO
%x MACRO_PARAM
%x LIST_MODE

%%


{BOL}{OWS}/{EOL} {
	EMIT_TEXT(TOK_EMPTY, "");
	pending = currentTokens.process(previousTokens);
	BEGIN(PENDING);
}


{BOL} {
	BEGIN(OPEN);
}


<PENDING>{EOL} {
	const Token *token = pending.getNext();
	if (token != NULL)
	{
		unput('\n');
		yylval->token = new Token(*token);
		return token->id;
	}
	previousTokens.reset();
	currentTokens.swap(previousTokens);
	pending.reset();
	BEGIN(INITIAL);
}


 /*
  * Begin of block quote line.
  */
<OPEN,LIST_MODE>">"+" " {
	std::string temp = yytext;

	size_t pos1 = temp.find_first_not_of(">");

	EMIT_TEXT(TOK_OPEN_BLOCKQUOTE, "");
	TOP()->counter = pos1;
}

 /*
  * Empty block quote.
  */
<OPEN,LIST_MODE>">"+{OWS}/{EOL} {
	std::string temp = yytext;

	size_t pos1 = temp.find_first_not_of(">");

	EMIT_TEXT(TOK_OPEN_BLOCKQUOTE, "");
	TOP()->counter = pos1;

	EMIT_TEXT(TOK_EMPTY, "");
	BEGIN(CONTENT);
}

 /*
  * Continuation of the previous statement.
  */
<OPEN>{CONTINUE} {
	EMIT_TEXT(TOK_OPEN_CONTINUATION, "");
}


 /*
  * Begin of heading.
  */
<OPEN,LIST_MODE>"="+" "{OWS} {
	std::string temp = yytext;

	size_t pos1 = temp.find_first_not_of("=");

	EMIT_TEXT(TOK_OPEN_HEADING, "");
	TOP()->counter = pos1;

	BEGIN(CONTENT);
}


<OPEN,LIST_MODE>{OWS}"---""-"+{OWS}/{EOL} {
	EMIT_TEXT(TOK_LINE, "");
	pending = currentTokens.process(previousTokens);
	BEGIN(PENDING);
}


<OPEN>{UNORDERED_LIST}+" " {
	EMIT_TEXT(TOK_OPEN_UNORDERED_LIST, "");
	BEGIN(LIST_MODE);
}


<OPEN>{ORDERED_LIST}+" " {
	EMIT_TEXT(TOK_OPEN_ORDERED_LIST, "");
	BEGIN(LIST_MODE);
}


 /*
  * Begin of non-empty line (paragraph).
  */
<OPEN,LIST_MODE>. {
	unput(yytext[0]);
	BEGIN(CONTENT);
	EMIT_TEXT(TOK_OPEN_PARAGRAPH, "");
}


<<EOF>> {
	if (!pending.hasNext())
	{
		pending = currentTokens.process(previousTokens);
		// clean up things to avoid an infinity loop here
		previousTokens.reset();
		currentTokens.reset();
	}

	const Token *token = pending.getNext();
	if (token != NULL)
	{
		yylval->token = new Token(*token);
		return token->id;
	}
	else
		yyterminate();
}


<OPEN,CONTENT>{EOL} {
	unput('\n');
	pending = currentTokens.process(previousTokens);
	BEGIN(PENDING);
}


<CONTENT>"["{HYPERLINK}"]"{OWS}"("{ADDRESS}")" {
	std::string hyperlink, address;
	std::string value = yytext + 1;

	size_t pos = value.find_first_of("]");
	hyperlink = value.substr(0, pos);
	pos = value.find_first_of("(", pos + 1);
	address = value.substr(pos + 1, value.length() - pos - 2);

	EMIT_TEXT(TOK_OPEN_URL, "");
	EMIT_TEXT(TOK_TEXT, markdowntown_trim(hyperlink));
	EMIT_TEXT(TOK_TEXT, markdowntown_trim(address));
	EMIT_TEXT(TOK_CLOSE_URL, "");
}


<CONTENT>{STRONG}		{ EMIT_TOGGLE(statements[STATE_STRONG], TOK_OPEN_STRONG, TOK_CLOSE_STRONG ); }
<CONTENT>{BOLD}			{ EMIT_TOGGLE(statements[STATE_BOLD],   TOK_OPEN_BOLD,   TOK_CLOSE_BOLD   ); }
<CONTENT>{ITALIC}		{ EMIT_TOGGLE(statements[STATE_ITALIC], TOK_OPEN_ITALIC, TOK_CLOSE_ITALIC ); }


<CONTENT>"`"+[^`\n]+"`"+ {
	std::string temp = yytext;

	size_t pos1 = temp.find_first_not_of("`");
	size_t pos2 = temp.find_first_of("`", pos1);
	temp = temp.substr(pos1, pos2 - pos1);

	EMIT_TEXT(TOK_CODE, temp);
	TOP()->counter = pos1;
}

<CONTENT>"{{{" {
	int count = 0;
	int current = 0;
	std::stringstream content;

	while ((current = yyinput(yyscanner)) != EOF)
	{
		if (current == '\2') continue;

		if (current == '}')
		{
			if (++count == 3)
			{
				EMIT_TEXT(TOK_RAW_TEXT, content.str().c_str());
				break;
			}
		}
		else
		{
			count = 0;
			content << (char) current;
		}
	}
}

<CONTENT>"{{" {
	EMIT_TEXT(TOK_OPEN_MACRO, "");
	BEGIN(MACRO);
}

<MACRO>{OWS}{IDENTIFIER}{OWS} {
	EMIT_TEXT(TOK_MACRO_IDENTIFIER, yytext);
}

<MACRO>"|" |
<MACRO_PARAM>"|" {
	EMIT_TEXT(TOK_MACRO_PIPE, "");
	BEGIN(MACRO_PARAM);
}

<MACRO_PARAM>"="{VALUE} {
	yytext[0] = ' ';
	EMIT_TEXT(TOK_MACRO_VALUE, markdowntown_trim(yytext));
}

<MACRO_PARAM>{OWS}{IDENTIFIER}{OWS} {
	EMIT_TEXT(TOK_MACRO_IDENTIFIER, markdowntown_trim(yytext));
}

<MACRO>"}}" |
<MACRO_PARAM>"}}" {
	EMIT_TEXT(TOK_CLOSE_MACRO, "");
	BEGIN(CONTENT);
}


<CONTENT>. {
	Token *top = TOP();
	if (top == NULL || top->id != TOK_TEXT)
	{
		EMIT_TEXT(TOK_TEXT, yytext);
	}
	else
	{
		top->value += yytext;
	}
}
