%{

#include <string>
#include <cstdio>
#include <sstream>
#include <vector>
#include <fstream>
#include <iostream>
#include "markdowntown.y.hh"
#include "TokenList.hh"


using namespace markdowntown;


enum StatementType
{
	STATE_BOLD,
	STATE_ITALIC,
	STATE_STRONG,
	STATE_CODE
};


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


#define CONTEXT() ( (LexerContext*) markdowntown_get_extra(yyscanner) )

#define EMIT(id) \
	{                                                                                  \
		CONTEXT()->currentTokens.add( Token((id), markdowntown_getTokenText( (id), yytext ) ) );  \
	}

#define EMIT_TEXT(id, text)                            \
	{                                                  \
		CONTEXT()->currentTokens.add( new Token( (id), (text) ) );    \
	}

#define TOP() ( CONTEXT()->currentTokens.last() )

#define EMIT_TOGGLE(variable, tokTrue, tokFalse) \
	{ \
		variable = !variable; \
		int id = (variable) ? tokTrue : tokFalse; \
		CONTEXT()->currentTokens.add( new Token((id), "none" ) ); \
	}

#define ECHO

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-compare"

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
	CONTEXT()->currentTokens.process(
		CONTEXT()->previousTokens,
		CONTEXT()->pending);
	BEGIN(PENDING);
}


{BOL} {
	BEGIN(OPEN);
}


<PENDING>{EOL} {
	Token *token = CONTEXT()->pending.getNext();
	if (token != NULL)
	{
		unput('\n');
		yylval->token = token;
		return token->id;
	}
	CONTEXT()->previousTokens.reset();
	CONTEXT()->currentTokens.swap(CONTEXT()->previousTokens);
	CONTEXT()->pending.reset();
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
	CONTEXT()->currentTokens.process(
		CONTEXT()->previousTokens,
		CONTEXT()->pending);
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
	if (!CONTEXT()->pending.hasNext())
	{
		CONTEXT()->currentTokens.process(
			CONTEXT()->previousTokens,
			CONTEXT()->pending);
		// clean up things to avoid an infinity loop here
		CONTEXT()->previousTokens.reset();
		CONTEXT()->currentTokens.reset();
	}

	Token *token = CONTEXT()->pending.getNext();
	if (token != NULL)
	{
		yylval->token = token;
		return token->id;
	}
	else
		yyterminate();
}


<OPEN,CONTENT>{EOL} {
	unput('\n');
	CONTEXT()->currentTokens.process(
		CONTEXT()->previousTokens,
		CONTEXT()->pending);
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


<CONTENT>{STRONG}		{ EMIT_TOGGLE(CONTEXT()->statements[STATE_STRONG], TOK_OPEN_STRONG, TOK_CLOSE_STRONG ); }
<CONTENT>{BOLD}			{ EMIT_TOGGLE(CONTEXT()->statements[STATE_BOLD],   TOK_OPEN_BOLD,   TOK_CLOSE_BOLD   ); }
<CONTENT>{ITALIC}		{ EMIT_TOGGLE(CONTEXT()->statements[STATE_ITALIC], TOK_OPEN_ITALIC, TOK_CLOSE_ITALIC ); }


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

<MACRO_PARAM>[^|}]+ {
	std::string content = yytext;
	size_t pos = content.find_first_of("=");
	if (pos == std::string::npos)
	{
		EMIT_TEXT(TOK_MACRO_IDENTIFIER, "");
		EMIT_TEXT(TOK_MACRO_VALUE, markdowntown_trim(content));
	}
	else
	{
		std::string name = content.substr(0, pos);
		if (pos == content.length() - 1)
			content = "";
		else
			content = content.substr(pos + 1);

		EMIT_TEXT(TOK_MACRO_IDENTIFIER, name);
		EMIT_TEXT(TOK_MACRO_VALUE, content);
	}
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
