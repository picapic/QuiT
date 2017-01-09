//������� v1.1.7

#include"quit.h"

#define MAX_HEAD_SIZE 40960
#define MAX_CMD_SIZE 40960
#define MAX_BUFF_SIZE 81920
#define TAB_BUFF_UP 128
#define MAX_COLUMNS_IN_TABLE 256
#define MAX_VALUES_IN_SELECT 256
#define FREE_ALL_VARS	delete [] cmd;\
			delete [] column_buff;\
			delete [] main_tab;\
			delete [] tab_info_name;\
			delete [] tab_info_values;\
			delete [] tab_columns_name;\
			delete [] tab_columns_values;\
			delete [] tab_default_name;\
			delete [] tab_default_values;\
			delete [] all_tables_name;\
			delete [] all_tables_values;\
			for(i = 0; i < column_index; ++i)\
			{\
				if(columns_sql_buff[i] != NULL)\
					delete columns_sql_buff[i];\
			}\
			for(i = 0; i < value_of_select_index; ++i)\
			{\
				if(value_of_select[i][0] != NULL)\
					delete value_of_select[i][0];\
				if(value_of_select[i][1] != NULL)\
					delete value_of_select[i][1];\
			}
#define FREE_DATA_OF_COLUMNS	for(i = 0; i < cols_num; ++i)\
				{\
					if(tab_col[i] != NULL)\
					{\
						delete [] tab_col[i]->name;\
						delete [] tab_col[i]->html_code;\
						delete [] tab_col[i]->html_hat;\
						delete [] tab_col[i]->col_hat;\
						if(tab_col[i]->selects != NULL)\
						{\
							for(j = 0; j < MAX_VALUES_IN_SELECT; ++j)\
							{\
								if(tab_col[i]->selects->values[j] != NULL)\
									delete [] tab_col[i]->selects->values[j];\
								if(tab_col[i]->selects->selects[j] != NULL)\
									delete [] tab_col[i]->selects->selects[j];\
							}\
							delete tab_col[i]->selects;\
						}\
						while(tab_col[i]->dynamic != NULL)\
						{\
							select_values *svlist;\
							svlist = tab_col[i]->dynamic->svnext;\
							for(j = 0; j < MAX_VALUES_IN_SELECT; ++j)\
							{\
								if(tab_col[i]->dynamic->values[j] != NULL)\
									delete [] tab_col[i]->dynamic->values[j];\
								if(tab_col[i]->dynamic->selects[j] != NULL)\
									delete [] tab_col[i]->dynamic->selects[j];\
							}\
							delete tab_col[i]->dynamic;\
							tab_col[i]->dynamic = svlist;\
						}\
						delete tab_col[i];\
					}\
				}


struct id_value_data_struct;
struct table_column;
struct select_values;

struct id_value_data_struct {
	int id;
	char *name;
	char *value;
	int index;
};

struct table_column {
	int id;
	int tab_id;
	char *name;
	int type;
	char *html_code;
	char *html_hat;
	char *col_hat;
	int col_size;
	int num;
	int sort;
	int desc;
	int col_desc_id;
	select_values *selects;
	select_values *dynamic;
};

struct select_values {
	int count;
	int tab_index; //��� ������������ ������������ ������� ����� ������ (��-���� ��� id �������)
	char *values[MAX_VALUES_IN_SELECT];
	char *selects[MAX_VALUES_IN_SELECT];
	select_values *svnext; //��������� ������
};

void mysql_error_thread_exit(MYSQL * const, SOCKET * const);
void error_thread_exit(MYSQL * const, SOCKET * const, char const * const);
char *get_head_value(char * const, char const * const);
int get_some_value(char * const, char const * const, char * const);
int get_id_value(char * const, id_value_data_struct * const);
void run_commands(char * const, MYSQL * const, SOCKET * const);
char *make_lt_gt(char const * const);
char *make_style(char const * const);
void cp_to_utf8(char *out_text, const char *str, int from_cp, size_t buf_size);
void utf8_to_cp(char *out_text, const char *str, int to_cp, size_t buf_size);

void create_new_table(MYSQL * const, SOCKET * const);
void delete_table(char const * const, MYSQL * const, SOCKET * const);

#ifdef WIN32
DWORD WINAPI
#else
void *
#endif
ProcessingThread(void *lpParams);

int PORTNUM = 7770;

