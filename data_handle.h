#ifdef __DATA_HANDLE__
#define __DATA_HANDLE__

int execute_cgi_get(struct evkeyvalq *params);

int execute_cgi_post(struct evhttp_request *req);
#endif
