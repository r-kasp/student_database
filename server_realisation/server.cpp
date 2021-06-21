#include "record.h"
#include "list.h"
#include "command.h"
#include "command_type.h"
#include "structure.h"
#include <time.h>
#include <iostream>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <chrono>
#include <thread>
// Определимся с номером порта и другими константами.
#define BUFLEN 1234
#define MOD 1024
#define k_groups 1001
#define STOP -1
#define QUIT -2
#define ERR -3
#define OKC 0
// Две вспомогательные функции для чтения/записи (см. ниже)
int readFromClient (int fd, char *buf);
int writeToClient (int fd, const char *buf);
int ProcessClient (int fd, char *buf, list & List, structure *arr, structure & old, int k, int & res);


int main (int args, char *argv[])
{
	printf("Server is loading...\n");
	const char *list_file_name = nullptr;
	int res = 0;
	int PORT;
	double time;
	char buf[2*BUFLEN];
	int k = 0;
	if (args == 3 && sscanf(argv[2], "%d", &PORT) == 1)
	{
		list_file_name = argv[1];
	}
	else
	{
		printf("ERROR INPUT, right is : ./server a.txt 5555\n");
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
	structure *arr = new structure[k_groups];
	structure old;
	old.init(List, k);
	
	list_node *cur = List.get_head();
	while (cur != nullptr)
	{
		arr[cur->get_group()].add(cur, k);
		cur = cur->get_next();
	}
	time = clock();

	printf("Server is ready\n");
	int i, err, opt = 1;
	int sock, new_sock;
	fd_set active_set, read_set;
	struct sockaddr_in addr;
	struct sockaddr_in client;
	//char buf[BUFLEN];
	socklen_t size;
	// Создаем TCP сокет для приема запросов на соединение
	sock = socket (PF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		fprintf(stdout, "Server: cannot create socket");
		return ERR;
	}
	setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof (opt));
	// Заполняем адресную структуру и
	// связываем сокет с любым адресом
	addr.sin_family = AF_INET;
	addr.sin_port = htons (PORT);
	addr.sin_addr.s_addr = htonl (INADDR_ANY);
	err = bind (sock, (struct sockaddr *) &addr, sizeof (addr));
	if (err < 0)
	{
		fprintf(stdout, "Server: cannot bind socket");
		return ERR;
	}
	// Создаем очередь на 20 входящих запросов соединения
	err = listen (sock, 20);
	if (err < 0)
	{
		fprintf(stdout, "Server: listen queue failure");
		return ERR;
	}
	// Подготавливаем множества дескрипторов каналов ввода-вывода.
	// Для простоты не вычисляем максимальное значение дескриптора,
	// а далее будем проверять все дескрипторы вплоть до максимально
	// возможного значения FD_SETSIZE.
	FD_ZERO (&active_set);
	FD_SET (sock, &active_set);
	// Основной бесконечный цикл проверки состояния сокетов
	int result = 0;
	while (1)
	{
		// Проверим, не появились ли данные в каком-либо сокет
		// В нашем варианте ждем до фактического появления данных.
		read_set = active_set;
		if (select (FD_SETSIZE, &read_set, NULL, NULL, NULL) < 0)
		{
			fprintf (stdout, "Server: select failure");
			return ERR;
		}
		// Данные появились. Проверим в каком сокете.
		for (i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET (i, &read_set))
			{
				if (i == sock)
				{
					// пришел запрос на новое соединение
					size = sizeof (client);
					new_sock = accept (sock, (struct sockaddr *) &client, &size);
					if (new_sock < 0)
					{
						fprintf (stdout, "accept error");
						close(sock);
						return ERR;
					}
					fprintf (stdout, "Server: connect from host %s, port %d.\n",
					inet_ntoa (client.sin_addr),
					(unsigned int) ntohs (client.sin_port));
					FD_SET (new_sock, &active_set);
				}
				else
				{
					// пришли данные в уже существующем соединени
					err = readFromClient (i, buf);
					if (err < 0)
					{
						// ошибка или конец данных
						close (i);
						FD_CLR (i, &active_set);
					}
					else
					{
						char buf2[BUFLEN];
						strcpy(buf2, buf);
						res = ProcessClient(i, buf, List, arr, old, k, result);
						if (res == STOP)
						{
							writeToClient(i, "quit\n");
							close(sock);
							time = (clock()-time) / CLOCKS_PER_SEC;
							printf ("%s : Result = %d Elapsed = %.2f\n", argv[0], result, time);
							delete [] arr;
							List.delete_list();
							return OKC;
						}
						else if (res == QUIT)
						{
							writeToClient(i, "quit\n");
							close(i);
							FD_CLR (i, &active_set);
							break;
						}
						else if (res < 0)
						{
							writeToClient(i, "quit\n");
							close(i);
							FD_CLR(i, &active_set);
							close(sock);
							return ERR;
						}
						
						sprintf(buf, "OK: %s\n", buf2);
						err = writeToClient(i, buf);
						if (err < 0)
						{
							close(i);
							FD_CLR(i, &active_set);
						}
					}
				}
			}		
		}
	}
	close(sock);
	time = (clock()-time) / CLOCKS_PER_SEC;
	printf ("%s : Result = %d Elapsed = %.2f\n", argv[0], result, time);
	delete [] arr;
	List.delete_list();
	return OKC;
}



