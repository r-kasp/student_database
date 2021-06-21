#include "command.h"


condition command::detect_condition(const char * str)
{
	if (str[0] != '\0' && str[1] != '\0')
	{
		if (str[2] == '\0')
		{
			if (str[0] == '<' && str[1] == '=')
				return condition::le;
			if (str[0] == '>' && str[1] == '=')
				return condition::ge;
			if (str[0] == '<' && str[1] == '>')
				return condition::ne;
		}
		else
		{
			if (str[0] == 'l')
				return condition::like;
			else
				return condition::nlike;
		}
	}
	else
	{
		if (str[0] == '>')
			return condition::gt;
		if (str[0] == '<')
			return condition::lt;
		if (str[0] == '=')
			return condition::eq;
	}
	return condition::none;
}

int command::parse_search_condition(const char * string)
{
	int ind = 0;
	int ret = 0;
	char word1[LEN], word2[LEN], word3[LEN];
	while (string[ind] != '\0' && string[ind] != '\n' && string[ind] != ' ')
		ind++;
	if (ind == 0)
		return -1;
	strncpy(word1, string, ind);
	if (word1[ind] != '\0')
		word1[ind] = '\0';
	while (string[ind] == ' ')
		ind++;
	string += ind;
		
	ret += ind;
	ind = 0;
	while (string[ind] != '\0' && string[ind] != '\n' && string[ind] != ' ')
		ind++;
	if (ind == 0)
		return -1;
	strncpy(word2, string, ind);
	if (word2[ind] != '\0')
		word2[ind] = '\0';
	while (string[ind] == ' ')
		ind++;
	string += ind;
	condition cond = detect_condition(word2);
	
	if (cond == condition::nlike)
	{
		ret += ind;
		ind = 0;
		while (string[ind] != '\0' && string[ind] != '\n' && string[ind] != ' ')
			ind++;
		if (ind == 0)
			return -1;
		strncpy(word2, string, ind);
		if (word2[ind] != '\0')
			word2[ind] = '\0';
		while (string[ind] == ' ')
			ind++;
		string += ind;
		if (strcmp(word2, "like") != 0)
			return -1;
	}
		
		
		
	ret += ind;
	ind = 0;
	while (string[ind] != '\0' && string[ind] != '\n' && string[ind] != ' ')
		ind++;
	if (ind == 0)
		return -1;
	strncpy(word3, string, ind);
	if (word3[ind] != '\0')
		word3[ind] = '\0';
	while (string[ind] == ' ')
		ind++;
	
	//if (string[ind] != '\0' && string[ind] != '\n')
	//	return false;
	ret += ind;

	if (strcmp(word1, "phone") == 0)
	{
		c_phone = cond;
		phone = std::stoi(word3);
	}
	else if (strcmp(word1, "name") == 0)
	{
		c_name = cond;
		name = std::make_unique<char []> (strlen(word3) + 1);
		if (!name) 
			return -1;
		strcpy(name.get(), word3);
	}
	else if (strcmp(word1, "group") == 0)
	{	
		c_group = cond;
		group = std::stoi(word3);
	}
	else
		return -1;

	return ret;	
}


int command::get_word(const char * string) const
{
	int i = 0;
	while (string[i] != ' ' && string[i] != '\n' && string[i] != '\t' && string[i] != '\0')
		i++;
	return i;
}


bool command::detect_order(char *buf, int number)
{
		if (strcmp(buf, "name") == 0)
			order[number-1] = ordering::name;
		else if (strcmp(buf, "phone") == 0)
			order[number-1] = ordering::phone;
		else if (strcmp(buf, "group") == 0)
			order[number-1] = ordering::group;
		else 
			return false;
		return true;
}


bool command::detect_order_by(char *buf, int number)
{
		if (strcmp(buf, "name") == 0)
			order_by[number-1] = ordering::name;
		else if (strcmp(buf, "phone") == 0)
			order_by[number-1] = ordering::phone;
		else if (strcmp(buf, "group") == 0)
			order_by[number-1] = ordering::group;
		else 
			return false;
		return true;
}


int command::skip_spaces(const char *string) const
{
	int i = 0;
	while (string[i] == ' ' || string[i] == '\t' || string[i] == '\n')
		i++;
	return i;
}

