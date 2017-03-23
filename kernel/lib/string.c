void strcpy(char* dst,char* src)
{
	while(*src!=0)
	{
		*(dst++)=*(src++);
	}
	*dst=0;
}

int strcmp(char* dst,char* src)
{
	while(*(dst++)==*(src++) && *src!=0 && *dst!=0);
	return *dst-*src;
}
