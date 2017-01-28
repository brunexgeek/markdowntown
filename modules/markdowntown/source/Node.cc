#include <markdowntown/Node.hh>
#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <vector>


namespace markdowntown {


using namespace std;


static const char * NODE_TYPES[] =
{
	"UNDEFINED",
    "PARAGRAPH",
    "TEXT",
    "BOLD",
    "ITALIC",
    "STRONG",
	"HEADING",
    "BLOCK",
	"BLOCKQUOTE",
	"LINE",
	"CODE",
	"RAW_TEXT",
	"UNORDERED_LIST",
	"ORDERED_LIST",
	"INLINE_URL",
	"CONTINUATION",
	"MACRO",
	"MACRO_IDENTIFIER",
	"MACRO_VALUE",
	"MACRO_PARAM",
	"MACRO_PARAM_LIST",
	"NULL"
};


Node::Node(
	int type,
	const char *text ) : type(type), counter(0), line(1), column(1), parent(NULL),
		prevNode(NULL), nextNode(NULL), children(NULL)
{
	if (text != NULL)
		this->text = text;
}


Node::Node(
	const Node &obj ) : type(0), counter(0), line(1), column(1), parent(NULL),
		prevNode(NULL), nextNode(NULL), children(NULL)
{
	// hidden constructor
}


Node::~Node()
{
	while (children != NULL)
	{
		Node *temp = children;
		children = children->next();
		delete temp;
	}
}


void Node::print(
	std::ostream &out,
	const char *(getTokenName)(int),
	int level,
	bool recursive ) const
{
	for (int i = 0; i < level; ++i)
		out << "   ";

	if (getTokenName == NULL)
		out << "["<< std::setw(3) << type << "] ";
	else
		out << "["<< getTokenName(type) << "] ";

	if (!text.empty())
		out << "'" << text << "' ";

	if (counter != 0)
		out << "(" << counter << ")";
	out << std::endl;

	if (!recursive) return;

	for (const Node *current = first(); current != NULL; current = current->next())
		current->print(out, getTokenName, level + 1);
}


Node *Node::find(
	int type )
{
	for (Node *current = children; current != NULL; current = current->next())
		if (current->type == type) return current;

	return NULL;
}

/*
void Node::setChild(
	int index,
	Node &value )
{
	if (index >= 0 && index < (int)children.size())
		children[index] = &value;
	value.parent = this;
}*/


void Node::remove(
	const Node *node )
{
	if (node == NULL) return;

	for (Node *current = children; current != NULL; current = current->nextNode)
	{
		if (current == node)
		{
			Node *prev = current->prevNode;
			Node *next = current->nextNode;

			if (prev != NULL)
				prev->nextNode = next;
			if (next != NULL)
				next->prevNode = prev;
			return;
		}
	}
/*
	if (index == -1)
	{
		for (size_t i = 0, n = children.size(); i < n; ++i)
			children[i]->parent = NULL;
		children.clear();
	}
	else
	if (index >= 0 && index < (int)children.size())
	{
		children[index]->parent = NULL;
		children.erase(children.begin() + index);
	}*/
}


void Node::append(
	Node *node)
{
	if (node == NULL) return;

	if (children == NULL)
	{
		children = node;
		node->prevNode = NULL;
	}
	else
	{
		Node *tail = children;
		while (tail->nextNode != NULL)
			tail = tail->nextNode;
		tail->nextNode = node;
		node->prevNode = tail;
	}

	node->nextNode = NULL;

	/*if (position < 0 || children.size() == 0 || position >= (int) children.size())
	{
		children.push_back(&value);
	}
	else
	{
		vector<Node*>::iterator it = children.begin();
		children.insert(it + position, &value);
	}
	value.parent = this;*/
}

/*
Node &Node::addChild(
	int type,
	const char *text )
{
	Node *node = new Node(type, text);
	addChild(*node);
	return *node;
}*/


size_t Node::size() const
{
	size_t size = 0;
	for (const Node *current = first(); current != NULL; current = current->next())
		++size;
	return size;
}

/*
Node *Node::operator[](
	size_t index )
{
	if (index < 0 || index >= children.size()) return NULL;
	return children[index];
}


const Node *Node::operator[](
	size_t index ) const
{
	if (index < 0 || index >= children.size()) return NULL;
	return children[index];
}*/


Node::operator std::string ()
{
	return text;
}


const char *Node::name(
	int nid )
{
	if (nid < 0 || nid > NTY_NULL) return "";
	return NODE_TYPES[nid];
}


void Node::operator <<(
	Node &source )
{
	if (source.children == NULL) return;

	if (children == NULL)
	{
		children = source.children;
	}
	else
	{
		Node *tail = children;
		while (tail->nextNode != NULL)
			tail = tail->nextNode;
		tail->nextNode = source.children;
		tail->nextNode->prevNode = tail;
	}

	source.children = NULL;

	/*std::vector<Node*>::iterator it = source.children.begin();
	for (; it != source.children.end(); ++it)
		children.push_back(*it);
	source.children.clear();*/
}


Node *Node::first()
{
	return children;
}


Node *Node::first() const
{
	return children;
}


Node *Node::previous()
{
	return prevNode;
}


Node *Node::previous() const
{
	return prevNode;
}


Node *Node::next()
{
	return nextNode;
}


Node *Node::next() const
{
	return nextNode;
}


} // namespace markdowntown