int main(int argc, char *argv[])
{
  SOCKET s, ns;
#ifdef WIN32
  WSADATA wsaData;
#endif
  int nport, k;
  struct sockaddr_in serv_addr, clnt_addr;
  char ProductName[] = "QuiT";
  char Version[] = "v1.1.7";
#ifdef WIN32
  NOTIFYICONDATA idata;
  static const GUID myGUID = {0x23977b55, 0x10e0, 0x4041, {0xb8, 0x62, 0xb1, 0x95, 0x41, 0x96, 0x36, 0x69}};
#else
  int STACK_SZ = 32;
  int stack_size;
  pthread_attr_t attr;
#endif

  bool fsilentmode;

  fsilentmode = false;


  //��������� ���������� � ������ �� ���������
  if(argc > 1)
  {
	  for(k = 1; k < argc; ++k)
	  {
		  if(strncmp(argv[k], "-s", 2) == 0)
		  {
			fsilentmode = true;
		  }
		  if(strncmp(argv[k], "-p", 2) == 0 && argc > k)
		  {
			  nport = atoi(argv[k+1]);
			  if(nport > 0)
				PORTNUM = nport;
		  }
		  if(strncmp(argv[k], "-h", 2) == 0)
		  {
			  fprintf(stderr, "%s %s\nUsage: %s [-h] [-p port] [-s]\n\n\t\t-h\t\thelp (this message)\n\t\t-p port\t\tport number (is 7770 by default)\n\t\t-s\t\tsilent mode\n\n", ProductName, Version, ProductName);
			  exit(0);
		  }
	  }
  }
#ifdef WIN32
  if(fsilentmode)
  {
	memset(&idata, 0, sizeof(idata));
	idata.cbSize = sizeof(idata);
	idata.hWnd = GetConsoleWindow();
	idata.guidItem = myGUID;
	idata.uFlags = NIF_ICON | NIF_TIP | NIF_GUID;
	idata.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	StringCchCopy(idata.szTip, ARRAYSIZE(idata.szTip), ProductName);
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	Shell_NotifyIcon(NIM_ADD,&idata);
  }

  WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
  nport = PORTNUM;
  nport = htons((u_short)nport);

  if((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    fprintf(stderr, "error: socket()\n");
    exit(1);
  }

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = nport;

  if(bind(s, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
  {
    fprintf(stderr, "error: bind()\n");
#ifdef LINUX
		shutdown(s, SHUT_RDWR);
#endif
    closesocket(s);
    exit(2);
  }

  fprintf(stderr, "%s %s\n", ProductName, Version);
  fprintf(stderr, "server is ready: port %d\n", PORTNUM);

  if(listen(s, 20) == -1)
  {
    fprintf(stderr, "error: listen()\n");
#ifdef LINUX
		shutdown(s, SHUT_RDWR);
#endif
    closesocket(s);
    exit(3);
  }
								
  printf("begin..\n");

  while(1)
  {
    int addrlen;
  	DWORD dwThread;

    memset(&clnt_addr, 0, sizeof(clnt_addr));
    addrlen = sizeof(clnt_addr);

	//��� ����������
    if((ns = accept(s, (struct sockaddr *) &clnt_addr, (socklen_t *)&addrlen)) == -1)
    {
      fprintf(stderr, "error: accept()\n");
#ifdef LINUX
		shutdown(s, SHUT_RDWR);
#endif
      closesocket(s);
      exit(4);
    }

    fprintf(stderr, "client = %s\n", inet_ntoa(clnt_addr.sin_addr));

#ifdef WIN32
    //��������� ����� ��������� ��������� ����������
    CreateThread(NULL, 0, ProcessingThread, (LPVOID)&ns, 0, &dwThread);
#else
    stack_size = 1024*1024 + STACK_SZ*1024; //1024K + 32K

    if(pthread_attr_init(&attr) != 0)
    {
      perror("attr init ");
    }

    if(pthread_attr_setstacksize(&attr, stack_size) != 0)
    {
      perror("setstacksize ");
    }

    if(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0)
    {
      perror("setdetachstate ");
    }

    if(pthread_create((pthread_t *)&dwThread, &attr, ProcessingThread, (void *) &ns) != 0)
    {
      perror("pthread_create (servers) ");
      usleep(100000);
    }

    if(pthread_attr_destroy(&attr) != 0)
    {
      perror("attr destroy ");
    }
#endif
  }
#ifdef WIN32
  WSACleanup();
#endif

  return 0;
}

#ifdef WIN32
DWORD WINAPI
#else
void *
#endif
ProcessingThread(void *lpParams)
{
	SOCKET ns, check_s;
	int nport;
	struct sockaddr_in clnt_addr;
  struct hostent *hp;
	MYSQL mysql;
	MYSQL_RES *res, *res_tbl, *res_col, *res_srv, *res_dsc;
	MYSQL_ROW row, row_tbl, row_col, row_srv, row_dsc;
	int num, num_tbl, num_col, num_srv, num_dsc;
	char head[MAX_HEAD_SIZE];		//������������ � �������� ������!!!
	char cmd[MAX_CMD_SIZE];		//����� ������ �������
	char buf[MAX_BUFF_SIZE];		//����� ������������� html-���������
	char main_table[256];	//����� ��� ����� ������� �������
	char main_server[256];   //����� ��� ����� �����
	char next_table[256];	//����� ����� ������� ��� ��������
	char command_list[2048]; //����� ��� ������ ������
	int nbytes, nsumbytes;		//���������� ��������/������������ ����
	char *send_data;
	size_t nbytes_sent, size_to_send;
	bool ffirst, fselects, fdynamic, fnodelim, fdelrow, fnewtable;
	int i, j, k, l, m, cols_num, index_row;
	int area_width, area_width_min;
	int len, max, data_size;
	char *ptr, *port_ptr, *ptr_del;
	struct id_value_data_struct id_value_data;
	struct table_column *tab_col[MAX_COLUMNS_IN_TABLE]; //������������� ����������� �� ���������� �������� � �������
	char Author[]="Bombo";

	fselects = false; //��������� ������������� ����� "������� �������� ���� ������"
	fdynamic = false; // -||- "������� �������� ���� �������"
	fdelrow = false;  // -||- "������� ������"
	fnewtable = false;// -||- "�������� ����� �������"

	ns = *((SOCKET *)lpParams);
	area_width = 20; //������ ������� � ��������
  max = 0;

  Author[0] = 'B'; //non warning

	if(!mysql_init(&mysql))
	{
	  fprintf(stderr, "error: mysql_init()\n");
	  closesocket(ns);
#ifdef WIN32	  
	  ExitThread(1);
#else
    pthread_exit(NULL);
#endif	  
	}

	if(!(mysql_real_connect(&mysql, 
						  "localhost",
						  "picsuser", "passwd", "quit", 0, NULL, CLIENT_FOUND_ROWS)))
	{	
		fprintf(stderr, "error: connection to DB (%s)\n", mysql_error(&mysql));
		mysql_close(&mysql);
		closesocket(ns);
#ifdef WIN32	  
	  ExitThread(6);
#else
    pthread_exit(NULL);
#endif	  
	}
	else
	fprintf(stderr, "opened!\n");

	//  mysql_query(&mysql, "set CHARACTER SET utf8");
	mysql_query(&mysql, "SET NAMES 'cp1251'");
	mysql_query(&mysql, "SET CHARACTER SET 'cp1251'");
	mysql_query(&mysql, "SET @@character_set_connection='cp1251'");
	mysql_query(&mysql, "SET @@character_set_result='cp1251'");
	mysql_query(&mysql, "SET @@character_set_client='cp1251'");
//	mysql_query(&mysql, "set sql_mode='NO_BACKSLASH_ESCAPES'"); //����� MySQL �� ������� �� '\'

	memset(buf, 0, sizeof(buf));
	nbytes = 0;
	nsumbytes = 0;
	data_size = 0;

  nbytes = recv(ns, buf+nsumbytes, sizeof(buf), 0);
	nsumbytes += nbytes;
	if(nbytes <= 0)
	{
		fprintf(stderr, "nbytes <= 0\n");
		mysql_close(&mysql);      
		closesocket(ns);
#ifdef WIN32	  
	  ExitThread(60);
#else
    pthread_exit(NULL);
#endif	  
	}
  fprintf(stderr, "\n\n%s\n\n", buf); //�������!!!
  if(strncmp(buf, "POST ", 5) == 0)
  {
	  ptr = strstr(buf, "Content-Length: ");
	  if(ptr != NULL)
	  {
	    ptr_del = strstr(buf, "\r\n\r\n");
	    if(ptr_del != NULL)
	    {
		 		data_size = (ptr_del-buf)/sizeof(char)+4+atoi(ptr+16);
		 	}
	  }
//	  fprintf(stderr, "\n\natoi(ptr+16)=%d\nlen=%d\ndata_size=%d\n\n", atoi(ptr+16),strlen(buf)-((ptr_del-buf)/sizeof(char)+4),data_size); //�������!!!

		while((unsigned int)nsumbytes < sizeof(buf) && (unsigned int)nsumbytes < (unsigned int)data_size)
		{
			nbytes = recv(ns, buf+nsumbytes, sizeof(buf), 0);
			nsumbytes += nbytes;
		}
  }
  
//	fprintf(stderr, "\nrecv_buf = %s\n\n\n", buf); //�������!!!
	if(nsumbytes <= 0)
	{
		fprintf(stderr, "nsumbytes <= 0\n");
		mysql_close(&mysql);      
		closesocket(ns);
#ifdef WIN32	  
	  ExitThread(60);
#else
    pthread_exit(NULL);
#endif	  
	}
	if(strncmp(buf, "POST ", 5) == 0)
	{
	  ptr = get_head_value(buf, "POST /");
	  //�������� �������� �������� �������
	  memset(main_table, 0, sizeof(main_table));
	  for(i = 0; ptr[i] != ' '; ++i)
	  {
		  main_table[i] = ptr[i];
	  }

	  //����� �������� �������� ������� ��� �������� ������� ��� ��������
	  memset(next_table, 0, sizeof(next_table));
	  strcpy_s(next_table, sizeof(next_table), main_table);

	  //�������� ����� ����������� ���������
	  ptr = get_head_value(buf, "Content-Length: ");
	  if(ptr == NULL)
		len = 0;
	  else
		len = atoi(ptr);
	  
	  //���� ��������� �� ������ - ���������� ��� ������
	  //������ ������� ������� ������ �� �������������
	  if(len > 0)
	  {
	    ptr = strstr(buf, "\r\n\r\n");
	    ptr += 4; //������������� �� ������ ������

		memset(cmd, 0, sizeof(cmd));
		while(get_some_value(ptr, "make_some_request", cmd)) //�������� ����� �������
		{
			//��������� ������
			if(mysql_query(&mysql, cmd) != 0)
			{
				mysql_error_thread_exit(&mysql, &ns);
			}
		}

		memset(cmd, 0, sizeof(cmd));
		if(get_some_value(ptr, "delete_table_from_database", cmd)) //�������� ����� ������� �� ���������� ��������
		{
			//������� �������
			delete_table(cmd, &mysql, &ns);
		}

		memset(cmd, 0, sizeof(cmd));
		if(get_some_value(ptr, "insert_new_default_value", cmd)) //�������� ����� ������� �� ���������� ��������
		{
			//��������� ������ � �������
			if(mysql_query(&mysql, cmd) != 0)
			{
				mysql_error_thread_exit(&mysql, &ns);
			}
		}

	    memset(cmd, 0, sizeof(cmd));
		if(get_some_value(ptr, "next_table_name", cmd)) //�������� ��� ������� ��� ��������
		{
			if(strlen(cmd) > 0)
			{
				memset(next_table, 0, sizeof(next_table));
				strcpy_s(next_table, sizeof(next_table), cmd); //���� ���� ������ ������� ��� ��������, ���������� � ���
			}
		}

		if(get_some_value(ptr, "delete_row_mode", cmd)) //����� �������� �����
		{
			if(strlen(cmd) > 0 && strncmp(cmd, "yes", 3) == 0)
			{
				fdelrow = true; //����� ����������� �������� �����
			}
		}

	    memset(cmd, 0, sizeof(cmd));
		memset(command_list, 0, sizeof(command_list));
		if(get_some_value(ptr, "command_list", cmd)) //�������� ������ ������
		{
			strcpy_s(command_list, sizeof(command_list), cmd);

			if(strstr(command_list, "create_new_table") != NULL)
			{
				fnewtable = true;
			}
		}

		len = strlen(ptr); //�.�. ���������� ptr ����� ����������
		
		while(len > 0)
		{
	      len = get_id_value(ptr, &id_value_data); //������� ���������� id ����������� ��������, ��� ������� � ����� ��������. 
		                                           //���������� ���������� ����� ���������.
												   //������ ������� %XX, ������������ �������, ���� ������� ��� ��������.
//		  fprintf(stderr, "len = %d\n", len);
		  if(len < 0) continue;
		  ptr = ptr + id_value_data.index;
		  memset(cmd, 0, sizeof(cmd));
		  if(strncmp(main_table, "server_info", 11) == 0 && strncmp(id_value_data.name, "addr", 4) == 0) //���� �������� ����� �������
		  {
			  //���������, ������� �� ���� �����
			  fprintf(stderr, "checking...\n");
			  memset(main_server, 0, sizeof(main_server));
			  strncpy(main_server, id_value_data.value, strlen(id_value_data.value));
			  
			  //������ �����
			  if((check_s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
			  {
				fprintf(stderr, "error: socket()\n");
				mysql_close(&mysql);      
				closesocket(ns);
			    fprintf(stderr, "The check is failed\n");
#ifdef WIN32	  
			  ExitThread(1);
#else
		    pthread_exit(NULL);
#endif
			  }
			  
			  memset(&clnt_addr, 0, sizeof(clnt_addr));
			  clnt_addr.sin_family = AF_INET;
			  clnt_addr.sin_addr.s_addr = INADDR_ANY;

			  //�������� ����� ����� � ������ (���� ������, ��-��������� 80)
			  nport = 80;
			  nport = htons((u_short)nport);
			  port_ptr = strstr(main_server, ":");
			  if(port_ptr == NULL)
				  clnt_addr.sin_port = nport;
			  else
			  {
				  clnt_addr.sin_port = htons((u_short)atoi(port_ptr+1));
				  port_ptr[0] = '\0'; //������� ���������
			  }

			  //������� ip-����� ������� (���� ������ ��� �����)
			  if(port_ptr != NULL)
				fprintf(stderr, "host: %s:%d\n", main_server, atoi(port_ptr+1));
			  else
				fprintf(stderr, "host: %s\n", main_server);
			  hp = gethostbyname(main_server);
			  if(hp == NULL)
			  {
				  delete [] id_value_data.name;
				  delete [] id_value_data.value;
				  fprintf(stderr, "error: gethostbyname()\n");
			      fprintf(stderr, "The check is failed\n");
				  closesocket(ns);
//				  ExitThread(1);
//			    pthread_exit(NULL);
				  break;
			  }

			  memcpy(&(clnt_addr.sin_addr), hp->h_addr, hp->h_length); //�������� ���������� ����� � ��������� ������ ������-�������
			  clnt_addr.sin_family = hp->h_addrtype; //�������� ���

			  //������������ � ���������� �������
			  //���� ����������� ������ ������� - ��������� �������
			  //(����� ��� �������� ������ ��������� �� ��������� ��������� ������� ��-���������)
			  //����� - ����������
			  if(connect(check_s, (struct sockaddr *) &clnt_addr, sizeof(clnt_addr)) != -1)
			  {
				  //���������, ����� ��� ������
				  //...

				  sprintf_s(cmd, sizeof(cmd), "UPDATE %s SET %s=\"%s\" WHERE id=%d", main_table, id_value_data.name, id_value_data.value, id_value_data.id);
				  fprintf(stderr, "%s\n", cmd);
				  delete [] id_value_data.name;
				  delete [] id_value_data.value;
 				  if(mysql_query(&mysql, cmd) != 0)
				  {
					fprintf(stderr, "error mysql_query() : %s\n", mysql_error(&mysql));
				  }
			  }
			  else
			  {
				  delete [] id_value_data.name;
				  delete [] id_value_data.value;
			      fprintf(stderr, "The check is failed (connect())\n");
			  }
			  //������� �����
			  closesocket(check_s);
		  }
		  else
		  {
 			  sprintf_s(cmd, sizeof(cmd), "UPDATE %s SET %s=\"%s\" WHERE id=%d", main_table, id_value_data.name, id_value_data.value, id_value_data.id);
			  fprintf(stderr, "%s\n", cmd);
			  delete [] id_value_data.name;
			  delete [] id_value_data.value;
 			  if(mysql_query(&mysql, cmd) != 0)
			  {
				fprintf(stderr, "error mysql_query() : %s\n", mysql_error(&mysql));
			  }
		  }
		}
		run_commands(command_list, &mysql, &ns); //��������� ���������� �������
	  }
	}

	if(fnewtable) //�������� ��� ����� �������
	{
		memset(cmd, 0, MAX_BUFF_SIZE);
		//����� �������� �������� ����� �������
		sprintf_s(cmd, MAX_BUFF_SIZE, "SELECT new_tab_name FROM new_table_info");
		if(mysql_query(&mysql, cmd) != 0)
		{
			mysql_error_thread_exit(&mysql, &ns);
		}
		else
		{
			res_col = mysql_store_result(&mysql);
			num_col = mysql_affected_rows(&mysql);
		}
		if( num_col != 0 && (row_col = mysql_fetch_row(res_col)) != NULL && row_col[0] != NULL)
		{
			memset(next_table, 0, sizeof(next_table));
			strncpy(next_table, row_col[0], strlen(row_col[0]));
		}
		if(num_col > 0)
		{
			mysql_free_result(res_col);
		}
	}

	//�������� �������� ������� �� ������� ��� ��������
    memset(main_table, 0, sizeof(main_table));
    strcpy_s(main_table, sizeof(main_table), next_table);
	
	memset(cmd, 0, sizeof(cmd));
	//����� �������� �������� �������
	sprintf_s(cmd, sizeof(cmd), "SELECT id,addr,default_index,col_width,col_width_min,addr_default FROM server_info");
	if(mysql_query(&mysql, cmd) != 0)
	{
		mysql_error_thread_exit(&mysql, &ns);
	}
	else
	{
		res_srv = mysql_store_result(&mysql);
		num_srv = mysql_affected_rows(&mysql);
	}
	if(num_srv > 0)
	{
		row_srv = mysql_fetch_row(res_srv);
		for(i = 0; i < 6; ++i)
		{
			if(row_srv[i] == NULL)
			{
				sprintf_s(cmd, sizeof(cmd), "error database: null value in 'server_info'");
				error_thread_exit(&mysql, &ns, cmd);
			}
		}
		area_width = atoi(row_srv[3]);
		area_width_min = atoi(row_srv[4]);
//		fprintf(stderr, "cols_num=%d, %d, %d,\n%s, %s\n", cols_num, area_width, area_width_min, row_srv[3], row_srv[4]);
	}
	else
	{
		sprintf_s(cmd, sizeof(cmd), "error database: wrong table 'server_info'");
	    error_thread_exit(&mysql, &ns, cmd);
	}
	

	if(strncmp(buf, "GET /favicon.ico", 16) == 0)
	{
	  delete [] id_value_data.name;
	  delete [] id_value_data.value;
#ifdef LINUX	  
	  shutdown(ns, SHUT_RDWR);
#endif	  
	  closesocket(ns);
#ifdef WIN32
		ExitThread(100);
#else
		pthread_exit(NULL);
#endif	  
	}

	if(strncmp(buf, "GET /", 5) == 0)
	{
	  ptr = get_head_value(buf, "GET /");
	  memset(main_table, 0, sizeof(main_table));
	  if(ptr[0] == ' ')
	  {
	    strcpy_s(main_table, sizeof(main_table), row_srv[2]);
	  }
	  else
	  {
	    memset(main_table, 0, sizeof(main_table));
	    for(i = 0; ptr[i] != ' '; ++i)
	    {
		  main_table[i] = ptr[i];
	    }
	  }
	}

//--------------------------------------------------------------�������� ������ � �������---------------------------------------------

	//����� �������� �������� �������� �������
	sprintf_s(cmd, sizeof(cmd), "\
SELECT a.id,a.col_name,a.col_type,a.html_code,a.html_hat,a.col_hat,a.col_size,a.col_sort \
FROM tab_columns a \
INNER JOIN all_tables b \
ON a.tab_id=b.id \
WHERE b.tab_name='%s' AND (a.hidden=0 OR a.col_num=0) \
ORDER BY a.col_num", main_table);
	if(mysql_query(&mysql, cmd) != 0)
	{
		mysql_error_thread_exit(&mysql, &ns);
	}
	else
	{
		res_col = mysql_store_result(&mysql);
		num_col = mysql_affected_rows(&mysql);
	}
	if(num_col > 0)
	{
		memset(tab_col, NULL, sizeof(tab_col));
		i = 0;
		//�������� ������ � �������� � ��������� �� � ���
		while( (row_col = mysql_fetch_row(res_col)) != NULL)
		{
			tab_col[i] = new table_column;

			//����
			tab_col[i]->id = atoi(row_col[0]);

			//��� �������
			tab_col[i]->name = new char[strlen(row_col[1])+1];
			memset(tab_col[i]->name, 0, strlen(row_col[1])+1);
			strcpy(tab_col[i]->name, row_col[1]);

			//��� �������
			tab_col[i]->type = atoi(row_col[2]);

			//��� ������
			tab_col[i]->html_code = new char[strlen(row_col[3])+1];
			memset(tab_col[i]->html_code, 0, strlen(row_col[3])+1);
			strcpy(tab_col[i]->html_code, row_col[3]);

			//��� ��������� �������
			tab_col[i]->html_hat = new char[strlen(row_col[4])+1];
			memset(tab_col[i]->html_hat, 0, strlen(row_col[4])+1);
			strcpy(tab_col[i]->html_hat, row_col[4]);

			//��������� ������� � �������
			tab_col[i]->col_hat = new char[strlen(row_col[5])+1];
			memset(tab_col[i]->col_hat, 0, strlen(row_col[5])+1);
			strcpy(tab_col[i]->col_hat, row_col[5]);

			//������ ������� � �������
			tab_col[i]->col_size = atoi(row_col[6]);

			//���������� �� ����� �������
			tab_col[i]->sort = atoi(row_col[7]);

			//������� ����� ��� �������
			tab_col[i]->desc = 0;

			//���������� ����� ������� � ���������, �� �������� ����������� �������
			tab_col[i]->col_desc_id = 0;

			//��������� ������ �� �������� ������
			tab_col[i]->selects = NULL;

			//��������� ������ �� �������� ������������� ������
			tab_col[i]->dynamic = NULL;

			i++;
		}
	}
	else
	{
	    mysql_free_result(res_srv);
	    mysql_free_result(res_col);
		sprintf_s(head, sizeof(head), "error database: no such table '%s' in tab_columns", main_table);
	    error_thread_exit(&mysql, &ns, head);
	}
	cols_num = i; //���������� ���������� �������� � ��������� �������

    mysql_free_result(res_col); //�����, �.�. ��� ������ ��� � ����

	//��������� ������ (�������), ��������� ���������� � ������
	for(i = 0; i < cols_num; ++i)
	{
		if(tab_col[i]->type == 3) //���� ��� ���� � ������� ��������
		{
			fselects = true; //��������� ���� �������� �������� ������
		}
		else if(tab_col[i]->type == 9) //���� ��� ���� � ������������ ������� ��������
		{
			fdynamic = true; //��������� ���� �������� �������� ������������� ������
		}
	}

	if(fselects) //���� � ������� ������������ ���� ������ - ���������� �� � ������
	{
		//�������� �������� ���� ����� ������ �������� �������
		sprintf_s(cmd, sizeof(cmd), "\
SELECT a.col_name,a.value_of_select \
FROM tab_selects a \
INNER JOIN all_tables b \
ON a.tab_id=b.id \
WHERE b.tab_name='%s'", main_table);
		if(mysql_query(&mysql, cmd) != 0)
		{
			num_tbl = 0;
			mysql_free_result(res_srv);
			mysql_error_thread_exit(&mysql, &ns);
		}
		else
		{
			res_tbl = mysql_store_result(&mysql);
			num_tbl = mysql_affected_rows(&mysql);
		}

		//����������� �������� ���� ������ � ������ (��������->������_������)
		if(num_tbl > 0)
		{
			while(num_tbl > 0)
			{
				row_tbl = mysql_fetch_row(res_tbl);

				if(row_tbl[0] == NULL || row_tbl[1] == NULL)
				{
					num_tbl--;
					continue;
				}

				for(i = 0; i < cols_num; ++i)
				{
					if(strcmp(tab_col[i]->name, row_tbl[0]) == 0)
					{
						//���������� ����� ���� �� ���������� ������
						len = strlen(row_tbl[1]);
						//������ ����� ��������� � ������� ��� �������� ����
						tab_col[i]->selects = new select_values;
						//�������� ��������� ���������
						memset(tab_col[i]->selects, 0, sizeof(select_values));
						//������������� ������� (��� ������ �������)
						memset(buf, 0, sizeof(buf));
						//�������� ���������� �����������
						fnodelim = false;
						//��������� ��������� ����������
						for(j = 0, k = 0, l = 0, m = 0; j < len; ++j)
						{
							switch(m)
							{
								case 0: //���� �������� ����
									{
										//���������, ���� �� � ������ ��������� �������� "::"
										if(fnodelim || strstr(row_tbl[1], "::") == NULL)
										{
											//���� ���� "::", ������ �������� ���� ������������� (�� �������, ������� � 0)
											sprintf_s(buf, sizeof(buf), "%d", l);
											fnodelim = true;
										}
										else if(row_tbl[1][j] != ':') //���� ������� ������ �� ':', ��������� �� �����
										{
											buf[k++] = row_tbl[1][j];
											continue;
										}
										else if(j < (len - 1) && row_tbl[1][j+1] != ':') //���� ���������� ��������� ������ ':', ��������� �� �����
										{
											buf[k++] = row_tbl[1][j];
											continue;
										}
										//�������������� �������� ������
										tab_col[i]->selects->values[l] = new char[strlen(buf)+1];
										memset(tab_col[i]->selects->values[l], 0, strlen(buf)+1);
										strcpy_s(tab_col[i]->selects->values[l], strlen(buf)+1, buf);
										//������� �����
										memset(buf, 0, sizeof(buf));
										//����������� ����������� ������ ��� ������
										tab_col[i]->selects->selects[l] = new char[2];
										memset(tab_col[i]->selects->selects[l], 0, 2);
										if(fnodelim)
											j--; //���� ���� �����������, ������� �� ����� � �� ��������� �� ������ ������
										else
											j++;
										k = 0; //��������� ������� ������
										m = 1; //��� ��������
									}
									break;
								case 1: //���� ������, ������������ ��� ������
									{
										if(row_tbl[1][j] != '\r' && row_tbl[1][j] != '\n')
										{
											buf[k++] = row_tbl[1][j];
											//����� �� ���� ���� ����������� ����������� �������� ������ ��������� ������
											if(j < (len-1))
												continue;
										}

										//������� ������� ��������
										if(tab_col[i]->selects->selects[l] != NULL)
											delete [] tab_col[i]->selects->selects[l];

										//�������������� ������ ��� ������
										tab_col[i]->selects->selects[l] = new char[strlen(buf)+1];
										memset(tab_col[i]->selects->selects[l], 0, strlen(buf)+1);
										strcpy_s(tab_col[i]->selects->selects[l], strlen(buf)+1, buf);
										//������� �����
										memset(buf, 0, sizeof(buf));
										//������� ��� ������ �������
										while((j+1) < len && (row_tbl[1][j+1] == '\r' || row_tbl[1][j+1] == '\n'))
										{
											j++;
										}
										k = 0;
										m = 0;
										//����������� ������� ��������� ����� ������
										l++;
									}
									break;
							}
						}
						tab_col[i]->selects->count = l;
						break;
					}
				}
				num_tbl--;
			}
		}

		mysql_free_result(res_tbl);
	}

	if(fdynamic) //���� � ������� ������������ ���� ������������� ������ - ���������� �� � ������
	{
		//�������� �������� ���� ����� ������������� ������ �������� �������
		sprintf_s(cmd, sizeof(cmd), "\
SELECT a.col_name,c.tab_name,a.dynamic_col_id,a.dynamic_col_name,a.dynamic_tab_desc,a.dynamic_tab_id \
FROM tab_dynamic a \
INNER JOIN (all_tables b, all_tables c) \
ON (a.tab_id=b.id AND a.dynamic_tab_id=c.id) \
WHERE b.tab_name='%s'", main_table);

		if(mysql_query(&mysql, cmd) != 0)
		{
			num_tbl = 0;
			mysql_free_result(res_srv);
			mysql_error_thread_exit(&mysql, &ns);
		}
		else
		{
			res_tbl = mysql_store_result(&mysql);
			num_tbl = mysql_affected_rows(&mysql);
		}

		//��������� ������
		if(num_tbl > 0)
		{
			while(num_tbl > 0)
			{
				//�������� ��������� ������ ������� tab_dynamic
				row_tbl = mysql_fetch_row(res_tbl);

				//���� �� ������� ��� ����������� ������ � ������� tab_dynamic
				if(row_tbl[0] == NULL || row_tbl[1] == NULL || row_tbl[2] == NULL || row_tbl[3] == NULL || row_tbl[4] == NULL || row_tbl[5] == NULL)
				{
					num_tbl--;
					continue;
				}

				//���� ����� �������� ��������� ������� ��� � ����� '�������'
				for(i = 0; i < cols_num; ++i)
				{
					if(strcmp(tab_col[i]->name, row_tbl[0]) == 0)
					{
						//������ ������ � ������� � �������� ����������, ��������� � tab_dynamic
						if(atoi(row_tbl[4]) == 1) //���� ������ ���� 'DESC'
						{
							//�������� ���� ������� ��� ������ ��������
							tab_col[i]->desc = 1;

							//������� ���������� ����� ������� � ���������, �� ������� ����������� ������
							for(j = 0; j < cols_num; ++j)
							{
								if(strcmp(tab_col[j]->name, row_tbl[2]) == 0)
								{
									tab_col[i]->col_desc_id = j;
									break;
								}
							}

							//������ ������ � ������� ������� �� ��������� � tab_dynamic �������,
							//������ � ����� ��� ��������� ������� � �������� ���������
							sprintf_s(cmd, sizeof(cmd), "SELECT a.tab_name,a.id FROM all_tables a INNER JOIN %s b ON a.id=b.%s GROUP BY a.id", main_table, row_tbl[2]);

							if(mysql_query(&mysql, cmd) != 0)
							{
								num_tbl = 0;
								mysql_free_result(res_srv);
								mysql_error_thread_exit(&mysql, &ns);
							}
							else
							{
								res_dsc = mysql_store_result(&mysql);
								num_dsc = mysql_affected_rows(&mysql);
							}

							//����������� �������� ���� ������ � ������ (��������->������_������)
							if(num_dsc > 0)
							{
								//������� � �������� ������� ������������
								//�������� ������������ ������ �������� ��� ������ �� ���

								//� ����� ��������� ������ �������� ������
								while(num_dsc > 0)
								{
									//�������� ������ � ������ �������
									row_dsc = mysql_fetch_row(res_dsc);

									if(row_dsc[0] == NULL || row_dsc[1] == NULL)
									{
										num_dsc--;
										continue;
									}

									sprintf_s(cmd, sizeof(cmd), "DESC %s", row_dsc[0]);

									if(mysql_query(&mysql, cmd) != 0)
									{
										num_tbl = 0;
										mysql_free_result(res_srv);
										mysql_error_thread_exit(&mysql, &ns);
									}
									else
									{
										res_col = mysql_store_result(&mysql);
										num_col = mysql_affected_rows(&mysql);
									}

									//����������� �������� ���� ������ � ������ (��������->������_������)
									if(num_col > 0)
									{
										if(tab_col[i]->dynamic == NULL)
										{
											tab_col[i]->dynamic = new select_values;
											memset(tab_col[i]->dynamic, 0, sizeof(select_values));
										}
										else
										{
											select_values *svlist;
											svlist = tab_col[i]->dynamic;
											tab_col[i]->dynamic = new select_values;
											memset(tab_col[i]->dynamic, 0, sizeof(select_values));
											tab_col[i]->dynamic->svnext = svlist;
										}

										//��������� ������� 'NULL'
										l = tab_col[i]->dynamic->count;
										tab_col[i]->dynamic->values[l] = new char[strlen("NULL")+1];
										strcpy_s(tab_col[i]->dynamic->values[l], strlen("NULL")+1, "NULL");
										tab_col[i]->dynamic->selects[l] = new char[strlen("NULL")+1];
										strcpy_s(tab_col[i]->dynamic->selects[l], strlen("NULL")+1, "NULL");
										//����������� �������� ������ �������������� ������ �������
										tab_col[i]->dynamic->tab_index = 0;
										tab_col[i]->dynamic->count++;

										while(num_col > 0)
										{
											//�������� ��������� ������ �������, ��������� � tab_dynamic
											row_col = mysql_fetch_row(res_col);

											if(row_col[0] == NULL)
											{
												num_col--;
												continue;
											}
											l = tab_col[i]->dynamic->count;
											tab_col[i]->dynamic->values[l] = new char[strlen(row_col[0])+1];
											strcpy_s(tab_col[i]->dynamic->values[l], strlen(row_col[0])+1, row_col[0]);
											tab_col[i]->dynamic->selects[l] = new char[strlen(row_col[0])+1];
											strcpy_s(tab_col[i]->dynamic->selects[l], strlen(row_col[0])+1, row_col[0]);
											//����������� �������� ������ ������ �������, ������� ������� � ��������� �������
											tab_col[i]->dynamic->tab_index = atoi(row_dsc[1]);

											num_col--;
											tab_col[i]->dynamic->count++;
										}
									}
									num_dsc--;
									mysql_free_result(res_col);
								}
								mysql_free_result(res_dsc);
							}
							else
							{
								//���� ������� ������������ �� ������ ������,
								//� ������ ���� ������� � �������� ������� �� ����������,
								//� ����� ���������� ����� �� ��������������� DESC ��� ������ �������

								tab_col[i]->desc = 0;
								sprintf_s(cmd, sizeof(cmd), "SELECT col_name FROM tab_columns");

								if(mysql_query(&mysql, cmd) != 0)
								{
									num_tbl = 0;
									mysql_free_result(res_srv);
									mysql_error_thread_exit(&mysql, &ns);
								}
								else
								{
									res_col = mysql_store_result(&mysql);
									num_col = mysql_affected_rows(&mysql);
								}

								//����������� �������� ���� ������ � ������ (��������->������_������)
								if(num_col > 0)
								{
									if(tab_col[i]->dynamic == NULL)
									{
										tab_col[i]->dynamic = new select_values;
										memset(tab_col[i]->dynamic, 0, sizeof(select_values));
									}
									else
									{
										select_values *svlist;
										svlist = tab_col[i]->dynamic;
										tab_col[i]->dynamic = new select_values;
										memset(tab_col[i]->dynamic, 0, sizeof(select_values));
										tab_col[i]->dynamic->svnext = svlist;
									}

									//��������� ������� 'NULL'
									l = tab_col[i]->dynamic->count;
									tab_col[i]->dynamic->values[l] = new char[strlen("NULL")+1];
									strcpy_s(tab_col[i]->dynamic->values[l], strlen("NULL")+1, "NULL");
									tab_col[i]->dynamic->selects[l] = new char[strlen("NULL")+1];
									strcpy_s(tab_col[i]->dynamic->selects[l], strlen("NULL")+1, "NULL");
									//����������� �������� ������ �������������� ������ �������
									tab_col[i]->dynamic->tab_index = 0;
									tab_col[i]->dynamic->count++;

									while(num_col > 0)
									{
										//�������� ��������� ������ �������, ��������� � tab_dynamic
										row_col = mysql_fetch_row(res_col);

										if(row_col[0] == NULL)
										{
											num_col--;
											continue;
										}
										l = tab_col[i]->dynamic->count;
										tab_col[i]->dynamic->values[l] = new char[strlen(row_col[0])+1];
										strcpy_s(tab_col[i]->dynamic->values[l], strlen(row_col[0])+1, row_col[0]);
										tab_col[i]->dynamic->selects[l] = new char[strlen(row_col[0])+1];
										strcpy_s(tab_col[i]->dynamic->selects[l], strlen(row_col[0])+1, row_col[0]);

										num_col--;
										tab_col[i]->dynamic->count++;
									}
								}

								mysql_free_result(res_col);
							}
						}
						else
						{
							//������� ������ ���� ������
							//�� ���� �������� (� �������� � ��������� ��� ������)

							if(strcmp(row_tbl[2], row_tbl[3]) != 0)
							{
								//���� ������ � �������� � ������ �� ���������
								sprintf_s(cmd, sizeof(cmd), "\
SELECT %s,%s \
FROM %s \
ORDER BY %s", row_tbl[2], row_tbl[3], row_tbl[1], row_tbl[2]);
							}
							else
							{
								//���� ������ � �������� � ������ ���������
/*
								sprintf_s(cmd, sizeof(cmd), "\
SELECT %s,%s \
FROM %s \
GROUP BY %s", row_tbl[2], row_tbl[3], row_tbl[1], row_tbl[2], row_tbl[2]);
*/
								sprintf_s(cmd, sizeof(cmd), "\
SELECT %s,%s \
FROM %s \
GROUP BY %s", row_tbl[2], row_tbl[3], row_tbl[1], row_tbl[2]);
							}

							fprintf(stderr, "%s\n", cmd);

							if(mysql_query(&mysql, cmd) != 0)
							{
								num_tbl = 0;
								mysql_free_result(res_srv);
								mysql_error_thread_exit(&mysql, &ns);
							}
							else
							{
								res_col = mysql_store_result(&mysql);
								num_col = mysql_affected_rows(&mysql);
							}

							//����������� �������� ���� ������ � ������ (��������->������_������)
							if(num_col > 0)
							{
								if(tab_col[i]->dynamic == NULL)
								{
									tab_col[i]->dynamic = new select_values;
									memset(tab_col[i]->dynamic, 0, sizeof(select_values));
								}
								else
								{
									select_values *svlist;
									svlist = tab_col[i]->dynamic;
									tab_col[i]->dynamic = new select_values;
									memset(tab_col[i]->dynamic, 0, sizeof(select_values));
									tab_col[i]->dynamic->svnext = svlist;
								}

								//��������� ������� '�����'
								l = tab_col[i]->dynamic->count;
								tab_col[i]->dynamic->values[l] = new char[strlen("NULL")+1];
								strcpy_s(tab_col[i]->dynamic->values[l], strlen("NULL")+1, "NULL");
								tab_col[i]->dynamic->selects[l] = new char[strlen(" ")+1];
								strcpy_s(tab_col[i]->dynamic->selects[l], strlen(" ")+1, " ");
								//����������� �������� ������ �������������� ������ �������
								tab_col[i]->dynamic->tab_index = 0;
								tab_col[i]->dynamic->count++;

								while(num_col > 0)
								{
									//�������� ��������� ������ �������, ��������� � tab_dynamic
									row_col = mysql_fetch_row(res_col);

									if(row_col[0] == NULL || row_col[1] == NULL)
									{
										num_col--;
										continue;
									}
									l = tab_col[i]->dynamic->count;
									tab_col[i]->dynamic->values[l] = new char[strlen(row_col[0])+1];
									strcpy_s(tab_col[i]->dynamic->values[l], strlen(row_col[0])+1, row_col[0]);
									tab_col[i]->dynamic->selects[l] = new char[strlen(row_col[1])+1];
									strcpy_s(tab_col[i]->dynamic->selects[l], strlen(row_col[1])+1, row_col[1]);

									num_col--;
									tab_col[i]->dynamic->count++;
								}
							}

							mysql_free_result(res_col);
						}
						break; //����� �� ����� �������� ���� �������� ������� ������� (� ������� �������� �������)
					}
				}
				num_tbl--;
			}
		}

		mysql_free_result(res_tbl);
	}

	//�������� html-��������� ������� main_table
	sprintf_s(cmd, sizeof(cmd), "\
SELECT a.html_head,a.html_end,a.html_tab_begin,a.html_tab_end,a.html_col_begin,a.html_col_end,a.html_buttons,a.http_head,a.default_insert,a.vertical \
FROM tab_info a \
INNER JOIN all_tables b \
ON a.tab_id=b.id \
WHERE b.tab_name='%s'", main_table);
	if(mysql_query(&mysql, cmd) != 0)
	{
		FREE_DATA_OF_COLUMNS;
		num_tbl = 0;
		mysql_free_result(res_srv);
	    mysql_error_thread_exit(&mysql, &ns);
	}
	else
	{
		res_tbl = mysql_store_result(&mysql);
		num_tbl = mysql_affected_rows(&mysql);
	}

	if(num_tbl > 0)
	{
		row_tbl = mysql_fetch_row(res_tbl);
		memset(buf, 0, sizeof(buf));
		//��������� ���������� ��������� � �������� ���������
		sprintf_s(buf, sizeof(buf), row_tbl[0], row_srv[1], main_table, row_srv[1], main_table, make_lt_gt(row_tbl[8])); //�������� ��������� ��������

		//��������� ������ ��������� ������
		strcpy_s(head, sizeof(head), "SELECT tab_button FROM all_tables WHERE tab_show=1 AND tab_system=1");
	//		strcpy_s(head, sizeof(head), "select tab_button from all_tables");
		if(mysql_query(&mysql, head) != 0)
		{
	//		  mysql_free_result(res);
		  mysql_free_result(res_srv);
		  mysql_free_result(res_tbl);
		  FREE_DATA_OF_COLUMNS;
		  mysql_error_thread_exit(&mysql, &ns);
		}
		else
		{
		  res_col = mysql_store_result(&mysql);
		  num_col = mysql_affected_rows(&mysql);
		}
		if(num_col > 0)
		{
		  strcat_s(buf, sizeof(buf)-strlen(buf), "<p align=center>"); //��������
		  while( (row_col = mysql_fetch_row(res_col)) != NULL)
		  {
			if(row_col != NULL)
			{
				sprintf_s(cmd, sizeof(cmd), row_col[0], row_srv[1]); //��������� ������ ������� �������
				strcat_s(buf, sizeof(buf)-strlen(buf), cmd); //��������� ������ �������� ������
			}
		  }
		  strcat_s(buf, sizeof(buf)-strlen(buf), "</p>"); //��������
	//		  strcat_s(buf, sizeof(buf)-strlen(buf), "<br>"); //�������� ������ �������
		}

		//��������� ������ ����������� ������
		strcpy_s(head, sizeof(head), "SELECT tab_button FROM all_tables WHERE tab_show=1 AND tab_system=0");
//		strcpy_s(head, sizeof(head), "select tab_button from all_tables");
		if(mysql_query(&mysql, head) != 0)
		{
//		  mysql_free_result(res);
		  mysql_free_result(res_srv);
		  mysql_free_result(res_tbl);
		  FREE_DATA_OF_COLUMNS;
		  mysql_error_thread_exit(&mysql, &ns);
		}
		else
		{
		  res_col = mysql_store_result(&mysql);
		  num_col = mysql_affected_rows(&mysql);
		}
		if(num_col > 0)
		{
		  strcat_s(buf, sizeof(buf)-strlen(buf), "<p align=center>"); //��������
		  while( (row_col = mysql_fetch_row(res_col)) != NULL)
		  {
			if(row_col != NULL)
			{
				sprintf_s(cmd, sizeof(cmd), row_col[0], row_srv[1]); //��������� ������ ������� �������
				strcat_s(buf, sizeof(buf)-strlen(buf), cmd); //��������� ������ �������� ������
			}
		  }
		  strcat_s(buf, sizeof(buf)-strlen(buf), "</p>"); //��������
		}

		strcat_s(buf, sizeof(buf)-strlen(buf), row_tbl[2]); //�������� ��������� �������
	}
	else
	{
		//�����
		FREE_DATA_OF_COLUMNS;
//  	    mysql_free_result(res);
		mysql_free_result(res_srv);
//  	    mysql_free_result(res_col);
		sprintf_s(head, sizeof(head), "error database: no such table '%s' in tab_info", main_table);
        error_thread_exit(&mysql, &ns, head);
	}

	if(fdelrow)
	{
		if( (ptr_del = strstr(buf, "<input type=\"hidden\" form=\"frm\" id=\"delete_row\" name=\"delete_row_mode\" value=\"")) != NULL)
		{
			strncpy(ptr_del+78, "no ", 3);
		}

	}

//--------------------------------------------------------------------����� �������� �������---------------------------------------------

	switch(atoi(row_tbl[9]))
	{
		case 0: //����� �������������� �������
			strcat_s(buf, sizeof(buf)-strlen(buf), row_tbl[4]); //������ ������
			strcat_s(buf, sizeof(buf)-strlen(buf), "<td align=center><h4>�</h4></td>\n");
			for(j = 1; j < cols_num; ++j) //j=1 - �� ���������� ������ ������� (����)
			{
			  //���������� ��������� �� ����
			  memset(cmd, 0, sizeof(cmd));
			  sprintf_s(cmd, sizeof(cmd), tab_col[j]->html_hat, tab_col[j]->col_hat);
			  //��������� ��������� � �������
			  strcat_s(buf, sizeof(buf)-strlen(buf), cmd);
			}

			if(fdelrow) //���� ����� ���� �������� ��������
			{
				sprintf_s(cmd, sizeof(cmd), "<td align=center>-</td>");
				strcat_s(buf, sizeof(buf)-strlen(buf), cmd); //������������ � ��������� ��������
			}

			strcat_s(buf, sizeof(buf)-strlen(buf), row_tbl[5]); //����� ������


			//����� �������� ������ �� ������� main_table
			//���������� ������ �� ������ ����������� ���� (������� ���������� ������� ��������)
			//� ����� ��������� ����������
			memset(cmd, 0, sizeof(cmd));
			memset(head, 0, sizeof(head));
			strcpy_s(head, sizeof(head), " ORDER BY ");
			strcpy_s(cmd, sizeof(cmd), "SELECT ");
			for(i = 0; i < cols_num; ++i)
			{
				strcat(cmd, tab_col[i]->name);

				if(i != cols_num-1)
					strcat(cmd, ",");

				switch(tab_col[i]->sort)
				{
					case 0:
						break;
					case 1:
					case 2:
						if(strlen(head) > 10)  //���� ������ ��� ����� �������: " ORDER BY ", �.�. ���� ������������ ��� ���� ������
							strcat_s(head, sizeof(head)-strlen(head), ",");
						strcat_s(head, sizeof(head)-strlen(head), tab_col[i]->name);
						if(tab_col[i]->sort == 2) //���� ����� ���������� � �������� �������
							strcat_s(head, sizeof(head)-strlen(head), " DESC");
						break;
				}
			}
			strcat(cmd, " FROM ");
			strcat(cmd, main_table); //������
			if(strlen(head) > 10) //���� ������ ��� ����� �������: " ORDER BY ", �.�. ���� ���� ����������
			  strcat(cmd, head); //����������
			if(mysql_query(&mysql, cmd) != 0)
			{
			  num = 0;
			  mysql_free_result(res_srv);
			  mysql_free_result(res_col);
 			  mysql_error_thread_exit(&mysql, &ns);
			}
			else
			{
			  res = mysql_store_result(&mysql);
			  num = mysql_affected_rows(&mysql);
			}
			  
			i = 1; //����� ������ (!)
			ffirst = true;
			if(num > 0)
			{
			  while(num > 0)
			  {
				row = mysql_fetch_row(res); //�������� �������� �������� i-��� ������
				if(row == NULL)
				{
				  fprintf(stderr, "error mysql_fetch_row() : %s\n", mysql_error(&mysql));
				  break;
				}

				memset(cmd, 0, sizeof(cmd));
				//��������� ������ �������
				//����: 0-���������������
				//      1-����������� ������������� ������
				//			3-�����
				//			4-��������
				//			5-������
				//			7-��/���
				//			8-��������
				//			9-�������
				//			10-������
				//			11-�����
				//      12-����
				//			13-�����
				sprintf_s(cmd, sizeof(cmd), "%s<td align=center>%d</td>\n", row_tbl[4], i); //������ ������ � ������ ������� (����� ������)

				max = 0;
				for(k = 0; k < cols_num; ++k)
				{
					if(row[k] != NULL)
						len = (strlen(row[k]))/( (strlen(tab_col[k]->col_hat)>((unsigned int)tab_col[k]->col_size))?(strlen(tab_col[k]->col_hat)):tab_col[k]->col_size );
					else
						continue;

					if(max < len)
						max = len;
				}

				for(j = 1; j < cols_num; ++j) //j=1 - �� ���������� ����
				{
					switch(tab_col[j]->type)
					{
						case 0: //���������������
							if(row[j] != NULL)
							{
								sprintf_s(head, sizeof(head), "<td>%s</td>\n", make_lt_gt(row[j]));
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td></td>\n");
							}
							break;
						case 11: //�����
							if(row[j] != NULL)
							{
								sprintf_s(head, sizeof(head), "<td width=%d align=\"justify\">%s</td>",
																					 tab_col[j]->col_size,//15, //������
																					 make_style(row[1]));
//								sprintf_s(head, sizeof(head), "<td align=\"justify\">%s</td>\n", make_style(row[j]));
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td></td>\n");
							}
							break;
						case 12: //����
							if(row[j] != NULL)
							{
								sprintf_s(head, sizeof(head), "<td><input form='frm' id='%s_%d' name='%s[%d]' rows=%d size=%d onChange=\"myreq('%s_%d')\" type='text' value='%s'></td>",
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 (strlen(row[j])/tab_col[j]->col_size>3)?(strlen(row[j])/tab_col[j]->col_size)+2:3, //������
																					 tab_col[j]->col_size,//15, //������
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 make_lt_gt(row[j]));
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td><input form='frm' id='%s_%d' name='%s[%d]' rows=%d size=%d onChange=\"myreq('%s_%d')\" type='text' value='%s'></td>",
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 3, //������
																					 15, //������
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 "");
							}
							break;
						case 13: //�����
							if(row[j] != NULL)
							{
								sprintf_s(head, sizeof(head), "<td><input form='frm' id='%s_%d' name='%s[%d]' rows=%d size=%d onChange=\"myreq('%s_%d')\" type='text' disabled='disabled' value='%s'></td>",
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 (strlen(row[j])/tab_col[j]->col_size>3)?(strlen(row[j])/tab_col[j]->col_size)+2:3, //������
																					 tab_col[j]->col_size,//15, //������
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 make_lt_gt(row[j]));
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td><input form='frm' id='%s_%d' name='%s[%d]' rows=%d size=%d onChange=\"myreq('%s_%d')\" type='text' disabled='disabled' value='%s'></td>",
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 3, //������
																					 15, //������
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 "");
							}
							break;
						case 1: //����������� ������������� ������
							if(row[j] != NULL)
							{
								sprintf_s(head, sizeof(head), tab_col[j]->html_code, tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 (max>0)?max:1,
																					 (strlen(tab_col[j]->col_hat)>(unsigned int)(tab_col[j]->col_size))?(strlen(tab_col[j]->col_hat)):tab_col[j]->col_size,
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 make_lt_gt(row[j]));
							}
							else
							{
								sprintf_s(head, sizeof(head), tab_col[j]->html_code, tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 (max>0)?max:1,
																					 (strlen(tab_col[j]->col_hat)>(unsigned int)(tab_col[j]->col_size))?(strlen(tab_col[j]->col_hat)):tab_col[j]->col_size,
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 "");
							}
							break;
						case 2: //�������
							if(row[j] != NULL)
							{
								char formatted_select[10240];

								if( (atoi(row[j]) != 0) || (strlen(row[j]) > 0 && !isdigit((int)row[j][0])) )
								{
									sprintf_s(formatted_select, sizeof(formatted_select), "<td align=center><input form=\"frm\" id=\"%%s_%%d\" name=\"%%s[%%d]\" value=\"0\" type=\"hidden\"><input id=\"%%s_%%d_ch\" rows=%%d cols=%%d onChange=\"ch_data('%%s_%%d')\" type=\"checkbox\" checked></td>");
									sprintf_s(head, sizeof(head), formatted_select, tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 10, //������
																					 tab_col[j]->col_size,//15, //������
																					 tab_col[j]->name,
																					 atoi(row[0]));
								}
								else
								{
									sprintf_s(formatted_select, sizeof(formatted_select), "<td align=center><input form=\"frm\" id=\"%%s_%%d\" name=\"%%s[%%d]\" value=\"1\" type=\"hidden\"><input id=\"%%s_%%d_ch\" rows=%%d cols=%%d onChange=\"ch_data('%%s_%%d')\" type=\"checkbox\"></td>");
									sprintf_s(head, sizeof(head), formatted_select, tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 10, //������
																					 tab_col[j]->col_size,//15, //������
																					 tab_col[j]->name,
																					 atoi(row[0]));
								}
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td width=\"%d\">NULL</td>\n",tab_col[j]->col_size);
							}
							break;
						case 3: //�����
							if(row[j] != NULL && tab_col[j]->selects != NULL)
							{
								char formatted_select[10240];

								sprintf_s(formatted_select, sizeof(formatted_select), "<td align=center><select form=\"frm\" id=\"%%s_%%d\" name=\"%%s[%%d]\" rows=%%d cols=%%d onChange=\"myreq('%%s_%%d')\">");
								sprintf_s(head, sizeof(head), formatted_select, tab_col[j]->name,
																				 atoi(row[0]),
																				 tab_col[j]->name,
																				 atoi(row[0]),
																				 10, //������
																				 tab_col[j]->col_size,//15, //������
																				 tab_col[j]->name,
																				 atoi(row[0]));
								for(m = 0; m < tab_col[j]->selects->count; ++m)
								{
									if(strcmp(row[j], tab_col[j]->selects->values[m]) == 0)
										sprintf_s(head+strlen(head), sizeof(head)-strlen(head), "<option value=\"%s\" selected>%s</option>", tab_col[j]->selects->values[m], tab_col[j]->selects->selects[m]);
									else
										sprintf_s(head+strlen(head), sizeof(head)-strlen(head), "<option value=\"%s\">%s</option>", tab_col[j]->selects->values[m], tab_col[j]->selects->selects[m]);
								}
								strcat_s(head, sizeof(head)-strlen(head), "</select></td>");
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td width=\"%d\"></td>\n",tab_col[j]->col_size);
							}
							break;
						case 4: //�������� �� ��������
							if(row[j] != NULL)
							{
								sprintf_s(head, sizeof(head), "<td width=\"%d\">%s</td>\n", tab_col[j]->col_size,//������
																							make_lt_gt(row[j]));
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td width=\"%d\"></td>\n",tab_col[j]->col_size);
							}
							break;
						case 5: //��� ���� - ���������� ������ ��������
							if(row[j] != NULL)
							{
								sprintf_s(head, sizeof(head), "<td width=\"%d\" height=\"10\">%s</td>\n", tab_col[j]->col_size,//������
																										  row[j]);
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td width=\"%d\"></td>\n",tab_col[j]->col_size);
							}
							break;
						case 7: //����� ��/���
							if(row[j] != NULL)
							{
								char formatted_select[10240];

								if(atoi(row[j]) != 0)
								{
									sprintf_s(formatted_select, sizeof(formatted_select), "<td><select form=\"frm\" id=\"%%s_%%d\" name=\"%%s[%%d]\" rows=%%d cols=%%d onChange=\"myreq('%%s_%%d')\"><option value=\"%%s\" selected>��</option><option value=\"0\">���</option></select></td>");
									sprintf_s(head, sizeof(head), formatted_select, tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 10, //������
																					 tab_col[j]->col_size,//15, //������
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 row[j]);
//									sprintf_s(head, sizeof(head), "<td width=\"%d\"><select><option value=\"%s\" selected>��</option><option value=\"0\">���</option></select></td>\n",tab_col[j]->col_size,row[j]);
								}
								else
								{
									sprintf_s(formatted_select, sizeof(formatted_select), "<td><select form=\"frm\" id=\"%%s_%%d\" name=\"%%s[%%d]\" rows=%%d cols=%%d onChange=\"myreq('%%s_%%d')\"><option value=\"1\">��</option><option value=\"0\" selected>���</option></select></td>");
									sprintf_s(head, sizeof(head), formatted_select, tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 10, //������
																					 tab_col[j]->col_size,//15, //������
																					 tab_col[j]->name,
																					 atoi(row[0]));
//									sprintf_s(head, sizeof(head), "<td width=\"%d\"><select><option value=\"1\">��</option><option value=\"0\" selected>���</option></select></td>\n",tab_col[j]->col_size);
								}
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td width=\"%d\"></td>\n",tab_col[j]->col_size);
							}
							break;
						case 8: //��������
							if(row[j] != NULL && strlen(row[j]) > 0)
							{
								sprintf_s(head, sizeof(head), "<td width=\"%d\">%s</td>\n", tab_col[j]->col_size*8,//������
																							make_style(row[j]));
							}
							else
							{
								sprintf_s(head, sizeof(head), tab_col[j]->html_code, tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 (max>0)?max:1,
																					 (strlen(tab_col[j]->col_hat)>(unsigned int)(tab_col[j]->col_size))?(strlen(tab_col[j]->col_hat)):tab_col[j]->col_size,
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 "");
							}
							break;
						case 9: //�������
							if(row[j] != NULL && tab_col[j]->dynamic != NULL)
							{
								char formatted_select[10240];
								select_values *svlist;

								svlist = tab_col[j]->dynamic;
								if(tab_col[j]->desc)
								{
									if(row[tab_col[j]->col_desc_id] != NULL)
									{
										while(svlist != NULL)
										{
											if(svlist->tab_index == atoi(row[tab_col[j]->col_desc_id]))
												break;
											svlist = svlist->svnext;
										}
									}
									if(svlist == NULL)
										svlist = tab_col[j]->dynamic;
								}

								sprintf_s(formatted_select, sizeof(formatted_select), "<td><select form=\"frm\" id=\"%%s_%%d\" name=\"%%s[%%d]\" rows=%%d cols=%%d onChange=\"myreq('%%s_%%d')\">");
								sprintf_s(head, sizeof(head), formatted_select, tab_col[j]->name,
																				 atoi(row[0]),
																				 tab_col[j]->name,
																				 atoi(row[0]),
																				 10, //������
																				 tab_col[j]->col_size,//15, //������
																				 tab_col[j]->name,
																				 atoi(row[0]));
								for(m = 0; m < svlist->count; ++m)
								{
									if(strcmp(row[j], svlist->values[m]) == 0)
										sprintf_s(head+strlen(head), sizeof(head)-strlen(head), "<option value=\"%s\" selected>%s</option>", svlist->values[m], svlist->selects[m]);
									else
										sprintf_s(head+strlen(head), sizeof(head)-strlen(head), "<option value=\"%s\">%s</option>", svlist->values[m], svlist->selects[m]);
								}
								strcat_s(head, sizeof(head)-strlen(head), "</select></td>");
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td width=\"%d\"></td>\n",tab_col[j]->col_size);
							}
							break;
						case 10: //������
							if(row[j] != NULL)
							{
								sprintf_s(head, sizeof(head), "<td><input form='frm' id='%s_%d' name='%s[%d]' rows=%d cols=%d onChange=\"myreq('%s_%d')\" type='password' value='%s'></td>",
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 (strlen(row[j])/tab_col[j]->col_size>3)?(strlen(row[j])/tab_col[j]->col_size)+2:3, //������
																					 tab_col[j]->col_size,//15, //������
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 strlen(row[j])>0?"---XpeH---":"");
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td><input form='frm' id='%s_%d' name='%s[%d]' rows=%d cols=%d onChange=\"myreq('%s_%d')\" type='password' value='%s'></td>", tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 3, //������
																					 15, //������
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 "");
							}
							break;
						default: 
							if(row[j] != NULL)
							{
								sprintf_s(head, sizeof(head), "<td>%s</td>\n", make_lt_gt(row[j]));
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td></td>\n");
							}
					}
					strcat_s(cmd, sizeof(cmd)-strlen(cmd), head); //������������ � ��������� ��������
				}

				if(fdelrow) //���� ����� ���� �������� ��������
				{
					sprintf_s(head, sizeof(head), "<td align=center><input type=\"button\" form=\"mega_form\" onClick=\"myreqins('tab_id_%d')\" value=\"-\" /><input type='hidden' form='frm' id='tab_id_%d' name='make_some_request' value='DELETE FROM %s WHERE id=%d'></td>", atoi(row[0]), atoi(row[0]), main_table, atoi(row[0]));
					strcat_s(cmd, sizeof(cmd)-strlen(cmd), head); //������������ � ��������� ��������
				}

				strcat_s(cmd, sizeof(cmd)-strlen(cmd), row_tbl[5]); //����� ������

				if((strlen(cmd)+strlen(buf)) > sizeof(buf))
				{
					if(ffirst)
					{
						//���������� ������ ������ ("[���������]\r\n\r\n[������ (hex)]\r\n[buf]")
						memset(head, 0, sizeof(head));
						sprintf_s(head, sizeof(head), "HTTP/1.1 200 OK\r\nServer: List/0.1.0\r\nReference: Win7\r\nContent-Type: text/html; charset=windows-1251\r\nTransfer-Encoding: chunked\r\n");
						strcat_s(head, sizeof(head)-strlen(head), "Connection: keep-alive\r\n\r\n");
						send_data = new char[strlen(buf)+strlen(head)+16];
						memset(send_data, 0, strlen(buf)+strlen(head)+16);
						sprintf_s(send_data, strlen(buf)+strlen(head)+15, "%s%x\r\n%s\r\n", head, strlen(buf), buf);
						ffirst = false;
					}
					else
					{
						//���������� ��������� ������ ("[������ (hex)]\r\n[buf]")
						send_data = new char[strlen(buf)+16];
						memset(send_data, 0, strlen(buf)+16);
						sprintf_s(send_data, strlen(buf)+15, "%x\r\n%s\r\n", strlen(buf), buf);
					}

					memset(buf, 0, sizeof(buf)); //����������� �����
					strcpy(buf, cmd); //��������� ����� ��������� � �������������� �����

					//��������� ������ ������� �������� � ����� ���������� (����������) ������������ �������
					//������������ �� ���� ��� ������ (���������� �� ������, ������� �� ������� �� ������� ������)
					size_to_send = strlen(send_data);
					nbytes_sent = 0;
					while(nbytes_sent < size_to_send)
					{
						nbytes = send(ns, send_data+nbytes_sent, size_to_send-nbytes_sent, 0);
						nbytes_sent += nbytes;
					}
					delete [] send_data;
				}
				else
				{
					strcat(buf, cmd);
				}
				num--;
				i++;
			  }
			}
			break;
		case 1: //����� ������������ �������
