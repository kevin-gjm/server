#include <stdio.h> 
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>     //for getopt, fork
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>     //for strcat
//for struct evkeyvalq
#include <sys/queue.h>
#include <event.h>
//for http
#include <evhttp.h>
#include <signal.h>
#include "mysqlhelper.h"
#include "http_handle.h"

#define MYHTTPD_SIGNATURE   "myhttpd v 0.0.1"
#define ROOT "www%s"
#define DE_FILE_TYPE "index.html"


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
/*===================
 * 功能： 获取http请求路径
 * 参数：uri：URL
 *		 path：路径 
 * 返回值：int
 *			路径后面是否有参数（cgi）
 *			0 没有
 *
 *			1 有
 ====================*/
int http_get_path(const char* uri,char *path) 
{
	int ret =0;
	char * temp_uri = NULL;
	char * query_string = NULL;
	temp_uri = calloc(strlen(uri)+1,sizeof(char));
	memcpy(temp_uri,uri,strlen(uri)+1);
	query_string = temp_uri;
	while((*query_string != '?')&&(*query_string != '\0'))
	{	
		query_string++;
	}

	if(*query_string == '?')
	{
		ret = 1;
		if(query_string - temp_uri > 450)
		{
			printf("error:path too longi%s\n",__func__);
			exit(-1);
		}
		*query_string = '\0';
	}
	sprintf(path,"www%s",temp_uri);
	if(path[strlen(path)-1] == '/')
		strcat(path,DE_FILE_TYPE);
	free(temp_uri);

	return ret;
}
void not_found(struct evhttp_request *req,const char *filename)
{
#define ERR_FORMAT "<html><head>" \
	"<title>404 Not Found</title>" \
	"</head><body>" \
	"<div style=\"text-align:center\">" \
	"<br>" \
	"<br>" \
	"<br>" \
	"<h1>Not Found</h1>" \
	"<p>The requested URL %s was not found on this server.</p>"\
	"</div>" \
	"</body></html>\n"

	struct evbuffer * buf = evbuffer_new();

	//	evhttp_response_code(req,HTTP_NOTFOUND,"Not Found");

	//	evbuffer_add_printf(buf,ERR_FORMAT,filename);

	//	evhttp_send_page(req,buf);

	//	evbuffer_free(buf);
	evhttp_send_reply_start(req,404,"Not Found");

	evbuffer_add_printf(buf,ERR_FORMAT,filename+3);

	evhttp_send_reply_chunk(req,buf);

	evhttp_send_reply_end(req);

	evbuffer_free(buf);
#undef ERR_FORMAT
}
void serve_file(struct evhttp_request *req,const char * filename)
{
	int fd,r;
	char tempbuf[1025];
	if((fd = open(filename,O_RDONLY)) < 0)
	{
		not_found(req,filename);	
	}
	else
	{
		struct evbuffer *buf;
		buf = evbuffer_new();
		//输出的内容
		evhttp_send_reply_start(req,200,"OK");
		//HTTP header
		evhttp_add_header(req->output_headers, "Server", MYHTTPD_SIGNATURE);
		evhttp_add_header(req->output_headers, "Content-Type", "text/plain; charset=UTF-8");
		evhttp_add_header(req->output_headers, "Connection", "Keep-Alive");
		while((r = read(fd,tempbuf,1024))>0)
		{
			tempbuf[r] = '\0';
			evbuffer_add_printf(buf,"%s",tempbuf);
		}
		sqltest();
		//evbuffer_add_printf(buf,"%s",out);
		//evbuffer_add_printf(buf, "It works!\n%s\n", output);
		//evhttp_send_reply(req, HTTP_OK, "OK", buf);
		//evhttp_response_code(req,HTTP_OK,"OK");

		//evhttp_send_page(req,buf);

		evhttp_send_reply_chunk(req,buf);

		evhttp_send_reply_end(req);

		evbuffer_free(buf);
	}
	close(fd);

}
