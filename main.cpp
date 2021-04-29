#include "record.h"
#include "list.h"
#include "command.h"
#include "command_type.h"
#include "structure.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <iostream>

#define LEN 1234
#define MOD 1024
#define k_groups 1001


int main(int args, char *argv[])
{
	const char *list_file_name = nullptr;
	int res = 0;
	double time;
	char buf[LEN];
	int k = 0;
	if (args > 1)
		list_file_name = argv[1];
	else
	{
		printf("ERROR INPUT\n");
		return 1;
	}
	//config.txt
	const char *path = argv[0];
	int ind = strlen(path);
	while (ind >= 0 && path[ind] != '/')
		ind--;
	char newpath[LEN];
	strncpy(newpath, path, ind);
	newpath[ind] = '\0';
	char str[11] = {'/', 'c', 'o', 'n', 'f', 'i', 'g', '.', 't', 'x', 't'};
	for (int i = 0; i < 11; i++)
	{
		newpath[ind] = str[i];
		ind++;
	}
	newpath[ind] = '\0';
	FILE *fp = fopen(newpath, "r");
	if (!fp)
	{
		printf("config.txt is absent\n");
		return 2;
	}
	while (fgets(buf, LEN, fp))
	{
		if (buf[0] == '#')
			continue;
		int ind = 0;
		while (buf[ind] != '\0' && (buf[ind] == '\t' || buf[ind] == ' ' || buf[ind] == '\n'))
			ind++;
		if (buf[ind] == '\0')
			continue;
		k = 0;
		while (buf[ind] != '\0' && (buf[ind] >= '0' && buf[ind] <= '9'))
		{
			k = 10*k + buf[ind] - '0';
			ind++;
		}
		while (buf[ind] != '\0' && (buf[ind] == '\t' || buf[ind] == ' ' || buf[ind] == '\n'))
			ind++;
		if (buf[ind] != '\0')
		{
			fclose(fp);
			printf("Wrong input for config.txt\n");
			return 3;
		}
	}
	fclose(fp);
	//end
	
	
	list List;
	res = List.read_list(list_file_name);
	if (res != (int)errors::OK)
	{
		List.delete_list();
		printf("ERROR\n");
		return 4;
	}
	time = clock();
	structure *arr = new structure[k_groups];
	structure old;
	old.init(List, k);
	
	list_node *cur = List.get_head();
	while (cur != nullptr)
	{
		arr[cur->get_group()].add(cur, k);
		cur = cur->get_next();
	}
	
	while (true)
	{
		int ind = 0;
		char c;
		bool flag = false;
		while ((c = fgetc(stdin)) != EOF)
		{
			flag = true;
			if (c == ';')
			{
				break;
			}
			buf[ind] = c;
			ind++;
		}
		//If we don't have quit;
		if (!flag)
			break;
		buf[ind] = '\0';
		command com;
		if (!com.parse(buf))
		{
			if (!(buf[1] == '\0' && buf[0] == '\n'))
				printf("Wrong request\n");
		}
		else
		{
			printf("\nQuery : %s\n", buf);
			if (com.get_type() == command_type::select)
			{
				if (com.get_order_by()[0] == ordering::none)
				{
					if (com.get_c_group() != condition::none && com.get_op() != operation::lor
						&& (com.get_c_name() == condition::eq || com.get_c_phone() == condition::eq))
					{
						int g = com.get_group();
						if (com.get_c_group() == condition::eq)
						{
							arr[g].Select(com, res, k);
						}
						else if (com.get_c_group() == condition::ne)
						{
							for (int i = 0; i < g; i++)
								arr[i].Select(com, res, k);
							for (int i = g+1; i < k_groups; i++)
								arr[i].Select(com, res, k);
						}
						else if (com.get_c_group() == condition::lt)
						{
							for (int i = 0; i < g; i++)
								arr[i].Select(com, res, k);
						}
						else if (com.get_c_group() == condition::gt)
						{
							for (int i = g+1; i < k_groups; i++)
								arr[i].Select(com, res, k);
						}
						else if (com.get_c_group() == condition::le)
						{
							for (int i = 0; i <= g; i++)
								arr[i].Select(com, res, k);
						}
						else if (com.get_c_group() == condition::ge)
						{
							for (int i = g; i < k_groups; i++)
								arr[i].Select(com, res, k);
						}
					}
					else
					{
						old.Select(com, res, k);		
					}
				}
				else
				{	
					if (com.get_c_group() != condition::none && com.get_op() != operation::lor
						&& (com.get_c_name() == condition::eq || com.get_c_phone() == condition::eq))
					{
						search_list ResList;
						int g = com.get_group();
						if (com.get_c_group() == condition::eq)
						{
							arr[g].Push(com, res, ResList, k);
						}
						else if (com.get_c_group() == condition::ne)
						{
							for (int i = 0; i < g; i++)
								arr[i].Push(com, res, ResList, k);
							for (int i = g+1; i < k_groups; i++)
								arr[i].Push(com, res, ResList, k);
						}
						else if (com.get_c_group() == condition::lt)
						{
							for (int i = 0; i < g; i++)
								arr[i].Push(com, res, ResList, k);
						}
						else if (com.get_c_group() == condition::gt)
						{
							for (int i = g+1; i < k_groups; i++)
								arr[i].Push(com, res, ResList, k);
						}
						else if (com.get_c_group() == condition::le)
						{
							for (int i = 0; i <= g; i++)
								arr[i].Push(com, res, ResList, k);
						}
						else if (com.get_c_group() == condition::ge)
						{
							for (int i = g; i < k_groups; i++)
								arr[i].Push(com, res, ResList, k);
						}
						ResList.mergeSort(com.get_order_by());
						ResList.print_list(com.get_order(), stdout);
						ResList.delete_list();
					}
					else
					{
						old.Select(com, res, k);
					}
				}
			}
			else if (com.get_type() == command_type::insert)
			{
				arr[com.get_group()].Insert(com, List, k);
				old.Insert(com, List, k);
			}
			else if (com.get_type() == command_type::del)
			{
				old.DeleteOnlyFromTree(com, k);
				
				if (com.get_c_group() != condition::none && com.get_op() != operation::lor
					&& (com.get_c_name() == condition::eq || com.get_c_phone() == condition::eq))
				{
					int g = com.get_group();
					if (com.get_c_group() == condition::eq)
					{
						arr[g].Delete(com, List, k);
					}
					else if (com.get_c_group() == condition::ne)
					{
						for (int i = 0; i < g; i++)
							arr[i].Delete(com, List, k);
						for (int i = g+1; i < k_groups; i++)
							arr[i].Delete(com, List, k);
					}
					else if (com.get_c_group() == condition::lt)
					{
						for (int i = 0; i < g; i++)
							arr[i].Delete(com, List, k);
					}
					else if (com.get_c_group() == condition::gt)
					{
						for (int i = g+1; i < k_groups; i++)
							arr[i].Delete(com, List, k);
					}
					else if (com.get_c_group() == condition::le)
					{
						for (int i = 0; i <= g; i++)
							arr[i].Delete(com, List, k);
					}
					else if (com.get_c_group() == condition::ge)
					{
						for (int i = g; i < k_groups; i++)
							arr[i].Delete(com, List, k);
					}
				}
				else
				{
					for (int i = 0; i < k_groups; i++)
						arr[i].Delete(com, List, k);
				}
			}
			else //quit;
			{
				printf("\n");
				break;
			}
			printf("\n");
		}
	}
	time = (clock()-time) / CLOCKS_PER_SEC;
	printf ("%s : Result = %d Elapsed = %.2f\n", argv[0], res, time);
	delete [] arr;
	List.delete_list();
	return 0;
}