//�������			strcat_s(buf, sizeof(buf)-strlen(buf), row_tbl[4]); //������ ������
//�������			strcat_s(buf, sizeof(buf)-strlen(buf), "<td align=center><h4>�</h4></td>\n");

			ffirst = true; //��� �� ����� �������� �� ���� (��� ������� ���������)
			index_row = 1; //����� ������� ������
			for(j = 1; j < cols_num; ++j) //j=1 - �� ���������� ������ ������� (����)
			{
//�������			  strcat_s(buf, sizeof(buf)-strlen(buf), row_tbl[5]); //����� ������
//				sprintf_s(cmd, sizeof(cmd), "%s<td align=center>%d</td>\n", row_tbl[4], index_row); //������ ������
				strcpy_s(cmd, sizeof(cmd), row_tbl[4]); //������ ������

				//����� �������� ������ �� ������� main_table
				//���������� ������ �� ������ ����������� ���� (������� ���������� ������� ��������)
				//� ����� ��������� ����������

				memset(head, 0, sizeof(head));
				//�������� ��� ������� �������� �������
				sprintf_s(head, sizeof(head), "SELECT id,%s FROM %s", tab_col[index_row]->name, main_table);
				for(i = 0; i < cols_num; ++i)
				{
					if(tab_col[i]->sort > 0)
					{
						if(strlen(head) > 0)
						{
							strcat_s(head, sizeof(head)-strlen(head), ",");
						}
						else
						{
							strcat_s(head, sizeof(head)-strlen(head), " ORDER BY ");
						}
						strcat_s(head, sizeof(head)-strlen(head), tab_col[i]->name);
					}
				}
				if(mysql_query(&mysql, head) != 0)
				{
				  num = 0;
				  mysql_free_result(res_srv);
				  mysql_free_result(res_col);
 				  mysql_error_thread_exit(&mysql, &ns);
				}
				else
				{
				  res = mysql_store_result(&mysql);
				  num = mysql_affected_rows(&mysql);
				}
				  
				//����� ����� �������
				memset(head, 0, sizeof(head));
				sprintf_s(head, sizeof(head), tab_col[index_row]->html_hat, tab_col[index_row]->col_hat); //���������� ��������� �� ����
				strcat_s(cmd, sizeof(cmd)-strlen(cmd), head); //��������� ��������� � �������

				if(num > 0)
				{
				  while(num > 0)
				  {
					row = mysql_fetch_row(res); //�������� �������� index_row-��� ������ j-���� �������
					if(row == NULL)
					{
					  fprintf(stderr, "error mysql_fetch_row() : %s\n", mysql_error(&mysql));
					  break;
					}

					//��������� ������ �������
					//����: 0-���������������
					//		1-����������� ������������� ������
					//		2-�������
					//		3-�����
					//		4-��������
					//    5-��� ���� (����� ���������, �������� ������)
					//		7-��/���
					//		8-��������
					//		9-�������
					//		10-������
					//    11-�����
					//    12-����
					//		13-�����
					switch(tab_col[j]->type)
					{
						case 0: //���������������
							if(row[1] != NULL)
							{
								sprintf_s(head, sizeof(head), "<td>%s</td>\n", make_lt_gt(row[1]));
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td></td>\n");
							}
							break;
						case 11: //�����
							if(row[1] != NULL)
							{
								sprintf_s(head, sizeof(head), "<td width=%d align=\"justify\">%s</td>",
																					 tab_col[j]->col_size,//15, //������
																					 make_style(row[1]));
//								sprintf_s(head, sizeof(head), "<td align=\"justify\">%s</td>\n", make_style(row[1]));
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td></td>\n");
							}
							break;
						case 12: //����
							if(row[1] != NULL)
							{
								sprintf_s(head, sizeof(head), "<td><input form='frm' id='%s_%d' name='%s[%d]' rows=%d width=%d onChange=\"myreq('%s_%d')\" type='text' value='%s'></td>",
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 (strlen(row[1])/tab_col[j]->col_size>3)?(strlen(row[1])/tab_col[j]->col_size)+2:3, //������
																					 tab_col[j]->col_size,//15, //������
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 make_lt_gt(row[1]));
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td><input form='frm' id='%s_%d' name='%s[%d]' rows=%d width=%d onChange=\"myreq('%s_%d')\" type='text' value='%s'></td>",
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 3, //������
																					 15, //������
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 "");
							}
							break;
						case 13: //�����
							if(row[1] != NULL)
							{
								sprintf_s(head, sizeof(head), "<td><input form='frm' id='%s_%d' name='%s[%d]' rows=%d width=%d onChange=\"myreq('%s_%d')\" type='text' disabled='disabled' value='%s'></td>",
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 (strlen(row[1])/tab_col[j]->col_size>3)?(strlen(row[1])/tab_col[j]->col_size)+2:3, //������
																					 tab_col[j]->col_size,//15, //������
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 make_lt_gt(row[1]));
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td><input form='frm' id='%s_%d' name='%s[%d]' rows=%d width=%d onChange=\"myreq('%s_%d')\" type='text' disabled='disabled' value='%s'></td>",
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 3, //������
																					 15, //������
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 "");
							}
							break;
						case 1: //����������� ������������� ������
							if(row[1] != NULL)
							{
								sprintf_s(head, sizeof(head), tab_col[j]->html_code, tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 (strlen(row[1])/tab_col[j]->col_size>3)?(strlen(row[1])/tab_col[j]->col_size)+2:3, //������
																					 (tab_col[j]->col_size > 50) ? tab_col[j]->col_size/5 : tab_col[j]->col_size,//15, //������
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 make_lt_gt(row[1]));
							}
							else
							{
								sprintf_s(head, sizeof(head), tab_col[j]->html_code, tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 3, //������
																					 15, //������
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 "");
							}
							break;
						case 2: //�������
							if(row[1] != NULL)
							{
								char formatted_select[10240];

								if( (atoi(row[1]) != 0) || (strlen(row[1]) > 0 && !isdigit((int)row[1][0])) )
								{
									sprintf_s(formatted_select, sizeof(formatted_select), "<td align=center><input form=\"frm\" id=\"%%s_%%d\" name=\"%%s[%%d]\" value=\"0\" type=\"hidden\"><input id=\"%%s_%%d_ch\" rows=%%d cols=%%d onChange=\"ch_data('%%s_%%d')\" type=\"checkbox\" checked></td>");
									sprintf_s(head, sizeof(head), formatted_select, tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 10, //������
																					 tab_col[j]->col_size,//15, //������
																					 tab_col[j]->name,
																					 atoi(row[0]));
								}
								else
								{
									sprintf_s(formatted_select, sizeof(formatted_select), "<td align=center><input form=\"frm\" id=\"%%s_%%d\" name=\"%%s[%%d]\" value=\"1\" type=\"hidden\"><input id=\"%%s_%%d_ch\" rows=%%d cols=%%d onChange=\"ch_data('%%s_%%d')\" type=\"checkbox\"></td>");
									sprintf_s(head, sizeof(head), formatted_select, tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 10, //������
																					 tab_col[j]->col_size,//15, //������
																					 tab_col[j]->name,
																					 atoi(row[0]));
								}
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td width=\"%d\">NULL</td>\n",tab_col[j]->col_size);
							}
							break;
						case 3: //�����
							if(row[1] != NULL && tab_col[j]->selects != NULL)
							{
								char formatted_select[10240];

								sprintf_s(formatted_select, sizeof(formatted_select), "<td align=center><select form=\"frm\" id=\"%%s_%%d\" name=\"%%s[%%d]\" rows=%%d cols=%%d onChange=\"myreq('%%s_%%d')\">");
								sprintf_s(head, sizeof(head), formatted_select, tab_col[j]->name,
																				 atoi(row[0]),
																				 tab_col[j]->name,
																				 atoi(row[0]),
																				 10, //������
																				 tab_col[j]->col_size,//15, //������
																				 tab_col[j]->name,
																				 atoi(row[0]));
								for(m = 0; m < tab_col[j]->selects->count; ++m)
								{
									if(strcmp(row[1], tab_col[j]->selects->values[m]) == 0)
										sprintf_s(head+strlen(head), sizeof(head)-strlen(head), "<option value=\"%s\" selected>%s</option>", tab_col[j]->selects->values[m], tab_col[j]->selects->selects[m]);
									else
										sprintf_s(head+strlen(head), sizeof(head)-strlen(head), "<option value=\"%s\">%s</option>", tab_col[j]->selects->values[m], tab_col[j]->selects->selects[m]);
								}
								strcat_s(head, sizeof(head)-strlen(head), "</select></td>");
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td width=\"%d\"></td>\n",tab_col[j]->col_size);
							}
							break;
						case 4: //��� ���� ��������..
							if(row[1] != NULL)
							{
								sprintf_s(head, sizeof(head), "<td width=\"%d\">%s</td>\n", tab_col[j]->col_size,//������
																							make_lt_gt(row[1]));
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td width=\"%d\"></td>\n",tab_col[j]->col_size);
							}
							break;
						case 5: //��� ���� (������������ � ��� ��������)
							if(row[1] != NULL)
							{
								sprintf_s(head, sizeof(head), "<td width=\"%d\">%s</td>\n", tab_col[j]->col_size, row[1]);
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td width=\"%d\"></td>\n",tab_col[j]->col_size);
							}
							break;
						case 7: //����� ��/���
							if(row[1] != NULL)
							{
								char formatted_select[10240];

								if(atoi(row[1]) != 0)
								{
									sprintf_s(formatted_select, sizeof(formatted_select), "<td><select form=\"frm\" id=\"%%s_%%d\" name=\"%%s[%%d]\" rows=%%d cols=%%d onChange=\"myreq('%%s_%%d')\"><option value=\"%%s\" selected>��</option><option value=\"0\">���</option></select></td>");
									sprintf_s(head, sizeof(head), formatted_select, tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 10, //������
																					 tab_col[j]->col_size,//15, //������
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 row[1]);
//									sprintf_s(head, sizeof(head), "<td width=\"%d\"><select><option value=\"%s\" selected>��</option><option value=\"0\">���</option></select></td>\n",tab_col[j]->col_size,row[1]);
								}
								else
								{
									sprintf_s(formatted_select, sizeof(formatted_select), "<td><select form=\"frm\" id=\"%%s_%%d\" name=\"%%s[%%d]\" rows=%%d cols=%%d onChange=\"myreq('%%s_%%d')\"><option value=\"1\">��</option><option value=\"0\" selected>���</option></select></td>");
									sprintf_s(head, sizeof(head), formatted_select, tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 10, //������
																					 tab_col[j]->col_size,//15, //������
																					 tab_col[j]->name,
																					 atoi(row[0]));
//									sprintf_s(head, sizeof(head), "<td width=\"%d\"><select><option value=\"1\">��</option><option value=\"0\" selected>���</option></select></td>\n",tab_col[j]->col_size);
								}
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td width=\"%d\"></td>\n",tab_col[j]->col_size);
							}
							break;
						case 8: //��������
							if(row[1] != NULL && strlen(row[1]) > 0)
							{
								sprintf_s(head, sizeof(head), "<td width=\"%d\">%s</td>\n", tab_col[j]->col_size*8,//������
																							make_style(row[1]));
							}
							else
							{
								sprintf_s(head, sizeof(head), tab_col[j]->html_code, tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 (max>0)?max:1,
																					 (strlen(tab_col[j]->col_hat)>(unsigned int)(tab_col[j]->col_size))?(strlen(tab_col[j]->col_hat)):tab_col[j]->col_size,
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 "");
							}
							break;
						case 9: //�������
							if(row[1] != NULL && tab_col[j]->dynamic != NULL)
							{
								char formatted_select[10240];
								select_values *svlist;

								svlist = tab_col[j]->dynamic;
								//if(tab_col[j]->desc)
								//{
								//	if(row[tab_col[j]->col_desc_id] != NULL)
								//	{
								//		while(svlist != NULL)
								//		{
								//			if(svlist->tab_index == atoi(row[tab_col[j]->col_desc_id]))
								//				break;
								//			svlist = svlist->svnext;
								//		}
								//	}
								//	if(svlist == NULL)
								//		svlist = tab_col[j]->dynamic;

								sprintf_s(formatted_select, sizeof(formatted_select), "<td><select form=\"frm\" id=\"%%s_%%d\" name=\"%%s[%%d]\" rows=%%d cols=%%d onChange=\"myreq('%%s_%%d')\">");
								sprintf_s(head, sizeof(head), formatted_select, tab_col[j]->name,
																				 atoi(row[0]),
																				 tab_col[j]->name,
																				 atoi(row[0]),
																				 10, //������
																				 tab_col[j]->col_size,//15, //������
																				 tab_col[j]->name,
																				 atoi(row[0]));
								for(m = 0; m < svlist->count; ++m)
								{
									if(strcmp(row[1], svlist->values[m]) == 0)
										sprintf_s(head+strlen(head), sizeof(head)-strlen(head), "<option value=\"%s\" selected>%s</option>", svlist->values[m], svlist->selects[m]);
									else
										sprintf_s(head+strlen(head), sizeof(head)-strlen(head), "<option value=\"%s\">%s</option>", svlist->values[m], svlist->selects[m]);
								}
								strcat_s(head, sizeof(head)-strlen(head), "</select></td>");
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td width=\"%d\"></td>\n",tab_col[j]->col_size);
							}
							break;
						case 10: //������
							if(row[1] != NULL)
							{
								sprintf_s(head, sizeof(head), "<td><input form='frm' id='%s_%d' name='%s[%d]' rows=%d cols=%d onChange=\"myreq('%s_%d')\" type='password' value='%s'></td>", tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 (strlen(row[1])/tab_col[j]->col_size>3)?(strlen(row[1])/tab_col[j]->col_size)+2:3, //������
																					 tab_col[j]->col_size,//15, //������
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 strlen(row[1])>0?"---XpeH---":"");
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td><input form='frm' id='%s_%d' name='%s[%d]' rows=%d cols=%d onChange=\"myreq('%s_%d')\" type='password' value='%s'></td>", tab_col[j]->name,
																					 atoi(row[0]),
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 3, //������
																					 15, //������
																					 tab_col[j]->name,
																					 atoi(row[0]),
																					 "");
							}
							break;
						default: 
							if(row[1] != NULL)
							{
								sprintf_s(head, sizeof(head), "<td>%s</td>\n", make_lt_gt(row[1]));
							}
							else
							{
								sprintf_s(head, sizeof(head), "<td></td>\n");
							}
					}
					strcat_s(cmd, sizeof(cmd)-strlen(cmd), head); //������������ � ��������� ��������
					num--;
				  }
				}
				else
				{
//					break; //�� �������� ��������� ������ ������
				}

				strcat_s(cmd, sizeof(cmd)-strlen(cmd), row_tbl[5]); //����� ������

				if((strlen(cmd)+strlen(buf)) > sizeof(buf))
				{
					if(ffirst)
					{
						//���������� ������ ������ ("[���������]\r\n\r\n[������ (hex)]\r\n[buf]")
						memset(head, 0, sizeof(head));
						sprintf_s(head, sizeof(head), "HTTP/1.1 200 OK\r\nServer: List/0.1.0\r\nReference: Win7\r\nContent-Type: text/html; charset=windows-1251\r\nTransfer-Encoding: chunked\r\n");
						strcat_s(head, sizeof(head)-strlen(head), "Connection: keep-alive\r\n\r\n");
						send_data = new char[strlen(buf)+strlen(head)+16];
						memset(send_data, 0, strlen(buf)+strlen(head)+16);
						sprintf_s(send_data, strlen(buf)+strlen(head)+15, "%s%x\r\n%s\r\n", head, strlen(buf), buf);
						ffirst = false;
					}
					else
					{
						//���������� ��������� ������ ("[������ (hex)]\r\n[buf]")
						send_data = new char[strlen(buf)+16];
						memset(send_data, 0, strlen(buf)+16);
						sprintf_s(send_data, strlen(buf)+15, "%x\r\n%s\r\n", strlen(buf), buf);
					}

					memset(buf, 0, sizeof(buf)); //����������� �����
					strcpy(buf, cmd); //��������� ����� ��������� � �������������� �����

					//��������� ������ ������� �������� � ����� ���������� (����������) ������������ �������
					//������������ �� ���� ��� ������ (���������� �� ������, ������� �� ������� �� ������� ������)
					size_to_send = strlen(send_data);
					nbytes_sent = 0;
					while(nbytes_sent < size_to_send)
					{
						nbytes = send(ns, send_data+nbytes_sent, size_to_send-nbytes_sent, 0);
						nbytes_sent += nbytes;
					}
					delete [] send_data;
				}
				else
				{
					strcat(buf, cmd);
				}
				index_row++;
			}
			break;
	}

