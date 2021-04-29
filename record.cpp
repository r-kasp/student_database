# include <string.h>
# include <stdio.h>
# include "record.h"
# define LEN 1234
using namespace std;


int record::init (const char *n, int p, int g)
{
	phone = p;
	group = g;
	if (n)
	{
		name = std::make_unique<char []> (strlen (n) + 1);
		if (!name) return -1;
		strcpy (name.get(), n);
	}
	else
		name = nullptr;
	return 0;
}


read_status record::read (FILE *fp)
{
	char buf[LEN];
	name = nullptr;
	if (fscanf (fp, "%s%d%d", buf, &phone, &group) != 3)
	{
		if (feof(fp)) return read_status::eof;
			return read_status::format;
	}
	if (init (buf, phone, group))
		return read_status::memory;
	return read_status::success;
}

void record::print (const ordering order[], FILE *fp)
{
	const int max_items = 3;
	const ordering default_ordering[max_items] = {ordering::name, ordering::phone, ordering::group};
	const ordering * p = (order[0] != ordering::none ? order : default_ordering);
	for (int i = 0; i < max_items; i++)
		switch (p[i])
		{
			case ordering::name:
				printf (" %s", name.get()); break;
			case ordering::phone:
				printf (" %d", phone); break;
			case ordering::group:
				printf (" %d", group); break;
			case ordering::none:
				continue;
		}
	fprintf (fp, "\n");
}


bool record::compare (const ordering order_by[], const record & y)
{
	const int max_items = 3;
	const ordering default_ordering[max_items] = {ordering::name, ordering::phone, ordering::group};
	const ordering * p = (order_by[0] != ordering::none ? order_by : default_ordering);
	for (int i = 0; i < max_items; i++)
		switch (p[i])
		{
			case ordering::name:
			{
				int cmp = strcmp(name.get(), y.name.get());
				if (cmp == 0)
					break;
				if (cmp < 0)
					return true;
				return false;
			}
			case ordering::phone:
			{
				if (phone == y.phone)
					break;
				if (phone < y.phone)
					return true;
				return false;
			}
			case ordering::group:
			{
				if (group == y.group)
					break;
				if (group < y.group)
					return true;
				return false;
			}
			case ordering::none:
				continue;
		}
	return false;
}


bool record::compare_phone (condition x, const record& y) const
{
	switch (x)
	{
		case condition::none: // not specified
			return true; // unspecified opeation is true
		case condition::eq:
		// equal
			return phone == y.phone;
		case condition::ne:
		// not equal
			return phone != y.phone;
		case condition::lt:
		// less than
			return phone > y.phone;
		case condition::gt:
		// less than
			return phone < y.phone;
		case condition::le:
		// less equal
			return phone >= y.phone;
		case condition::ge:
		// great equal
			return phone <= y.phone;
		case condition::like: // strings only: match pattern
			return false; // cannot be used for phone
		case condition::nlike:
			return false;
	}
	return false;
}


bool record::compare_name (condition x, const record& y) const
{
	int cmp = strcmp(name.get(), y.name.get());
	switch (x)
	{
		case condition::none: // not specified
			return true; // unspecified opeation is true
		case condition::eq:
		// equal
			return cmp == 0;
		case condition::ne:
		// not equal
			return cmp != 0;
		case condition::lt:
		// less than
			return cmp > 0;
		case condition::gt:
		// less than
			return cmp < 0;
		case condition::le:
		// less equal
			return cmp >= 0;
		case condition::ge:
		// great equal
			return cmp <= 0;
		case condition::like: 
			return false; 
		case condition::nlike:
			return false;
	}
	return false;
}


bool record::compare_group (condition x, const record& y) const
{
	switch (x)
	{
		case condition::none: // not specified
			return true; // unspecified opeation is true
		case condition::eq:
		// equal
			return group == y.group;
		case condition::ne:
		// not equal
			return group != y.group;
		case condition::lt:
		// less than
			return group > y.group;
		case condition::gt:
		// great than
			return group < y.group;
		case condition::le:
		// less equal
			return group >= y.group;
		case condition::ge:
		// great equal
			return group <= y.group;
		case condition::like:
			return false; 
		case condition::nlike:
			return false;
	}
	return false;
}
