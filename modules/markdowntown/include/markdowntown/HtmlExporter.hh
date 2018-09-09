#ifndef MARKDOWNTOWN_HTMLEXPORTER_HH
#define MARKDOWNTOWN_HTMLEXPORTER_HH


#include <iostream>
#include <string>
#include <markdowntown/Node.hh>

namespace markdowntown {


struct HeadingEntry
{
	std::string title;
	std::string number;
	std::string id;
	int level;

	HeadingEntry(
		const std::string &title,
		const std::string &number,
		const std::string &id,
		int level = 1 ) : title(title), number(number),
			id(id), level(level)
	{
	}

};


class HtmlExporter
{
	public:
		HtmlExporter();

		~HtmlExporter();

		void write(
			std::ostream &out,
			const Node &node );

	private:
		int headings[6];
		std::vector<std::string> css;
		std::vector<HeadingEntry> titles;
		std::string charSet;

		void writeChildren(
			std::ostream &out,
			const Node &node );

		void writeFormat(
			std::ostream &out,
			const Node &node );

		void writeHeading(
			std::ostream &out,
			const Node &node );

		void writeParagraph(
			std::ostream &out,
			const Node &node );

		void writeBlockQuote(
			std::ostream &out,
			const Node &node );

		void writeList(
			std::ostream &out,
			const Node &node );

		void writeNode(
			std::ostream &out,
			const Node &node );

		void writeHyperlink(
			std::ostream &out,
			const Node &node );

		void incHeading(
			size_t level );

		void resetHeading();

		void findCSS(
			const Node &node );

		void findTitles(
			const Node &node );

		void writeTOC(
			std::ostream &out,
			const Node &node );

		void findCharSet(
			const Node &node );

};


std::ostream &operator<<(
	std::ostream &out,
	const HtmlExporter &object );


}

#endif // MARKDOWNTOWN_HTMLEXPORTER_HH