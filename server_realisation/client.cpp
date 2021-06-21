#include <iostream>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
// Определимся с портом, адресом сервера и другими константами.
// В данном случае берем произвольный порт и адрес обратной связи
// (тестируем на одной машине).
#define BUFLEN 1234
#define ERROR -1
#define OK 0
// Две вспомогательные функции для чтения/записи (см. ниже)
int writeToServer (int fd, const char *buf);
int readFromServer (int fd, char *buf);

int main (int args, char *argv[])
{
	int SERVER_PORT = 5555;
	char SERVER_NAME[BUFLEN];
	if (args == 3 && sscanf(argv[2], "%d", &SERVER_PORT) == 1)
	{
		strcpy(SERVER_NAME, argv[1]);
	}
	else
	{
		printf("WRONG INPUT, right is : ./client 127.0.0.1 5555\n");
		return 1; 
	}
	
	//int i;
	int err;
	int sock;
	struct sockaddr_in server_addr;
	struct hostent *hostinfo;
	// Получаем информацию о сервере по его DNS имени
	// или точечной нотации IP адреса.
	hostinfo = gethostbyname (SERVER_NAME);
	if (hostinfo == NULL)
	{
		fprintf (stderr, "Unknown host %s.\n", SERVER_NAME);
		exit (EXIT_FAILURE);
	}
	// Заполняем адресную структуру для последующего
	// использования при установлении соединения
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons (SERVER_PORT);
	server_addr.sin_addr = *(struct in_addr *) hostinfo->h_addr;
	//for (i=0;i<100000;i++) {
	// Создаем TCP сокет.
	sock = socket (PF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror ("Client: socket was not created");
		exit (EXIT_FAILURE);
	}
	// Устанавливаем соединение с сервером
	err = connect (sock, (struct sockaddr *) &server_addr, sizeof (server_addr));
	if (err < 0)
	{
		perror ("Client: connect failure");
		exit (EXIT_FAILURE);
	}
	//fprintf (stdout, "Connection is ready\n");
	// Обмениваемся данными
	char buf[BUFLEN];
	char buf2[BUFLEN];
	buf2[0] = '\0';
	int ind = 0;
	char c;
	(void)c;
	/*
	while (fgets(buf, BUFLEN, stdin))
	{
		ind = strlen(buf)-1;
		if (buf[ind] != ';')
			ind--;
		buf[ind] = '\0';
		if (ind != 0)
			err = writeToServer(sock, buf);
		if (err < 0)
		{
			close(sock);
			return ERROR;
		}
		buf2[0] = '\0';
		while (strcmp(buf2, "quit\n;") != 0 && strncmp(buf2, "OK", 2) != 0 && strncmp(buf2, "ERROR", 5) != 0)
		{
			err = readFromServer(sock, buf2);
			if (err < 0)
			{
				close(sock);
				return ERROR;
			}
		}
		if (strcmp(buf2, "quit\n") == 0)
		{
			close(sock);
			return OK;
		}
	}*/
	while ((c = fgetc(stdin)) != EOF)
	{
		if (c == ';')
		{
			buf[ind] = '\0';
			err = writeToServer(sock, buf);
			if (err < 0)
			{
				close(sock);
				return ERROR;
			}
			buf2[0] = '\0';
			while (strncmp(buf2, "quit", 4) && strncmp(buf2, "OK", 2) && strncmp(buf2, "ERROR", 5))
			{
				err = readFromServer(sock, buf2);
				if (err < 0)
				{
					close(sock);
					return ERROR;
				}
			}
			if (strncmp(buf2, "quit", 4) == 0)
			{
				close(sock);
				return OK;
			}
			ind = 0;
		}
		else
		{
			buf[ind] = c;
			ind++;
		}
	}
	if (strncmp(buf2, "quit", 4))
	{
		writeToServer(sock, "quit");
		err = readFromServer(sock, buf2);
		if (err < 0)
		{
			close(sock);
			return ERROR;
		}
	}
	// Закрываем socket
	close (sock);
	//}
	return OK;
}



int writeToServer (int fd, const char *buf)
{
	int nbytes = 0, len = strlen(buf)+1, i, sz = sizeof(int);
	char *arr = (char *)(& len);
	//fprintf (stdout, "Send to server > ");
	// Пересылаем длину сообщения
	for (i = 0; sz > 0; i += nbytes, sz -= nbytes)
	{
		nbytes = write (fd, arr + i, sz);
		if (nbytes < 0)
		{
			fprintf(stdout, "write length");
			return ERROR;
		}
		else if (nbytes == 0)
		{
			fprintf(stdout, "write length truncated");
			return ERROR;
		}
	}
	// Пересылаем len байт
	for (i = 0; len > 0; i += nbytes, len -= nbytes)
	{
		nbytes = write (fd, buf + i, len);
		if (nbytes < 0)
		{
			fprintf(stdout, "write");
			return ERROR;
		}
		else if (nbytes == 0)
		{
			fprintf(stdout, "write truncated");
			return ERROR;
		}
	}
	return OK;
}


int readFromServer (int fd, char *buf)
{
	int nbytes = 0, len = 0, i, sz = sizeof(int);
	char *arr = (char *)(& len);
	// Получаем длину сообщения
	for (i = 0; sz > 0; i += nbytes, sz -= nbytes)
	{
		nbytes = read(fd, arr+i, sz);
		if (nbytes < 0)
		{
			fprintf(stdout, "read length");
			return ERROR;
		}
		else if (nbytes == 0)
		{
			fprintf (stdout, "read length truncated");
			return ERROR;
		}
	}
	/*
	if (read (fd, &len, sizeof (int)) != (int) sizeof (int))
	{
		// ошибка чтения
		perror ("read length");
		exit (EXIT_FAILURE);
	}*/
	
	// Получаем len байт
	for (i = 0; len > 0; i += nbytes, len -= nbytes)
	{
		nbytes = read (fd, buf + i, len);
		if (nbytes < 0)
		{
			fprintf(stdout, "read");
			return ERROR;
		}
		else if (nbytes == 0)
		{
			fprintf(stdout, "read truncated");
			return ERROR;
		}
	}
	// Длина находится в i
	if (i == 0)
	{
		// нет данных
		fprintf (stderr, "Client: no message\n");
	}
	else if (buf[0] == ' '  || strncmp(buf, "ERROR", 5) != 0)
	{
		// ответ успешно прочитан
		fprintf (stdout,"%s", buf);
	}
	return 0;
}