//--------------------------------------------------------------------���������� �������---------------------------------------------

	strcpy_s(cmd, sizeof(cmd), row_tbl[3]); //����� �������
	strcat_s(cmd, sizeof(cmd)-strlen(cmd), row_tbl[6]); //������
	strcat_s(cmd, sizeof(cmd)-strlen(cmd), row_tbl[1]); //����� �����

	if(ffirst)
	{
	  sprintf_s(head, sizeof(head), "HTTP/1.1 200 OK\r\nServer: List/0.1.0\r\nReference: Win7\r\nContent-Type: text/html; charset=windows-1251\r\nTransfer-Encoding: chunked\r\n");
	  strcat_s(head, sizeof(head)+strlen(head), "Connection: close\r\n\r\n");
	}
	else
	{
	  memset(head, 0, sizeof(head));
	}

	send_data = new char[strlen(buf)+strlen(cmd)+strlen(head)+16];
	memset(send_data, 0, strlen(buf)+strlen(cmd)+strlen(head)+16);
	sprintf_s(send_data, strlen(buf)+15+strlen(cmd)+strlen(head), "%s%x\r\n%s%s\r\n0\r\n\r\n", head, strlen(buf)+strlen(cmd), buf, cmd);
	//	fprintf(stderr, "%s\n", send_data); //�������
	size_to_send = strlen(send_data);
	nbytes_sent = 0;
	while(nbytes_sent < size_to_send)
	{
		nbytes = send(ns, send_data+nbytes_sent, size_to_send-nbytes_sent, 0);
		nbytes_sent += nbytes;
	}
	delete [] send_data;

	Sleep(500);
	mysql_free_result(res);
	mysql_free_result(res_srv);
	mysql_free_result(res_col);
	mysql_free_result(res_tbl);
	FREE_DATA_OF_COLUMNS;
	mysql_close(&mysql);