int command::parse_print_condition(const char * string)
{
	int ind = 0, plus = 0;
	char buf[LEN];
	bool is_next_word_where = false;
	if (type == command_type::select)
	{
		//first word
		if (string[ind] == '*')
		{
			ind++;
		}
		else
		{
			plus = get_word(string+ind);
			if (plus != 0 && string[plus-1] == ',')
			{
				strncpy(buf, string, plus-1);
				buf[plus-1] = '\0';
			}
			else
			{
				strncpy(buf, string, plus);
				buf[plus] = '\0';
			}
			if (!detect_order(buf, 1))
				return -1;
		}
		
		ind += plus;
		ind += skip_spaces(string+ind);
		
		if (string[ind] == '\0')
			return ind;
		
		//second word
		plus = get_word(string+ind);
		if (string[ind+plus-1] == ',')
		{
			strncpy(buf, string+ind, plus-1);
			buf[plus-1] = '\0';
		}
		else
		{
			strncpy(buf, string+ind, plus);
			buf[plus] = '\0';
		}
		if (strcmp(buf, "where") != 0)
		{	
			if (strcmp(buf, "order") == 0)
				return ind;
			if (!detect_order(buf, 2))
				return -1;
			ind += plus;
			ind += skip_spaces(string+ind);
			
			if (string[ind] == '\0')
				return ind;
			
			//third word
			plus = get_word(string+ind);
			strncpy(buf, string+ind, plus);
			buf[plus] = '\0';
			if (strcmp(buf, "where") != 0)
			{
				if (strcmp(buf, "order") == 0)
					return ind;
				if (!detect_order(buf, 3))
					return -1;
				ind += plus;
				ind += skip_spaces(string+ind);
				
				if (string[ind] == '\0')
					return ind;
			}
			else 
				is_next_word_where = true;
		}
		else
			is_next_word_where = true;
	}
	if (!is_next_word_where)
	{
		plus = get_word(string+ind);
		strncpy(buf, string+ind, plus);
		buf[plus] = '\0';
		if (strcmp(buf, "where") != 0)
			return -1;
	}
	ind += plus;
	ind += skip_spaces(string+ind);
	
	//now the requests
	//first
	plus = parse_search_condition(string+ind);
	if (plus < 0)
		return -1;
	
	ind += plus;
	ind += skip_spaces(string + ind);
	
	
	if (string[ind] == '\0' || string[ind] == '\n')
		return ind;
	
	//operation
	plus = get_word(string+ind);
	strncpy(buf, string+ind, plus);
	buf[plus] = '\0';
	
	if (strcmp(buf, "order") == 0)
		return ind;
	else if (strcmp(buf, "or") == 0)
		op = operation::lor;
	else if (strcmp(buf, "and") == 0)
		op = operation::land;
	else
		return -1;
	
	ind += plus;
	ind += skip_spaces(string+ind);
	
	
	//second word
	plus = parse_search_condition(string+ind);
	if (plus < 0)
		return -1;
		
	ind += plus;
	ind += skip_spaces(string + ind);

	if (string[ind] == '\0' || string[ind] == '\n')
		return ind;
		
	//operation
	plus = get_word(string+ind);
	strncpy(buf, string+ind, plus);
	buf[plus] = '\0';
	
	if (strcmp(buf, "order") == 0)
		return ind;
	
	if (strcmp(buf, "or") == 0 && op != operation::lor)
		return -1;
	if (strcmp(buf, "and") == 0 && op != operation::land)
		return -1;
		
	ind += plus;
	ind += skip_spaces(string+ind);
	
	//third word;
	plus = parse_search_condition(string+ind);
	if (plus < 0)
		return -1;
		
	ind += plus;
	ind += skip_spaces(string + ind);
	
	return ind;
}


