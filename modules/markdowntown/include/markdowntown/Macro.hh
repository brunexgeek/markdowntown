#ifndef MARKDOWNTOWN_MACRO_HH
#define MARKDOWNTOWN_MACRO_HH


#include <map>
#include <string>
#include <markdowntown/Node.hh>


namespace markdowntown {


class Macro
{
	public:
		Macro(
			const Node &node )
		{
			typedef std::pair< std::string, std::string > MacroEntry;

			name = node.first()->text;

			Node *current = node.first()->next()->first();
			while (current != NULL)
			{
				params.insert( MacroEntry(current->first()->text, current->first()->next()->text) );
				current = current->next();
			}

		}

		~Macro() {}

		std::string getName() const;

		std::string getParameter(
			const std::string &name ) const
		{
			std::map< std::string, std::string >::const_iterator it = params.find(name);
			if (it != params.end())
				return it->second;
			else
				return "";
		}

	private:
		std::string name;
		std::map< std::string, std::string > params;
};


}



#endif // MARKDOWNTOWN_MACRO_HH