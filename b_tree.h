#ifndef	b_tree_H
#define b_tree_H
#include <memory>
#include <stdio.h>
#include <cstring>
#include <iostream>
#include "list.h"
#include "condition.h"
#include "command.h"
#include "ordering.h"
#include "record.h"

template <class T> class b_tree;
template <class T>
class b_tree_node
{
private:
	T * values = nullptr;
	int size = 0;
	b_tree_node ** children = nullptr;
public:
	b_tree_node() = default;
	int init(int m)
	{
		size = 0;
		values = new T[2*m];
		if (values == nullptr)
			return (int)read_status::memory;
		children = new b_tree_node<T>*[2*m+1];
		if (children == nullptr)
		{
			delete [] values;
			return (int)read_status::memory;
		}
		for (int i = 0; i < 2*m; i++)
			values[i] = nullptr;
		for (int i = 0; i < 2*m+1; i++)
			children[i] = nullptr;
		return (int)read_status::success;
	}
	b_tree_node(int m)
	{
		init(m);
	}
	b_tree_node(const b_tree_node & x) = delete;
	~b_tree_node()
	{
		if (values != nullptr)
			delete [] values;
		for (int i = 0; i < size+1; i++)
			if (children[i] != nullptr)
				delete children[i];
		if (children != nullptr)
			delete [] children;		
	}
	b_tree_node(b_tree_node&& x): T((T &&)(x))
	{
		values = x.values;
		children = x.children;
		size = x.size;
		x.values = nullptr;
		x.children = nullptr;
		x.size = 0;
	}
	b_tree_node & operator = (const b_tree_node & x) = delete;
	b_tree_node & operator = (b_tree_node && x)
	{
		*(T*)(this) = (T &&)(x);
		values = x.values;
		children = x.children;
		size = x.size;
		x.values = nullptr;
		x.children = nullptr;
		x.size = 0;
		return *this;
	}
	int get_size() { return size; }
	T * get_values() { return values; }
	b_tree_node ** get_children() { return children; }
	friend class b_tree<T>; 
};


template <class T>
class b_tree
{
private:
	int m = 1;
	b_tree_node<T> * root = nullptr;
public:
	b_tree() = default;
	
	
	~b_tree()
	{
		if (root != nullptr)
			delete root;
	}
	
	
	void set_m(int x)
	{
		m = x;
	}
	
	
	int get_m() const
	{
		return m;
	}
	
	
	b_tree_node<T> * get_root()
	{
		return root;
	}
	
	bool compare(const record & x, const record & y)
	{
		if (x.get_phone() < y.get_phone())
			return true;
		if (x.get_phone() == y.get_phone() && x.get_group() < y.get_group())
			return true;
		if (x.get_phone() == y.get_phone() && x.get_group() == y.get_group() && strcmp(x.get_name(), y.get_name()) < 0)
			return true;
		return false;
	}
	
	
	bool compare_less(const char *name, int phone, int group, const record & y)
	{
		if (phone < y.get_phone())
			return true;
		if (phone == y.get_phone() && group < y.get_group())
			return true;
		if (phone == y.get_phone() && group == y.get_group() && strcmp(name, y.get_name()) < 0)
			return true;
		return false;
	}
	
	bool compare_more(const char *name, int phone, int group, const record & y)
	{
		if (phone > y.get_phone())
			return true;
		if (phone == y.get_phone() && group > y.get_group())
			return true;
		if (phone == y.get_phone() && group == y.get_group() && strcmp(name, y.get_name()) > 0)
			return true;
		return false;
	}
	
	
	b_tree_node<T> * get_root() const
	{
		return root;
	}
	
	
	void copymem(b_tree_node<T> *& first, b_tree_node<T> *& p, int from, int to, int flag)
	{
		for (int i = to-1; i >= from; i--)
		{
			p->values[i-m+flag] = (T &&)(first->values[i]);				
			p->children[i-m+1+flag] = first->children[i+1];
			first->children[i+1] = nullptr;
		}
	}
	
	
	void cycletrip(b_tree_node<T> *& first, int from, int to)
	{
		for (int i = to-1; i >= from; i--)
		{
			first->values[i] = (T &&)(first->values[i-1]);
			first->children[i+1] = first->children[i];
		}	
	}
	
	
	int binsearch(const T & x, T * values, int size)
	{
		int l = 0, r = size;
		while (r != l)
		{
			int m = (r+l)/2;
			if (compare(*values[m], *x))//(values[m] < x)
				l = m+1;
			else
				r = m;
		}
		return l;
	}

	
	