bool command::parse(const char * string)
{
	int ind = 0;
	ind += skip_spaces(string);
	int plus = get_word(string+ind);
	char word[LEN];
	strncpy(word, string+ind, plus);
	word[plus] = '\0';
	
	if (strcmp(word, "quit") == 0)
	{
		type = command_type::quit;
	}
	else if (strcmp(word, "select") == 0)
	{
		type = command_type::select;
	}
	else if (strcmp(word, "insert") == 0)
	{
		type = command_type::insert;
	}
	else if (strcmp(word, "delete") == 0)
	{
		type = command_type::del;
	}
	else
		return false;
		
	ind += plus;
	ind += skip_spaces(string+ind);
	if (string[ind] == '\0')
	{
		if (type == command_type::quit)
			return true;
		else if (type == command_type::insert)
			return false;
	}	
	
	if (type == command_type::insert)
	{
		//обработать три слова
		char n[LEN], p[LEN], g[LEN];
		ind += skip_spaces(string+ind);
		if (string[ind] != '(')
			return false;
		ind++;
		
		plus = 0;
		while (string[ind+plus] != ',')
			plus++;
		strncpy(n, string+ind, plus);
		n[plus] = '\0';
		ind += plus;
		
		if (string[ind] != ',')
			return false;
		
		ind++;
		ind += skip_spaces(string + ind);
		plus = 0;
		while (string[ind+plus] != ',')
			plus++;
		strncpy(p, string+ind, plus);
		p[plus] = '\0';
		ind += plus;
		if (string[ind] != ',')
			return false;
		ind++;
		ind += skip_spaces(string + ind);
		plus = 0;
		while (string[ind+plus] != ')' && string[ind+plus] != '\0')
			plus++;
		strncpy(g, string+ind, plus);
		g[plus] = '\0';
		ind += plus;
		
		ind += skip_spaces(string+ind);
		if (string[ind] != ')')
			return false;
		ind++;
		ind += skip_spaces(string+ind);
		if (string[ind] != '\0')
			return false;
		init(n, std::stoi(p), std::stoi(g));
		return true;
	}
	else
	{
		if (string[ind] == '\0')
			return true;
		plus = parse_print_condition(string + ind);
		if (plus < 0)
			return false;
		ind += plus;
		if (type == command_type::del)
		{
			if (string[ind] == '\0')
				return true;
			else
				return false;
		}
		
		//if select
		if (string[ind] == '\0')
			return true;
			
		plus = get_word(string+ind);
		strncpy(word, string+ind, plus);
		word[plus] = '\0';
		
		if (strcmp(word, "order") != 0)
			return false;
		
		
		ind += plus;
		ind += skip_spaces(string + ind);	
				
		plus = get_word(string+ind);
		strncpy(word, string+ind, plus);
		word[plus] = '\0';
		
		if (strcmp(word, "by") != 0)
			return false;

		ind += plus;
		ind += skip_spaces(string + ind);	
				
				
		//обработать три слова
		if (string[ind] == '*')
		{
			ind++;
			ind += skip_spaces(string+ind);
			if (string[ind] == '\0')
				return true;
		}
		plus = get_word(string+ind);
		if (plus != 0 && string[ind+plus-1] == ',')
		{
			strncpy(word, string+ind, plus-1);
			word[plus-1] = '\0';
		}
		else
		{
			strncpy(word, string+ind, plus);
			word[plus] = '\0';
		}
		if (!detect_order_by(word, 1))
			return false;
		
		ind += plus;
		ind += skip_spaces(string+ind);
		
		if (string[ind] == '\0')
			return true;
		
		plus = get_word(string+ind);
		if (plus != 0 && string[ind+plus-1] == ',')
		{
			strncpy(word, string+ind, plus-1);
			word[plus-1] = '\0';
		}
		else
		{
			strncpy(word, string+ind, plus);
			word[plus] = '\0';
		}
		if (!detect_order_by(word, 2))
			return false;
		
		ind += plus;
		ind += skip_spaces(string+ind);
		
		
		if (string[ind] == '\0')
			return true;
		
		plus = get_word(string+ind);
		
		strncpy(word, string+ind, plus);
		word[plus] = '\0';
		
		if (!detect_order_by(word, 3))
			return false;
		
		
		ind += plus;
		ind += skip_spaces(string+ind);
		
		if (string[ind] == '\0')
			return true;
		else 
			return false;
	}
}


void command::print(FILE *fp) const
{
	fprintf(fp, "%s %d %d %d %d %d %d\n", name.get(), (int)c_name, (int)c_phone, (int)c_group, phone, group, (int)op); 
}


