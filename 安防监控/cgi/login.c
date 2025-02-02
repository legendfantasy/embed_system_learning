#include <stdio.h>   
#include <stdlib.h>   
#include <string.h>   

char name[64];   
char pass[64];  
  
char* getcgidata(FILE* fp, char* requestmethod)   
{   
	char* input;   
	int len;   
	int size = 1024;   
	int i = 0;   
	  
	if (!strcmp(requestmethod, "GET"))   
	{   
		input = getenv("QUERY_STRING");   
		return input;   
	}   
	else if (!strcmp(requestmethod, "POST"))   
	{   
		len = atoi(getenv("CONTENT_LENGTH"));   
		input = (char*)malloc(sizeof(char)*(size + 1));   
		
		if (len == 0)   
		{   
		input[0] = '\0';   
		return input;   
		}   
		
		while(1)   
		{   
		input[i] = (char)fgetc(fp);   
		if (i == size)   {   
				input[i+1] = '\0';   
				return input;   
		}   										  
		--len;   
		if (feof(fp) || (!(len)))   
		{   
				i++;   
				input[i] = '\0';   
				return input;   
		}   
		i++;   
						
		}   
	}   
	return NULL;  
}	

void unencode_for_name_pass(char *input)
{
	int i = 0;   
	int j = 0;   
	
	// 我们获取的input字符串可能像如下的形式   
	// Username="admin"&Password="aaaaa"   
	// 其中"Username="和"&Password="都是固定的   
	// 而"admin"和"aaaaa"都是变化的，也是我们要获取的   
		
	// 前面9个字符是UserName=   
	// 在"UserName="和"&"之间的是我们要取出来的用户名   	
	for ( i = 9; i < (int)strlen(input); i++ ) 
	{   
		if (input[i] == '&')   
		{   
			name[j] = '\0';   
			break;   
		}                                       
		name[j++] = input[i];   
	}   
   
	// 前面9个字符 + "&Password="10个字符 + Username的字符数   
	// 是我们不要的，故省略掉，不拷贝   
	for ( i = 19 + strlen(name), j = 0; i < (int)strlen(input); i++ )
	{   
		pass[j++] = input[i];   
	}   
	pass[j] = '\0';   	
	
	printf("Content-type: text/html\n\n");   //告诉编译器，用html语法来解析
	if((strcmp(name,"R") == 0)&&(strcmp(pass,"1") == 0))
	{
		printf("<script language='javascript'>document.location = 'http://192.168.0.200/choose.html'</script>"); //自动跳转到这个页面	
	}
	else{
		printf("wrong username or password!<br><br>");  
		//exit(-1);
	}
}

int main()   
{   
	char *input;   
	char *req_method;   
 
	printf("Content-type: text/html\n\n");   //告诉编译器，用html语法来解析
	printf("The following is query reuslt:<br><br>");   //下面是请求结果

	req_method = getenv("REQUEST_METHOD");   
	input = getcgidata(stdin, req_method);   //获取URL 编码的数据
	unencode_for_name_pass(input);   //解码，并判断用户名，密码,如果正确，跳转至选择仓库界面，否则提示错误	

	return 0;   
}   
   
			