	int binsearch2(const char *name, int phone, int group, T * values, int size)
	{
		int l = 0, r = size;
		while (r != l)
		{
			int m = (r+l)/2;
			if (compare_more(name, phone, group, *values[m])) // if (name, phone, group) > values[m]
				l = m+1;
			else
				r = m;
		}
		return l;
	}
	
	
	bool equal_record(const char *name, int phone, int group, record & x)
	{
		if (phone == x.get_phone() && group == x.get_group() && strcmp(name, x.get_name()) == 0)
			return true;
		return false;
	}
	
	
	bool find_node(b_tree_node<T> * v, const char * name, int phone, int group)
	{
		if (v == nullptr)
		{
			return false;
		}
		int ind = binsearch2(name, phone, group, v->values, v->size);
		if (ind < v->size && equal_record(name, phone, group, *v->values[ind]))
			return true;
		if (ind < v->size && compare_less(name, phone, group, *v->values[ind]))
		{
			return find_node(v->children[ind], name, phone, group);
		}
		else
		{
			if (ind < v->size)
				return find_node(v->children[ind+1], name, phone, group); 
			else
				return find_node(v->children[ind], name, phone, group);
		}
	}
	
	
	void insert(T & x, int size, int index, b_tree_node<T> *& p, b_tree_node<T> *& second)
	{
		for (int i = size; i > index; i--)
		{
			p->values[i] = (T &&)(p->values[i-1]);
			p->children[i+1] = p->children[i];
		}
		p->children[index+1] = second;
		p->values[index] = (T &&)(x);
	}
	
	
	bool addsubtree(T & x, b_tree_node<T> *& first, b_tree_node<T> *& second, T & y)
	{
		int index;
		index = binsearch(x, first->values, first->size);
		b_tree_node<T> *p = first->children[index];
		if (p != nullptr)
		{
			bool check = addsubtree(x, p, second, y);
			if (check)
				return true;
		}
		else
		{
			second = nullptr;
			y = (T &&)(x);
		}
		if (first->size < 2*m)
		{
			insert(y, first->size, index, first, second);
			(first->size)++;
			return true;
		}
		p = new b_tree_node<T>(m);
		if (index < m)
		{
			copymem(first, p, m, 2*m, 0);
			cycletrip(first, index+1, m+1);	
			first->children[index+1] = second;
			first->values[index] = (T &&)(y);
			p->children[0] = first->children[m+1];
			first->children[m+1] = nullptr;
		}
		else if (index == m)
		{
			copymem(first, p, m, 2*m, 0);
			p->children[0] = second;
		}
		else if (index == 2*m)
		{
			p->values[m-1] = (T &&)(y);
			p->children[m] = second;
			copymem(first, p, m+1, 2*m, -1);
			p->children[0] = first->children[m+1];
			first->children[m+1] = nullptr;
		}
		else //if (index > m)
		{
			p->children[m] = first->children[2*m];
			copymem(first, p, index, 2*m, 0);
			p->children[index-m] = second;
			p->values[index-m-1] = (T &&)(y);
			copymem(first, p, m+1, index, -1);
			p->children[0] = first->children[m+1];
			first->children[m+1] = nullptr;
		}
		second = p;
		if (index != m)
			y = (T &&)(first->values[m]);
		first->size = m;
		second->size = m;
		return false;
	}
	
	
	void add_node(T & x)
	{
		if (root == nullptr)
		{
			root = new b_tree_node<T>(m);
			root->values[0] = (T &&)x;
			root->size = 1;
			return;
		}
		T y;
		b_tree_node<T> * first = root, * second = nullptr;
		int rootsz = root->size;
		bool check = addsubtree(x, first, second, y);
		if (check)
			return;
		if (rootsz < 2*m)
		{
			int index = binsearch(x, first->values, first->size);
			insert(y, first->size, index, first, second);
			(root->size)++;
		}
		else
		{
			root = new b_tree_node<T>(m);
			root->values[0] = (T &&)y;
			root->children[0] = first;
			root->children[1] = second;
			root->size = 1;
		}
	}
	
