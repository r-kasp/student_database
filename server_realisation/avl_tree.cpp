#include "avl_tree.h"


avl_node::avl_node(avl_node&& x)
{
	left = x.left;
	right = x.right;
	height = x.height;
	x.left = nullptr;
	x.right = nullptr;
}


avl_node & avl_node::operator = (avl_node && x)
{
	*(record*)(this) = (record &&)(x);
	left = x.left;
	right = x.right;
	height = x.height;
	x.left = nullptr;
	x.right = nullptr;
	return *this;
}

bool avl_tree::comp(avl_node *x, avl_node *y)
{
	int cmp = strcmp(x->value->get_name(), y->value->get_name());
	if (cmp < 0)
		return true;
	else if (cmp == 0 && x->value->get_phone() < y->value->get_phone())
		return true;
	else if (cmp == 0 && x->value->get_phone() == y->value->get_phone() && x->value->get_group() < y->value->get_group())
		return true;
	return false;
}

bool avl_tree::comp_list(list_node *x, list_node *y)
{
	int cmp = strcmp(x->get_name(), y->get_name());
	if (cmp < 0)
		return true;
	else if (cmp == 0 && x->get_phone() < y->get_phone())
		return true;
	else if (cmp == 0 && x->get_phone() == y->get_phone() && x->get_group() < y->get_group())
		return true;
	return false;
}

void avl_node::delete_node()
{
	if (left != nullptr)
	{
		left = nullptr;
	}
	if (right != nullptr)
	{
		right = nullptr;
	}
}

void avl_tree::delete_tree(avl_node *v)
{
	if (v == nullptr)
		return;
	if (v->left != nullptr)
	{
		delete_tree(v->left);
	}
	if (v->right != nullptr)
	{
		delete_tree(v->right);
	}
	delete v;
}

void avl_tree::cnt_height(avl_node *v)
{
	if (v == nullptr)
	{
		return;
	}
	int hl = get_height(v->left);
	int hr = get_height(v->right);
	if (hl > hr)
		v->height = hl+1;
	else
		v->height = hr+1;
}

bool avl_tree::find_node(avl_node *v, const char *name, int phone, int group)
{
	if (v == nullptr)
		return false;
	int cmp = strcmp(name, v->value->get_name());
	if (cmp == 0 && phone == v->value->get_phone() && group == v->value->get_group())
		return true;
	if (cmp < 0 || (cmp == 0 && phone < v->value->get_phone()) || (cmp == 0 && phone == v->value->get_phone() && group < v->value->get_group()))
		return find_node(v->left, name, phone, group);
	return find_node(v->right, name, phone, group);
}

bool avl_tree::find(const char *name, int phone, int group)
{
	return find_node(root, name, phone, group);
}


avl_node * avl_tree::add_node(avl_node *cur, avl_node *v)
{
	if (cur == nullptr)
	{
		return v;
	}
	if (comp(v,cur)) // v < cur
	{
		cur->left = add_node(cur->left, v);
	}
	else
	{
		cur->right = add_node(cur->right, v);
	}
	return balance(cur);
}

void avl_tree::insert_node(avl_node *v)
{
	root = add_node(root, v);
}

avl_node* avl_tree::balance(avl_node *v)
{
	cnt_height(v);
	if (get_height(v->right) - get_height(v->left) == 2)
	{
		avl_node *r = v->right;
		if (get_height(r->right) - get_height(r->left) < 0)
		{
			v->right = rot_right(v->right);
		}
		return rot_left(v);
	}
	else if (get_height(v->left) - get_height(v->right) == 2)
	{
		avl_node *l = v->left;
		if (get_height(l->right) - get_height(l->left) > 0)
		{
			v->left = rot_left(v->left);	
		}
		return rot_right(v);
	}
	return v;
}


avl_node* avl_tree::rot_left(avl_node *v)
{
	avl_node *buf = v->right;
	v->right = buf->left;
	buf->left = v;
	cnt_height(v);
	cnt_height(buf);
	return buf; 
}


avl_node* avl_tree::rot_right(avl_node *v)
{
	avl_node *buf = v->left;
	v->left = buf->right;
	buf->right = v;
	cnt_height(v);
	cnt_height(buf);
	return buf;
}


void avl_tree::big_rot_left(avl_node *v)
{
	rot_right(v->right);
	rot_left(v);
}


void avl_tree::big_rot_right(avl_node *v)
{
	rot_left(v->left);
	rot_right(v);
}


avl_node * avl_tree::remove(avl_node *cur, list_node *v)
{
	if (cur == nullptr)
		return nullptr;
	if (comp_list(v,cur->value))
	{
		cur->left = remove(cur->left, v);
	}
	else if (comp_list(cur->value,v))
	{
		cur->right = remove(cur->right, v);
	}
	else
	{
		avl_node *l = cur->left, *r = cur->right;
		delete cur;
		if (r == nullptr)
			return l;
		avl_node *min_node = get_min_node(r);
		min_node->right = remove_min_node(r);
		min_node->left = l;
		//if (cur == root)
		//{
		//	min_node->parent = nullptr;
		//	root = min_node;
		//}
		return balance(min_node);
	}
	return balance(cur);
}

void avl_tree::delete_node(list_node *v)
{
	root = remove(root, v);
}