#ifdef LINUX
	shutdown(ns, SHUT_RDWR);
#endif
	closesocket(ns);
#ifdef WIN32
  ExitThread(0);
#else
  pthread_exit(NULL);
#endif
}

void mysql_error_thread_exit(MYSQL * const mysql, SOCKET * const ns)
{
	char cmd[MAX_CMD_SIZE];
	char head[MAX_HEAD_SIZE];

	sprintf_s(cmd, sizeof(cmd), "error mysql_query() : %s", mysql_error(mysql));
	fprintf(stderr, "%s\n", cmd);
	//����� (���� ��������� �� ������)
	sprintf_s(head, sizeof(head), "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=windows-1251\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n%x\r\n%s\r\n0\r\n\r\n", strlen(cmd), cmd);
	send(*ns, head, strlen(head), 0);
	Sleep(500);
#ifdef LINUX
  shutdown(*ns, SHUT_RDWR);
#endif
	closesocket(*ns);
	mysql_close(mysql);
#ifdef WIN32
	  ExitThread(10);
#else
    pthread_exit(NULL);
#endif
}

void error_thread_exit(MYSQL * const mysql, SOCKET * const ns, char const * const err_text)
{
	char head[MAX_HEAD_SIZE];

	fprintf(stderr, "%s\n", err_text);
	//����� (���� ��������� �� ������)
	sprintf_s(head, sizeof(head), "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=windows-1251\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n%x\r\n%s\r\n0\r\n\r\n", strlen(err_text), err_text);
	send(*ns, head, strlen(head), 0);
	Sleep(500);
#ifdef LINUX
  shutdown(*ns, SHUT_RDWR);
#endif
	closesocket(*ns);
	mysql_close(mysql);
#ifdef WIN32
	  ExitThread(10);
#else
    pthread_exit(NULL);
#endif
}

char *get_head_value(char * const ptr, char const * const name)
{
	char *res;

	res = strstr(ptr, name);
	if(res == NULL)
	{
		return NULL;
	}
	else
	{
		res = res+strlen(name);
	}

	return res;
}

int get_some_value(char * const ptr, char const * const name, char * const value)
{
	char *pindex, *pindex_start;
	int len, main_len;
	int i, j, index;
	char *buf;
	char ch[3];
	int val;

    if(name == NULL)
		return 0;

    if(value == NULL)
		return 0;

	len = 0;
    pindex = ptr;
	while(pindex != NULL)
	{
		pindex = strstr(pindex, name);
		if(pindex == NULL)
			return 0;
		if(pindex[strlen(name)] != '=') //���� ����� ����� �� ����� ���� '='
		{
			pindex++;
			continue;
		}
		else //����� �������������� value � ������� ��� �������
		{
			len = strlen(pindex);
			for(i = 0; i < len; ++i) //������� ����� ������
			{
				if(pindex[i] == '+')
				{
					pindex[i] = ' ';
					continue;
				}
				if(pindex[i] == '&')
				{
					pindex[i] = '\0';
					break;
				}
				if(pindex[i] == '\0')
					break;
			}

			main_len = strlen(pindex); //�������� ����� ������� 'name'
			index = i;
			pindex_start = pindex + strlen(name) + 1; //������������� ������� ������ ��������
			buf = new char[index*2+1]; //����� ������ ������� � ������
			memset(buf, 0, index*2+1);

			//������� %XX
			for(i = 0, j = 0; i < index; ++i, ++j)
			{
				if(pindex_start[i] != '%')
				{
					buf[j] = pindex_start[i];
				}
				else
				{
					ch[0] = pindex_start[i+1];
					ch[1] = pindex_start[i+2];
					ch[2] = '\n';
					sscanf(ch, "%X", &val);
					buf[j] = (char)val;
					i++;
					i++;
				}

				if( (buf[j] == '\"')||(buf[j] == '\\') )
				{
					buf[j+1] = buf[j];
					buf[j] = '\\';
					j++;
				}
			}

			strcpy_s(value, MAX_HEAD_SIZE, buf);
			fprintf(stderr, "buf: %s\n", buf);
			delete [] buf;
			for(i = 0;;++i) //�������� ��� ������
			{
				pindex[i] = pindex[main_len+i+1];
				if(pindex[i] == '\0')
					break;
			}
//			strcpy(pindex, &pindex[main_len+1]); //�������� ��� ������
			if(strlen(value) > 0)
				len = 1;
			else
				len = 0;
			break;
		}
	}

	return len;
}

int get_id_value(char * const ptr, id_value_data_struct * const id_value_data)
{
	int len;
	int i, j, index;
	char *buf, *pindex;
	char ch[3];
	int val;

	if(ptr == NULL)
	{
		fprintf(stderr, "error: ptr==NULL\n");
		return -1;
	}

	if(strlen(ptr) == 0)
	{
		fprintf(stderr, "error: strlen(ptr)==0\n");
		return -1;
	}

	if(id_value_data == NULL)
	{
		fprintf(stderr, "error: id_value_data==NULL\n");
		return -1;
	}

//	fprintf(stderr, "ptr: %s\n", ptr);

	len = 0;

	j = strlen(ptr);
    //������� ���� ���������� ��������
	for(i = 0; i < j; ++i)
	{
		if(ptr[i] == '+')
		{
			ptr[i] = ' ';
			continue;
		}

		if(ptr[i] == '&')
		{
		    len = strlen(ptr+i); //����� �������
			id_value_data->index = i+1; //�������� ptr
			break;
		}
	}
	index = i; //���������� �������
	buf = new char[index*2+1]; //����� ������ ������� � ������
	memset(buf, 0, index*2+1);
	//������� %XX
  for(i = 0, j = 0; i < index; ++i, ++j)
	{
		if(ptr[i] != '%')
		{
			buf[j] = ptr[i];
		}
		else
		{
			ch[0] = ptr[i+1];
			ch[1] = ptr[i+2];
			ch[2] = '\n';
			sscanf(ch, "%X", &val);
			buf[j] = (char)val;
			i++;
			i++;
		}

		if( (buf[j] == '\"')||(buf[j] == '\\')  )
		{
			buf[j+1] = buf[j];
			buf[j] = '\\';
			j++;
		}
	}


	index = j;
	pindex = strstr(buf, "[");
	if(pindex == NULL)
	{
		fprintf(stderr, "error: can not find '['\n");
		return -1;
	}
	pindex[0] = '\0';
	id_value_data->name = new char[strlen(buf)+1];
	memset(id_value_data->name, 0, strlen(buf)+1);
	strcpy(id_value_data->name, buf); //�������� ��� �������
	id_value_data->id = atoi(pindex+1); //���������� id ������
	pindex = strstr(pindex+1, "="); //������� ������ ��������
	if(pindex == NULL)
	{
		delete [] id_value_data->name;
		fprintf(stderr, "error: can not find '='\n");
		return -1;
	}
	id_value_data->value = new char[strlen(pindex+1)+1];
	memset(id_value_data->value, 0, strlen(pindex+1)+1);
	strcpy(id_value_data->value, pindex+1);
//	fprintf(stderr, "\n%d, %s, %s\n", id_value_data->id, id_value_data->name, id_value_data->value);

	delete [] buf;
	return len;
}

void run_commands(char * const cmd_list, MYSQL * const pmysql, SOCKET * const ns)
{
	char *ptr;

	if(cmd_list == NULL)
		return;
	if(strlen(cmd_list) == 0)
		return;
	if(pmysql == NULL)
		return;

	ptr = strtok(cmd_list, ",");
	while(ptr != NULL)
	{
		if(strncmp(ptr, "create_new_table", strlen(ptr)) == 0) //���� ������������ ������� '������� ����� �������'
		{
			create_new_table(pmysql, ns);
		}

		ptr = strtok(NULL, ",");
	}
}

char *make_lt_gt(char const * const in_text)
{
	static char out_buff[MAX_BUFF_SIZE];
	size_t len;
	int i, j;

	memset(out_buff, 0, sizeof(out_buff));
	if(in_text == NULL)
		return out_buff;
	len = (strlen(in_text)>sizeof(out_buff))?sizeof(out_buff):strlen(in_text);
	for(i = 0, j = 0; i < (int)len; ++i, ++j)
	{
		if(in_text[i] == '&')
		{
			out_buff[j++] = '&';
			out_buff[j++] = '#';
			out_buff[j++] = '0';
			out_buff[j++] = '3';
			out_buff[j++] = '8';
			out_buff[j]   = ';';
			continue;
		}
		if(in_text[i] == '\'')
		{
			out_buff[j++] = '&';
			out_buff[j++] = '#';
			out_buff[j++] = '0';
			out_buff[j++] = '3';
			out_buff[j++] = '9';
			out_buff[j]   = ';';
			continue;
		}
		if(in_text[i] == '\"')
		{
			out_buff[j++] = '&';
			out_buff[j++] = 'q';
			out_buff[j++] = 'u';
			out_buff[j++] = 'o';
			out_buff[j++] = 't';
			out_buff[j]   = ';';
			continue;
		}
		if(in_text[i] == '<')
		{
			out_buff[j++] = '&';
			out_buff[j++] = 'l';
			out_buff[j++] = 't';
			out_buff[j]   = ';';
			continue;
		}
		if(in_text[i] == '>')
		{
			out_buff[j++] = '&';
			out_buff[j++] = 'g';
			out_buff[j++] = 't';
			out_buff[j]   = ';';
			continue;
		}

		out_buff[j] = in_text[i];
	}

	return out_buff;
}

