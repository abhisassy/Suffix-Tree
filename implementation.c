#include "suffix_tree.h"


NODE* create_node(NODE* father, DBL_WORD start, DBL_WORD end, DBL_WORD position){
     
    NODE* node   = (NODE*)malloc(sizeof(NODE));
    if(node == 0)
    {
      printf("\nOut of memory.\n");
      exit(0);
    }

    node->sons             = 0;
    node->right_sibling    = 0;
    node->left_sibling     = 0;
    node->suffix_link      = 0;
    node->father           = father;
    node->path_position    = position;
    node->edge_label_start = start;
    node->edge_label_end   = end;
    return node;
}


NODE* find_son(SUFFIX_TREE* tree, NODE* node, char character){
    node = node->sons;
    while(node != 0 && tree->tree_string[node->edge_label_start] != character){
		counter++;
		node = node->right_sibling;
   }
   return node;
}

DBL_WORD get_node_label_end(SUFFIX_TREE* tree, NODE* node){
   
   if(node->sons == 0)
      return tree->e;
   
   return node->edge_label_end;
}


DBL_WORD get_node_label_length(SUFFIX_TREE* tree, NODE* node){
 
   return get_node_label_end(tree, node) - node->edge_label_start + 1;
}

char is_last_char_in_edge(SUFFIX_TREE* tree, NODE* node, DBL_WORD edge_pos){
   
   if(edge_pos == get_node_label_length(tree,node)-1)
      return 1;
   return 0;
}

void connect_siblings(NODE* left_sib, NODE* right_sib){
   
   if(left_sib != 0)
      left_sib->right_sibling = right_sib;
   
   if(right_sib != 0)
      right_sib->left_sibling = left_sib;
}

/******************************************************************************/
/*
   apply_extension_rule_2 :
   Apply "extension rule 2" in 2 cases:
   1. A new son (leaf 4) is added to a node that already has sons:
                (1)	       (1)
               /   \	 ->   / | \
              (2)  (3)      (2)(3)(4)
   2. An edge is split and a new leaf (2) and an internal node (3) are added:
              | 	  |
              | 	 (3)
              |     ->   / \
             (1)       (1) (2)
   Input : See parameters.
   Output: A pointer to the newly created leaf (new_son case) or internal node
   (split case).
*/

NODE* apply_extension_rule_2(
                      /* Node 1 (see drawings) */
                      NODE*           node,            
                      /* Start index of node 2's incoming edge */
                      DBL_WORD        edge_label_begin,   
                      /* End index of node 2's incoming edge */
                      DBL_WORD        edge_label_end,      
                      /* Path start index of node 2 */
                      DBL_WORD        path_pos,         
                      /* Position in node 1's incoming edge where split is to be
		         performed */
                      DBL_WORD        edge_pos,         
                      /* Can be 'new_son' or 'split' */
                      RULE_2_TYPE     type)            
{
   NODE *new_leaf,
        *new_internal,
        *son;
   /*-------new_son-------*/
   if(type == new_son){
      /* Create a new leaf (4) with the characters of the extension */
      new_leaf = create_node(node, edge_label_begin , edge_label_end, path_pos);
      /* Connect new_leaf (4) as the new son of node (1) */
      son = node->sons;
      while(son->right_sibling != 0)
         son = son->right_sibling;
      connect_siblings(son, new_leaf);
      /* return (4) */
      return new_leaf;
   }
   /*-------split-------*/
   /* Create a new internal node (3) at the split point */
   new_internal = create_node(
                      node->father,
                      node->edge_label_start,
                      node->edge_label_start+edge_pos,
                      node->path_position);
   /* Update the node (1) incoming edge starting index (it now starts where node
   (3) incoming edge ends) */
   node->edge_label_start += edge_pos+1;

   /* Create a new leaf (2) with the characters of the extension */
   new_leaf = create_node(
                      new_internal,
                      edge_label_begin,
                      edge_label_end,
                      path_pos);
   
   /* Connect new_internal (3) where node (1) was */
   /* Connect (3) with (1)'s left sibling */
   connect_siblings(node->left_sibling, new_internal);   
   /* connect (3) with (1)'s right sibling */
   connect_siblings(new_internal, node->right_sibling);
   node->left_sibling = 0;

   /* Connect (3) with (1)'s father */
   if(new_internal->father->sons == node)            
      new_internal->father->sons = new_internal;
   
   /* Connect new_leaf (2) and node (1) as sons of new_internal (3) */
   new_internal->sons = node;
   node->father = new_internal;
   connect_siblings(node, new_leaf);
   /* return (3) */
   return new_internal;
}

