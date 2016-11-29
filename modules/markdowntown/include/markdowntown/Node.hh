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
		/*Node(
			int type,
			const char *value,
			Node *parent,
			int childNo,
			... );*/

		Node(
			const Node &obj );

		virtual ~Node();

		void setChild(
			int index,
			Node &value );

		void removeChild(
			int index = -1 );

		void addChild(
			Node &value,
			int position = -1 );

		Node &addChild(
			int type,
			const char *text );

		int getChildCount() const;

		Node &operator[](
			int index );

		const Node &operator[](
			int index ) const;

		void print(
			std::ostream &out,
			const char *(getTokenName)(int) = NULL,
			int level = 0,
			bool recursive = true );

		bool hasChild(
			int modifier );

		operator std::string();

		static const char *name(
			int nid );

	private:
		std::vector<Node*> children;
};

}


#endif // MARKDOWNTOWN_NODE_HH