char *make_style(char const * const in_text)
{
	static char out_buff[MAX_BUFF_SIZE];
	size_t len;
	int i, j;

	memset(out_buff, 0, sizeof(out_buff));
	if(in_text == NULL)
		return out_buff;
	len = (strlen(in_text)>sizeof(out_buff))?sizeof(out_buff):strlen(in_text);
	for(i = 0, j = 0; i < (int)len; ++i, ++j)
	{
		if(i == 0 && in_text[i] == ' ')
		{
			out_buff[j++] = '&';
			out_buff[j++] = 'n';
			out_buff[j++] = 'b';
			out_buff[j++] = 's';
			out_buff[j++] = 'p';
			out_buff[j]   = ';';
			continue;
		}
		if(in_text[i] == '&')
		{
			out_buff[j++] = '&';
			out_buff[j++] = '#';
			out_buff[j++] = '0';
			out_buff[j++] = '3';
			out_buff[j++] = '8';
			out_buff[j]   = ';';
			continue;
		}
		if(in_text[i] == '\'')
		{
			out_buff[j++] = '&';
			out_buff[j++] = '#';
			out_buff[j++] = '0';
			out_buff[j++] = '3';
			out_buff[j++] = '9';
			out_buff[j]   = ';';
			continue;
		}
		if(in_text[i] == '\"')
		{
			out_buff[j++] = '&';
			out_buff[j++] = 'q';
			out_buff[j++] = 'u';
			out_buff[j++] = 'o';
			out_buff[j++] = 't';
			out_buff[j]   = ';';
			continue;
		}
		if(in_text[i] == '<')
		{
			out_buff[j++] = '&';
			out_buff[j++] = 'l';
			out_buff[j++] = 't';
			out_buff[j]   = ';';
			continue;
		}
		if(in_text[i] == '>')
		{
			out_buff[j++] = '&';
			out_buff[j++] = 'g';
			out_buff[j++] = 't';
			out_buff[j]   = ';';
			continue;
		}
/*
		if(in_text[i] == ' ')
		{
			out_buff[j++] = '&';
			out_buff[j++] = 'n';
			out_buff[j++] = 'b';
			out_buff[j++] = 's';
			out_buff[j++] = 'p';
			out_buff[j]   = ';';
			continue;
		}
*/
		if(in_text[i] == '\n')
		{
			out_buff[j++] = '<';
			out_buff[j++] = 'b';
			out_buff[j++] = 'r';
			out_buff[j] = '>';
			while(in_text[i+1] == ' ')
			{
			  j++;
				out_buff[j++] = '&';
				out_buff[j++] = 'n';
				out_buff[j++] = 'b';
				out_buff[j++] = 's';
				out_buff[j++] = 'p';
				out_buff[j]   = ';';
				i++;
			}
			continue;
		}

		out_buff[j] = in_text[i];
	}

	return out_buff;
}

char *make_screen(char const * const in_text)
{
	static char out_buff[MAX_BUFF_SIZE];
	size_t len;
	int i, j;

	memset(out_buff, 0, sizeof(out_buff));
	if(in_text == NULL)
		return out_buff;
	len = (strlen(in_text)>sizeof(out_buff))?sizeof(out_buff):strlen(in_text);
	for(i = 0, j = 0; i < (int)len; ++i, ++j)
	{
		if(in_text[i] == '\"')
		{
			out_buff[j++] = '\\';
			out_buff[j]   = '\"';
			continue;
		}
		if(in_text[i] == '\\')
		{
			out_buff[j++] = '\\';
			out_buff[j]   = '\\';
			continue;
		}
		if(in_text[i] == '\'')
		{
			out_buff[j++] = '\\';
			out_buff[j]   = '\'';
			continue;
		}

		out_buff[j] = in_text[i];
	}

	return out_buff;
}

void cp_to_utf8(char *out_text, const char *str, int from_cp, size_t size)
{
#ifdef WIN32
	int result_u, result_c;

	result_u = MultiByteToWideChar(from_cp,0,str,-1,0,0);
	
	if (!result_u)
		return;

	wchar_t *ures = new wchar_t[result_u];

	if(!MultiByteToWideChar(from_cp,0,str,-1,ures,result_u))
	{
		delete[] ures;
		return;
	}

	result_c = WideCharToMultiByte(CP_UTF8,0,ures,-1,0,0,0,0);

	if(!result_c)
	{
		delete [] ures;
		return;
	}

	char *cres = new char[result_c];

	if(!WideCharToMultiByte(CP_UTF8,0,ures,-1,cres,result_c,0,0))
	{
		delete[] cres;
		return;
	}
	delete[] ures;

	strcpy_s(out_text, size, cres);
	delete[] cres;
	return;
#endif
}

void utf8_to_cp(char *out_text, const char *str, int to_cp, size_t size)
{
#ifdef WIN32
	int result_u, result_c;

	result_u = MultiByteToWideChar(CP_UTF8,0,str,-1,0,0);
	
	if (!result_u)
		return;

	wchar_t *ures = new wchar_t[result_u];

	if(!MultiByteToWideChar(CP_UTF8,0,str,-1,ures,result_u))
	{
		delete[] ures;
		return;
	}

	result_c = WideCharToMultiByte(to_cp,0,ures,-1,0,0,0, 0);

	if(!result_c)
	{
		delete [] ures;
		return;
	}

	char *cres = new char[result_c];

	if(!WideCharToMultiByte(to_cp,0,ures,-1,cres,result_c,0,0))
	{
		delete[] cres;
		return;
	}
	delete[] ures;
	strcpy_s(out_text, size, cres);
	delete[] cres;
	return;
#endif
}

void create_new_table(MYSQL * const pmysql, SOCKET * const ns)
{
	char *cmd;//[MAX_BUFF_SIZE];
	MYSQL_RES *res_col, *res_inf;
	MYSQL_ROW row_col, row_inf;
	int num_col, num_inf;
	int i;
	char tab_id[16];
	char table_name[128];
	char table_title_name[1024];
	char column_name[128];
	char *column_buff;//[MAX_BUFF_SIZE];
	char *main_tab;//[MAX_BUFF_SIZE];
	char *tab_info_name, *tab_info_values;//[MAX_BUFF_SIZE];
	char *tab_columns_name, *tab_columns_values;//[MAX_BUFF_SIZE];
	char *tab_default_name, *tab_default_values;//[MAX_BUFF_SIZE];
	char *all_tables_name, *all_tables_values;//[MAX_BUFF_SIZE];
	char *value_of_select[MAX_COLUMNS_IN_TABLE][2];
	int column_index_tab_info_name,column_index_tab_info_values,column_index_all_tables_name,column_index_all_tables_values;
	int column_index_main_tab,column_index_tab_columns_name,column_index_tab_columns_values,column_index_tab_default_name;
	int column_index_tab_default_values;
	int column_index;
	int value_of_select_index;
	char *columns_sql_buff[MAX_COLUMNS_IN_TABLE];
	int len;

	cmd = new char[MAX_BUFF_SIZE];
	column_buff = new char[MAX_BUFF_SIZE];
	main_tab = new char[MAX_BUFF_SIZE];
	tab_info_name = new char[MAX_BUFF_SIZE];
	tab_info_values = new char[MAX_BUFF_SIZE];
	tab_columns_name = new char[MAX_BUFF_SIZE];
	tab_columns_values = new char[MAX_BUFF_SIZE];
	tab_default_name = new char[MAX_BUFF_SIZE];
	tab_default_values = new char[MAX_BUFF_SIZE];
	all_tables_name = new char[MAX_BUFF_SIZE];
	all_tables_values = new char[MAX_BUFF_SIZE];
	memset(columns_sql_buff, 0, sizeof(columns_sql_buff));
	memset(value_of_select, 0, sizeof(value_of_select));
	//�������������� � ���� ����� ��� ������� FREE_ALL_VARS
	column_index = 0;
	value_of_select_index = 0;

	memset(cmd, 0, MAX_BUFF_SIZE);
	//����� �������� �������� �������� �������
	sprintf_s(cmd, MAX_BUFF_SIZE, "SELECT \
id,\
new_tab_name,\
new_tab_public_name,\
new_table_save_button,\
new_table_add_button,\
new_table_is_system,\
new_table_head,\
new_table_end,\
new_table_tab_begin,\
new_table_tab_end,\
new_table_col_begin,\
new_table_col_end,\
new_table_http_head,\
new_table_default_insert,\
new_table_type\
 FROM new_table_info");
	if(mysql_query(pmysql, cmd) != 0)
	{
		FREE_ALL_VARS;
		mysql_error_thread_exit(pmysql, ns);
	}
	else
	{
		res_inf = mysql_store_result(pmysql);
		num_inf = mysql_affected_rows(pmysql);
	}
	if(num_inf == 0)
	{
		FREE_ALL_VARS;
		return;
	}
	if( (row_inf = mysql_fetch_row(res_inf)) == NULL )
	{
		FREE_ALL_VARS;
		return;
	}

	memset(cmd, 0, MAX_BUFF_SIZE);
	//����� �������� �������� ����� �������
	sprintf_s(cmd, MAX_BUFF_SIZE, "SELECT \
id,\
new_col_name,\
new_col_type,\
new_col_size,\
new_col_html_code,\
new_col_html_hat,\
new_col_hat,\
new_col_num,\
new_col_hidden,\
new_col_default_insert,\
new_col_fix_size,\
new_col_sort,\
new_col_group,\
new_col_search,\
new_col_select_value\
 FROM new_table");
	if(mysql_query(pmysql, cmd) != 0)
	{
		FREE_ALL_VARS;
		mysql_error_thread_exit(pmysql, ns);
	}
	else
	{
		res_col = mysql_store_result(pmysql);
		num_col = mysql_affected_rows(pmysql);
	}
	if(num_col == 0)
	{
		FREE_ALL_VARS;
		return;
	}

//1. ��������� ��������:
//	�) �� ���������� ����� ������� (��� ������: ��� table_name(...) � ��� values(...));
//	�) �� ���������� � ������� tab_info (��� ������: ��� insert into tab_info(...) � values(...));
//	�) �� ���������� � ������� tab_columns (��� ������: ��� insert into tab_columns(...) � values(...)).
//2. ���������� ����� �������.
//3. ���������� � ������� all_tables.
//4. ���������� � ������� tab_info � tab_columns.

//info:
// 0	id
// 1	new_tab_name
// 2	new_tab_public_name
// 3	new_table_save_button
// 4	new_table_add_button
// 5	new_table_is_system
// 6	new_table_head
// 7	new_table_end
// 8	new_table_tab_begin
// 9	new_table_tab_end
//10	new_table_col_begin
//11	new_table_col_end
//12	new_table_http_head
//13	new_table_default_insert
//14	new_table_type

	memset(tab_info_name, 0, MAX_BUFF_SIZE);
	strcpy_s(tab_info_name, MAX_BUFF_SIZE, "INSERT INTO tab_info(");
	memset(tab_info_values, 0, MAX_BUFF_SIZE);
	strcpy_s(tab_info_values, MAX_BUFF_SIZE, " VALUES(");

	memset(all_tables_name, 0, MAX_BUFF_SIZE);
	strcpy_s(all_tables_name, MAX_BUFF_SIZE, "INSERT INTO all_tables(");
	memset(all_tables_values, 0, MAX_BUFF_SIZE);
	strcpy_s(all_tables_values, MAX_BUFF_SIZE, " VALUES(");

	column_index_tab_info_name = 0;
	column_index_tab_info_values = 0;
	column_index_all_tables_name = 0;
	column_index_all_tables_values = 0;

	//��������� ������ �� �������� �������� �������
	for(i = 0; i < 15; ++i)
	{
		if(row_inf == NULL)
		{
			FREE_ALL_VARS;
			mysql_free_result(res_inf);
			mysql_free_result(res_col);
			error_thread_exit(pmysql, ns, "error in new_table_info: NULL value in some column\n");
		}
		switch(i)
		{
			case 1: //char ��������� ��� �������
				memset(table_name, 0, sizeof(table_name));
				if(row_inf[i] == NULL)
				{
					sprintf_s(table_name, sizeof(table_name), "tab%d", (int)(time(NULL)-1360000000L)); //���������� ��������� ��� ����� �������
				}
				else if(strlen(row_inf[i]) == 0)
				{
					sprintf_s(table_name, sizeof(table_name), "tab%d", (int)(time(NULL)-1360000000L)); //���������� ��������� ��� ����� �������
				}
				else
					strcpy_s(table_name, sizeof(table_name), row_inf[i]); //���������� ��������� ��� ����� �������

				//���������� � all_tables:
				if(column_index_all_tables_name)
				{
					strcat_s(all_tables_name, MAX_BUFF_SIZE-strlen(all_tables_name), ",");
				}
				if(column_index_all_tables_values)
				{
					strcat_s(all_tables_values, MAX_BUFF_SIZE-strlen(all_tables_values), ",");
				}
				column_index_all_tables_name++;
				column_index_all_tables_values++;

				//��� ��� ��������:
				strcat_s(all_tables_name, MAX_BUFF_SIZE-strlen(all_tables_name), "tab_name");
				//��� ��������:
				memset(cmd, 0, MAX_BUFF_SIZE);
				sprintf_s(cmd, MAX_BUFF_SIZE, "'%s'", table_name);
				strcat_s(all_tables_values, MAX_BUFF_SIZE-strlen(all_tables_values), cmd);
				break;
			case 2: //char ������� ��� �������
				memset(column_buff, 0, MAX_BUFF_SIZE);
				if(row_inf[i] == NULL)
				{
					strcpy_s(column_buff, MAX_BUFF_SIZE, "����� �������");
				}
				else if(strlen(row_inf[i]) == 0)
				{
					strcpy_s(column_buff, MAX_BUFF_SIZE, "����� �������");
				}
				else
					strcpy_s(column_buff, MAX_BUFF_SIZE, row_inf[i]);

				if(column_index_all_tables_name)
				{
					strcat_s(all_tables_name, MAX_BUFF_SIZE-strlen(all_tables_name), ",");
				}
				if(column_index_all_tables_values)
				{
					strcat_s(all_tables_values, MAX_BUFF_SIZE-strlen(all_tables_values), ",");
				}
				column_index_all_tables_name++;
				column_index_all_tables_values++;

				memset(table_title_name, 0, sizeof(table_title_name));
				strcpy_s(table_title_name, sizeof(table_title_name), column_buff);

				//��� ��� ��������:
				strcat_s(all_tables_name, MAX_BUFF_SIZE-strlen(all_tables_name), "tab_title_name");
				//��� ��������:
				memset(cmd, 0, MAX_BUFF_SIZE);
				sprintf_s(cmd, MAX_BUFF_SIZE, "'%s'", column_buff);
				strcat_s(all_tables_values, MAX_BUFF_SIZE-strlen(all_tables_values), cmd);
				break;
			case 3: //char ��� ������ '���������'
				memset(column_buff, 0, MAX_BUFF_SIZE);
				strcpy_s(column_buff, MAX_BUFF_SIZE, "<p align=center>");
				if(row_inf[i] == NULL)
				{
					strcat_s(column_buff, MAX_BUFF_SIZE, "<input type=\"submit\" form=\"mega_form\" value=\"���������\" />");
				}
				else if(strlen(row_inf[i]) == 0)
				{
					strcat_s(column_buff, MAX_BUFF_SIZE, "<input type=\"submit\" form=\"mega_form\" value=\"���������\" />");
				}
				else
					strcat_s(column_buff, MAX_BUFF_SIZE, row_inf[i]);
				break;
			case 4: //char ��� ������ '+'
				if(row_inf[i] == NULL)
				{
					strcat_s(column_buff, MAX_BUFF_SIZE, "<input type=\"button\" form=\"mega_form\" onClick=\"myreqins('insert_new')\" value=\"+\" />");
					strcat_s(column_buff, MAX_BUFF_SIZE, "</p>");
				}
				else if(strlen(row_inf[i]) == 0)
				{
					strcat_s(column_buff, MAX_BUFF_SIZE, "<input type=\"button\" form=\"mega_form\" onClick=\"myreqins('insert_new')\" value=\"+\" />");
					strcat_s(column_buff, MAX_BUFF_SIZE, "</p>");
				}
				else
					strcat_s(column_buff, MAX_BUFF_SIZE, row_inf[i]);

				if(column_index_tab_info_name)
				{
					strcat_s(tab_info_name, MAX_BUFF_SIZE-strlen(tab_info_name), ",");
				}
				if(column_index_tab_info_values)
				{
					strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), ",");
				}
				column_index_tab_info_name++;
				column_index_tab_info_values++;

				//��� ��� ��������:
				strcat_s(tab_info_name, MAX_BUFF_SIZE-strlen(tab_info_name), "html_buttons");
				//��� ��������:
				memset(cmd, 0, MAX_BUFF_SIZE);
				sprintf_s(cmd, MAX_BUFF_SIZE, "'%s'", column_buff);
				strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), cmd);
				break;
			case 5: //int ���� '��������� �������'
				memset(column_buff, 0, MAX_BUFF_SIZE);
				if(row_inf[i] == NULL)
				{
					strcpy_s(column_buff, MAX_BUFF_SIZE, "0");
				}
				else if(strlen(row_inf[i]) == 0)
				{
					strcpy_s(column_buff, MAX_BUFF_SIZE, "0");
				}
				else
					strcpy_s(column_buff, MAX_BUFF_SIZE, row_inf[i]);

				if(column_index_all_tables_name)
				{
					strcat_s(all_tables_name, MAX_BUFF_SIZE-strlen(all_tables_name), ",");
				}
				if(column_index_all_tables_values)
				{
					strcat_s(all_tables_values, MAX_BUFF_SIZE-strlen(all_tables_values), ",");
				}
				column_index_all_tables_name++;
				column_index_all_tables_values++;

				//��� ��� ��������:
				strcat_s(all_tables_name, MAX_BUFF_SIZE-strlen(all_tables_name), "tab_system");
				//��� ��������:
				strcat_s(all_tables_values, MAX_BUFF_SIZE-strlen(all_tables_values), column_buff);
				break;
			case 6: //char ��� ��������� �����
				memset(column_buff, 0, MAX_BUFF_SIZE);
				if(row_inf[i] == NULL)
				{
					strcpy_s(column_buff, MAX_BUFF_SIZE, "");
				}
				else if(strlen(row_inf[i]) == 0)
				{
					strcpy_s(column_buff, MAX_BUFF_SIZE, "");
				}
				else
					strcpy_s(column_buff, MAX_BUFF_SIZE, row_inf[i]);

				if(column_index_tab_info_name)
				{
					strcat_s(tab_info_name, MAX_BUFF_SIZE-strlen(tab_info_name), ",");
				}
				if(column_index_tab_info_values)
				{
					strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), ",");
				}
				column_index_tab_info_name++;
				column_index_tab_info_values++;

				//��� ��� ��������:
				strcat_s(tab_info_name, MAX_BUFF_SIZE-strlen(tab_info_name), "html_head");
				//��� ��������:
				memset(cmd, 0, MAX_BUFF_SIZE);
				sprintf_s(cmd, MAX_BUFF_SIZE, "'%s'", column_buff);
				strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), cmd);
				break;
			case 7: //char ��� ����� �����
				memset(column_buff, 0, MAX_BUFF_SIZE);
				if(row_inf[i] == NULL)
				{
					strcpy_s(column_buff, MAX_BUFF_SIZE, "");
				}
				else if(strlen(row_inf[i]) == 0)
				{
					strcpy_s(column_buff, MAX_BUFF_SIZE, "");
				}
				else
					strcpy_s(column_buff, MAX_BUFF_SIZE, row_inf[i]);

				if(column_index_tab_info_name)
				{
					strcat_s(tab_info_name, MAX_BUFF_SIZE-strlen(tab_info_name), ",");
				}
				if(column_index_tab_info_values)
				{
					strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), ",");
				}
				column_index_tab_info_name++;
				column_index_tab_info_values++;

				//��� ��� ��������:
				strcat_s(tab_info_name, MAX_BUFF_SIZE-strlen(tab_info_name), "html_end");
				//��� ��������:
				memset(cmd, 0, MAX_BUFF_SIZE);
				sprintf_s(cmd, MAX_BUFF_SIZE, "'%s'", column_buff);
				strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), cmd);
				break;
			case 8: //char ��� ������ �������
				memset(column_buff, 0, MAX_BUFF_SIZE);
				if(row_inf[i] == NULL)
				{
					strcpy_s(column_buff, MAX_BUFF_SIZE, "");
				}
				else if(strlen(row_inf[i]) == 0)
				{
					strcpy_s(column_buff, MAX_BUFF_SIZE, "");
				}
				else
					strcpy_s(column_buff, MAX_BUFF_SIZE, row_inf[i]);

				if(column_index_tab_info_name)
				{
					strcat_s(tab_info_name, MAX_BUFF_SIZE-strlen(tab_info_name), ",");
				}
				if(column_index_tab_info_values)
				{
					strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), ",");
				}
				column_index_tab_info_name++;
				column_index_tab_info_values++;

				//��� ��� ��������:
				strcat_s(tab_info_name, MAX_BUFF_SIZE-strlen(tab_info_name), "html_tab_begin");
				//��� ��������:
				memset(cmd, 0, MAX_BUFF_SIZE);
				sprintf_s(cmd, MAX_BUFF_SIZE, "'%s'", column_buff);
				strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), cmd);
				break;
			case 9: //char ��� ����� �������
				memset(column_buff, 0, MAX_BUFF_SIZE);
				if(row_inf[i] == NULL)
				{
					strcpy_s(column_buff, MAX_BUFF_SIZE, "");
				}
				else if(strlen(row_inf[i]) == 0)
				{
					strcpy_s(column_buff, MAX_BUFF_SIZE, "");
				}
				else
					strcpy_s(column_buff, MAX_BUFF_SIZE, row_inf[i]);

				if(column_index_tab_info_name)
				{
					strcat_s(tab_info_name, MAX_BUFF_SIZE-strlen(tab_info_name), ",");
				}
				if(column_index_tab_info_values)
				{
					strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), ",");
				}
				column_index_tab_info_name++;
				column_index_tab_info_values++;

				//��� ��� ��������:
				strcat_s(tab_info_name, MAX_BUFF_SIZE-strlen(tab_info_name), "html_tab_end");
				//��� ��������:
				memset(cmd, 0, MAX_BUFF_SIZE);
				sprintf_s(cmd, MAX_BUFF_SIZE, "'%s'", column_buff);
				strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), cmd);
				break;
			case 10: //char ��� ������ �������
				memset(column_buff, 0, MAX_BUFF_SIZE);
				if(row_inf[i] == NULL)
				{
					strcpy_s(column_buff, MAX_BUFF_SIZE, "");
				}
				else if(strlen(row_inf[i]) == 0)
				{
					strcpy_s(column_buff, MAX_BUFF_SIZE, "");
				}
				else
					strcpy_s(column_buff, MAX_BUFF_SIZE, row_inf[i]);

				if(column_index_tab_info_name)
				{
					strcat_s(tab_info_name, MAX_BUFF_SIZE-strlen(tab_info_name), ",");
				}
				if(column_index_tab_info_values)
				{
					strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), ",");
				}
				column_index_tab_info_name++;
				column_index_tab_info_values++;

				//��� ��� ��������:
				strcat_s(tab_info_name, MAX_BUFF_SIZE-strlen(tab_info_name), "html_col_begin");
				//��� ��������:
				memset(cmd, 0, MAX_BUFF_SIZE);
				sprintf_s(cmd, MAX_BUFF_SIZE, "'%s'", column_buff);
				strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), cmd);
				break;
			case 11: //char ��� ����� �������
				memset(column_buff, 0, MAX_BUFF_SIZE);
				if(row_inf[i] == NULL)
				{
					strcpy_s(column_buff, MAX_BUFF_SIZE, "");
				}
				else if(strlen(row_inf[i]) == 0)
				{
					strcpy_s(column_buff, MAX_BUFF_SIZE, "");
				}
				else
					strcpy_s(column_buff, MAX_BUFF_SIZE, row_inf[i]);

				if(column_index_tab_info_name)
				{
					strcat_s(tab_info_name, MAX_BUFF_SIZE-strlen(tab_info_name), ",");
				}
				if(column_index_tab_info_values)
				{
					strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), ",");
				}
				column_index_tab_info_name++;
				column_index_tab_info_values++;

				//��� ��� ��������:
				strcat_s(tab_info_name, MAX_BUFF_SIZE-strlen(tab_info_name), "html_col_end");
				//��� ��������:
				memset(cmd, 0, MAX_BUFF_SIZE);
				sprintf_s(cmd, MAX_BUFF_SIZE, "'%s'",column_buff);
				strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), cmd);
				break;
			case 12: //char http-���������
				memset(column_buff, 0, MAX_BUFF_SIZE);
				if(row_inf[i] == NULL)
				{
					strcpy_s(column_buff, MAX_BUFF_SIZE, "");
				}
				else if(strlen(row_inf[i]) == 0)
				{
					strcpy_s(column_buff, MAX_BUFF_SIZE, "");
				}
				else
					strcpy_s(column_buff, MAX_BUFF_SIZE, row_inf[i]);

				if(column_index_tab_info_name)
				{
					strcat_s(tab_info_name, MAX_BUFF_SIZE-strlen(tab_info_name), ",");
				}
				if(column_index_tab_info_values)
				{
					strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), ",");
				}
				column_index_tab_info_name++;
				column_index_tab_info_values++;

				//��� ��� ��������:
				strcat_s(tab_info_name, MAX_BUFF_SIZE-strlen(tab_info_name), "http_head");
				//��� ��������:
				memset(cmd, 0, MAX_BUFF_SIZE);
				sprintf_s(cmd, MAX_BUFF_SIZE, "'%s'", column_buff);
				strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), cmd);
				break;
			case 13: //char insert-������ ��-���������
				break;
			case 14: //int ��� ����� ������� (������������/��������������)
				memset(column_buff, 0, MAX_BUFF_SIZE);
				if(row_inf[i] == NULL)
				{
					strcpy_s(column_buff, MAX_BUFF_SIZE, "0");
				}
				else if(strlen(row_inf[i]) == 0)
				{
					strcpy_s(column_buff, MAX_BUFF_SIZE, "0");
				}
				else
					strcpy_s(column_buff, MAX_BUFF_SIZE, row_inf[i]);

				if(column_index_tab_info_name)
				{
					strcat_s(tab_info_name, MAX_BUFF_SIZE-strlen(tab_info_name), ",");
				}
				if(column_index_tab_info_values)
				{
					strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), ",");
				}
				column_index_tab_info_name++;
				column_index_tab_info_values++;

				//��� ��� ��������:
				strcat_s(tab_info_name, MAX_BUFF_SIZE-strlen(tab_info_name), "vertical");
				//��� ��������:
				memset(cmd, 0, MAX_BUFF_SIZE);
				sprintf_s(cmd, MAX_BUFF_SIZE, "%s", column_buff);
				strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), cmd);
				break;
			default:
				break;
		}
	}

