#ifndef avl_tree_H
#define avl_tree_H
#include <memory>
#include <stdio.h>
#include <cstring>
#include <iostream>
#include "list.h"
#include "condition.h"
#include "command.h"
#include "ordering.h"
#include "record.h"

class avl_node
{
private:
	list_node *value = nullptr;
	avl_node *left = nullptr;
	avl_node *right = nullptr;
	int height = 0;
public:
	avl_node() = default;
	avl_node(const avl_node & x) = delete;
	avl_node(avl_node&& x);
	~avl_node() { delete_node(); }
	avl_node & operator = (const avl_node & x) = delete;
	avl_node & operator = (avl_node && x);
	void delete_node();
	void set_height(int h) { height = h; }
	void set_value(list_node *a) { value = a; }
	int get_height() { return height; }
	list_node * get_value() { return value; }
	avl_node *get_left() { return left; }
	avl_node *get_right() { return right; }
	
	friend class avl_tree;
};

class avl_tree
{
private:
	avl_node *root = nullptr;
public:
	~avl_tree() { delete_tree(root); }
	avl_node * get_root() { return root; }
	void delete_tree(avl_node *v);
	int get_height(avl_node *v) { return (v == nullptr) ? 0 : v->height; }
	avl_node* add_node(avl_node *cur, avl_node *v);
	avl_node* balance(avl_node *v);
	void cnt_height(avl_node *v);
	avl_node* rot_left(avl_node *v);
	avl_node* rot_right(avl_node *v);
	void big_rot_left(avl_node *v);
	void big_rot_right(avl_node *v);
	avl_node * remove(avl_node *cur, list_node *v);
	avl_node * remove_min_node(avl_node *v);
	avl_node * get_min_node(avl_node *v);
	void delete_node(list_node *v);
	void insert_node(avl_node *v);
	bool comp(avl_node *x, avl_node *y);
	bool comp_list(list_node *x, list_node *y);
	bool find_node(avl_node *v, const char *name, int phone, int group);
	bool find(const char *name, int phone, int group);
	void print_rec(avl_node *cur, command & x, int & res, bool goleft, bool goright);
	void add_rec_to_list(avl_node *cur, command & x, int & res, search_list & L, bool goleft, bool goright);
	void delete_rec(avl_node *cur, command & x, search_list & L, bool goleft, bool goright);
	void push(avl_node *cur, command & x, int & res, bool goleft, bool goright, search_list & ResList);
	void trip_add(avl_node *cur, command & x, search_list & ResList, int & res, bool addres);
	void trip_print(avl_node *cur, command & x, int & res);
	
	//functions for debug
	void print() 
	{
		//std::cout << (root->value->get_group()) << std::endl;
		//std::cout << (root->right->value->get_group()) << std::endl;
		//std::cout << root->value->get_name() << " " << root->left->value->get_name()  << std::endl;
		//std::cout << root->left->parent->value->get_name() << std::endl; 
		//std::cout << root->right->right->value->get_name() << std::endl; 
	}
	int cnt_deep(avl_node *v)
	{
		if (v == nullptr)
			return 0;
		int l = cnt_deep(v->left);
		int r = cnt_deep(v->right);
		return 1 + (l > r ? l : r);
	}
	int cnt_nodes(avl_node *v)
	{
		if (v == nullptr)
			return 0;
		return 1 + cnt_nodes(v->left) + cnt_nodes(v->right);
	}
};

# endif
