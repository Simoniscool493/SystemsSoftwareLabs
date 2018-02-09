#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
// List of functions
int file_select();
extern int alphasort();

main() 
{
    int count,i;
    struct direct **files;
    
    char pathname[MAXPATHLEN];
    int t = 1;
    getcwd(pathname,MAXPATHLEN);

    count = scandir(pathname, &files, file_select, alphasort);
    
    /* No files in Dir */
    if (count <= 0)
    {
        printf("No files in Dir\n");
        exit(0);
    }
    
    for (i=1;i<count+1;++i)
    {
	if(strcmp(".",files[i-1]->d_name)!=0 && strcmp("..",files[i-1]->d_name)!=0)
	{
        	printf("%s\n",files[i-1]->d_name);
	}

    }
}

int file_select(struct direct *entry)
{
return (1);
}