//col:
// 0	id
// 1	new_col_name
// 2	new_col_type
// 3	new_col_size
// 4	new_col_html_code
// 5	new_col_html_hat
// 6	new_col_hat
// 7	new_col_num
// 8	new_col_hidden
// 9	new_col_default_insert
//10	new_col_fix_size
//11	new_col_sort
//12	new_col_group
//13	new_col_search
//14	new_col_select_value

	//��������� �������
	memset(main_tab, 0, MAX_BUFF_SIZE);
	sprintf_s(main_tab, MAX_BUFF_SIZE, "CREATE TABLE %s(id int auto_increment not null, key(id), ", table_name);

	memset(tab_default_name, 0, MAX_BUFF_SIZE);
	sprintf_s(tab_default_name, MAX_BUFF_SIZE, "\"INSERT INTO %s(", table_name);
	memset(tab_default_values, 0, MAX_BUFF_SIZE);
	strcpy_s(tab_default_values, MAX_BUFF_SIZE, " VALUES(");

	column_index_main_tab = 0;
	column_index_tab_default_name = 0;
	column_index_tab_default_values = 0;

	while( (row_col = mysql_fetch_row(res_col)) != NULL)
	{
		memset(tab_columns_name, 0, MAX_BUFF_SIZE);
		strcpy_s(tab_columns_name, MAX_BUFF_SIZE, "INSERT INTO tab_columns(");
		memset(tab_columns_values, 0, MAX_BUFF_SIZE);
		strcpy_s(tab_columns_values, MAX_BUFF_SIZE, " VALUES(");

		column_index_tab_columns_name = 0;
		column_index_tab_columns_values = 0;

		for(i = 0; i < 15; ++i)
		{
			if(row_col == NULL)
			{
				FREE_ALL_VARS;
				mysql_free_result(res_inf);
				mysql_free_result(res_col);
				error_thread_exit(pmysql, ns, "error in new_table: NULL value in some column\n");
			}
			switch(i)
			{
				case 1: //char ��������� ��� �������
					memset(column_name, 0, sizeof(column_name));

					if(column_index_main_tab)
					{
						strcat_s(main_tab, MAX_BUFF_SIZE-strlen(main_tab), ",\n");
					}
					if(column_index_tab_columns_name)
					{
						strcat_s(tab_columns_name, MAX_BUFF_SIZE-strlen(tab_columns_name), ",");
					}
					if(column_index_tab_columns_values)
					{
						strcat_s(tab_columns_values, MAX_BUFF_SIZE-strlen(tab_columns_values), ",");
					}
					if(column_index_tab_default_name)
					{
						strcat_s(tab_default_name, MAX_BUFF_SIZE-strlen(tab_default_name), ",");
					}
					column_index_main_tab++;
					column_index_tab_columns_name++;
					column_index_tab_columns_values++;
					column_index_tab_default_name++;

					//���������� ������ �� �������� ����� �������
					if(strlen(row_col[i]) != 0)
					{
						sprintf_s(column_name, sizeof(column_name), "%s", row_col[i]);
					}
					else //���������� �������� ������� ��� ������ 'create table ...'
					{
						sprintf_s(column_name, sizeof(column_name),  "col%d", column_index_main_tab);
					}
					sprintf_s(cmd, MAX_BUFF_SIZE, "%s mediumtext charset 'cp1251' not null", column_name);
					strcat_s(main_tab, MAX_BUFF_SIZE-strlen(main_tab), cmd); //��������� ��� ������� � SQL-������

					//���������� ������ �� ���������� � tab_columns
					//��� ��� ��������:
					strcat_s(tab_columns_name, MAX_BUFF_SIZE-strlen(tab_columns_name), "col_name");
					//��� �������� ��������:
					memset(cmd, 0, MAX_BUFF_SIZE);
					sprintf_s(cmd, MAX_BUFF_SIZE, "'%s'", column_name);
					strcat_s(tab_columns_values, MAX_BUFF_SIZE-strlen(tab_columns_values), cmd);

					//���������� ������ �� ���������� � tab_info �������� ��-���������
					//��� ��� ��-���������
					strcat_s(tab_default_name, MAX_BUFF_SIZE-strlen(tab_default_name), column_name);
					break;
				case 2: //int ��� �������
					memset(column_buff, 0, MAX_BUFF_SIZE);
					if(row_col[i] == NULL)
					{
						strcpy_s(column_buff, MAX_BUFF_SIZE, "1");
					}
					else if(strlen(row_col[i]) == 0)
					{
						strcpy_s(column_buff, MAX_BUFF_SIZE, "1");
					}
					else
						strcpy_s(column_buff, MAX_BUFF_SIZE, row_col[i]);

					if(column_index_tab_columns_name)
					{
						strcat_s(tab_columns_name, MAX_BUFF_SIZE-strlen(tab_columns_name), ",");
					}
					if(column_index_tab_columns_values)
					{
						strcat_s(tab_columns_values, MAX_BUFF_SIZE-strlen(tab_columns_values), ",");
					}
					column_index_tab_columns_name++;
					column_index_tab_columns_values++;

					//��� ��� ��������:
					strcat_s(tab_columns_name, MAX_BUFF_SIZE-strlen(tab_columns_name), "col_type");
					//��� ��������:
					strcat_s(tab_columns_values, MAX_BUFF_SIZE-strlen(tab_columns_values), column_buff);
					break;
				case 3: //int ������ �������
					memset(column_buff, 0, MAX_BUFF_SIZE);
					if(row_col[i] == NULL)
					{
						strcpy_s(column_buff, MAX_BUFF_SIZE, "10");
					}
					else if(strlen(row_col[i]) == 0)
					{
						strcpy_s(column_buff, MAX_BUFF_SIZE, "10");
					}
					else
						strcpy_s(column_buff, MAX_BUFF_SIZE, row_col[i]);

					if(column_index_tab_columns_name)
					{
						strcat_s(tab_columns_name, MAX_BUFF_SIZE-strlen(tab_columns_name), ",");
					}
					if(column_index_tab_columns_values)
					{
						strcat_s(tab_columns_values, MAX_BUFF_SIZE-strlen(tab_columns_values), ",");
					}
					column_index_tab_columns_name++;
					column_index_tab_columns_values++;

					//��� ��� ��������:
					strcat_s(tab_columns_name, MAX_BUFF_SIZE-strlen(tab_columns_name), "col_size");
					//��� ��������:
					strcat_s(tab_columns_values, MAX_BUFF_SIZE-strlen(tab_columns_values), column_buff);
					break;
				case 4: //char html-��� ������
					memset(column_buff, 0, MAX_BUFF_SIZE);
					if(row_col[i] == NULL)
					{
						strcpy_s(column_buff, MAX_BUFF_SIZE, "<td><textarea form=\"frm\" id=\"%s_%d\" name=\"%s[%d]\" rows=%d cols=%d onChange=\"myreq('%s_%d')\">%s</textarea></td>");
					}
					else if(strlen(row_col[i]) == 0)
					{
						strcpy_s(column_buff, MAX_BUFF_SIZE, "<td><textarea form=\"frm\" id=\"%s_%d\" name=\"%s[%d]\" rows=%d cols=%d onChange=\"myreq('%s_%d')\">%s</textarea></td>");
					}
					else
						strcpy_s(column_buff, MAX_BUFF_SIZE, row_col[i]);

					if(column_index_tab_columns_name)
					{
						strcat_s(tab_columns_name, MAX_BUFF_SIZE-strlen(tab_columns_name), ",");
					}
					if(column_index_tab_columns_values)
					{
						strcat_s(tab_columns_values, MAX_BUFF_SIZE-strlen(tab_columns_values), ",");
					}
					column_index_tab_columns_name++;
					column_index_tab_columns_values++;

					//��� ��� ��������:
					strcat_s(tab_columns_name, MAX_BUFF_SIZE-strlen(tab_columns_name), "html_code");
					//��� ��������:
					memset(cmd, 0, MAX_BUFF_SIZE);
					sprintf_s(cmd, MAX_BUFF_SIZE, "'%s'", make_screen(column_buff));
					strcat_s(tab_columns_values, MAX_BUFF_SIZE-strlen(tab_columns_values), cmd);
					break;
				case 5: //char html-��� ��������� ������� � ����� �������
					memset(column_buff, 0, MAX_BUFF_SIZE);
					if(row_col[i] == NULL)
					{
						strcpy_s(column_buff, MAX_BUFF_SIZE, "<td align=center><h4>%s</h4></td>");
					}
					else if(strlen(row_col[i]) == 0)
					{
						strcpy_s(column_buff, MAX_BUFF_SIZE, "<td align=center><h4>%s</h4></td>");
					}
					else
						strcpy_s(column_buff, MAX_BUFF_SIZE, row_col[i]);

					if(column_index_tab_columns_name)
					{
						strcat_s(tab_columns_name, MAX_BUFF_SIZE-strlen(tab_columns_name), ",");
					}
					if(column_index_tab_columns_values)
					{
						strcat_s(tab_columns_values, MAX_BUFF_SIZE-strlen(tab_columns_values), ",");
					}
					column_index_tab_columns_name++;
					column_index_tab_columns_values++;

					//��� ��� ��������:
					strcat_s(tab_columns_name, MAX_BUFF_SIZE-strlen(tab_columns_name), "html_hat");
					//��� ��������:
					memset(cmd, 0, MAX_BUFF_SIZE);
					sprintf_s(cmd, MAX_BUFF_SIZE, "'%s'", column_buff);
					strcat_s(tab_columns_values, MAX_BUFF_SIZE-strlen(tab_columns_values), cmd);
					break;
				case 6: //char ����� ��������� �������
					memset(column_buff, 0, MAX_BUFF_SIZE);
					if(row_col[i] == NULL)
					{
						strcpy_s(column_buff, MAX_BUFF_SIZE, "");
					}
					else if(strlen(row_col[i]) == 0)
					{
						strcpy_s(column_buff, MAX_BUFF_SIZE, "");
					}
					else
						strcpy_s(column_buff, MAX_BUFF_SIZE, row_col[i]);

					if(column_index_tab_columns_name)
					{
						strcat_s(tab_columns_name, MAX_BUFF_SIZE-strlen(tab_columns_name), ",");
					}
					if(column_index_tab_columns_values)
					{
						strcat_s(tab_columns_values, MAX_BUFF_SIZE-strlen(tab_columns_values), ",");
					}
					column_index_tab_columns_name++;
					column_index_tab_columns_values++;

					//��� ��� ��������:
					strcat_s(tab_columns_name, MAX_BUFF_SIZE-strlen(tab_columns_name), "col_hat");
					//��� ��������:
					memset(cmd, 0, MAX_BUFF_SIZE);
					sprintf_s(cmd, MAX_BUFF_SIZE, "'%s'", column_buff);
					strcat_s(tab_columns_values, MAX_BUFF_SIZE-strlen(tab_columns_values), cmd);
					break;
				case 7: //int ���������� ����� ������� � �������
					memset(column_buff, 0, MAX_BUFF_SIZE);
					if(row_col[i] == NULL)
					{
						strcpy_s(column_buff, MAX_BUFF_SIZE, "1");
					}
					else if(strlen(row_col[i]) == 0)
					{
						strcpy_s(column_buff, MAX_BUFF_SIZE, "1");
					}
					else
						strcpy_s(column_buff, MAX_BUFF_SIZE, row_col[i]);

					if(column_index_tab_columns_name)
					{
						strcat_s(tab_columns_name, MAX_BUFF_SIZE-strlen(tab_columns_name), ",");
					}
					if(column_index_tab_columns_values)
					{
						strcat_s(tab_columns_values, MAX_BUFF_SIZE-strlen(tab_columns_values), ",");
					}
					column_index_tab_columns_name++;
					column_index_tab_columns_values++;

					//��� ��� ��������:
					strcat_s(tab_columns_name, MAX_BUFF_SIZE-strlen(tab_columns_name), "col_num");
					//��� ��������:
					memset(cmd, 0, MAX_BUFF_SIZE);
					sprintf_s(cmd, MAX_BUFF_SIZE, "%s", column_buff);
					strcat_s(tab_columns_values, MAX_BUFF_SIZE-strlen(tab_columns_values), cmd);
					break;
				case 8: //int ���� '������ �������'
					memset(column_buff, 0, MAX_BUFF_SIZE);
					if(row_col[i] == NULL)
					{
						strcpy_s(column_buff, MAX_BUFF_SIZE, "0");
					}
					else if(strlen(row_col[i]) == 0)
					{
						strcpy_s(column_buff, MAX_BUFF_SIZE, "0");
					}
					else
						strcpy_s(column_buff, MAX_BUFF_SIZE, row_col[i]);

					if(column_index_tab_columns_name)
					{
						strcat_s(tab_columns_name, MAX_BUFF_SIZE-strlen(tab_columns_name), ",");
					}
					if(column_index_tab_columns_values)
					{
						strcat_s(tab_columns_values, MAX_BUFF_SIZE-strlen(tab_columns_values), ",");
					}
					column_index_tab_columns_name++;
					column_index_tab_columns_values++;

					//��� ��� ��������:
					strcat_s(tab_columns_name, MAX_BUFF_SIZE-strlen(tab_columns_name), "hidden");
					//��� ��������:
					memset(cmd, 0, MAX_BUFF_SIZE);
					sprintf_s(cmd, MAX_BUFF_SIZE, "%s", column_buff);
					strcat_s(tab_columns_values, MAX_BUFF_SIZE-strlen(tab_columns_values), cmd);
					break;
				case 9: //char �������� ��-���������
					memset(column_buff, 0, MAX_BUFF_SIZE);
					if(row_col[i] == NULL)
					{
						strcpy_s(column_buff, MAX_BUFF_SIZE, "");
					}
					else if(strlen(row_col[i]) == 0)
					{
						strcpy_s(column_buff, MAX_BUFF_SIZE, "");
					}
					else
						strcpy_s(column_buff, MAX_BUFF_SIZE, row_col[i]);

					if(column_index_tab_default_values)
					{
						strcat_s(tab_default_values, MAX_BUFF_SIZE-strlen(tab_default_values), ",");
					}
					column_index_tab_default_values++;

					//��������� � 'tab_info' �������� ��� ���������� ��-���������
					//��� �������� ��-���������:
					memset(cmd, 0, MAX_BUFF_SIZE);
					sprintf_s(cmd, MAX_BUFF_SIZE, "'%s'", make_screen(column_buff));
					strcat_s(tab_default_values, MAX_BUFF_SIZE-strlen(tab_default_values), cmd);
					break;
				case 10: //int ���� '����������� ������'
					break;
				case 11: //int ���� '����������� �� ������� �������'
					memset(column_buff, 0, MAX_BUFF_SIZE);
					if(row_col[i] == NULL)
					{
						strcpy_s(column_buff, MAX_BUFF_SIZE, "0");
					}
					else if(strlen(row_col[i]) == 0)
					{
						strcpy_s(column_buff, MAX_BUFF_SIZE, "0");
					}
					else
						strcpy_s(column_buff, MAX_BUFF_SIZE, row_col[i]);

					if(column_index_tab_columns_name)
					{
						strcat_s(tab_columns_name, MAX_BUFF_SIZE-strlen(tab_columns_name), ",");
					}
					if(column_index_tab_columns_values)
					{
						strcat_s(tab_columns_values, MAX_BUFF_SIZE-strlen(tab_columns_values), ",");
					}
					column_index_tab_columns_name++;
					column_index_tab_columns_values++;

					//��� ��� ��������:
					strcat_s(tab_columns_name, MAX_BUFF_SIZE-strlen(tab_columns_name), "col_sort");
					//��� ��������:
					memset(cmd, 0, MAX_BUFF_SIZE);
					sprintf_s(cmd, MAX_BUFF_SIZE, "%s", column_buff);
					strcat_s(tab_columns_values, MAX_BUFF_SIZE-strlen(tab_columns_values), cmd);
					break;
				case 12: //int ���� '������������ �� ������� �������'
					break;
				case 13: //int ���� '������ � ������ �������'
					memset(column_buff, 0, MAX_BUFF_SIZE);
					if(row_col[i] == NULL)
					{
						strcpy_s(column_buff, MAX_BUFF_SIZE, "1");
					}
					else if(strlen(row_col[i]) == 0)
					{
						strcpy_s(column_buff, MAX_BUFF_SIZE, "1");
					}
					else
						strcpy_s(column_buff, MAX_BUFF_SIZE, row_col[i]);

					if(column_index_tab_columns_name)
					{
						strcat_s(tab_columns_name, MAX_BUFF_SIZE-strlen(tab_columns_name), ",");
					}
					if(column_index_tab_columns_values)
					{
						strcat_s(tab_columns_values, MAX_BUFF_SIZE-strlen(tab_columns_values), ",");
					}
					column_index_tab_columns_name++;
					column_index_tab_columns_values++;

					//��� ��� ��������:
					strcat_s(tab_columns_name, MAX_BUFF_SIZE-strlen(tab_columns_name), "col_search");
					//��� ��������:
					memset(cmd, 0, MAX_BUFF_SIZE);
					sprintf_s(cmd, MAX_BUFF_SIZE, "%s", column_buff);
					strcat_s(tab_columns_values, MAX_BUFF_SIZE-strlen(tab_columns_values), cmd);
					break;
				case 14: //char �������� ��� ������
					memset(column_buff, 0, MAX_BUFF_SIZE);
					if(row_col[i] == NULL)
					{
						strcpy_s(column_buff, MAX_BUFF_SIZE, "");
					}
					else if(strlen(row_col[i]) == 0)
					{
						strcpy_s(column_buff, MAX_BUFF_SIZE, "");
					}
					else
						strcpy_s(column_buff, MAX_BUFF_SIZE, row_col[i]);

					//������� ����� (���� �� �� ����) � ���������� � �������
					if(strlen(column_buff) > 0)
					{
						value_of_select[value_of_select_index][0] = new char[strlen(column_name)+1];
						strcpy_s(value_of_select[value_of_select_index][0], strlen(column_name)+1, column_name);
						value_of_select[value_of_select_index][1] = new char[strlen(column_buff)+1];
						strcpy_s(value_of_select[value_of_select_index][1], strlen(column_buff)+1, column_buff);

						value_of_select_index++;
					}

					break;
				default:
					break;
			}
		}

		memset(cmd, 0, MAX_BUFF_SIZE);
		strcpy_s(cmd, MAX_BUFF_SIZE, tab_columns_name);
		strcat_s(cmd, MAX_BUFF_SIZE-strlen(cmd), ",tab_id)");
		strcat_s(cmd, MAX_BUFF_SIZE-strlen(cmd), tab_columns_values);
		len = strlen(tab_columns_values)+TAB_BUFF_UP;
		columns_sql_buff[column_index] = new char[len];
		memset(columns_sql_buff[column_index], 0, len);
		strcpy_s(columns_sql_buff[column_index], len-1, cmd); //������ columns_sql_buff �������� ����� ������� ������
		column_index++;
	}

	memset(cmd, 0, MAX_BUFF_SIZE);
	strcpy_s(cmd, MAX_BUFF_SIZE, "INSERT INTO tab_columns(col_name,col_type,html_code,html_hat,col_hat,col_size,col_num,hidden,col_sort,col_search,tab_id) \
VALUES('id',0,'<td><textarea form=\"frm\" id=\"%s_%d\" name=\"%s[%d]\" rows=%d cols=%d onChange=\"myreq(&#039;%s_%d&#039;)\">%s</textarea></td>\
','<td align=center><h4>%s</h4></td>','����',4,0,1,0,0");
	len = strlen(cmd)+TAB_BUFF_UP;
	columns_sql_buff[column_index] = new char[len];
	memset(columns_sql_buff[column_index], 0, len);
	strcpy_s(columns_sql_buff[column_index], len-1, cmd); //������ columns_sql_buff �������� ����� ������� ������
	column_index++;

	mysql_free_result(res_inf);
	mysql_free_result(res_col);

	//1) ������� ������ �������������: ") charset 'cp1251'"
	//2) ��������� tab_info ��������� �� default 'insert into ...'
	//3) ��������� tab_id ��� ������ tab_info � tab_columns

	//����������� ��������� �������
	strcat_s(main_tab, MAX_BUFF_SIZE-strlen(main_tab), ") charset 'cp1251'");
	fprintf(stderr, "%s\n\n",main_tab);
	//������ ���� �������
	if(mysql_query(pmysql, main_tab) != 0)
	{
		FREE_ALL_VARS;
		mysql_error_thread_exit(pmysql, ns);
	}

	//��������� ��� ������� �� ��������� ������� new_table_info (��� ��������� �����)
	memset(cmd, 0, MAX_BUFF_SIZE);
	sprintf_s(cmd, MAX_BUFF_SIZE, "UPDATE new_table_info SET new_tab_name='%s'", table_name);
	if(mysql_query(pmysql, cmd) != 0)
	{
		FREE_ALL_VARS;
		mysql_error_thread_exit(pmysql, ns);
	}

	//��� ������ � all_tables � ���������� tab_id
	//��������� ������ �������:
	memset(cmd, 0, MAX_BUFF_SIZE);
	sprintf_s(cmd, MAX_BUFF_SIZE, "'<input type=\"button\" value=\"%s\" onclick=\"location.href=&#039;http://%%s/%s&#039;\" />'", strlen(table_title_name)?table_title_name:table_name, table_name);
	strcat_s(all_tables_name, MAX_BUFF_SIZE-strlen(all_tables_name), ",");
	strcat_s(all_tables_values, MAX_BUFF_SIZE-strlen(all_tables_values), ",");
	strcat_s(all_tables_name, MAX_BUFF_SIZE-strlen(all_tables_name), "tab_button");
	strcat_s(all_tables_values, MAX_BUFF_SIZE-strlen(all_tables_values), cmd);
	//��������� �������� �� ������ �������:
	memset(cmd, 0, MAX_BUFF_SIZE);
	strcat_s(all_tables_name, MAX_BUFF_SIZE-strlen(all_tables_name), ",");
	strcat_s(all_tables_values, MAX_BUFF_SIZE-strlen(all_tables_values), ",");
	strcat_s(all_tables_name, MAX_BUFF_SIZE-strlen(all_tables_name), "delete_button");
	strcat_s(all_tables_values, MAX_BUFF_SIZE-strlen(all_tables_values), "' '");
	//��������� ��������� �������:
	strcat_s(all_tables_name, MAX_BUFF_SIZE-strlen(all_tables_name), ")");
	strcat_s(all_tables_values, MAX_BUFF_SIZE-strlen(all_tables_values), ")");
	//��������� ������������� SQL-������ � ��������� ���:
	memset(cmd, 0, MAX_BUFF_SIZE);
	strcpy_s(cmd, MAX_BUFF_SIZE, all_tables_name);
	strcat_s(cmd, MAX_BUFF_SIZE-strlen(cmd), all_tables_values);
	fprintf(stderr, "%s\n\n",cmd);
	if(mysql_query(pmysql, cmd) != 0)
	{
		FREE_ALL_VARS;
		mysql_error_thread_exit(pmysql, ns);
	}
	//��� ���������� tab_id:
	memset(cmd, 0, MAX_BUFF_SIZE);
	sprintf_s(cmd, MAX_BUFF_SIZE, "SELECT id FROM all_tables WHERE tab_name='%s'", table_name);
	fprintf(stderr, "%s\n\n",cmd);
	if(mysql_query(pmysql, cmd) != 0)
	{
		FREE_ALL_VARS;
		mysql_error_thread_exit(pmysql, ns);
	}
	else
	{
		res_inf = mysql_store_result(pmysql);
		num_inf = mysql_affected_rows(pmysql);
	}
	if(num_inf == 0)
	{
		FREE_ALL_VARS;
		return;
	}
	if( (row_inf = mysql_fetch_row(res_inf)) == NULL )
	{
		FREE_ALL_VARS;
		return;
	}
	memset(tab_id, 0, sizeof(tab_id));
	strcpy_s(tab_id, sizeof(tab_id), row_inf[0]);
	mysql_free_result(res_inf);

	//��������� all_tables ������� ��� �������� �������
	memset(cmd, 0, MAX_BUFF_SIZE);
	sprintf_s(cmd, MAX_BUFF_SIZE, "UPDATE all_tables SET delete_button=\"<input type='button' value='�������' form='frm' onclick=\\\"del_tab('delete_table','tab_name_%s')\\\" />\" WHERE id=%s", tab_id, tab_id);
	fprintf(stderr, "%s\n\n",cmd);
	if(mysql_query(pmysql, cmd) != 0)
	{
		FREE_ALL_VARS;
		mysql_error_thread_exit(pmysql, ns);
	}

	//��������� tab_info ��������� �� default 'insert into ...'
	strcat_s(tab_default_name, MAX_BUFF_SIZE-strlen(tab_default_name), ")");
	strcat_s(tab_default_values, MAX_BUFF_SIZE-strlen(tab_default_values), ")\"");
	strcat_s(tab_info_name, MAX_BUFF_SIZE-strlen(tab_info_name), ",default_insert");
	strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), ",");
	strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), tab_default_name);
	strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), tab_default_values);

	//��������� tab_id ��� ������ tab_info � tab_columns
	//tab_info:
	strcat_s(tab_info_name, MAX_BUFF_SIZE-strlen(tab_info_name), ",tab_id");
	strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), ",");
	strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), tab_id);
	strcat_s(tab_info_name, MAX_BUFF_SIZE-strlen(tab_info_name), ")");
	strcat_s(tab_info_values, MAX_BUFF_SIZE-strlen(tab_info_values), ")");
	//������ �������� � ������� tab_info
	//�������� tab_info:
	memset(cmd, 0, MAX_BUFF_SIZE);
	strcpy_s(cmd, MAX_BUFF_SIZE, tab_info_name);
	strcat_s(cmd, MAX_BUFF_SIZE-strlen(cmd), tab_info_values);
	fprintf(stderr, "%s\n\n",cmd);
	if(mysql_query(pmysql, cmd) != 0)
	{
		FREE_ALL_VARS;
		mysql_error_thread_exit(pmysql, ns);
	}

	//������ �������� � ������� tab_columns
	for(i = 0; i < column_index; ++i)
	{
		strcat_s(columns_sql_buff[i], strlen(columns_sql_buff[i])+TAB_BUFF_UP, ",");
		strcat_s(columns_sql_buff[i], strlen(columns_sql_buff[i])+TAB_BUFF_UP-1, tab_id);
		strcat_s(columns_sql_buff[i], strlen(columns_sql_buff[i])+TAB_BUFF_UP-16, ")");
		fprintf(stderr, "%s\n\n",columns_sql_buff[i]);
		if(mysql_query(pmysql, columns_sql_buff[i]) != 0)
		{
			FREE_ALL_VARS;
			mysql_error_thread_exit(pmysql, ns);
		}
	}

	//������ �������� ����� ���� '�����'
	for(i = 0; i < value_of_select_index; ++i)
	{
		memset(cmd, 0, MAX_BUFF_SIZE);
		sprintf_s(cmd, MAX_BUFF_SIZE, "INSERT INTO tab_selects(tab_id,col_name,value_of_select) VALUES(%s,\"%s\",\"%s\")", tab_id, value_of_select[i][0], value_of_select[i][1]);
		fprintf(stderr, "%s\n\n",cmd);
		if(mysql_query(pmysql, cmd) != 0)
		{
			FREE_ALL_VARS;
			mysql_error_thread_exit(pmysql, ns);
		}
	}

