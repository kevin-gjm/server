/*======================================================
#
# Copyright(C),2015,Company
#
# Author: kevin gjm_kevin@163.com
#
# Last modified: 2016-01-02 18:39
#
# Filename: myhttpd.c
#
# Version:
#
# Description: 
#
=====================================================*/
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
#include "http_handle.h"
#include "mysqlhelper.h"
#include "data_handle.h"

#define MYHTTPD_SIGNATURE   "myhttpd v 0.0.1"
#define ROOT "www%s"
#define DE_FILE_TYPE "index.html"

//处理模块
void httpd_handler(struct evhttp_request *req, void *arg) 
{

	printf("get a call\n\n");
	char path[512];

	int cgi =0;
	//获取客户端请求的URI(使用evhttp_request_uri或直接req->uri)
	const char *uri;

	struct stat st;

	//get uri
	//	sprintf(tmp, "uri=%s\n", req->uri);
	//	what's the diference of req->uri and uri??????
	uri = evhttp_request_uri(req);
	//decoded uri
	char *decoded_uri;
	decoded_uri = evhttp_decode_uri(uri);

	//get the path. cgi =1 have param
	cgi = http_get_path(uri,path);
	/*
	if(req->type == EVHTTP_REQ_GET )
	{
		//解析URI的参数(即GET方法的参数)
	
		struct evkeyvalq params;
		evhttp_parse_query(decoded_uri, &params);
		sprintf(tmp, "q=%s\n", evhttp_find_header(&params, "q"));
		strcat(output, tmp);
		sprintf(tmp, "s=%s\n", evhttp_find_header(&params, "s"));
		strcat(output, tmp);
	}else if(req->type == EVHTTP_REQ_POST)
	{
		cgi =1;
		//获取POST方法的数据
		char *post_data = (char *) EVBUFFER_DATA(req->input_buffer);
		sprintf(tmp, "post_data=%s\n", post_data);
		strcat(output, tmp);
	}

*/
	//	若路径处的文件是一个可执行文件 设置cgi=1
	if(stat(path,&st) == -1)
	{
		not_found(req,path);	
	}
	else
	{
		//文件存在
		//1.目录拼接默认文件
		if((st.st_mode & S_IFMT) == S_IFDIR)
		{	
			strcat(path,"/");
			strcat(path,DE_FILE_TYPE);
			if(stat(path,&st) == -1)
			{
				not_found(req,path);	
			}
		}
		//有可执行权限
		if((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))

			cgi = 1;
		if(req->type == EVHTTP_REQ_POST)
			cgi = 1;

		//printf("cgi:%d\n",cgi);
		//cgi=1 的所有情况
		// 1. GET后面带有参数
		// 2.请求文件为可执行文件
		// 3.POST请求
		if(!cgi)
		{
			serve_file(req,path);
		}
		else
		{
			if(req->type == EVHTTP_REQ_GET )
			{	
				struct evkeyvalq params;
				evhttp_parse_query(decoded_uri, &params);
				execute_cgi_get(&params);
			}
			else
			{
				execute_cgi_post(req);
	  		}
		}

	}


	free(decoded_uri);
	/*
	   具体的：可以根据GET/POST的参数执行相应操作，然后将结果输出
	   ...
	   通过判断req的type字段确定相应的类型,
	   */

	/* 输出到客户端 */

	/*
	//HTTP header
	evhttp_add_header(req->output_headers, "Server", MYHTTPD_SIGNATURE);
	evhttp_add_header(req->output_headers, "Content-Type", "text/plain; charset=UTF-8");
	evhttp_add_header(req->output_headers, "Connection", "close");
	//输出的内容
	struct evbuffer *buf;
	buf = evbuffer_new();
	evbuffer_add_printf(buf, "It works!\n%s\n", output);
	evbuffer_add_printf(buf, "It works!\n%s\n", output);
	evhttp_send_reply(req, HTTP_OK, "OK", buf);
	evbuffer_free(buf);
	*/
}


void show_help() {
	char *help = "written by Min (http://54min.com)\n\n"
		"-l <ip_addr> interface to listen on, default is 0.0.0.0\n"
		"-p <num>     port number to listen on, default is 1984\n"
		"-d           run as a deamon\n"
		"-t <second>  timeout for a http request, default is 120 seconds\n"
		"-h           print this help and exit\n"
		"\n";
	fprintf(stderr, help);
}
//当向进程发出SIGTERM/SIGHUP/SIGINT/SIGQUIT的时候，终止event的事件侦听循环
void signal_handler(int sig) {
	switch (sig) {
		case SIGTERM:
		case SIGHUP:
		case SIGQUIT:
		case SIGINT:
			event_loopbreak();  //终止侦听event_dispatch()的事件侦听循环，执行之后的代码
			break;
	}
}

int main(int argc, char *argv[]) {
	//自定义信号处理函数
	signal(SIGHUP, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);

	//默认参数
	char *httpd_option_listen = "0.0.0.0";
	int httpd_option_port = 8080;
	int httpd_option_daemon = 0;
	int httpd_option_timeout = 120; //in seconds

	//获取参数
	int c;
	while ((c = getopt(argc, argv, "l:p:dt:h")) != -1) {
		switch (c) {
			case 'l' :
				httpd_option_listen = optarg;
				break;
			case 'p' :
				httpd_option_port = atoi(optarg);
				break;
			case 'd' :
				httpd_option_daemon = 1;
				break;
			case 't' :
				httpd_option_timeout = atoi(optarg);
				break;
			case 'h' :
			default :
				show_help();
				exit(EXIT_SUCCESS);
		}
	}

	//判断是否设置了-d，以daemon运行
	if (httpd_option_daemon) {
		pid_t pid;
		pid = fork();
		if (pid < 0) {
			perror("fork failed");
			exit(EXIT_FAILURE);
		}
		if (pid > 0) {
			//生成子进程成功，退出父进程
			exit(EXIT_SUCCESS);
		}
	}

	/* 使用libevent创建HTTP Server */

	//初始化event API
	event_init();

	//创建一个http server
	struct evhttp *httpd;
	httpd = evhttp_start(httpd_option_listen, httpd_option_port);
	evhttp_set_timeout(httpd, httpd_option_timeout);

	//指定generic callback
	evhttp_set_gencb(httpd, httpd_handler, NULL);
	//也可以为特定的URI指定callback
	//evhttp_set_cb(httpd, "/", specific_handler, NULL);

	//循环处理events
	event_dispatch();

	evhttp_free(httpd);
	return 0;
}


