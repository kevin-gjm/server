#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mysqlhelper.h"
#include "sql.h"
char *username = "root";
char *passwd = "ming";
char *host = "localhost";
char *database = "paper";

char out[20480] = {};
char temp[256] = {};

void sqltest()
{
	int row;

	char strsql[200];

	mysqlhelper_init(host,username,passwd,database);

//	operate_data("insert into user values(1,'ming','ming','10.9.3.20')");

	sprintf(strsql,"select * from user");
	
	char * jout =NULL;
	int count =0;
	jout  = (char *)mysqlhelper_select_json_data(strsql,&count);
	if(NULL != jout)
	{
		printf("\n\njson:%s\n\n",jout);	
		free(jout);
	}else
	{	
		printf("\n\njout is NULL\n\n");
	}
	mysqlhelper_destroy();
} 

