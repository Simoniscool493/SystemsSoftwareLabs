#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
// List of functions
int file_select();
char* getFileModifiedTime();
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
		char* modifiedTime = getFileModifiedTime(files[i-1]->d_name);

        	printf("%s\n\t%s\n",files[i-1]->d_name,modifiedTime);
	}

    }
}

int file_select(struct direct *entry)
{
return (1);
}

char* getFileModifiedTime(char *path) {
    struct stat attr;
    stat(path, &attr);
    //printf("Last modified time: %s", ctime(&attr.st_mtime));
    return ctime(&attr.st_mtime);
}
