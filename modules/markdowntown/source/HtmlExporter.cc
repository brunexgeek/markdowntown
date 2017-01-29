#include <markdowntown/HtmlExporter.hh>
#include <markdowntown/Macro.hh>
#include <sstream>


namespace markdowntown {


static const char *NEWLINE = "\n";


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
		out << "<strong>";
	else
	if (node.type == NTY_ITALIC)
		out << "<em>";
	else
	if (node.type == NTY_STRONG)
		out << "<strong><em>";

	writeChildren(out, node);

	if (node.type == NTY_BOLD)
		out << "</strong>";
	else
	if (node.type == NTY_ITALIC)
		out << "</em>";
	else
	if (node.type == NTY_STRONG)
		out << "</em></strong>";
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

	out << "</h" << node.counter << ">" << NEWLINE;
}


void HtmlExporter::writeParagraph(
	std::ostream &out,
	const Node &node )
{
	out << "<p>";
	writeChildren(out, node);
	out << "</p>" << NEWLINE;
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
		case NTY_RAW_TEXT:
			out << "<!-- RAW TEXT BEGIN -->" << NEWLINE;
			out << node.text;
			out << "<!-- RAW TEXT END -->" << NEWLINE;
			break;
		case NTY_EMPTY:
			break;
		case NTY_MACRO:
			if (node.first()->text == "TOC")
				writeTOC(out, node);
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
	findTitles(node);
	resetHeading();

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


static void captureText(
	const Node *node,
	std::string &text )
{
	if (node->type == NTY_TEXT)
		text += node->text;

	Node *current = node->first();
	while (current != NULL)
	{
		captureText(current, text);
		current = current->next();
	}
}


void HtmlExporter::findTitles(
	const Node &node )
{
	Node *current = node.first();
	while (current != NULL)
	{
		if (current->type == NTY_HEADING && current->first() != NULL)
		{
			incHeading(current->counter);

			// generate the heading ID
			std::stringstream id, value;
			id << "heading_";
			for (int i = 0; i < current->counter; ++i)
			{
				id << headings[i];
				value << headings[i] << '.';
				if (i + 1 < current->counter) id << "_";
			}

			std::string title;
			captureText(current, title);
			titles.push_back( HeadingEntry(title, value.str(), id.str(), current->counter) );
		}

		Node *item = current->first();
		while (item != NULL)
		{
			findTitles(*item);
			item = item->next();
		}

		current = current->next();
	}
}


void HtmlExporter::writeTOC(
	std::ostream &out,
	const Node &node )
{
	Macro macro(node);

	std::string title = macro.getParameter("title");
	if (title.empty()) title = "Summary";

	out << "<h1 class='toc'>" << title << "</h1><ul id='toc' class='toc'>";

	std::vector<HeadingEntry>::const_iterator it = titles.begin();
	for (; it != titles.end(); ++it)
	{
		for (int i = 1; i < it->level; ++i)
			out << "<ul class='level" << i << "'>";

		out << "<li><a href='#" << it->id << "'>";
		out << "<span class='numbering'>" << it->number << "</span>";
		out << it->title << "</a></li>";

		for (int i = 1; i < it->level; ++i)
			out << "</ul>";
	}

	out << "</ul>";
}


}