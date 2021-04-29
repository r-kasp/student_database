# ifndef condition_H
# define condition_H
enum class condition
{
	none, //not specified
	eq,
	//equal
	ne,
	//not equal
	lt,
	//less than
	gt,
	//greather than
	le,
	//less equal
	ge,
	//great equal
	like, //strings only: match pattern
	nlike,
};
# endif
