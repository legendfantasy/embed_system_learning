#include <stdio.h>

int main(int argc,char *argv[])
{
    FILE *fp;
    int rec;
    fp = fopen("1.txt","r");
    if(fp==NULL)
    {
        perror("fopen");
        return 0;

    }
    
    rec = fgetc(fp);
    printf("Get char=%c\n",rec);
	return 0;
}