	bool is_leaf(b_tree_node<T> * v)
	{
		if (v->children[0] == nullptr)
			return true;
		return false;
	}
	
	b_tree_node<T> * get_most_left(b_tree_node<T> * v)
	{
		while (!is_leaf(v))
		{
			v = v->children[0];
		}
		return v;
	}
	
	b_tree_node<T> * get_most_right(b_tree_node<T> * v)
	{
		while (!is_leaf(v))
		{
			v = v->children[v->size];
		}
		return v;
	}
	
	void remove_from_leaf(b_tree_node<T> * v, int ind)
	{
		//delete v->values[ind];
		for (int i = ind; i < v->size-1; i++)
		{
			v->values[i] = (T &&)v->values[i+1];
		}
		v->values[v->size-1] = nullptr;
		v->size--;
	}
	
	/*
	void remove_from_non_leaf(b_tree_node<T> * v, int ind, const char * name, int phone, int group)
	{
		if (v->children[ind]->size >= m+1)
		{
			b_tree_node<T> * node = get_most_right(v->children[ind]);
			//delete v->values[ind];
			v->values[ind] = (T &&)node->values[node->size-1];
			remove_from_leaf(node, node->size-1);
		}
		else if (v->children[ind+1]->size >= m+1)
		{
			b_tree_node<T> * node = get_most_left(v->children[ind+1]);
			//delete v->values[ind];
			v->values[ind] = (T &&)node->values[0];
			remove_from_leaf(node, 0);
		}
		else
		{
			delete_node(v->children[ind], name, phone, group);
			merge(v, ind);
		} 
	}*/
	
	void remove_from_non_leaf(b_tree_node<T> * v, int ind)
	{
		b_tree_node<T> * node = get_most_right(v->children[ind]);
		//delete v->values[ind];
		T buf = node->values[node->size-1];
		v->values[ind] = (T &&)buf;
		delete_node(v->children[ind], buf->get_name(), buf->get_phone(), buf->get_group());
	}
	
	
	void merge(b_tree_node<T> * v, int ind)
	{
		
		b_tree_node<T> *left = v->children[ind], *right = v->children[ind+1];
		left->values[left->size] = (T &&)v->values[ind];
		for (int i = 0; i < right->size; i++)
		{
			left->values[i + left->size + 1] = (T &&)right->values[i];
		}
		if (!is_leaf(left))
		{
			for (int i = 0; i <= right->size; i++)
			{
				left->children[i + left->size + 1] = right->children[i];
			}
		}
		for (int i = ind; i < v->size-1; i++)
			v->values[i] = (T &&)v->values[i+1];
		for (int i = ind+2; i <= v->size; i++)
			v->children[i-1] = v->children[i];
		v->children[v->size] = nullptr;
		left->size += right->size + 1;
		v->size--;
		//maybe it is not need
		for (int i = 0; i <= right->size; i++)
			right->children[i] = nullptr;
		delete right;
	}
		