avl_node* avl_tree::remove_min_node(avl_node *v)
{
	if (v->left == nullptr)
		return v->right;
	v->left = remove_min_node(v->left);
	return balance(v);
}


avl_node* avl_tree::get_min_node(avl_node *v)
{
	if (v->left != nullptr)
		return get_min_node(v->left);
	return v;
}

/*
void avl_tree::print_rec(avl_node *cur, command & x, int & res, bool goleft, bool goright)
{
	if (cur == nullptr)
		return;
	int cmp = strcmp(cur->value->get_name(), x.get_name());
	if (cmp == 0)
	{
		if (x.apply(*cur->value))
		{
			cur->value->print(x.get_order(), stdout);
			res++;
		}
		if (cur->left)
		{
			int cmp2 = strcmp(cur->left->value->get_name(), x.get_name());
			if (goleft)
				print_rec(cur->left, x, res, (cmp2 >= 0), true);
		}
		if (cur->right)
		{
			int cmp2 = strcmp(cur->right->value->get_name(), x.get_name());
			if (goright)
				print_rec(cur->right, x, res, true, (cmp2 <= 0));
		}
	}	
	else if (cmp < 0)
	{
		if (goright)
			print_rec(cur->right, x, res, true, true);
	}
	else
	{
		if (goleft)
			print_rec(cur->left, x, res, true, true);
	}
}*/


void avl_tree::add_rec_to_list(avl_node *cur, command & x, int & res, search_list & L, bool goleft, bool goright)
{
	if (cur == nullptr)
		return;
	int cmp = strcmp(cur->value->get_name(), x.get_name());
	if (cmp == 0)
	{
		if (x.apply(*cur->value))
		{
			search_node *add = new search_node;
			add->set_link(cur->value);
			res++;
			L.AddToHead(add);
		}
		if (cur->left)
		{
			int cmp2 = strcmp(cur->left->value->get_name(), x.get_name());
			if (goleft)
				add_rec_to_list(cur->left, x, res, L, (cmp2 >= 0), true);
		}
		if (cur->right)
		{
			int cmp2 = strcmp(cur->right->value->get_name(), x.get_name());
			if (goright)
				add_rec_to_list(cur->right, x, res, L, true, (cmp2 <= 0));
		}
	}	
	else if (cmp < 0)
	{
		if (goright)
			add_rec_to_list(cur->right, x, res, L, true, true);
	}
	else
	{
		if (goleft)
			add_rec_to_list(cur->left, x, res, L, true, true);
	}
}


void avl_tree::delete_rec(avl_node *cur, command & x, search_list & L, bool goleft, bool goright)
{
	if (cur == nullptr)
		return;
	int cmp = strcmp(cur->value->get_name(), x.get_name());
	if (cmp == 0)
	{
		if (x.apply(*cur->value))
		{
			search_node *add = new search_node;
			add->set_link(cur->value);
			L.AddToHead(add);
		}
		if (cur->left)
		{
			int cmp2 = strcmp(cur->left->value->get_name(), x.get_name());
			if (goleft)
				delete_rec(cur->left, x, L, (cmp2 >= 0), true);
		}
		if (cur->right)
		{
			int cmp2 = strcmp(cur->right->value->get_name(), x.get_name());
			if (goright)
				delete_rec(cur->right, x, L, true, (cmp2 <= 0));
		}
	}	
	else if (cmp < 0)
	{
		if (goright)
			delete_rec(cur->right, x, L, true, true);
	}
	else
	{
		if (goleft)
			delete_rec(cur->left, x, L, true, true);
	}
}


void avl_tree::push(avl_node *cur, command & x, int & res, bool goleft, bool goright, search_list & ResList)
{
	if (cur == nullptr)
		return;
	int cmp = strcmp(cur->value->get_name(), x.get_name());
	if (cmp == 0)
	{
		if (x.apply(*cur->value))
		{
			search_node *add = new search_node;
			add->set_link(cur->value);
			ResList.AddToHead(add);
			res++;
		}
		if (cur->left)
		{
			int cmp2 = strcmp(cur->left->value->get_name(), x.get_name());
			if (goleft)
				push(cur->left, x, res, (cmp2 >= 0), true, ResList);
		}
		if (cur->right)
		{
			int cmp2 = strcmp(cur->right->value->get_name(), x.get_name());
			if (goright)
				push(cur->right, x, res, true, (cmp2 <= 0), ResList);
		}
	}	
	else if (cmp < 0)
	{
		if (goright)
			push(cur->right, x, res, true, true, ResList);
	}
	else
	{
		if (goleft)
			push(cur->left, x, res, true, true, ResList);
	}
}


void avl_tree::trip_add(avl_node *cur, command & x, search_list & ResList, int & res, bool addres)
{
	if (cur == nullptr)
		return;
	if (x.apply(*cur->value))
	{
		search_node *add = new search_node;
		add->set_link(cur->value);
		ResList.AddToHead(add);
		if (addres)
			res++;
	}
	trip_add(cur->left, x, ResList, res, addres);
	trip_add(cur->right, x, ResList, res, addres);
}
/*
void avl_tree::trip_print(avl_node *cur, command & x, int & res)
{
	if (cur == nullptr)
		return;
	if (x.apply(*cur->value))
	{
		cur->value->print(x.get_order(), stdout);
		res++;
	}
	trip_print(cur->left, x, res);
	trip_print(cur->right, x, res);
}*/
