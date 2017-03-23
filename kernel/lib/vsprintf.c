int strlen(const char* str)
{
	int i=0;
	while(str[i]!=0) i++;
	return i;
}

char num2ch(unsigned int num)
{
	if(num>=0 && num<=9) return '0'+num;
	else return 'a'+num-10;
}

int vsprintf(char* buf,int size,const char* format,void* list)
{
	int index=0;
	int len = strlen(format);
	int i=0;
	while(i<len)
	{
		if(format[i]!='%')
		{
			buf[index]=format[i];
			index++;
			i++;
		}
		else
		{
			switch(format[i+1])
			{
				case 'd':
					{
						int res = *((int*)list);
						list=list+4;
						i=i+2;
						int flag = (res<0);
						int  temp=res;
						int len=0;
						while(temp!=0)
						{
							len++;
							temp=temp/10;
						}
						temp=0;
						if(res==0)
						{
							buf[index]='0';
							index++;
						}
						else if(flag)
						{
							for(temp=len;temp>=1;--temp)
							{
								buf[index+temp]=-(res%10)+'0';
								res=res/10;
							}
							buf[index]='-';
							index=index+len+1;
						}
						else
						{
							for(temp=len-1;temp>=0;--temp)
							{
								buf[index+temp]=res%10+'0';
								res=res/10;
							}
							index=index+len;
						}
						break;
					}
				case 'x':
				        {
				        	unsigned int res = *((int*)list);
							list=list+4;
							i=i+2;
							unsigned int a[8];
							int k;
							for(k=7;k>=0;--k)
							{
								a[k]=(unsigned )(res&0xf);
								res=res>>4;
							}
							k=0;
							while(a[k++]==0 && k<8);
							for(k--;k<8;++k)
							{
								buf[index++]=num2ch(a[k]);
							}
							break;
				        }
				case 'c':
					{
						char ch = *((char*)list);
						list=list+4;
						i=i+2;
						buf[index]=ch;
						++index;
						break;
					}
				case 's':
					{
						char* str=*((char**)list);
						list=list+4;
						i=i+2;
						int len = strlen(str);
						int j=0;
						for(j=0;j<len;++j)
						{
							buf[index]=str[j];
							++index;
						}
						break;
					}
				default:break;
			}
		}
		if(index>=size) return index;
	}
	return index;
}
