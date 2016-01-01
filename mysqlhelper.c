#include <stdio.h>
#include <string.h>
#include "mysqlhelper.h"
#include "cJSON.h"
static MYSQL *DBOHBJ;


int mysqlhelper_init(const char *host,const char *username,const char *passwd,const char *database)
{
	void* ret = NULL;

	DBOHBJ = mysql_init(NULL);
	
	if((ret = mysql_real_connect(DBOHBJ,host,username,passwd,database,0,NULL,0)) == NULL) 
	{
		fprintf(stderr,"connect error:%s:%s\n",__func__,mysql_error(DBOHBJ));
		return -1;
	}
	
	return 0;
}

void  mysqlhelper_destroy()
{
	mysql_close(DBOHBJ);
}
/*
void display_head(MYSQL_RES * res,void (*show_head)(char*))
{
	MYSQL_FIELD * field_ptr;
	while((field_ptr = mysql_fetch_field(res))!=NULL)
	{
		show_head(field_ptr->name);
	}
}

int mysqlhelper_select_data(const char *sql,char * out)
{
	int ret=-1;
	MYSQL_RES *res;
	MYSQL_ROW row;
	cJSON *root,*header,*content;
	unsigned int field_count =0;

	if(mysql_query(DBOHBJ,sql))
	{
		fprintf(stderr,"select data error:%s:%s\n",__func__,mysql_error(DBOHBJ));
	
		return -1;
	}else
	{
	//	res = mysql_use_result(DBOHBJ);
		res = mysql_store_result(DBOHBJ);
	
		if(res)
		{
			root = cJSON_CreateObject();
			cJSON_AddItemToObject(root,"Headers",header=cJSON_CreateObject());
			display_head(res,header);
			ret = mysql_num_rows(res);
			
			while((row = mysql_fetch_row(res)) != NULL)
			{
				field_count = 0;
				show_line_begin();
				while(field_count < mysql_field_count(DBOHBJ))
				{
					show_element(row[field_count]);
					field_count++;
				}
				show_line_end();
			}
			mysql_free_result(res);
		}
		else
		{	
			fprintf(stderr,"select data error:%s:%s\n",__func__,mysql_error(DBOHBJ));
			
			return -1;
		}
	}

	return ret;
}
*/
void display_head(MYSQL_RES * res,void (*show_head)(char*))
{
	MYSQL_FIELD * field_ptr;
	while((field_ptr = mysql_fetch_field(res))!=NULL)
	{
		show_head(field_ptr->name);
	}
}

int mysqlhelper_select_data(const char *sql,void(*show_line_begin)(),void(*show_line_end)(),void(*show_element)(char*),void(*show_head)(char*))
{
	int ret=-1;
	MYSQL_RES *res;
	MYSQL_ROW row;
	unsigned int field_count =0;

	if(mysql_query(DBOHBJ,sql))
	{
		fprintf(stderr,"select data error:%s:%s\n",__func__,mysql_error(DBOHBJ));
	
		return -1;
	}else
	{
	//	res = mysql_use_result(DBOHBJ);
		res = mysql_store_result(DBOHBJ);
	
		if(res)
		{
			
			display_head(res,show_head);
			ret = mysql_num_rows(res);
			
			while((row = mysql_fetch_row(res)) != NULL)
			{
				field_count = 0;
				show_line_begin();
				while(field_count < mysql_field_count(DBOHBJ))
				{
					show_element(row[field_count]);
					field_count++;
				}
				show_line_end();
			}
			mysql_free_result(res);
		}
		else
		{	
			fprintf(stderr,"select data error:%s:%s\n",__func__,mysql_error(DBOHBJ));
			
			return -1;
		}
	}

	return ret;
}
int	operate_data(const char *sql)
{
	int ret = -1;
	if(mysql_real_query(DBOHBJ,"BEGIN;",(unsigned int)strlen("BEGIN;")))
	{
		
		fprintf(stderr,"operate data error:%s:%s\n",__func__,mysql_error(DBOHBJ));
			
		return -1;
	}
	ret = mysql_query(DBOHBJ,sql);
	if(!ret)
	{
		mysql_real_query(DBOHBJ,"COMMIT;",(unsigned int)strlen("COMMIT;"));
		return 0;
	}else
	{	

		mysql_real_query(DBOHBJ,"ROLLBACK;",(unsigned int)strlen("ROLLBACK;"));
		return -1;
	}

}