bool command::apply(const record & x) const
{
	if (op == operation::none)
	{
		if (c_phone != condition::none)
		{
			return (this->compare_phone(c_phone, x));
		}
		else if (c_group != condition::none)
		{
			return (this->compare_group(c_group, x));
		}
		else if (c_name != condition::none)
		{
			if (c_name == condition::like)
			{
				char *word1 = x.get_name();
				char *word2 = name.get();
				return check_word(word1, word2);
			}
			else if (c_name == condition::nlike)
			{
				char *word1 = x.get_name();
				char *word2 = name.get();
				return !check_word(word1, word2);
			}
			else
			{
				return (this->compare_name(c_name, x));
			}
		}
		return true;
	}
	else if (op == operation::lor)
	{
		if (c_phone != condition::none && this->compare_phone(c_phone, x))
			return true;
		if (c_group != condition::none && this->compare_group(c_group, x))
			return true;
		if (c_name != condition::none)
		{
			if (c_name == condition::like)
			{
				char *word1 = x.get_name();
				char *word2 = name.get();
				if (check_word(word1, word2))
					return true;
			}
			else if (c_name == condition::nlike)
			{
				char *word1 = x.get_name();
				char *word2 = name.get();
				if (!check_word(word1, word2))
					return true;
			}
			else
			{
				if (this->compare_name(c_name, x))
					return true;
			}
		}
		return false;
	}
	else //and
	{
		if (c_phone != condition::none && !this->compare_phone(c_phone, x))
			return false;
		if (c_group != condition::none && !this->compare_group(c_group, x))
			return false;
		if (c_name != condition::none)
		{
			if (c_name == condition::like)
			{
				char *word1 = x.get_name();
				char *word2 = name.get();
				if (!check_word(word1, word2))
					return false;
			}
			else if (c_name == condition::nlike)
			{
				char *word1 = x.get_name();
				char *word2 = name.get();
				if (check_word(word1, word2))
					return false;
			}
			else
			{
				if (!this->compare_name(c_name, x))
					return false;
			}
		}
		return true;
	}
}

int command::get_symbol(char *word) const
{
	char spec[6];
	strcpy(spec, "[]^_%");
	if (word[0] != '\\')
		return 0;
	for (int i = 0; spec[i] != '\0'; i++)
	{
		if (word[1] == spec[i])
			return 1;
	}
	return 0; 
}

bool command::check_word(char *str1, char *str2) const
{
	int i1 = 0, i2 = 0;
	char *word1 = str1, *word2 = str2;
	char l, r;
	while (word1[i1] != '\0' && word2[i2] != '\0')
	{
		if (word2[i2] != '\\')
		{
			if (word2[i2] == '[')
			{
				int ind = i2+1+get_symbol(word2+i2+1);
				bool flag = (word2[ind] == '^');
				if (flag)
				{
					if (!(ind == i2+1 && word2[ind] == '^'))
						return false;
					ind++;
					if (word2[ind] == '\0')
						return false;
					ind += get_symbol(word2+ind);
				}
				if (word2[ind] == '\0')
					return false;
				l = word2[ind];
				ind++;
				if (word2[ind] != '-')
					return false;
				ind++;
				if (word2[ind] == '\0')
					return false;
				ind += get_symbol(word2+ind);
				if (word2[ind] == '\0')
					return false;
				r = word2[ind];
				ind++;
				if (word2[ind] != ']')
					return false;
				if (!flag)
				{
					if (!(word1[i1] >= l && word1[i1] <= r))
						return false;
				}
				else
				{
					if (word1[i1] >= l && word1[i1] <= r)
						return false;
				}
				i2 = ind+1;
				i1++;
			}
			else if (word2[i2] == '%')
			{
				int k = 0;
				bool check = false;
				while (word1[i1+k] != '\0')
				{
					check = check_word(word1+i1+k, word2+i2+1);
					if (check)
					{
						return true;
					}
					k++;
				}
				if (word1[i1+k] == '\0' && word2[i2+1] == '\0') 
					return true;
				if (!check)
					return false;
			}
			else if (word2[i2] == '_')
			{
				i2++;
				i1++;
			}
			else if (word2[i2] == word1[i1])
			{
				i2++;
				i1++;
			}
			else
				return false;
		}
		else
		{
			char spec[7];
			strcpy(spec, "[]^_%\\");
			int i = 0;
			for (i = 0; spec[i] != '\0'; i++)
			{
				if (word2[i2+1] == spec[i] && word1[i1] == spec[i])
				{
					i2 += 2;
					i1++;
					break;
				}
			}
			if (spec[i] == '\0')
				return false;
		}
	}
	while (word2[i2] == '%')
		i2++;
	if (word1[i1] == '\0' && word2[i2] == '\0')
		return true;
	return false;	
}