//	fprintf(stderr, "%s\n\n%s\n\n%s\n\n%s\n\n%s\n\n%s\n\n%s\n\n%s\n\n%s\n\n", 
//					main_tab,tab_info_name,tab_info_values,tab_columns_name,tab_columns_values,
//					all_tables_name,all_tables_values,tab_default_name,tab_default_values);

	FREE_ALL_VARS;
	return;
}

void delete_table(char const * const table_name, MYSQL * const pmysql, SOCKET * const ns)
{
	char *cmd;//[MAX_BUFF_SIZE];
//	MYSQL_RES *res_inf;
//	MYSQL_ROW row_inf;
//	int num_inf;

	cmd = new char[MAX_BUFF_SIZE];
	memset(cmd, 0, MAX_BUFF_SIZE);

	//������� �������� �������� �������
	sprintf_s(cmd, MAX_BUFF_SIZE, "DELETE FROM tab_columns WHERE tab_id=(SELECT id FROM all_tables WHERE tab_name=\"%s\")", table_name);
	if(mysql_query(pmysql, cmd) != 0)
	{
		delete [] cmd;
		mysql_error_thread_exit(pmysql, ns);
	}
	memset(cmd, 0, MAX_BUFF_SIZE);

	//������� �������� �������
	sprintf_s(cmd, MAX_BUFF_SIZE, "DELETE FROM tab_info WHERE tab_id=(SELECT id FROM all_tables WHERE tab_name=\"%s\")", table_name);
	if(mysql_query(pmysql, cmd) != 0)
	{
		delete [] cmd;
		mysql_error_thread_exit(pmysql, ns);
	}
	memset(cmd, 0, MAX_BUFF_SIZE);

	//������� �������� �������� ���� '�����' �������
	sprintf_s(cmd, MAX_BUFF_SIZE, "DELETE FROM tab_selects WHERE tab_id=(SELECT id FROM all_tables WHERE tab_name=\"%s\")", table_name);
	if(mysql_query(pmysql, cmd) != 0)
	{
		delete [] cmd;
		mysql_error_thread_exit(pmysql, ns);
	}
	memset(cmd, 0, MAX_BUFF_SIZE);

	//������� �������� �������� ���� '�������' �������
	sprintf_s(cmd, MAX_BUFF_SIZE, "DELETE FROM tab_dynamic WHERE tab_id=(SELECT id FROM all_tables WHERE tab_name=\"%s\")", table_name);
	if(mysql_query(pmysql, cmd) != 0)
	{
		delete [] cmd;
		mysql_error_thread_exit(pmysql, ns);
	}
	memset(cmd, 0, MAX_BUFF_SIZE);

	//������� ������ � �������
	sprintf_s(cmd, MAX_BUFF_SIZE, "DELETE FROM all_tables WHERE tab_name=\"%s\"", table_name);
	if(mysql_query(pmysql, cmd) != 0)
	{
		delete [] cmd;
		mysql_error_thread_exit(pmysql, ns);
	}

	//������� ���� �������
	sprintf_s(cmd, MAX_BUFF_SIZE, "DROP TABLE %s", table_name);
	if(mysql_query(pmysql, cmd) != 0)
	{
		delete [] cmd;
		mysql_error_thread_exit(pmysql, ns);
	}

	delete [] cmd;
}