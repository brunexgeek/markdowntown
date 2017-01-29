#ifndef MARKDOWNTOWN_MACRO_HH
#define MARKDOWNTOWN_MACRO_HH


#include <map>
#include <string>
#include <markdowntown/Node.hh>


namespace markdowntown {


struct MacroParameter
{
	std::string identifier;
	std::string value;
};


class Macro
{
	public:
		Macro(
			const Node &node )
		{
			name = node.first()->text;

			Node *current = node.first()->next()->first();
			while (current != NULL)
			{
				MacroParameter entry;
				entry.identifier = current->first()->text;
				entry.value = current->first()->next()->text;

				params.push_back(entry);
				current = current->next();
			}

		}

		~Macro() {}

		std::string getName() const
		{
			return name;
		}

		std::string getParameter(
			int index ) const
		{
			if (index < 0 || index >= (int) params.size())
				return "";
			return params[index].value;
		}

		std::string getParameter(
			const std::string &name,
			int index = -1 ) const
		{
			// FIXME: this isn't efficient, but we need an ordered and indexable structure

			std::vector<MacroParameter>::const_iterator it = params.begin();
			for (; it != params.end(); ++it)
			{
				if (it->identifier == name)
					return it->value;
			}

			if (index >= 0)
				return getParameter(index);
			else
				return "";
		}

	private:
		std::string name;
		std::vector<MacroParameter> params;
};


}



#endif // MARKDOWNTOWN_MACRO_HH