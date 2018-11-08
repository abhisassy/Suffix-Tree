Advance Algo Assignment 2

-Abhishek Saseendran 
-01FB16ECS018
-AA SECTION-B

Files needed:
	main.c
	implementation.c
	suffix_tree.h

Compilation Instruction:
	> gcc main.c implementation.c
	> a.exe

Input :
	First line of input : "filename.txt" 
	Second Line 		:  n <no of queries>
	Remaining n Lines 	:  query words 

Output :
	<index> , ... , <execution_time>

NOTE:
	execution time is in seconds, if more precision is needed change specifier to %g in main.c , where time is printed 
	by default it is not case sensitive 
	if case sensitivity is needed , change value of 'choice' to 'y' in main.c  

Algorithm Implemented to create suffix tree: Ukkonnens's Suffix tree algo

Side note: code looks a little messy with lot of comments,etc , this was originally meant for assignment 3 , changed it for assignment 2 needs.