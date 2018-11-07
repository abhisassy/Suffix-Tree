#include "suffix_tree.h"

double time_elapsed(struct timespec *start, struct timespec *end) {
	double t;
	t = (end->tv_sec - start->tv_sec); // diff in seconds
	t += (end->tv_nsec - start->tv_nsec) * 0.000000001; //diff in nanoseconds
	return t;
}



int main(int argc, char* argv[]){
	
	time_t t; 
	srand((unsigned)time(&t));
	struct timespec start;
	struct timespec end;
	
	
	SUFFIX_TREE* tree;
	DBL_WORD i,len = 0;
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

	//printf("Case Sensitive? (y/n)\n");
	choice = 'n';
	//fflush(stdin);
	//scanf("%c",&choice);

	//printf("Enter no of query strings: \n");
	int n;
	scanf("%d",&n);
	char query[n][100];

	int newline =0,doc_no=1,index=0;
	char letter;

	for (int x = 0; x < n; ++x)
	{
	scanf("%s",query[x]);
		
	clock_gettime(CLOCK_REALTIME, &start);
	fseek(file, 0, SEEK_SET);
	doc_no=1;
	index=0;
	strcpy(str,"");
	//printf("out\n");
	while(!feof(file)){
		//printf("Entered\n");
		letter = fgetc(file);
		//printf("%d\n",strlen(str) );
		//printf("%c %s\n",letter,str );
		if(letter!='@'){
		//printf("concat\n");

			str[index++] = letter;
			str[index] ='\0';
		}
		else{
			//printf("%s\n",str );
			//printf("make tree\n");

			if(choice=='n'){
				
				strcpy(str,strlwr(str));
				//printf("Constructing tree.....");
				tree = ST_CreateTree(str,strlen(str));
				//printf("Done.\n");
			
				//printf("%s\n",query[x] );
				ST_FindSubstring(tree, (unsigned char*)strlwr(query[x]), strlen(query[x]),doc_no);
									
			}
			else{
			
				//printf("Constructing tree.....");
				tree = ST_CreateTree(str,strlen(str));
				//printf("Done.\n");
				
				
				//printf("%s\n",query[x] );
				ST_FindSubstring(tree, (unsigned char*)(query[x]), strlen(query[x]),doc_no);
						
			}
			doc_no++;
			index=0;
			strcpy(str,"");
			ST_DeleteTree(tree);
		}
		
	}
	clock_gettime(CLOCK_REALTIME, &end);
	double t2=time_elapsed(&start, &end);
	printf("%g",t2 );
	printf("\n");
	
	}
	fclose(file);
	free(str);
	
	return 0;
}