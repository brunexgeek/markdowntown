%{

#include <string>
#include <cstdio>
#include <sstream>
#include <vector>
#include <fstream>
#include <iostream>
#include "markdowntown.y.hh"


enum StatementType
{
	STATE_BOLD,
	STATE_ITALIC,
	STATE_STRONG,
	STATE_CODE
};


static bool statements[4] = { false };


class Token
{
	public:
		int id;
		std::string value;

		Token(int id, const std::string &value)
		{
			this->id = id;
			this->value = value;
		}
};


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

			while (index < (int) entries.size())
			{
				if (index >= (int) object.entries.size())
				{
					output.add( entries[index++] );
					continue;
				}

				const Token &current  = entries[index];
				const Token &previous = object.entries[index];

//std::cout << markdowntown_get_token_name(current.id) << " != " << markdowntown_get_token_name(previous.id) << "?\n";

				if (current.id != previous.id)
					break;

				if (current.id == TOK_OPEN_PARAGRAPH || previous.id == TOK_OPEN_PARAGRAPH)
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
				default:
					return -1;
			}
		}

};


static TokenList pending;

static TokenList currentTokens;

static TokenList previousTokens;


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

#define EMIT_TEXT(id, text) \
	{                                                           \
		currentTokens.add( Token((id), text ) );  \
	}

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
OWS                     {WS}*

UNORDERED_LIST          "*"+" "
ORDERED_LIST            "#"+" "
HEADING                 "="+" "
BLOCKQUOTE              ">"+" "

TEXT                    [^=\[\]\{\};:'`\2\n]+
URL                     [^ \n\t()\[\]\{\}'"]+

ITALIC                  "''"
BOLD                    "'''"
STRONG                  "'''''"

%x PENDING
%x OPEN
%x CONTENT
%x URL

%%


{BOL}{OWS}/{EOL} {
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
		yylval->node = strdup(token->value.c_str());
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
<OPEN>{BLOCKQUOTE} {
	EMIT(TOK_OPEN_BLOCKQUOTE);
}


 /*
  * Begin of heading.
  */
<OPEN>{HEADING} {
	BEGIN(CONTENT);
	EMIT(TOK_OPEN_HEADING);
}


<OPEN>{OWS}"---""-"+{OWS}/{EOL} {
	EMIT(TOK_LINE);
	pending = currentTokens.process(previousTokens);
	BEGIN(PENDING);
}


<OPEN>{UNORDERED_LIST} {
	if (yytext != NULL)
	{
		for (size_t i = 0; yytext[i] == '*'; ++i)
			EMIT(TOK_OPEN_UNORDERED_LIST);
	}
}


<OPEN>{ORDERED_LIST} {
	if (yytext != NULL)
	{
		for (size_t i = 0; yytext[i] == '#'; ++i)
			EMIT(TOK_OPEN_ORDERED_LIST);
	}
}


 /*
  * Begin of non-empty line (paragraph).
  */
<OPEN>. {
	unput(yytext[0]);
	BEGIN(CONTENT);
	EMIT(TOK_OPEN_PARAGRAPH);
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
		yylval->node = strdup(token->value.c_str());
		return token->id;
	}
	else
		yyterminate();
}


<OPEN>{EOL} |
<CONTENT>{EOL} {
	unput('\n');
	pending = currentTokens.process(previousTokens);
	BEGIN(PENDING);
}


<CONTENT>"["{OWS}/{TEXT}{OWS}"]"{OWS}"("{OWS}{URL}{OWS}")" {
	EMIT(TOK_OPEN_URL);
	BEGIN(URL);
}

<URL>{TEXT}/{OWS}"]" {
	EMIT(TOK_TEXT);
}

<URL>{URL}/{OWS}")" {
	EMIT(TOK_TEXT);
}

<URL>")" {
	EMIT(TOK_CLOSE_URL);
	BEGIN(CONTENT);
}


<CONTENT>{STRONG}		{ EMIT_TOGGLE(statements[STATE_STRONG], TOK_OPEN_STRONG, TOK_CLOSE_STRONG ); }
<CONTENT>{BOLD}			{ EMIT_TOGGLE(statements[STATE_BOLD],   TOK_OPEN_BOLD,   TOK_CLOSE_BOLD   ); }
<CONTENT>{ITALIC}		{ EMIT_TOGGLE(statements[STATE_ITALIC], TOK_OPEN_ITALIC, TOK_CLOSE_ITALIC ); }


<CONTENT>"="+{OWS}/{EOL} {
	/* ignores the heading close */
}

<CONTENT>"`"[^`]*"`"						{ EMIT(TOK_CODE); }
<CONTENT>"{{{" {
	int count = 0;
	int current = 0;
	std::string content;

	while ((current = yyinput(yyscanner)) != EOF)
	{
		if (current == '\2' || current == '\n') continue;

		if (current == '}')
		{
			if (++count == 3)
			{
				EMIT_TEXT(TOK_RAW_TEXT, content.c_str());
			}
		}
		else
		{
			count = 0;
			content += (char) current;
		}
	}
}

<CONTENT>{TEXT}                          { EMIT(TOK_TEXT); }

<CONTENT>.                               { EMIT(TOK_TEXT); }