int readFromClient (int fd, char *buf)
{
	int nbytes = 0, len = 0, i, sz = sizeof(int);
	// Получаем длину сообщения
	char *arr = (char *)(& len);
	
	for (i = 0; sz > 0; i += nbytes, sz -= nbytes)
	{
		nbytes = read(fd, arr+i, sz);
		if (nbytes < 0)
		{
			fprintf(stdout, "read length");
			return ERR;
		}
		else if (nbytes == 0)
		{
			fprintf(stdout, "read length truncated");
			return ERR;
		}
	}
	// Здесь неплохо бы проверить размер буфера
	// Получаем len байт
	for (i = 0; len > 0; i += nbytes, len -= nbytes)
	{
		nbytes = read (fd, buf + i, len);
		if (nbytes < 0)
		{
			fprintf(stdout, "read");
			return ERR;
		}
		else if (nbytes == 0)
		{
			fprintf(stdout, "read truncated");
			return ERR;
		}
	}
	// Длина находится в i
	if (i == 0)
	{
		// нет данных
		fprintf (stderr, "Server: no message\n");
		return ERR;
	}
	else
	{
		// есть данные
		//fprintf (stdout, "Server got %d bytes of message: %s\n", i, buf);
		return i;
	}
}


int writeToClient (int fd, const char *buf)
{
	int nbytes = 0, len = strlen(buf) + 1, i, sz = sizeof(int);
	char *arr = (char *)(&len);
	// Пересылаем длину сообщения
	for (i = 0; sz > 0; i += nbytes, sz -= nbytes)
	{
		nbytes = write (fd, arr + i, sz);
		if (nbytes < 0)
		{
			fprintf(stdout, "write length");
			return ERR;
		}
		else if (nbytes == 0)
		{
			fprintf(stdout, "write length truncated");
			return ERR;
		}
	}
	// Пересылаем len байт
	for (i = 0; len > 0; i += nbytes, len -= nbytes)
	{
		nbytes = write (fd, buf + i, len);
		if (nbytes < 0)
		{
			fprintf(stdout, "write");
			return ERR;
		}
		else if (nbytes == 0)
		{
			fprintf(stdout, "write truncated");
			return ERR;
		}
	}
	return 0;
}


int ProcessClient (int fd, char *buf, list & List, structure *arr, structure & old, int k, int & res)
{
	command com;
	if (strcmp(buf, "stop") == 0 || strcmp(buf, "stop;") == 0)
		return STOP;
	if (strcmp(buf, "quit") == 0 || strcmp(buf, "quit;") == 0)
		return QUIT;	

	char bufans[BUFLEN];
	sprintf(bufans, "\nQuery : \n");
	sprintf(bufans, "%s", buf);
	sprintf(bufans, "\n");
	
	if (!com.parse(buf))
	{
		sprintf(bufans, "ERROR : wrong request\n");
		int error = writeToClient(fd, bufans);
		if (error < 0)
		{
			return ERR;
		}
	}
	else
	{
		int error = writeToClient(fd, bufans);
		if (error < 0)
		{
			return ERR;
		}
		if (com.get_type() == command_type::select)
		{
			search_list ResList;
			if (com.get_c_group() != condition::none && com.get_op() != operation::lor
				&& (com.get_c_name() == condition::eq || com.get_c_phone() == condition::eq))
			{
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
			}
			else
			{
				old.Push(com, res, ResList, k);
			}
			
			if (com.get_order_by()[0] != ordering::none)
				ResList.mergeSort(com.get_order_by());
			//sprintf to buf
			//ResList.print_list(com.get_order(), bufans);
			search_node* v = ResList.get_head();
			while (v != nullptr)
			{
				res++;
				v->get_link()->print(com.get_order(), bufans);
				error = writeToClient(fd, bufans);
				if (error < 0)
				{
					ResList.delete_list();
					return ERR;
				}
				v = v->get_next();
			}
			ResList.delete_list();
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
	}
	return OKC;
}