NODE* trace_single_edge(
                      SUFFIX_TREE*    tree, 
                      /* Node to start from */
                      NODE*           node,         
                      /* String to trace */
                      PATH            str,         
                      /* Last matching position in edge */
                      DBL_WORD*       edge_pos,      
                      /* Last matching position in tree source string */
                      DBL_WORD*       chars_found,   
                      /* Skip or no_skip*/
                      SKIP_TYPE       type,          
                      /* 1 if search is done, 0 if not */
                      int*            search_done)   
{
   NODE*      cont_node;
   DBL_WORD   length,str_len;

   /* Set default return values */
   *search_done = 1;
   *edge_pos    = 0;

   /* Search for the first character of the string in the outcoming edge of
      node */
   cont_node = find_son(tree, node, tree->tree_string[str.begin]);
   if(cont_node == 0)
   {
      /* Search is done, string not found */
      *edge_pos = get_node_label_length(tree,node)-1;
      *chars_found = 0;
      return node;
   }
   
   /* Found first character - prepare for continuing the search */
   node    = cont_node;
   length  = get_node_label_length(tree,node);
   str_len = str.end - str.begin + 1;

   /* Compare edge length and string length. */
   /* If edge is shorter then the string being searched and skipping is
      enabled - skip edge */
   if(type == skip)
   {
      if(length <= str_len)
      {
         (*chars_found)   = length;
         (*edge_pos)      = length-1;
         if(length < str_len)
            *search_done  = 0;
      }
      else
      {
         (*chars_found)   = str_len;
         (*edge_pos)      = str_len-1;
      }
    return node;
   }
   else
   {
      /* Find minimum out of edge length and string length, and scan it */
      if(str_len < length)
         length = str_len;

      for(*edge_pos=1, *chars_found=1; *edge_pos<length; (*chars_found)++,(*edge_pos)++)
      {

         /* Compare current characters of the string and the edge. If equal - 
	    continue */
         if(tree->tree_string[node->edge_label_start+*edge_pos] != tree->tree_string[str.begin+*edge_pos])
         {
            (*edge_pos)--;
            return node;
         }
      }
   }

   /* The loop has advanced *edge_pos one too much */
   (*edge_pos)--;

   if((*chars_found) < str_len)
      /* Search is not done yet */
      *search_done = 0;

   return node;
}


NODE* trace_string(
                      SUFFIX_TREE*    tree, 
                      /* Node to start from */
                      NODE*           node,         
                      /* String to trace */
                      PATH            str,         
                      /* Last matching position in edge */
                      DBL_WORD*       edge_pos,      
                      /* Last matching position in tree string */
                      DBL_WORD*       chars_found,
                      /* skip or not */
                      SKIP_TYPE       type)         
{
   /* This variable will be 1 when search is done.
      It is a return value from function trace_single_edge */
   int      search_done = 0;

   /* This variable will hold the number of matching characters found in the
      current edge. It is a return value from function trace_single_edge */
   DBL_WORD edge_chars_found;

   *chars_found = 0;

   while(search_done == 0)
   {
      *edge_pos        = 0;
      edge_chars_found = 0;
      node = trace_single_edge(tree, node, str, edge_pos, &edge_chars_found, type, &search_done);
      str.begin       += edge_chars_found;
      *chars_found    += edge_chars_found;
   }
   return node;
}

DBL_WORD findleaves(NODE* node, int x){
	DBL_WORD count =0;
	if(node== NULL)
		return 0;
	if(node->sons==0){
		printf("%d:%ld, ",x,node->path_position );
		return 1;
	}
	else{
		node=node->sons;
		while(node!=0){
			count += findleaves(node,x);
			node = node->right_sibling;
		}
		return count;
	}
}

void ST_FindSubstring(
                      /* The suffix array */
                      SUFFIX_TREE*    tree,      
                      /* The substring to find */
                      char*  W,         
                      /* The length of W */
                      DBL_WORD        P,
                      int x)         
{
	//printf("%ld\n",index );
   /* Starts with the root's son that has the first character of W as its
      incoming edge first character */
   NODE* node   = find_son(tree, tree->root, W[0]);
   DBL_WORD k,j = 0, node_label_end;

   /* Scan nodes down from the root untill a leaf is reached or the substring is
      found */
   while(node!=0 )
   {
      k=node->edge_label_start;
      node_label_end = get_node_label_end(tree,node);
      //printf("%ld %ld\n",j,P );
      /* Scan a single edge - compare each character with the searched string */
      while(j<P && k<=node_label_end && tree->tree_string[k] == W[j])
      {	
      	//printf("inside while \n");
      	//printf("j%ld k%ld\n",j,k );
      	//printf("%c %c %ld\n",tree->tree_string[k+index],W[j],index);
         j++;
         k++;
      }
      //printf("Outside while\n");
      //printf("%ld %ld %ld\n",j,k,node_label_end );
      /* Checking which of the stopping conditions are true */
      if(j == P)
      {
         //printf("yes %ld\n",node->path_position);
         //sleep(1);
         //printf("Finding Substrings  ...\n");
         //sleep(1);//return;
         DBL_WORD ans = findleaves(node,x);
         //printf("\n");
         //printf("Total Occurences of %s : %ld\n",W,ans );
         return;
         /* W was found - it is a substring. Return its path starting index */
      	 //ST_FindSubstring(tree,W,P,(node->path_position)+index);
         //return node->path_position;
      }
      else if(k >= node_label_end)/* Current edge is found to match, continue to next edge */
         node = find_son(tree, node, W[j]);
      else
      {
         /* One non-matching symbols is found - W is not a substring */
         return;
      }
   }
   return;
}

