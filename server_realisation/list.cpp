#include "list.h"
#include <iostream>

void list::delete_list()
{
	list_node *curr;
	while (head != nullptr)
	{
		curr = head->next;
		delete head;
		head = curr;
	}
	head = nullptr;
}


void list::print_list(const ordering order[], char *buf)
{
	int i = 0;
	list_node *v = head;
	while (v != nullptr)
	{
		v->print(order, buf);
		v = v->next;
		i++;
	}
}


int list::read_list(const char *name)
{
	FILE *fp;
	list_node *tail, *cur;
	int res;
	fp = fopen(name, "r");
	if (!fp)
		return (int)errors::ERROR_OPEN;
	head = new list_node;
	res = (int)head->read(fp);
	if (res != (int)errors::OK)
	{
		delete head;
		head = nullptr;
		fclose(fp);
		return res;
	}
	tail = head;
	while (1)
	{
		cur = new list_node;
		res = (int)cur->read(fp);
		if (res != (int)errors::OK)
		{
			delete cur;
			if (feof(fp))
				break;
			tail->next = nullptr;
			delete_list();
			fclose(fp);
			return (int)errors::ERROR_READ;
		}
		cur->prev = tail;
		tail->next = cur;
		tail = tail->next;
	}
	if (!feof(fp))
	{
		delete_list();
		fclose(fp);
		return (int)errors::ERROR_READ;
	}
	tail->next = nullptr;
	head->prev = nullptr;
	fclose(fp);
	return (int)errors::OK;
}



void list::DeleteFromList(list_node *v)
{
	if (v == nullptr)
		return;
	if (v->prev == nullptr)
	{
		if (v->next == nullptr)
		{
			head = nullptr;
		}
		else
		{
			head = head->next;	
			head->prev = nullptr;
		}	
	}
	else 
	{
		if (v->next == nullptr)
		{
			v->prev->next = nullptr;
		}
		else
		{
			v->prev->next = v->next;
			v->next->prev = v->prev;
		}
	}
	delete v;
}


void list::AddToHead(list_node * add)
{
	if (add == nullptr)
		return;
	add->next = head;
	if (head != nullptr)
		head->prev = add;
	add->prev = nullptr;
	head = add;
}


void list::AddTo(list_node * v, list_node * add)
{
	if (v == nullptr)
	{
		head = add;
		add->prev = nullptr;
		add->next = nullptr;
	}
	else
	{
		v->next = add;
		add->prev = v;
		add->next = nullptr;
	}
}


search_node* search_list::merge(search_node *A, search_node *B, search_node *C, ordering order_by[max_items])
{
	search_node *va = A, *vb = B, *vc;
	if (C == nullptr)
	{
		if (va == nullptr && vb != nullptr)
		{
			head = vb;
			vb = vb->next;
		}
		else if (vb == nullptr && va != nullptr)
		{
			head = va;
			va = va->next;
		}
		else if (va != nullptr && vb != nullptr)
		{
			if ((va->link)->compare(order_by, *(vb->link))) //*va < *vb
			{
				head = va;
				va = va->next;
			}
			else
			{
				head = vb;
				vb = vb->next;
			}
		}
		vc = head;
	}
	else
		vc = C;
	while (va != nullptr && vb != nullptr)
	{
		if ((va->link)->compare(order_by, *(vb->link)))
		{
			vc->next = va;
			va = va->next;
		}
		else
		{
			vc->next = vb;
			vb = vb->next;
		}
		vc = vc->next;
	}
	while (va != nullptr)
	{
		vc->next = va;
		vc = vc->next;
		va = va->next;
	}
	while (vb != nullptr)
	{
		vc->next = vb;
		vc = vc->next;
		vb = vb->next;
	}
	vc->next = nullptr;
	return vc;
}


void search_list::mergeSort(ordering order_by[max_items])
{
	if (head == nullptr)
		return;
	search_list *blist = new search_list;
	search_node *v = head, *ahead, *bhead, *va, *vb, *vc, *buf;
	int sz = 1, len = 0, i, j;
	while (v != nullptr)
	{
		len++;
		v = v->next;
	}
	while (sz < len)
	{
		i = 1;
		v = head;
		vc = nullptr;
		while (i < len)
		{
			va = v;
			ahead = va;
			if (v != nullptr)
			{
				i++;
				v = v->next;
			}
			j = 1;
			while (v != nullptr && j < sz)
			{
				va->next = v;
				va = va->next;
				v = v->next;
				i++;
				j++;
			}
			if (va != nullptr)
			{
				va->next = nullptr;
			}
			vb = v;
			bhead = vb;
			j = 1;
			if (v != nullptr)
			{
				i++;
				v = v->next;
			}
			while (v != nullptr && j < sz)
			{
				vb->next = v;
				vb = vb->next;
				v = v->next;
				j++;
				i++;
			}
			if (vb != nullptr)
			{
				vb->next = nullptr;
			}
			vc = blist->merge(ahead, bhead, vc, order_by);
		}
		while (v != nullptr)
		{
			vc->next = v;
			vc = vc->next;
			v = v->next;
		}
		buf = blist->head;
		blist->head = head;
		head = buf;
		blist->head = nullptr;
		sz *= 2;
	}
	delete blist;
}


void search_list::AddTo(search_node * v, search_node * add)
{
	if (v == nullptr)
	{
		head = add;
		add->next = nullptr;
	}
	else
	{
		v->next = add;
		add->next = nullptr;
	}
}

void search_list::print_list(const ordering order[], char *buf)
{
	int i = 0;
	search_node *v = head;
	while (v != nullptr)
	{
		v->link->print(order, buf);
		v = v->next;
		i++;
	}
}

void search_list::delete_list()
{
	search_node *curr;
	while (head != nullptr)
	{
		curr = head->next;
		delete head;
		head = curr;
	}
	head = nullptr;
}


void search_list::AddToHead(search_node * add)
{
	if (add == nullptr)
		return;
	add->next = head;
	head = add;
}
