#ifndef MARKDOWNTOWN_TOKENLIST_HH
#define MARKDOWNTOWN_TOKENLIST_HH


#include "Token.hh"
#include <list>


namespace markdowntown {


class TokenList
{
	public:
		std::vector<Token*> entries;
		std::list<Token*> history; // list of all added tokens

		int index;

		TokenList() : index(-1)
		{
		}

		~TokenList()
		{
			std::list<Token*>::iterator it = history.begin();
			for (; it != history.end(); ++it)
				delete *it;
		}

		Token *getNext()
		{
			if (index < 0 || index >= (int) entries.size()) return NULL;
			return entries[index++];
		}

		/**
		 * Never use TokenList::add() inside this function!
		 */
		void process(
			const TokenList &object,
			TokenList &output )
		{
			output.reset();
			output.index = 0;

			int temp = index;
			index = 0;
			bool listMode = false;

			while (index < (int) entries.size())
			{
				if (index >= (int) object.entries.size())
				{
					output.entries.push_back( entries[index++] );
					continue;
				}

				const Token &current  = *entries[index];
				const Token &previous = *object.entries[index];

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
				int id = closeToken( *object.entries[i] );
				if (id >= 0)
				{
					Token *temp = new Token(id, "");
					output.entries.push_back(temp);
					history.push_back(temp);
				}
			}

			// output the remaining tokens from current
			for (int i = index; i < (int)entries.size(); ++i)
				output.entries.push_back( entries[i] );
			index = temp;
			std::cerr << "=====+--------+ " << output.entries.size() << std::endl;
		}

		void add( Token *token )
		{
			entries.push_back(token);
			history.push_back(token);
			if (index < 0) index = 0;
		}

		size_t size()
		{
			return entries.size();
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

		void swap(
			TokenList &object )
		{
			object.entries.swap(entries);
			int i = object.index;
			object.index = index;
			index = i;
		}

		Token * last()
		{
			if (entries.size() > 0)
				return entries[ entries.size() - 1 ];
			else
				return NULL;
		}

		/*
		 * Returns the closing token for statements whose do not have
		 * an explicit closing representation.
		 */
		int closeToken(
			const Token &token )
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


struct LexerContext
{
	Token *tokens; // keep a list of all allocated tokens
	bool statements[4];
	TokenList pending;
	TokenList currentTokens;
	TokenList previousTokens;

	LexerContext()
	{
		for (size_t i = 0; i < sizeof(statements) / sizeof(bool); ++i)
			statements[i] = false;
	}
};


}


#endif // MARKDOWNTOWN_TOKENLIST_HH