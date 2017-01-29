#include <markdowntown/HtmlExporter.hh>
#include <markdowntown/Macro.hh>
#include <sstream>


namespace markdowntown {


HtmlExporter::HtmlExporter( )
{
	resetHeading();
}


HtmlExporter::~HtmlExporter()
{

}


void HtmlExporter::incHeading(
	size_t level )
{
	#define MAX_LEVEL (sizeof(headings) / sizeof(int))

	if (level < 1 || level > MAX_LEVEL) return;

	for (size_t i = 0; i < level - 1; ++i)
		if (headings[i] == 0) headings[i] = 1;

	++headings[level - 1];

	for (size_t i = level; i < MAX_LEVEL; ++i)
		headings[i] = 0;
}


void HtmlExporter::resetHeading()
{
	for (size_t i = 0; i < (sizeof(headings) / sizeof(int)); ++i)
		headings[i] = 0;
}


void HtmlExporter::writeChildren(
	std::ostream &out,
	const Node &node )
{
	for (const Node *current = node.first(); current != NULL; current = current->next())
		writeNode(out, *current);
}


void HtmlExporter::writeHyperlink(
	std::ostream &out,
	const Node &node )
{
	out << "<a href='";

	out << node.first()->next()->text << "'>";
	out << node.first()->text << "</a>";
}


void HtmlExporter::writeFormat(
	std::ostream &out,
	const Node &node )
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
	const Node &node )
{
	incHeading(node.counter);

	// generate the heading ID
	std::stringstream id;
	id << "heading_";
	for (int i = 0; i < node.counter; ++i)
	{
		id << headings[i];
		if (i + 1 < node.counter) id << "_";
	}

	out << "<h" << node.counter << " id='" << id.str();
	out << "'><span class='numbering'>";
	for (int i = 0; i < node.counter; ++i)
		out << headings[i] << ".";
	out << "</span>";

	writeChildren(out, node);

	out << "</h" << node.counter << ">";
}


void HtmlExporter::writeParagraph(
	std::ostream &out,
	const Node &node )
{
	out << "<p>";
	writeChildren(out, node);
	out << "</p>";
}


void HtmlExporter::writeBlockQuote(
	std::ostream &out,
	const Node &node )
{
	out << "<blockquote class='type" << node.counter << "'>";
	writeChildren(out, node);
	out << "</blockquote>";
}


void HtmlExporter::writeList(
	std::ostream &out,
	const Node &node )
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
		case NTY_INLINE_URL:
			writeHyperlink(out, node);
		case NTY_EMPTY:
		case NTY_MACRO:
			break;
		default:
			std::cerr << "Unknown node '" << Node::name(node.type) << '\'' << std::endl;
	}

}


void HtmlExporter::write(
	std::ostream &out,
	const Node &node )
{
	findCSS(node);

	out << "<html><head>";
	std::vector<std::string>::const_iterator it = css.begin();
	for (; it != css.end(); ++it)
		out << "<link rel='stylesheet' type='text/css' href='" << *it << "'/>";
	out << "</head><body>";
	writeNode(out, node);
	out << "</body></html>";
}


void HtmlExporter::findCSS(
	const Node &node )
{
	Node *current = node.first();
	while (current != NULL)
	{
		if (current->type == NTY_PARAGRAPH && current->first() != NULL)
		{
			Node *item = current->first();
			if (item->type == NTY_MACRO && item->first()->text == "StyleSheet")
			{
				Macro macro(*item);
				css.push_back( macro.getParameter("href") );
			}
		}
		current = current->next();
	}
}


}