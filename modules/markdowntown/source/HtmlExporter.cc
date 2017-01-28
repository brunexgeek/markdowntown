#include <markdowntown/HtmlExporter.hh>


namespace markdowntown {


static void write(
	std::ostream &out,
	const Node &node );


static void writeChildren(
	std::ostream &out,
	const Node &node )
{
	for (const Node *current = node.first(); current != NULL; current = current->next())
		write(out, *current);
}


static void writeParagraph(
	std::ostream &out,
	const Node &node )
{
	out << "<p>";
	writeChildren(out, node);
	out << "</p>";
}


static void writeBlockQuote(
	std::ostream &out,
	const Node &node )
{
	out << "<blockquote>";
	writeChildren(out, node);
	out << "</blockquote>";
}


static void writeList(
	std::ostream &out,
	const Node &node )
{
	std::string tag;

	if (node.type == NTY_ORDERED_LIST)
		tag = "ol";
	else
		tag = "ul";

	const Node *previous = node.previous();
	//if (previous != NULL)
	//	std::cerr << "Previous is " << Node::name(previous->type) << " and current is " << Node::name(node.type) << std::endl;
	if (previous == NULL || previous->type != node.type)
		out << "<" << tag << ">";

	out << "<li>";
	writeChildren(out, node);
	out << "</li>";

	const Node *next = node.next();
	if (next == NULL || next->type != node.type)
		out << "</" << tag << ">";
}


static void write(
	std::ostream &out,
	const Node &node )
{
	switch (node.type)
	{
		case NTY_PARAGRAPH:
			writeParagraph(out, node);
			break;
		case NTY_BLOCKQUOTE:
			writeBlockQuote(out, node);
			break;
		case NTY_ORDERED_LIST:
		case NTY_UNORDERED_LIST:
			writeList(out, node);
			break;
		case NTY_BLOCK:
			writeChildren(out, node);
			break;
		case NTY_TEXT:
			out << node.text;
		default:
			std::cerr << "Unknown " << Node::name(node.type) << std::endl;
	}
}


HtmlExporter::HtmlExporter( )
{

}


HtmlExporter::~HtmlExporter()
{

}


void HtmlExporter::write(
	std::ostream &out,
	const Node &node ) const
{
	out << "<html><head></head><body>";
	markdowntown::write(out, node);
	out << "</body></html>";
}


}