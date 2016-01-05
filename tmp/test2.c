#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

int main()
{
	cJSON * root =  cJSON_CreateArray();
	cJSON * array;
	//cJSON * root =  cJSON_CreateObject();
	if(NULL == root)
	{
		printf("error\n");
		return -1;
	}
	int i=0,j=0;
	char * he[10]={"a","b","c","d","e","f","g","h","l","m"};
	for(j=0;j<5;j++)
	{
		cJSON_AddItemToArray(root,array=cJSON_CreateArray());
		for(i=0;i<10;i++)
		{
			cJSON_AddStringToObject(array,"",he[i]);
		}
	}
	char * out = cJSON_Print(root);
	printf("\n\njson:\n\n%s\n\n",out);
	free(out);
	cJSON_Delete(root);

	return 0;
}
