#ifndef MARKDOWNTOWN_HTMLEXPORTER_HH
#define MARKDOWNTOWN_HTMLEXPORTER_HH


#include <iostream>
#include <string>
#include <markdowntown/Node.hh>

namespace markdowntown {


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

};


std::ostream &operator<<(
	std::ostream &out,
	const HtmlExporter &object );


}

#endif // MARKDOWNTOWN_HTMLEXPORTER_HH