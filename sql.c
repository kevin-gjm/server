#include <stdio.h>
#include <string.h>
#include "mysqlhelper.h"
#include "sql.h"
char *username = "root";
char *passwd = "ming";
char *host = "localhost";
char *database = "paper";

char out[20480] = {};
char temp[256] = {};

void show_head(char * s)
{
	
}
void show_line_begin()
{
	sprintf(temp,"<tr>");
	strcat(out,temp);
}
void show_line_end()
{
	sprintf(temp,"</tr>");
	strcat(out,temp) ;
}
void show_element(char *s)
{
	if(s!=NULL)
	{
		sprintf(temp,"<th>%s</th>",s);
	}else
	{
		sprintf(temp,"<th>NULL</th>");
	}
	strcat(out,temp);
}


/*
void select_data(char * strsql,char * out)
{

	cJSON *root,*header,*content;

	mysqlhelper_init(host,username,passwd,database);

//	operate_data("insert into user values(1,'ming','ming','10.9.3.20')");


	row = mysqlhelper_select_data(strsql,show_line_begin,show_line_end,show_element,show_head);

	mysqlhelper_destroy();
} 
*/
void sqltest()
{
	int row;

	char strsql[200];

	mysqlhelper_init(host,username,passwd,database);

//	operate_data("insert into user values(1,'ming','ming','10.9.3.20')");

	sprintf(strsql,"select * from user");
	
	strcat(out,"<table border = \"1\">");

	row = mysqlhelper_select_data(strsql,show_line_begin,show_line_end,show_element,show_head);

	strcat(out,"</table>");

//	printf("row nums:%d\n",row);

//	printf("%s\n",out);

	mysqlhelper_destroy();
} 

