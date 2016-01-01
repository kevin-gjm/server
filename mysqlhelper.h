#ifndef __MYSQLHELPER_H__
#define __MYSQLHELPER_H__

#include <mysql/mysql.h>



int mysqlhelper_init(const char *host,const char *username,const char *passwd,const char *database);


void mysqlhelper_destroy();

/*
 * Param:
 *  sql: the select query
 *  show_line_begin: user handle before the line 
 *  show_line_end: user handle after the line 
 *  show_element: the user element show function 
 * Return values:
 *  error: -1
 *  success:the column of the select data
 */
int mysqlhelper_select_data(const char *sql,void(*show_line_begin)(),void(*show_line_end)(),void(*show_element)(char*),void(*show_head)(char*));


int operate_data(const char *sql);

#endif
