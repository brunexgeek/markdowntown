#ifndef MARKDOWNTOWN_NODE_HH
#define MARKDOWNTOWN_NODE_HH


#include <vector>
#include <string>
#include <stdint.h>

namespace markdowntown {


enum NodeType
{
	NTY_UNDEFINED,
	NTY_PARAGRAPH,
	NTY_TEXT,
	NTY_BOLD,
	NTY_ITALIC,
	NTY_STRONG,
	NTY_HEADING,
	NTY_BLOCK,
	NTY_BLOCKQUOTE,
	NTY_LINE,
	NTY_CODE,
	NTY_RAW_TEXT,
	NTY_UNORDERED_LIST,
	NTY_ORDERED_LIST,
	NTY_INLINE_URL,
	NTY_CONTINUATION,
	NTY_MACRO,
	NTY_MACRO_IDENTIFIER,
	NTY_MACRO_VALUE,
	NTY_MACRO_PARAM,
	NTY_MACRO_PARAM_LIST,
	NTY_EMPTY,
	NTY_NULL
};


class Node
{
	public:
		std::string text;
		int type;
		int counter;
		uint32_t line;
		uint32_t column;
		Node *parent;

		Node(
			int type,
			const char *text = NULL);

		virtual ~Node();

		void remove(
			const Node *node );

		void append(
			Node *node );

		size_t size() const;

		void print(
			std::ostream &out,
			const char *(getTokenName)(int) = NULL,
			int level = 0,
			bool recursive = true ) const;

		Node *find(
			int type );

		operator std::string();

		static const char *name(
			int nid );

		void operator <<(
			Node &source );

		Node *first();

		Node *first() const;

		Node *previous();

		Node *previous() const;

		Node *next();

		Node *next() const;

	private:
		//std::vector<Node*> children;
		Node *prevNode, *nextNode, *children;

		Node(
			const Node &obj );
};

}


#endif // MARKDOWNTOWN_NODE_HH