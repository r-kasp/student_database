# ifndef list_H
# define list_H
# include <stdio.h>
# include "record.h"

enum class errors
{
	OK,
	ERROR_MEMORY,
	ERROR_OPEN,
	ERROR_READ,
	ERROR_UNKNOWN
};

class list_node : public record
{
private:
	list_node *prev = nullptr;
	list_node *next = nullptr;
public:
	list_node() = default;
	list_node(const list_node & x) = delete;
	list_node *get_next() { return next; }
	list_node *get_prev() { return prev; }
	void set_next(list_node *v) { next = v; }
	void set_prev(list_node *v) { prev = v; }
	list_node(list_node && x): record((record &&)(x))
	{
		next = x.next;
		prev = x.prev;
		x.next = nullptr;
		x.prev = nullptr;
	}
	~list_node()
	{
		next = nullptr;
		prev = nullptr;
	}
	list_node & operator = (const list_node & x) = delete;
	list_node & operator = (list_node && x)
	{
		next = x.next;
		prev = x.prev;
		x.next = nullptr;
		x.prev = nullptr;
		return *this;
	}
	friend class list;
};


class search_node
{
private:
	list_node *link = nullptr;
	search_node *next = nullptr;
public:
	search_node() = default;
	search_node(const search_node & x) = delete;
	search_node *get_next() { return next; }
	list_node *get_link() { return link; }
	void set_next(search_node *v) { next = v; }
	void set_link(list_node *v) { link = v; }
	search_node(search_node && x)
	{
		next = x.next;
		x.next = nullptr;
	}
	~search_node()
	{
		next = nullptr;
	}
	search_node & operator = (const search_node & x) = delete;
	search_node & operator = (search_node && x)
	{
		next = x.next;
		x.next = nullptr;
		return *this;
	}
	friend class search_list;
};

class list
{
private:
	list_node *head = nullptr;
	const static int max_items = 3;
public:
	list() = default;
	~list()
	{
		delete_list();
	}
	list_node *get_head() { return head; }
	void set_head(list_node *v) { head = v; }
	void delete_list();
	void print_list(const ordering order[], FILE *fp);
	int read_list(const char* name);
	//list_node* merge(list_node *A, list_node *B, list_node *C, ordering order_by[max_items]);
	//void mergeSort(ordering order_by[max_items]);
	void AddTo(list_node * v, list_node * add);
	void AddToHead(list_node * add);
	void DeleteFromList(list_node *v);
};


class search_list
{
private:
	search_node *head = nullptr;
	const static int max_items = 3;
public:
	search_list() = default;
	~search_list()
	{
		delete_list();
	}
	search_node *get_head() { return head; }
	void set_head(search_node *v) { head = v; }
	void delete_list();
	void print_list(const ordering order[], FILE *fp);
	//int read_list(const char* name);
	search_node* merge(search_node *A, search_node *B, search_node *C, ordering order_by[max_items]);
	void mergeSort(ordering order_by[max_items]);
	void AddTo(search_node * v, search_node * add);
	void AddToHead(search_node * add);
	//void DeleteFromList(search_node *v);
};
#endif
