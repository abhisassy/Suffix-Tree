#include "suffix_tree.h"

double time_elapsed(struct timespec *start, struct timespec *end) {
	double t;
	t = (end->tv_sec - start->tv_sec); // diff in seconds
	t += (end->tv_nsec - start->tv_nsec) * 0.000000001; //diff in nanoseconds
	return t;
}

int cmpfunc (const void * a, const void * b) {
   return ( *(DBL_WORD*)a - *(DBL_WORD*)b );
}

int main(int argc, char* argv[]){
	
	time_t t; 
	srand((unsigned)time(&t));
	struct timespec start;
	struct timespec end;
	double time;
	
	SUFFIX_TREE* tree;
	DBL_WORD len = 0,count=0;
	unsigned char *str = NULL;

	FILE* file = 0;
	char* filename = (char*)malloc(sizeof(char)*30);
	scanf("%s",filename);
	
	file = fopen(filename,"r");
	free(filename);
	if(file == 0){
			printf("can't open file.\n");
			return 0;
		}


	fseek(file, 0, SEEK_END);
	len = ftell(file);
	fseek(file, 0, SEEK_SET);
	str = (unsigned char*)malloc(len*sizeof(unsigned char));
	if(str == 0){
		printf("\nOut of memory.\n");
		exit(0);
	}

	fread(str,sizeof(unsigned char),len,file);

	//printf("Case Sensitive? (y/n)\n");
	choice = 'n';
	//fflush(stdin);
	//scanf("%c",&choice);

	//printf("Enter no of query strings: \n");
	int n;
	scanf("%d",&n);
	char query[n][100];

	int index[len];

	for (int x = 0; x < n; ++x)
	{
		scanf("%s",query[x]);
	}

	if(choice=='n'){
				
		strcpy(str,strlwr(str));
		//printf("Constructing tree.....");
		tree = ST_CreateTree(str,strlen(str));
		//printf("Done.\n");
		
		for (int i = 0; i < n; ++i)
		{
			clock_gettime(CLOCK_REALTIME, &start);

			count = ST_FindSubstring(tree, (unsigned char*)strlwr(query[i]), strlen(query[i]),index);
			int c =count;
			if(c>0)
				qsort(index,c,sizeof(int),cmpfunc);
			
			for (int i = 0; i < c; ++i)
			{
			if(index[i] != -1)
				printf("%d, ",index[i]);
			}

			clock_gettime(CLOCK_REALTIME, &end);
			time = time_elapsed(&start, &end);
			printf("%g",time );
			printf("\n");

			//memset(index,0,c);
			c=0;
		}

	}
	else{

		//printf("Constructing tree.....");
		tree = ST_CreateTree(str,strlen(str));
		//printf("Done.\n");
		for (int i = 0; i < n; ++i)
		{
			clock_gettime(CLOCK_REALTIME, &start);
				
			count = ST_FindSubstring(tree, (unsigned char*)(query[i]), strlen(query[i]),index);
			int c= count;
			if(c>0)
				qsort(index,c,sizeof(int),cmpfunc);

			for (int i = 0; i < c; ++i)
			{
			if(index[i] != -1)
				printf("%d, ",index[i]);
			}
			clock_gettime(CLOCK_REALTIME, &end);
			time = time_elapsed(&start, &end);
			printf("%g",time );
			printf("\n");

			//memset(index,0,c);
			c=0;	
		}
	}	
	
	ST_DeleteTree(tree);	
	fclose(file);
	free(str);
	
	return 0;
}