void follow_suffix_link(SUFFIX_TREE* tree, POS* pos){

   /* gama is the string between node and its father, in case node doesn't have
      a suffix link */
   PATH      gama;            
   /* dummy argument for trace_string function */
   DBL_WORD  chars_found = 0;   
   
   if(pos->node == tree->root)
   {
      return;
   }

   if(pos->node->suffix_link == 0 || is_last_char_in_edge(tree,pos->node,pos->edge_pos) == 0)
   {
           if(pos->node->father == tree->root)
      {
         pos->node = tree->root;
         return;
      }
      
      /* Store gama - the indices of node's incoming edge */
      gama.begin      = pos->node->edge_label_start;
      gama.end      = pos->node->edge_label_start + pos->edge_pos;
      /* Follow father's suffix link */
      pos->node      = pos->node->father->suffix_link;
      /* Down-walk gama back to suffix_link's son */
      pos->node      = trace_string(tree, pos->node, gama, &(pos->edge_pos), &chars_found, skip);
   }
   else
   {
      /* If a suffix link exists - just follow it */
      pos->node      = pos->node->suffix_link;
      pos->edge_pos   = get_node_label_length(tree,pos->node)-1;
   }
}

void create_suffix_link(NODE* node, NODE* link)
{
   node->suffix_link = link;
}


void SEA(SUFFIX_TREE*   tree, POS* pos,PATH str,DBL_WORD* rule_applied,char after_rule_3)
{
   DBL_WORD   chars_found = 0 , path_pos = str.begin;
   NODE*      tmp;


   /* Follow suffix link only if it's not the first extension after rule 3 was applied */
   if(after_rule_3 == 0)
      follow_suffix_link(tree, pos);
   /* If node is root - trace whole string starting from the root, else - trace last character only */
   if(pos->node == tree->root)
   {
      pos->node = trace_string(tree, tree->root, str, &(pos->edge_pos), &chars_found, no_skip);
   }
   else
   {
      str.begin = str.end;
      chars_found = 0;

      /* Consider 2 cases:
         1. last character matched is the last of its edge */
      if(is_last_char_in_edge(tree,pos->node,pos->edge_pos))
      {
         /* Trace only last symbol of str, search in the  NEXT edge (node) */
         tmp = find_son(tree, pos->node, tree->tree_string[str.end]);
         if(tmp != 0)
         {
            pos->node      = tmp;
            pos->edge_pos   = 0;
            chars_found      = 1;
         }
      }
      /* 2. last character matched is NOT the last of its edge */
      else
      {
         /* Trace only last symbol of str, search in the CURRENT edge (node) */
         if(tree->tree_string[pos->node->edge_label_start+pos->edge_pos+1] == tree->tree_string[str.end])
         {
            pos->edge_pos++;
            chars_found   = 1;
         }
      }
   }

   /* If whole string was found - rule 3 applies */
   if(chars_found == str.end - str.begin + 1)
   {
      *rule_applied = 3;
      /* If there is an internal node that has no suffix link yet (only one may 
         exist) - create a suffix link from it to the father-node of the 
         current position in the tree (pos) */
      if(suffixless != 0)
      {
         create_suffix_link(suffixless, pos->node->father);
         /* Marks that no internal node with no suffix link exists */
         suffixless = 0;
      }

   
      return;
   }
   
   /* If last char found is the last char of an edge - add a character at the 
      next edge */
   if(is_last_char_in_edge(tree,pos->node,pos->edge_pos) || pos->node == tree->root)
   {
      /* Decide whether to apply rule 2 (new_son) or rule 1 */
      if(pos->node->sons != 0)
      {
         /* Apply extension rule 2 new son - a new leaf is created and returned 
            by apply_extension_rule_2 */
         apply_extension_rule_2(pos->node, str.begin+chars_found, str.end, path_pos, 0, new_son);
         *rule_applied = 2;
         /* If there is an internal node that has no suffix link yet (only one 
            may exist) - create a suffix link from it to the father-node of the 
            current position in the tree (pos) */
         if(suffixless != 0)
         {
            create_suffix_link(suffixless, pos->node);
            /* Marks that no internal node with no suffix link exists */
            suffixless = 0;
         }
      }
   }
   else
   {
      /* Apply extension rule 2 split - a new node is created and returned by 
         apply_extension_rule_2 */
      tmp = apply_extension_rule_2(pos->node, str.begin+chars_found, str.end, path_pos, pos->edge_pos, split);
      if(suffixless != 0)
         create_suffix_link(suffixless, tmp);
      /* Link root's sons with a single character to the root */
      if(get_node_label_length(tree,tmp) == 1 && tmp->father == tree->root)
      {
         tmp->suffix_link = tree->root;
         /* Marks that no internal node with no suffix link exists */
         suffixless = 0;
      }
      else
         /* Mark tmp as waiting for a link */
         suffixless = tmp;
      
      /* Prepare pos for the next extension */
      pos->node = tmp;
      *rule_applied = 2;
   }
}

