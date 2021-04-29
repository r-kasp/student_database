# ifndef STRUCTURE_H
# define STRUCTURE_H

#include "avl_tree.h"
#include "b_tree.h"
#include "list.h"

#define MOD 1024

class structure
{
private:
	avl_tree *avl_tree_array = nullptr;
	b_tree<list_node *> *b_tree_array = nullptr;
public:
	structure();
	~structure();
	void init(list & List, int k);
	void add(list_node *cur, int k);
	int hash(const char *string, int k);
	int hash2(int phone, int k);
	void Select(command & com, int & res, int k);
	void Insert(command & com, list & List, int k);
	void Delete(command & com, list & List, int k);
	void Push(command & com, int & res, search_list & ResList,  int k);
	void DeleteElt(list_node * curr, list & List, int k);
	void DeleteOnlyFromTree(command & com, int k);
};

# endif
