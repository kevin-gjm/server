#ifdef  __HTTP_HANDLE__
#define __HTTP_HANDLE__


int http_get_path(const char* uri,char *path) ;
void not_found(struct evhttp_request *req,const char *filename);
void serve_file(struct evhttp_request *req,const char * filename);
#endif
