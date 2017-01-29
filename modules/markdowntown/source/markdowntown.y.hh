/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_MARKDOWNTOWN_MEDIA_DADOS_PROJETOS_MARKDOWNTOWN_MODULES_MARKDOWNTOWN_SOURCE_MARKDOWNTOWN_Y_HH_INCLUDED
# define YY_MARKDOWNTOWN_MEDIA_DADOS_PROJETOS_MARKDOWNTOWN_MODULES_MARKDOWNTOWN_SOURCE_MARKDOWNTOWN_Y_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int markdowntown_debug;
#endif
/* "%code requires" blocks.  */
#line 8 "/media/dados/projetos/markdowntown/modules/markdowntown/source/markdowntown.y" /* yacc.c:1909  */



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



#line 90 "/media/dados/projetos/markdowntown/modules/markdowntown/source/markdowntown.y.hh" /* yacc.c:1909  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TOK_CODE = 258,
    TOK_TEXT = 259,
    TOK_HASHTAG = 260,
    TOK_SM = 261,
    TOK_COLON = 262,
    TOK_BOL = 263,
    TOK_OPEN_BLOCKQUOTE = 264,
    TOK_OPEN_PARAGRAPH = 265,
    TOK_OPEN_HEADING = 266,
    TOK_OPEN_BOLD = 267,
    TOK_OPEN_ITALIC = 268,
    TOK_OPEN_STRONG = 269,
    TOK_OPEN_UNORDERED_LIST = 270,
    TOK_OPEN_ORDERED_LIST = 271,
    TOK_OPEN_URL = 272,
    TOK_OPEN_CONTINUATION = 273,
    TOK_OPEN_MACRO = 274,
    TOK_CLOSE_BLOCKQUOTE = 275,
    TOK_CLOSE_PARAGRAPH = 276,
    TOK_CLOSE_HEADING = 277,
    TOK_CLOSE_BOLD = 278,
    TOK_CLOSE_ITALIC = 279,
    TOK_CLOSE_STRONG = 280,
    TOK_CLOSE_UNORDERED_LIST = 281,
    TOK_CLOSE_ORDERED_LIST = 282,
    TOK_CLOSE_URL = 283,
    TOK_CLOSE_CONTINUATION = 284,
    TOK_CLOSE_MACRO = 285,
    TOK_LINE = 286,
    TOK_RAW_TEXT = 287,
    TOK_MACRO = 288,
    TOK_MACRO_IDENTIFIER = 289,
    TOK_MACRO_VALUE = 290,
    TOK_MACRO_PIPE = 291,
    TOK_EMPTY = 292
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 204 "/media/dados/projetos/markdowntown/modules/markdowntown/source/markdowntown.y" /* yacc.c:1909  */

	Token *token;

#line 144 "/media/dados/projetos/markdowntown/modules/markdowntown/source/markdowntown.y.hh" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int markdowntown_parse (parser_context_t *parserContext);

#endif /* !YY_MARKDOWNTOWN_MEDIA_DADOS_PROJETOS_MARKDOWNTOWN_MODULES_MARKDOWNTOWN_SOURCE_MARKDOWNTOWN_Y_HH_INCLUDED  */
