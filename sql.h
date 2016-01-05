#ifndef __SQL_H__
#define __SQL_H__


char * select_json_by_sql(const char *sql,int *count);
int operate_by_sql(const char *sql);
void sqltest();

#endif