	void merge_right(b_tree_node<T> * v, int ind)
	{
		
		b_tree_node<T> *left = v->children[ind-1], *right = v->children[ind];
		
		for (int i = 0; i < right->size; i++)
		{
			right->values[left->size+1] = right->values[i];
		}
		if (!is_leaf(right))
		{
			for (int i = 0; i <= right->size; i++)
				right->children[left->size+1] = right->children[i];
		}
		for (int i = 0; i < left->size; i++)
			right->values[i] = left->values[i];
		if (!is_leaf(left))
		{
			for (int i = 0; i <= left->size; i++)
				right->children[i] = left->children[i];
		}
		right->values[left->size] = v->values[ind-1];
		v->values[ind-1] = nullptr;
		right->size += left->size + 1;
		for (int i = 0; i <= left->size; i++)
			left->children[i] = nullptr;
		delete left;
		v->children[ind-1] = right;
		v->children[ind] = nullptr;
		v->size--;
	}
	
	void delete_node(b_tree_node<T> * v, const char * name, int phone, int group)
	{
		if (v == nullptr)
		{
			return;
		}
		int ind = binsearch2(name, phone, group, v->values, v->size);
		if (ind < v->size && equal_record(name, phone, group, *v->values[ind]))
		{
			if (is_leaf(v))
			{
				remove_from_leaf(v, ind);
				return;
			}
			else
				remove_from_non_leaf(v, ind);
		}
		else
		{	
			if (is_leaf(v))
				return;
			delete_node(v->children[ind], name, phone, group);
		}
		if (v->children[ind] && v->children[ind]->size < m) //ind != v->size балансировать после удаления
		{
			fill(v, ind);
		}
	
	}
	
	void remove_node(const char *name, int phone, int group)
	{
		if (root == nullptr)
			return;
		delete_node(root, name, phone, group);
		if (root->size == 0)
		{
			if (is_leaf(root))
			{
				delete root;
				root = nullptr;
			}
			else
			{
				b_tree_node<T> *buf = root;
				root = root->children[0];
				//buf->children[0] = nullptr;
				for (int i = 0; i < 2*m; i++)
					buf->values[i] = nullptr;
				for (int i = 0; i <= 2*m; i++)
					buf->children[i] = nullptr;
				delete buf;
			}
		}
	}
	
	void fill(b_tree_node<T> * v, int ind)
	{
		if (ind != 0 && v->children[ind-1]->size >= m+1) //ind != 0  m+1
			squezee_left(v, ind);
		else if (ind != v->size && v->children[ind+1] && v->children[ind+1]->size >= m+1) //ind != v->size   m+1
			squezee_right(v, ind);
		else
		{
			if (ind == v->size)
			{
				//merge(v, ind-1);
				merge_right(v, ind);
			}
			else
				merge(v, ind);
		}
	}
	
	void squezee_left(b_tree_node<T> * v, int ind)
	{
		b_tree_node<T> *left = v->children[ind-1], *right = v->children[ind];
		for (int i = right->size-1; i >= 0; i--)
			right->values[i+1] = (T &&)right->values[i];
		if (!is_leaf(right))
		{
			for (int i = right->size; i >= 0; i--)
				right->children[i+1] = right->children[i]; 
		}
		right->values[0] = (T &&)v->values[ind-1]; //ind-1
		if (!is_leaf(right))
		{
			right->children[0] = left->children[left->size];
			left->children[left->size] = nullptr;
		}
		v->values[ind-1] = (T &&)left->values[left->size-1];//ind-1
		left->size--;
		right->size++;
	}
	
	void squezee_right(b_tree_node<T> * v, int ind)
	{
		b_tree_node<T> *left = v->children[ind], *right = v->children[ind+1];
		left->values[left->size] = (T &&)v->values[ind];
		if (!is_leaf(left))
		{
			left->children[left->size+1] = right->children[0];
		}
		v->values[ind] = (T &&)right->values[0];
		for (int i = 1; i < right->size; i++)
			right->values[i-1] = (T &&)right->values[i];
		if (!is_leaf(right))
		{
			for (int i = 1; i <= right->size; i++)
				right->children[i-1] = right->children[i];
			right->children[right->size] = nullptr;
		}
		left->size++;
		right->size--;
	}
	
