#include <markdowntown/HtmlExporter.hh>


namespace markdowntown {


HtmlExporter::HtmlExporter( )
{

}


HtmlExporter::~HtmlExporter()
{

}


void HtmlExporter::writeChildren(
	std::ostream &out,
	const Node &node ) const
{
	for (const Node *current = node.first(); current != NULL; current = current->next())
		writeNode(out, *current);
}


void HtmlExporter::writeFormat(
	std::ostream &out,
	const Node &node ) const
{
	std::string tag;

	if (node.type == NTY_BOLD)
		tag = "strong";
	else
	if (node.type == NTY_ITALIC)
		tag = "em";
	else
		tag = "strong";

	out << "<" << tag << ">";
	writeChildren(out, node);
	out << "</" << tag << ">";
}


void HtmlExporter::writeHeading(
	std::ostream &out,
	const Node &node ) const
{
	out << "<h" << node.counter << ">";
	writeChildren(out, node);
	out << "</h" << node.counter << ">";
}


void HtmlExporter::writeParagraph(
	std::ostream &out,
	const Node &node ) const
{
	out << "<p>";
	writeChildren(out, node);
	out << "</p>";
}


void HtmlExporter::writeBlockQuote(
	std::ostream &out,
	const Node &node ) const
{
	out << "<blockquote class='type" << node.counter << "'>";
	writeChildren(out, node);
	out << "</blockquote>";
}


void HtmlExporter::writeList(
	std::ostream &out,
	const Node &node ) const
{
	std::string tag;

	if (node.type == NTY_ORDERED_LIST)
		tag = "ol";
	else
		tag = "ul";

	const Node *previous = node.previous();
	if (previous == NULL || previous->type != node.type)
		out << "<" << tag << ">";

	out << "<li>";
	writeChildren(out, node);
	out << "</li>";

	const Node *next = node.next();
	if (next == NULL || next->type != node.type)
		out << "</" << tag << ">";
}


void HtmlExporter::writeNode(
	std::ostream &out,
	const Node &node ) const
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
			break;
		case NTY_CODE:
			out << "<code class='type" << node.counter << "'>" << node.text << "</code>";
			break;
		case NTY_HEADING:
			writeHeading(out, node);
			break;
		case NTY_BOLD:
		case NTY_ITALIC:
		case NTY_STRONG:
			writeFormat(out, node);
			break;
		case NTY_EMPTY:
		case NTY_MACRO:
			break;
		default:
			std::cerr << "Unknown node '" << Node::name(node.type) << '\'' << std::endl;
	}

}


void HtmlExporter::write(
	std::ostream &out,
	const Node &node ) const
{
	out << "<html><head></head><body>";
	writeNode(out, node);
	out << "</body></html>";
}


}