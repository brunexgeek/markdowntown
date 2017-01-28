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
			const Node &node ) const;
};


std::ostream &operator<<(
	std::ostream &out,
	const HtmlExporter &object );


}

#endif // MARKDOWNTOWN_HTMLEXPORTER_HH