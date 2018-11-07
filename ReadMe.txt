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
	<document_number>:<index within doc> , ... , <execution_time>
	<''''''''''-------- repeated for n queries -------- ''''''''>

Algorithm Implemented to create suffix tree: Ukkonnens's Suffix tree algo



Psedo Code : 
	
for every query word:	
	doc_no -> 1
	while(not end of file):
		take character by character input from file
		if character != @ :
			append to str
		else:
			create_suffix_tree(str) 
			check_for_query_word(query)
			doc_no++
			empty(str) 			



