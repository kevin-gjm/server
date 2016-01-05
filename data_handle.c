#include <stdio.h>
#include <event.h>
//for http
#include <evhttp.h>
#include <signal.h>
#include "data_handle.h"
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