void SPA(SUFFIX_TREE* tree,POS* pos,DBL_WORD phase,DBL_WORD* extension,char* repeated_extension){

   /* No such rule (0). Used for entering the loop */
   DBL_WORD   rule_applied = 0;   
   PATH       str;
   
   /* Leafs Trick: Apply implicit extensions 1 through prev_phase */
   tree->e = phase+1;

   /* Apply explicit extensions untill last extension of this phase is reached 
      or extension rule 3 is applied once */
   while(*extension <= phase+1)            
   {
      str.begin       = *extension;
      str.end         = phase+1;
      
      /* Call Single-Extension-Algorithm */
      SEA(tree, pos, str, &rule_applied, *repeated_extension);
      
      /* Check if rule 3 was applied for the current extension */
      if(rule_applied == 3)
      {
         /* Signaling that the next phase's first extension will not follow a 
            suffix link because same extension is repeated */
         *repeated_extension = 1;
         break;
      }
      *repeated_extension = 0;
      (*extension)++;
   }
   return;
}


SUFFIX_TREE* ST_CreateTree(const char* str, DBL_WORD length)
{
   SUFFIX_TREE*  tree;
   DBL_WORD      phase , extension;
   char          repeated_extension = 0;
   POS           pos;

   if(str == 0)
      return 0;

   /* Allocating the tree */
   tree = malloc(sizeof(SUFFIX_TREE));
   if(tree == 0)
   {
      printf("\nOut of memory.\n");
      exit(0);
   }

   /* Calculating string length (with an ending $ sign) */
   tree->length = length+1;
   //ST_ERROR     = length+10;
   
   /* Allocating the only real string of the tree */
   tree->tree_string = malloc((tree->length+1)*sizeof(char));
   if(tree->tree_string == 0)
   {
      printf("\nOut of memory.\n");
      exit(0);
   }
   //heap+=(tree->length+1)*sizeof(char);

   memcpy(tree->tree_string+sizeof(char),str,length*sizeof(char));
   /* $ is considered a uniqe symbol */
   tree->tree_string[tree->length] = '$';
   
   /* Allocating the tree root node */
   tree->root            = create_node(0, 0, 0, 0);
   tree->root->suffix_link = 0;

   /* Initializing algorithm parameters */
   extension = 2;
   phase = 2;
   
   /* Allocating first node, son of the root (phase 0), the longest path node */
   tree->root->sons = create_node(tree->root, 1, tree->length, 1);
   suffixless       = 0;
   pos.node         = tree->root;
   pos.edge_pos     = 0;

   /* Ukkonen's algorithm begins here */
   for(; phase < tree->length; phase++)
   {
      /* Perform Single Phase Algorithm */
      SPA(tree, &pos, phase, &extension, &repeated_extension);
   }
   return tree;
}


void ST_DeleteSubTree(NODE* node)
{
   /* Recoursion stoping condition */
   if(node == 0)
      return;
   /* Recoursive call for right sibling */
   if(node->right_sibling!=0)
      ST_DeleteSubTree(node->right_sibling);
   /* Recoursive call for first son */
   if(node->sons!=0)
      ST_DeleteSubTree(node->sons);
   /* Delete node itself, after its whole tree was deleted as well */
   free(node);
}

void ST_DeleteTree(SUFFIX_TREE* tree)
{
   if(tree == 0)
      return;
   ST_DeleteSubTree(tree->root);
   free(tree);
}

