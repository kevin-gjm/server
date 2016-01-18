#include <stdio.h>
#include <event.h>
//for http
#include <evhttp.h>
#include <signal.h>
#include <string.h>
#include "data_handle.h"
#include "cJSON.h"
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
//	char out[4096]={0};

	const char * length=evhttp_find_header(req->input_headers, "Content-Length");  
	printf("length: %s\n",length);

	size_t post_size=EVBUFFER_LENGTH(req->input_buffer);
	char *post_data = (char *) EVBUFFER_DATA(req->input_buffer);	
//	sprintf(out, "%s", post_data);
//	out[post_size]='\0';
	printf("postdata start :\n%s\npostdata end\n",post_data);
//	printf("post start :\n%s\npost end\n",out);


	cJSON *json;
	json=cJSON_Parse(post_data);
	if(!json){printf("Error before:[%s]\n",cJSON_GetErrorPtr()); return ;}
	else
	{
		cJSON * pSubCount = cJSON_GetObjectItem(json, "count");
		if(NULL == pSubCount)
		{
			//get object named "hello" faild
		}
	//	printf("obj_1 : %d\n", pSub->valueint);
		cJSON *pSubTable = cJSON_GetObjectItem(json, "table");
		if(NULL == pSubTable)
		{
			//get object named "hello" faild
			return ;
		}
	//	printf("obj_2 : %s\n", pSub->valuestring);

		cJSON *pSub = cJSON_GetObjectItem(json, "Content");
		if(NULL == pSub)
		{
			//get object named "hello" faild
			return ;
		}
		int iSize = cJSON_GetArraySize(pSub);
		int i=0,j=0;
		for(i=0;i<iSize;i++)
		{
			cJSON *pSubSub = cJSON_GetArrayItem(pSub,i);
			if(NULL == pSubSub )
				continue;
			int iSizeSize= cJSON_GetArraySize(pSubSub);
			char out[4096]={0};
			char temp[1024]={0};
			sprintf(temp,"insert into %s  (eqid,type,method) values(",pSubTable->valuestring);
			strcat(out,temp);
			for(j=0;j<iSizeSize;j++)
			{	
				cJSON *pSubSubSub = cJSON_GetArrayItem(pSubSub,j);
				if(NULL == pSubSubSub)
					continue;
				if(j==iSizeSize-1)
				{
					sprintf(temp,"%s );",pSubSubSub->valuestring);
					strcat(out,temp);
				}else
				{
					sprintf(temp,"%s,",pSubSubSub->valuestring);
					strcat(out,temp);
				}
				printf("value[%d][%d]:%s\n",i,j,pSubSubSub->valuestring);
			}
			printf("sql:%s\n",out);
			if(operate_by_sql(out) < 0)
			{	
				printf("op sql fail\n");
				return;
			}else
			{
				printf("success!\n");
			}
		}
	}
	cJSON_Delete(json);

	struct evbuffer *buf;
	buf = evbuffer_new();
	//输出的内容
	evhttp_send_reply_start(req,200,"OK");
	//HTTP header
//	evhttp_add_header(req->output_headers, "Server", MYHTTPD_SIGNATURE);
//	evhttp_add_header(req->output_headers, "Content-Type", "text/plain; charset=UTF-8");
	//evhttp_add_header(req->output_headers, "Connection", "Keep-Alive");
	evhttp_add_header(req->output_headers, "Connection", "close");

	evbuffer_add_printf(buf,"%s","success");
	//add content
	evhttp_send_reply_chunk(req,buf);

	evhttp_send_reply_end(req);
	evbuffer_free(buf);

}
