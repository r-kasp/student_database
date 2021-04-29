#include "structure.h"

structure::structure()
{
	avl_tree_array = new avl_tree[MOD];
	b_tree_array = new b_tree<list_node *>[MOD];
}

structure::~structure()
{
	delete [] avl_tree_array;
	delete [] b_tree_array;
}

void structure::init(list & List, int k)
{
	list_node *cur = List.get_head();
	while (cur != nullptr)
	{
		add(cur, k);
		cur = cur->get_next();
	}
}

void structure::add(list_node *cur, int k)
{
	avl_node *node = new avl_node;
	node->set_value(cur);
	int ind = hash(cur->get_name(), k);
	avl_tree_array[ind].insert_node(node);
	int ind2 = hash2(cur->get_phone(), k);
	b_tree_array[ind2].add_node(*&cur);
}


int structure::hash(const char *string, int k)
{	
	int len = strlen(string);
	int ind = 0;
	if (len > k)
		ind = len-k;
	size_t res = 0;
	for (int i = ind; i < len; i++)
		res = (res + (int)string[ind]) % MOD;
	return res;
}


int structure::hash2(int phone, int k)
{	
	int len = 0;
	int x = phone;
	while (x != 0)
	{
		len++;
		x /= 10;
	}
	x = phone;
	for (int i = 0; i < len-k; i++)
	{
		x /= 10;
	} 
	return x % MOD;
}


void structure::Select(command & com, int & res, int k)
{
	if (com.get_c_name() == condition::eq && com.get_op() != operation::lor)
	{
		int ind = hash(com.get_name(), k);
		
		if (com.get_order_by()[0] != ordering::none)
		{
			search_list ResList;
			avl_tree_array[ind].add_rec_to_list(avl_tree_array[ind].get_root(), com, res, ResList, true, true);
			ResList.mergeSort(com.get_order_by());
			ResList.print_list(com.get_order(), stdout);
			ResList.delete_list();
		}
		else
		{
			avl_tree_array[ind].print_rec(avl_tree_array[ind].get_root(), com, res, true, true);
		}
	}
	else if (com.get_c_phone() == condition::eq && com.get_op() != operation::lor)
	{
		int ind2 = hash2(com.get_phone(), k);
		
		if (com.get_order_by()[0] != ordering::none)
		{
			search_list ResList;
			b_tree_array[ind2].add_rec_to_list(b_tree_array[ind2].get_root(), com, res, ResList);
			ResList.mergeSort(com.get_order_by());
			ResList.print_list(com.get_order(), stdout);
			ResList.delete_list();
		}
		else
		{
			b_tree_array[ind2].print_rec(b_tree_array[ind2].get_root(), com, res);
		}
	}
	else
	{
		if (com.get_order_by()[0] != ordering::none)
		{
			search_list ResList;
			
			for (int i = 0; i < MOD; i++)
			{
				avl_tree_array[i].trip_add(avl_tree_array[i].get_root(), com, ResList, res, true);
			}
			
			ResList.mergeSort(com.get_order_by());
			ResList.print_list(com.get_order(), stdout);
			ResList.delete_list();
		}
		else
		{
			for (int i = 0; i < MOD; i++)
			{
				avl_tree_array[i].trip_print(avl_tree_array[i].get_root(), com, res);
			}
		}
	}
}


void structure::Insert(command & com, list & List, int k)
{
	bool add = true;
	int ind = hash(com.get_name(), k);
	int ind2 = hash2(com.get_phone(), k);
	if (avl_tree_array[ind].find(com.get_name(), com.get_phone(), com.get_group()))
		add = false;
	if (add)
	{
		list_node *v = new list_node;
		v->init(com.get_name(), com.get_phone(), com.get_group());
		List.AddToHead(v);
		avl_node *node = new avl_node;
		node->set_value(v);
		avl_tree_array[ind].insert_node(node);
		b_tree_array[ind2].add_node(v);
	}
}