	void print_node(int max_level, int level = 0, b_tree_node<T> *p = nullptr)
	{
		int spaces = level*5;
		if (max_level <= level)
			return;
		for (int i = 0; i < p->size; i++)
		{
			for (int i = 0; i < spaces; i++)
				printf(" ");
			printf("%s %d %d\n", p->values[i]->get_name(), p->values[i]->get_phone(), p->values[i]->get_group());
		}
		for (int i = 0; i < p->size+1 && p->children[i] != nullptr; i++)
			print_node(max_level, level+1, p->children[i]);
	}
	void print_tree(int r)
	{
		if (r == 0)
			return;
		if (root != nullptr)
			print_node(r,0,root);	
	}
	int cnt_nodes(b_tree_node<T> *v)
	{
		int res = 0;
		if (v == nullptr)
			return 0;
		res += v->size;
		for (int i = 0; i <= v->size; i++)
			res += cnt_nodes(v->children[i]);
		return res;
	}
	
	void print_rec(b_tree_node<T> *cur, command & x, int & res)
	{
		if (cur == nullptr)
			return;
		int phone1 = x.get_phone();
		int phone2 = -1;
		for (int ind = 0; ind < cur->size; ind++)
		{
			phone2 = cur->values[ind]->get_phone();
			if (phone1 == phone2)
			{
				if (x.apply(*cur->values[ind]))
				{
					cur->values[ind]->print(x.get_order(), stdout);
					res++;
				}
				print_rec(cur->children[ind], x, res);
			}
			else if (phone1 < phone2)
			{
				print_rec(cur->children[ind], x, res);
			}
		}
		if (phone1 >= phone2)
			print_rec(cur->children[cur->size], x, res);
	}
	
	
	void add_rec_to_list(b_tree_node<T> *cur, command & x, int & res, search_list & L)
	{
		if (cur == nullptr)
			return;
		int phone1 = x.get_phone();
		int phone2 = -1;
		for (int ind = 0; ind < cur->size; ind++)
		{
			phone2 = cur->values[ind]->get_phone();
			if (phone1 == phone2)
			{
				if (x.apply(*cur->values[ind]))
				{
					search_node *add = new search_node;
					add->set_link(cur->values[ind]);
					res++;
					L.AddToHead(add);
				}
				add_rec_to_list(cur->children[ind], x, res, L);
			}
			else if (phone1 < phone2)
			{
				add_rec_to_list(cur->children[ind], x, res, L);
			}
		}
		if (phone1 >= phone2)
			add_rec_to_list(cur->children[cur->size], x, res, L);
	}
	
	void delete_rec(b_tree_node<T> *cur, command & x, search_list & L)
	{
		if (cur == nullptr)
			return;
		int phone1 = x.get_phone();
		int phone2 = -1;
		for (int ind = 0; ind < cur->size; ind++)
		{
			phone2 = cur->values[ind]->get_phone();
			if (phone1 == phone2)
			{
				if (x.apply(*cur->values[ind]))
				{
					search_node *add = new search_node;
					add->set_link(cur->values[ind]);
					L.AddToHead(add);
				}
				delete_rec(cur->children[ind], x, L);
			}
			else if (phone1 < phone2)
			{
				delete_rec(cur->children[ind], x, L);
			}
		}
		if (phone1 >= phone2)
			delete_rec(cur->children[cur->size], x, L);
	}
	
	
	void push(b_tree_node<T> *cur, command & x, int & res, search_list & ResList)
	{
		if (cur == nullptr)
			return;
		int phone1 = x.get_phone();
		int phone2 = -1;
		for (int ind = 0; ind < cur->size; ind++)
		{
			phone2 = cur->values[ind]->get_phone();
			if (phone1 == phone2)
			{
				if (x.apply(*cur->values[ind]))
				{
					search_node *add = new search_node;
					add->set_link(cur->values[ind]);
					ResList.AddToHead(add);
					res++;
				}
				push(cur->children[ind], x, res, ResList);
			}
			else if (phone1 < phone2)
			{
				push(cur->children[ind], x, res, ResList);
			}
		}
		if (phone1 >= phone2)
			push(cur->children[cur->size], x, res, ResList);
	}
};

# endif
