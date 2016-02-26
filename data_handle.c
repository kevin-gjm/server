#include <stdio.h>
#include <event.h>
//for http
#include <evhttp.h>
#include <signal.h>
#include <string.h>
#include "data_handle.h"
#include "cJSON.h"

void sync_table_equipment( cJSON* insert,cJSON *out ,char *IP,char*);
void sync_table_info(cJSON * insert,cJSON *out,char * IP,char*);

int execute_cgi_get(struct evkeyvalq *params)
{
	char tmp[1024]={0};
	sprintf(tmp,"key=%s",evhttp_find_header(params,"key"));
	printf("%s\n",tmp);
	sprintf(tmp,"name=%s",evhttp_find_header(params,"name"));
	printf("%s\n",tmp);
	sqltest();
	return 1;

}

int execute_cgi_post(struct evhttp_request *req)
{
	char post[4096]={0};

	const char * length=evhttp_find_header(req->input_headers, "Content-Length");  
	printf("length: %s\n",length);

	size_t post_size=EVBUFFER_LENGTH(req->input_buffer);
	char *post_data = (char *) EVBUFFER_DATA(req->input_buffer);	
	
	sprintf(post, "%s", post_data);
	post[post_size]='\0';
	printf("postdata start :\n%s\npostdata end\n",post);
//	printf("post start :\n%s\npost end\n",out);
	
	/*
	 *create the return json
	 *
	 *
	 */
	cJSON *root,*line,*id;
	root = cJSON_CreateObject();
//	cJSON_AddItemToArray(line,id=cJSON_CreateArray());

	char ret[1024]={0};

	/*
	 *
	 * parase the post json
	 * return the success json id
	 *
	 */
	cJSON *json;
	json=cJSON_Parse(post);
	if(!json){printf("Error before:[%s]\n",cJSON_GetErrorPtr()); return ;}
	else
	{
		cJSON * pSubIP = cJSON_GetObjectItem(json, "IP");
		if(NULL == pSubIP)
		{
			//get object named "hello" faild
			return;
		}
		printf("IP :%s\n", pSubIP->valuestring);
	//	printf("obj_1 : %d\n", pSub->valueint);
		cJSON *pSubTable = cJSON_GetObjectItem(json, "table");
		if(NULL == pSubTable)
		{
			//get object named "hello" faild
			return ;
		}
	//	printf("obj_2 : %s\n", pSub->valuestring);
		printf("table :%s\n", pSubTable->valuestring);

		cJSON *pSub = cJSON_GetObjectItem(json, "Content");
		if(NULL == pSub)
		{
			//get object named "hello" faild
			return ;
		}
		printf("before strcmp\n");
		char *table_equipment="equipment";
		char *table_info="info";
		strcat(ret,pSubTable->valuestring);
		strcat(ret,";");

		cJSON_AddStringToObject(root,"table",pSubTable->valuestring);	
		cJSON_AddItemToObject(root,"ID",id=cJSON_CreateArray());
		if(strncmp(pSubTable->valuestring,table_equipment,9) == 0)
		{
			//sync_table_equipment(pSub,id,pSubIP->valuestring);
			sync_table_equipment(pSub,id,pSubIP->valuestring,ret);
		}
		else if(strncmp(pSubTable->valuestring,table_info,4) == 0)
		{
			//sync_table_info(pSub,id,pSubIP->valuestring);
			sync_table_info(pSub,id,pSubIP->valuestring,ret);
		}else
		{
			//fault
		}
	}
	cJSON_Delete(json);
	printf("return char:%s\n",ret);
	printf("return json:%s\n",cJSON_Print(root));

	struct evbuffer *buf;
	buf = evbuffer_new();
	//输出的内容
	evhttp_send_reply_start(req,200,"OK");
	//HTTP header
//	evhttp_add_header(req->output_headers, "Server", MYHTTPD_SIGNATURE);
//	evhttp_add_header(req->output_headers, "Content-Type", "text/plain; charset=UTF-8");
	//evhttp_add_header(req->output_headers, "Connection", "Keep-Alive");
	evhttp_add_header(req->output_headers, "Connection", "close");

	//evbuffer_add_printf(buf,"%s",cJSON_Print(root));
	evbuffer_add_printf(buf,"%s",ret);
	//add content
	evhttp_send_reply_chunk(req,buf);

	evhttp_send_reply_end(req);
	evbuffer_free(buf);
	cJSON_Delete(root);

}

void sync_table_equipment( cJSON* content,cJSON *json_id ,char *IP,char *ret)
{
	printf("in function\n");
		cJSON *insert = (cJSON*) content;			
		int iSize = cJSON_GetArraySize(insert);
		int i=0,j=0;
		for(i=0;i<iSize;i++)
		{
			cJSON *pSubSub = cJSON_GetArrayItem(insert,i);
			if(NULL == pSubSub )
				continue;
			int iSizeSize= cJSON_GetArraySize(pSubSub);
			char out[4096]={0};
			char temp[1024]={0};
			char id[50]={0};
			sprintf(temp,"insert into equipment(ip,eqid,type,method) values('%s',",IP);
			strcat(out,temp);
			for(j=0;j<iSizeSize;j++)
			{	
				cJSON *pSubSubSub = cJSON_GetArrayItem(pSubSub,j);
				if(NULL == pSubSubSub)
					continue;
				if(j==0)
				{
					sprintf(id,"%s",pSubSubSub->valuestring);
				}
				if(j==iSizeSize-1)
				{
					sprintf(temp,"'%s' ) on duplicate key update type=values(type),method=values(method) ;",pSubSubSub->valuestring);
					strcat(out,temp);
				}else
				{
					sprintf(temp,"'%s',",pSubSubSub->valuestring);
					strcat(out,temp);
				}
				printf("value[%d][%d]:%s\n",i,j,pSubSubSub->valuestring);
			}
			printf("sql:%s\n",out);
			if(operate_by_sql(out) < 0)
			{	
				printf("op sql fail\n");
	//			return;
			}else
			{
				printf("success!\n");
				strcat(ret,id);
				strcat(ret,",");
				cJSON_AddStringToObject(json_id,"",id);
				printf("after add string\n");	
			}
		}
}

void sync_table_info(cJSON * insert,cJSON *out,char * IP,char * ret)
{

}