void structure::Delete(command & com, list & List, int k)
{
	if (com.get_c_name() == condition::eq && com.get_op() != operation::lor)
	{
		int ind = hash(com.get_name(), k);	
		search_list ResList;			
		avl_tree_array[ind].delete_rec(avl_tree_array[ind].get_root(), com, ResList, true, true);
		search_node *cur = ResList.get_head();
		while (cur != nullptr)
		{
			avl_tree_array[ind].delete_node(cur->get_link());
			int ind2 = hash2(cur->get_link()->get_phone(), k);
			b_tree_array[ind2].remove_node(cur->get_link()->get_name(), cur->get_link()->get_phone(), cur->get_link()->get_group());
			List.DeleteFromList(cur->get_link());
			cur = cur->get_next();
		}
		ResList.delete_list();
	}
	else if (com.get_c_phone() == condition::eq && com.get_op() != operation::lor)	
	{
		int ind2 = hash2(com.get_phone(), k);	
		search_list ResList;			
		b_tree_array[ind2].delete_rec(b_tree_array[ind2].get_root(), com, ResList);
		search_node *cur = ResList.get_head();
		
		while (cur != nullptr)
		{
			b_tree_array[ind2].remove_node(cur->get_link()->get_name(), cur->get_link()->get_phone(), cur->get_link()->get_group());
			int ind = hash(cur->get_link()->get_name(), k);
			avl_tree_array[ind].delete_node(cur->get_link());
			List.DeleteFromList(cur->get_link());
			cur = cur->get_next();
		}
		ResList.delete_list();
	}
	else
	{
		search_list ResList;	
		int res;
		for (int i = 0; i < MOD; i++)
		{
			avl_tree_array[i].trip_add(avl_tree_array[i].get_root(), com, ResList, res, false);
		}
		
		search_node * cur = ResList.get_head();
		while (cur != nullptr)
		{
			int ind2 = hash2(cur->get_link()->get_phone(), k);
			b_tree_array[ind2].remove_node(cur->get_link()->get_name(), cur->get_link()->get_phone(), cur->get_link()->get_group());
			int ind = hash(cur->get_link()->get_name(), k);
			avl_tree_array[ind].delete_node(cur->get_link());
			List.DeleteFromList(cur->get_link());
			cur = cur->get_next();
		}
		ResList.delete_list();
	}
}

void structure::DeleteOnlyFromTree(command & com, int k)
{
	if (com.get_c_name() == condition::eq && com.get_op() != operation::lor)
	{
		int ind = hash(com.get_name(), k);	
		search_list ResList;			
		avl_tree_array[ind].delete_rec(avl_tree_array[ind].get_root(), com, ResList, true, true);
		search_node *cur = ResList.get_head();
		while (cur != nullptr)
		{
			avl_tree_array[ind].delete_node(cur->get_link());
			int ind2 = hash2(cur->get_link()->get_phone(), k);
			b_tree_array[ind2].remove_node(cur->get_link()->get_name(), cur->get_link()->get_phone(), cur->get_link()->get_group());
			cur = cur->get_next();
		}
		ResList.delete_list();
	}
	else if (com.get_c_phone() == condition::eq && com.get_op() != operation::lor)	
	{
		int ind2 = hash2(com.get_phone(), k);	
		search_list ResList;			
		b_tree_array[ind2].delete_rec(b_tree_array[ind2].get_root(), com, ResList);
		search_node *cur = ResList.get_head();
		
		while (cur != nullptr)
		{
			b_tree_array[ind2].remove_node(cur->get_link()->get_name(), cur->get_link()->get_phone(), cur->get_link()->get_group());
			int ind = hash(cur->get_link()->get_name(), k);
			avl_tree_array[ind].delete_node(cur->get_link());
			cur = cur->get_next();
		}
		ResList.delete_list();
	}
	else
	{
		search_list ResList;	
		int res;
		for (int i = 0; i < MOD; i++)
		{
			avl_tree_array[i].trip_add(avl_tree_array[i].get_root(), com, ResList, res, false);
		}
		
		search_node * cur = ResList.get_head();
		while (cur != nullptr)
		{
			int ind2 = hash2(cur->get_link()->get_phone(), k);
			b_tree_array[ind2].remove_node(cur->get_link()->get_name(), cur->get_link()->get_phone(), cur->get_link()->get_group());
			int ind = hash(cur->get_link()->get_name(), k);
			avl_tree_array[ind].delete_node(cur->get_link());
			cur = cur->get_next();
		}
		ResList.delete_list();
	}
}

void structure::DeleteElt(list_node * curr, list & List, int k)
{
	int ind = hash(curr->get_name(), k);
	int ind2 = hash2(curr->get_phone(), k);
	avl_tree_array[ind].delete_node(curr);
	b_tree_array[ind2].remove_node(curr->get_name(), curr->get_phone(), curr->get_group());
	List.DeleteFromList(curr);		
}


void structure::Push(command & com, int & res, search_list & ResList, int k)
{
	if (com.get_c_name() == condition::eq && com.get_op() != operation::lor)
	{
		int ind = hash(com.get_name(), k);
		
		avl_tree_array[ind].push(avl_tree_array[ind].get_root(), com, res, true, true, ResList);
	}
	else if (com.get_c_phone() == condition::eq && com.get_op() != operation::lor)
	{
		int ind2 = hash2(com.get_phone(), k);
		
		b_tree_array[ind2].push(b_tree_array[ind2].get_root(), com, res, ResList);
	}
	else
	{
		for (int i = 0; i < MOD; i++)
		{
			avl_tree_array[i].trip_add(avl_tree_array[i].get_root(), com, ResList, res, true);
		}
	}
}
