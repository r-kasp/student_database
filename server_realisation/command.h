#ifndef command_H
#define command_H
#include <stdio.h>
#include <cstring>
#include <iostream>
#include "record.h"
#include "ordering.h"
#include "operation.h"
#include "command_type.h"

#define LEN 1234

class command : public record
{
private:
	const static int max_items = 3;
	command_type type = command_type::none;
	condition c_name = condition::none;
	condition c_phone = condition::none;
	condition c_group = condition::none;
	operation op = operation::none;
	ordering order[max_items] = {ordering::none, ordering::none, ordering::none}; 
	ordering order_by[max_items] = {ordering::none, ordering::none, ordering::none};
public:
	command () = default;
	~command () = default;
	
	command_type get_type() { return type; }
	condition get_c_name() { return c_name; }
	condition get_c_phone() { return c_phone; }
	condition get_c_group() { return c_group; }
	operation get_op() { return op; }
	ordering * get_order() { return order; }
	ordering * get_order_by() { return order_by; }
	// Convert string command to data structure
	// Example 1: "phone = 1234567" parsed to
	//(command::phone = 1234567, command::c_phone = condition::eq)
	//other fields are unspecified
	// Example 2: "name like St%" parsed to
	//(command::name = "St%", command::c_name = condition::like)
	//other fields are unspecified
	int parse_search_condition (const char * string);
	int parse_print_condition (const char * string);
	bool parse (const char * string);
	// Print parsed structure
	void print (FILE *fp = stdout) const;
	// Apply command, return comparision result for record ’x’
	bool apply (const record& x) const;
	
	condition detect_condition (const char *str);
	
	int get_symbol(char *word) const;
	
	bool check_word(char *str1, char *str2) const;
	
	int skip_spaces(const char *string) const;
	
	bool detect_order(char *buf, int number);
	
	bool detect_order_by(char *buf, int number);
	
	int get_word(const char * string) const;
};
#endif
