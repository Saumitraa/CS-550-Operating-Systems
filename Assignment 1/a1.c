#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

int main(int argc,char * argv[])
{
	char *h,*c,newh[2];
	long int height=0,child=0,parent=0;
	pid_t pid[child];
	pid_t ppid[parent];

	if(argc==3)
	{
		
		height=strtol(argv[1],&h,10);
		child=strtol(argv[2],&c,10);
		printf("\n[%d] : Process starting \n",getpid());
		printf("[%d] : Parent's ID = %d \n",getpid(),getppid());
		printf("[%d] : Height in the tree = %li \n",getpid(),height);
		if(height>1)
		{	
			printf("[%d] : Creating %li children at height %li \n\n",getpid(),child,height-1);
			long int temp;
			int stat;
				for(temp=0;temp<child;temp++)
				{
					pid[temp]=fork();
					if(pid[temp]==0)
					{
						
						sprintf(newh,"%li",--height);
						if(execlp(argv[0],argv[0],newh,argv[2],(char *) NULL) == -1)
						{
							printf("Error \n");
						}
						exit(0);
					}
					if(pid[temp]==-1)
					{
						printf("Error\n");
					}
				}
			
				while(child>0 && height>0)
				{
					pid[parent]=wait(&stat);
					child=child-1;
				}
	
				 printf("\n[%d] : Terminating at the height %li \n",getpid(),height);
				
			}
	
			else
			{
				printf("\n[%d] : Terminating at the height of %li \n",getpid(),height);
				
			}
	
		}
	
		else
		{
			printf("Incorrect Arguments \n");
			exit (0);
		}
	
		return 0;
}		
