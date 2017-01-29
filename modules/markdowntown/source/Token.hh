#ifndef TOKEN_HH
#define TOKEN_HH

class Token
{
	public:
		int id;
		int counter;
		std::string value;

		Token(
			int id,
			const std::string &value) : id(id), counter(0), value(value)
		{
		}
};


#endif // TOKEN_HH