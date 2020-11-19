
	/* B+ Tree Implementation (Done!)*/
	
	/*SPECIFICATION: 
     *There are Different Vartiations of a B+ Tree structure 
	 *In this program ,the program makes the following assumptions regarding the B+ Tree structure :
	 *1.ALl Entries (Records) are stored only at the Leaf level and all leaves are doubly linked
	 *2.Inner Nodes hold Keys in general (which may or maynot be present  in the entries) which  serve as Decision Markers
	 *  helping in searching the required entry
	 *3.Keys corresponding to all entries are present in the inner nodes though entries corresponding to all keys maynot be present in the leaves.
	 */	


#include <stdio.h>
#include <stdlib.h>

#define MAX 4
#define MIN 2
#define DATAMAX 4
#define DATAMIN 2

typedef int Key_type ;
typedef int Data_type ;
typedef enum {FALSE,TRUE} boolean;
typedef struct BplusNode BplusNode;

typedef struct Treeentry {
	Key_type key;
	Data_type val;
	
}Treeentry;

typedef struct Leafnode{

	int count;
	Treeentry entry[DATAMAX+1];
	struct Leafnode *prev;
	struct Leafnode *next;

}Leafnode;

typedef struct Treenode{

	int count;
	Key_type keys[MAX+1];
	boolean isBleaf;
	BplusNode *branch[MAX+2];

}Treenode;

typedef union Node{
	Treenode *innernode;
	Leafnode *leafnode;
}Node; 		//Wrapper around a basic structure of a node which is variable ( Inner Node / Leaf Node ) in a B+ Tree. 


typedef struct BplusNode{
	
	boolean isleaf;
	Node n;
	
}BplusNode; //Basic structure of a B+ Tree Node. 

Treenode* maketreenode(){
	
	/*Make a new Treenode(innernode) */
	
	Treenode *nptr;
	nptr = (Treenode*)malloc(sizeof(Treenode));
	nptr->count = 0;
	int i ;
	for(i = 0;i<=MAX;i++){
		nptr->keys[i] = 0;
		nptr->branch[i] = NULL;//initializing all children to NULL
	}
	nptr->branch[i] = NULL;
	return nptr;
}

Leafnode* makeleafnode(){
	
	/*Make a new Leafnode */
	
	Leafnode *nptr;
	nptr = (Leafnode*)malloc(sizeof(Leafnode));
	nptr->count = 0;
	int i;
	for(i = 0;i<=DATAMAX;i++){
		nptr->entry[i].key = nptr->entry[i].val = 0;
	}
	nptr->prev = nptr->next = NULL;
	return nptr;
}

boolean search(BplusNode *root,Key_type key,int *pos){
	
	/*If a BplusNode is Leaf node 	  :   return TRUE  -> key found in the leaf node ,*pos holds the position of the key.
	 *                               	  return FALSE -> key not found in the database ! ;*pos holds garbage!
	 *If a BplusNode is an inner node :   return TRUE  -> key found in the inner node,*pos holds the index from where next search is to be continued       
	 *                                    return FALSE -> key not found in the current node ,*pos holds the index from where next search is to be continued 
	 */									 					
	
	
	boolean retval = FALSE;
	if(root == NULL){
		printf("Empty Tree\n");
		retval = FALSE;
	}
	else{
		if(root->isleaf == FALSE){
			boolean stop = FALSE;
			int i = 0;
			while(i<root->n.innernode->count && !stop){
				
				if(root->n.innernode->keys[i] >= key){
					*pos = i;
					stop = TRUE;
				}
				else{
					i++;
					
				}
			}
			if(stop == TRUE){
				if(root->n.innernode->keys[i] == key){
					*pos += 1;
					retval = TRUE;
				}
			}
			else{
				*pos = i;
			}
		}
		else{
			boolean stop = FALSE;
			int i = 0;
			while(i<root->n.leafnode->count && !stop){
				if(root->n.leafnode->entry[i].key >= key){
					*pos = i;
					stop = TRUE;
				}
				else{
					i++;
					
				}				
			}
			if(stop == TRUE){
				if(root->n.leafnode->entry[i].key == key){
					//*pos += 1;
					retval = TRUE;
				}
			}
			else{
				*pos = i;
			}			
		}
	}
	return retval;
	
}

boolean split_detector(BplusNode *root){
	
	/*Detects whether a B+ Node whether an innernode or a leaf node needs a split or not */
	
	boolean retval = FALSE;
	if(root != NULL){
		if(root->isleaf == TRUE){
			if(root->n.leafnode->count > DATAMAX){
				retval = TRUE;
			}
		}
		else{
			if(root->n.innernode->count > MAX){
				retval = TRUE;
			}			
		}
	}
	return retval;
}

void Insertinleaf(BplusNode *root,Treeentry entry,int pos){
	
	/*Inserts Entry entry in the leafnode corresponding to B+ Node root at position pos*/
	
	int i;
	i = root->n.leafnode->count - 1 ;
	while(i>=pos){
		root->n.leafnode->entry[i+1] = root->n.leafnode->entry[i];
		i--;
	}
	root->n.leafnode->entry[i+1] = entry;
	root->n.leafnode->count += 1;
	
}

Key_type split(BplusNode *root,BplusNode **newright){
	
	/*Split a B+ Node whether inner or leaf into two creating a new B+ Node and return the key around which split happened*/
	
	if(root){
		Key_type retval;
		int i;
		int median;
		if(root->isleaf == FALSE){
			
			/*If the B+ Node is a Leaf Node*/
			
			median =   (root->n.innernode->count - 1)/2;
			retval = root->n.innernode->keys[median]; //mid value to be returned
			
			BplusNode *nptr = (BplusNode*)malloc(sizeof(BplusNode)); //Creation of a new B+ Node
			nptr->n.innernode = maketreenode();
			nptr->isleaf = FALSE;
			
			for(i = median+1;i<root->n.innernode->count;i++){
				nptr->n.innernode->count += 1;
				nptr->n.innernode->keys[i-median-1] = root->n.innernode->keys[i];
				nptr->n.innernode->branch[i-median-1] = root->n.innernode->branch[i];
			}
			
			nptr->n.innernode->branch[i-median-1] = root->n.innernode->branch[i];
			root->n.innernode->count = median;
			*newright = nptr;
			
		}
		else{
			
			/*If the B+ Node is an inner Node*/
			
			median = (root->n.leafnode->count - 1)/2;
			retval = root->n.leafnode->entry[median].key;//mid value to be returned
			
			BplusNode *nptr = (BplusNode*)malloc(sizeof(BplusNode)); //Creation of a new B+ Node
			nptr->n.leafnode = makeleafnode();
			nptr->n.leafnode->prev = root->n.leafnode;
			nptr->n.leafnode->next = root->n.leafnode->next;
			root->n.leafnode->next = nptr->n.leafnode;
			nptr->isleaf = TRUE;
			
			for(i = median;i<root->n.leafnode->count;i++){
				nptr->n.leafnode->count += 1;
				nptr->n.leafnode->entry[i-median] = root->n.leafnode->entry[i];
				 
			}
			root->n.leafnode->count = median;
			*newright = nptr;			
		}
		
		return retval;
	}
}

void Insertinnode(BplusNode *root,Key_type midentry,BplusNode **newright,int pos,boolean *fromleaf){
	
	/*Inserting midentry key  at position pos in an innernode corresponding to a B+ Node root */
	
	if(*fromleaf == FALSE){
		root->n.innernode->isBleaf = FALSE;
		 
	}else{
		root->n.innernode->isBleaf = TRUE;
	}
	int i;
	i = root->n.innernode->count;
	i -= 1;
	while(i>=0 && i>= pos){
		
		root->n.innernode->keys[i+1] = root->n.innernode->keys[i];
		root->n.innernode->branch[i+2] = root->n.innernode->branch[i+1];
		i--;
	}
	root->n.innernode->keys[i+1] = midentry;
	root->n.innernode->branch[i+2] = *newright;
	
	root->n.innernode->count += 1;
}

boolean Insert(BplusNode *root,Treeentry entry,Key_type *midentry,BplusNode **newright,boolean *fromleaf){
	
	/*Inserts entry 'entry' in B+ Tree rooted at  root  and returns a boolean
	 *return TRUE  -> a new key ( *midentry ) is to be passed up to be inserted in  the parent Node
	 *return FALSE -> No new key to be inserted in the parent
     */
	
	boolean retval = FALSE;
	int pos;
	if(root->isleaf == TRUE){
		if(search(root,entry.key,&pos) ==  TRUE){
			root->n.leafnode->entry[pos].val += 1;
		}
		else{
			Insertinleaf(root,entry,pos);
			if(split_detector(root) == TRUE){
				*midentry = split(root,newright);
				retval = TRUE;
				*fromleaf = TRUE;
			}
		
		}
	}
	else{
	
		search(root,entry.key,&pos);
		
		if(Insert(root->n.innernode->branch[pos],entry,midentry,newright,fromleaf) == TRUE){
			Insertinnode(root,*midentry,newright,pos,fromleaf);
			if(split_detector(root) == TRUE){
				*midentry = split(root,newright);
				retval = TRUE;
				*fromleaf = FALSE;
			}
		}
	}
	return retval;
}

BplusNode* Insertion(BplusNode *root, Treeentry entry){
	
	/*Main Insertion driver function*/
	
	BplusNode *newroot;
	boolean fromleaf;
	BplusNode *newright = NULL;
	Key_type midentry;
	if(root == NULL){

		root = (BplusNode*)malloc(sizeof(BplusNode));
		root->isleaf = TRUE;
		root->n.leafnode = makeleafnode();
		root->n.leafnode->entry[0] = entry;
		root->n.leafnode->count  = 1;

	}//If the B+ Tree is Empty ,create a new node (Initial Case)
	else if(Insert(root,entry,&midentry,&newright,&fromleaf) == TRUE){
		
		/*If new entry has bubbled upwards from the root itself , height of the tree increases ,so a new root is created*/
		
		newroot = (BplusNode*)malloc(sizeof(BplusNode));
		newroot->isleaf = FALSE;
		newroot->n.innernode = maketreenode();
		if(fromleaf == TRUE){
			newroot->n.innernode->isBleaf = TRUE;
		}
		newroot->n.innernode->branch[1] = newright;
		newroot->n.innernode->branch[0] = root;
		newroot->n.innernode->keys[0] = midentry;
		newroot->n.innernode->count += 1;
		root = newroot;
		
	}
	return root;
}

void traverse(BplusNode *root){
	
	/*Traverse all the entries present in the leaves(In the Doubly Linked List)*/
	
	if(root != NULL){
		if(root->isleaf == FALSE){
			
			int i;
			while(root->isleaf == FALSE){
				root = root->n.innernode->branch[0];
			}
	
			Leafnode *nptr;
			nptr = root->n.leafnode;
			while(nptr != NULL){
				for(i = 0;i<nptr->count;i++){
					printf("%d,",nptr->entry[i]);
				}
				printf(" <--> ");
				nptr = nptr->next;
			}
			
		}
		else{
			Leafnode *nptr;
			int i;
			nptr = root->n.leafnode;
			while(nptr != NULL){
				for(i = 0;i<nptr->count;i++){
					printf("%d,",nptr->entry[i]);
				}
				printf(" <--> ");
				nptr = nptr->next;
			}			
			
			
		}
	}
	else{
		printf("B+ Tree is empty!");
	}	

}
		
	/*B+ Tree Deletion*/
	
void merge(BplusNode *prev,BplusNode *root,int p){

	/*Merge an innernode corresponding to B+ Node root with the pth child of prev (parent node)*/
	
	BplusNode *right;
	right = prev->n.innernode->branch[p+1];
	int i;
	
	root->n.innernode->keys[root->n.innernode->count] = prev->n.innernode->keys[p];
	root->n.innernode->count += 1;
	
	i = 0;
	while(i < right->n.innernode->count){
		root->n.innernode->keys[root->n.innernode->count] = right->n.innernode->keys[i];
		root->n.innernode->branch[root->n.innernode->count] = right->n.innernode->branch[i];
		root->n.innernode->count += 1;
		i++;
	}
	root->n.innernode->branch[root->n.innernode->count] = right->n.innernode->branch[i];
	i = p;
	while(i< prev->n.innernode->count){
		
		prev->n.innernode->keys[i] = prev->n.innernode->keys[i+1];
		prev->n.innernode->branch[i+1] = prev->n.innernode->branch[i+2];
		i++;
	}
	prev->n.innernode->count -=1 ;
	free(right);
}

void merge_leaves(BplusNode *prev,BplusNode *root,int p){
	
	/*Merge function for leaves 
	 *Used in case of deletion when the a leafnode(corresponding to root) falls short of entries
	 *and even the siblings have minimum entries
	 */
	
	BplusNode *right;
	right = prev->n.innernode->branch[p+1];
	int i;

	i = 0;
	while(i < right->n.leafnode->count){
		root->n.leafnode->entry[root->n.leafnode->count] = right->n.leafnode->entry[i];
		root->n.leafnode->count += 1;
		i++;
	}
	
	if(right->n.leafnode->next){
		right->n.leafnode->next->prev = root->n.leafnode;
	}
	root->n.leafnode->next = right->n.leafnode->next; //Adjusting Doubly Linked Pointeers
	
	i = p;
	while(i< prev->n.innernode->count){
		
		prev->n.innernode->keys[i] = prev->n.innernode->keys[i+1];
		prev->n.innernode->branch[i+1] = prev->n.innernode->branch[i+2];
		i++;
	}
	prev->n.innernode->count -=1 ;
	free(right);
}

void fromleft(BplusNode *prev,BplusNode *root,int p){
	
	/*In this function, root borrows an entry from the parent and parent (prev) borrows the last entry of the left sibling of root*/
	
	
	BplusNode *left = prev->n.innernode->branch[p-1];
	
	int i;
	for(i = (root->n.innernode->count) - 1;i>=0;i--){
		root->n.innernode->keys[i+1] = root->n.innernode->keys[i];
	
		root->n.innernode->branch[i+2] = root->n.innernode->branch[i+1];
		
	}
	
	root->n.innernode->branch[1] = root->n.innernode->branch[0];
	root->n.innernode->keys[0] = prev->n.innernode->keys[p-1];
	prev->n.innernode->keys[p-1] = left->n.innernode->keys[(left->n.innernode->count)-1];
	root->n.innernode->branch[0] = left->n.innernode->branch[left->n.innernode->count + 1];
	
	root->n.innernode->count += 1;
	left->n.innernode->count -= 1;
	
}

void fromleft_leaf(BplusNode *prev,BplusNode *root,int p){
		
	/*In this function, leaf corresponding to root borrows an entry from the parent 
	 *and parent (prev) borrows the last entry of the left sibling of leaf node corresponding to root
	 */
	
	BplusNode *left = prev->n.innernode->branch[p-1];
	int i;
	for(i = (root->n.leafnode->count) - 1;i>=0;i--){
		root->n.leafnode->entry[i+1] = root->n.leafnode->entry[i];		
	}
	
	root->n.leafnode->entry[0] = left->n.leafnode->entry[(left->n.leafnode->count)-1];
	prev->n.innernode->keys[p-1] = left->n.leafnode->entry[(left->n.leafnode->count)-1].key;
	
	root->n.leafnode->count += 1;
	left->n.leafnode->count -= 1;
	
}


void fromright(BplusNode *prev,BplusNode *root,int p){

	/*In this function, root borrows an entry from the parent and parent (prev) borrows  the first entry of the right sibling of root*/

		
	BplusNode *right;
	right = prev->n.innernode->branch[p+1];
	int i;
	
	root->n.innernode->keys[root->n.innernode->count] = prev->n.innernode->keys[p];
	root->n.innernode->count += 1;
	root->n.innernode->branch[root->n.innernode->count] = right->n.innernode->branch[0];
	
	prev->n.innernode->keys[p] = right->n.innernode->keys[0];
	right->n.innernode->branch[0] = right->n.innernode->branch[1];
	
	i = 1;
	while(i < right->n.innernode->count){
		
		right->n.innernode->keys[i-1] = right->n.innernode->keys[i];
		right->n.innernode->branch[i-1] = right->n.innernode->branch[i];
		i++;
	}
	right->n.innernode->count -= 1;	
}

void fromright_leaf(BplusNode *prev,BplusNode *root,int p){

	/*In this function, leaf corresponding to root borrows an entry from the parent 
	 *and parent (prev) borrows  the first entry of the right sibling of leaf corresponding to root
	 */

		
	BplusNode *right;
	right = prev->n.innernode->branch[p+1];
	root->n.leafnode->entry[root->n.leafnode->count] = right->n.leafnode->entry[0];
	root->n.leafnode->count += 1;
	int i;
	i = 1;
	while(i < right->n.leafnode->count){
		
		right->n.leafnode->entry[i-1] = right->n.leafnode->entry[i];
		i++;
	}
	prev->n.innernode->keys[p] = right->n.leafnode->entry[0].key;
	right->n.leafnode->count -= 1;	
}

void restore_inner(BplusNode **root,BplusNode *prev,int p){

	/*This function checks when and which migrations (across inner nodes) to perform and performs the migrations accordingly*/

		
	if(prev != NULL){
		if(p == 0){
			if(prev->n.innernode->branch[1]->n.innernode->count > MIN){
				fromright(prev,*root,p);
			}
			else{
				merge(prev,*root,p);
			}
		}
		else if(p == prev->n.innernode->count){
			if(prev->n.innernode->branch[p-1]->n.innernode->count > MIN){
				fromleft(prev,*root,p);
			}
			else{
				merge(prev,prev->n.innernode->branch[p-1],p-1);
				*root = NULL;
			}
		}
		else{
			if(prev->n.innernode->branch[p-1]->n.innernode->count > MIN || prev->n.innernode->branch[p+1]->n.innernode->count > MIN){
				if(prev->n.innernode->branch[p-1]->n.innernode->count > MIN){
					
					fromleft(prev,*root,p);
				}
				else if(prev->n.innernode->branch[p+1]->n.innernode->count > MIN){
					fromright(prev,*root,p);
				}
			}
			else{
				merge(prev,*root,p);
			}
		}

	}
	else{
		if((*root)->n.innernode->count == 0){
			BplusNode *temp;
			temp = *root;
			*root = (*root)->n.innernode->branch[0];
			free(temp);
		}
	}
}

void restore_leaf(BplusNode **root,BplusNode *prev,int p){

	/*This function checks when and which migrations (across leaves) to perform and performs the migrations accordingly*/

		
	if(prev != NULL){
		if(p == 0){
			if(prev->n.innernode->branch[1]->n.leafnode->count > DATAMIN){
				
			//	printf("yes2\n");
				fromright_leaf(prev,*root,p);
			}
			else{
				merge_leaves(prev,*root,p);
			}
		}
		else if(p == prev->n.innernode->count){
			if(prev->n.innernode->branch[p-1]->n.leafnode->count > DATAMIN){
				fromleft_leaf(prev,*root,p);
			}
			else{
				merge_leaves(prev,prev->n.innernode->branch[p-1],p-1);
				*root = NULL;
			}
		}
		else{
			if(prev->n.innernode->branch[p-1]->n.leafnode->count > DATAMIN || prev->n.innernode->branch[p+1]->n.leafnode->count > DATAMIN){
				if(prev->n.innernode->branch[p-1]->n.leafnode->count > DATAMIN){
					
					fromleft_leaf(prev,*root,p);
				}
				else if(prev->n.innernode->branch[p+1]->n.leafnode->count > DATAMIN){
					fromright_leaf(prev,*root,p);
				}
			}
			else{
				merge_leaves(prev,*root,p);
			}
		}

	}
	else{
		if((*root)->n.leafnode->count == 0){
			
			free(*root);
			*root = NULL;
		}
	}
}

void delete_from_node(BplusNode *root,int pos){
		
	/*Delete a key at position pos from an innernode corresponding to B+ Node root */
	
	int i;
	i = pos + 1;
	while(i<(root->n.leafnode->count)){
		root->n.leafnode->entry[i-1] = root->n.leafnode->entry[i];
		i++;
	}
	root->n.leafnode->count -= 1;
}


boolean isleaf(BplusNode *root){
	
	/*checks whether a B+ Node is a leaf or not*/
	
	if(root){
		return root->isleaf;
	}
	
}


BplusNode* DeletionTree(BplusNode *root,BplusNode*prev,Key_type key,int p){

	/*Deletes an entry with key 'key' from the B+ tree rooted at root.*/


	int pos;
	if(root == NULL){
		printf("Empty Tree\n");
	}
	else{
		if(search(root,key,&pos) == TRUE){
			if(isleaf(root) != TRUE){
				
				root->n.innernode->branch[pos] = DeletionTree(root->n.innernode->branch[pos],root,key,pos);
			}
			else{
				delete_from_node(root,pos);
			}
		}
		else{
			root->n.innernode->branch[pos] = DeletionTree(root->n.innernode->branch[pos],root,key,pos);
		}
		if(isleaf(root) == TRUE){
			if(root->n.leafnode->count < DATAMIN){
				restore_leaf(&root,prev,p);
			}
		}
		else{
			if(root->n.innernode->count < MIN){
				restore_inner(&root,prev,p);
			}			
		}

	}	
	return root;
}


int main() {
	
	int choice = 0;
	Treeentry n;

	BplusNode *root = NULL;
	while(choice != 4){
		
		printf("\nThe choices are:\n");
		printf("1.Insertion\n");
		printf("2.Deletion \n");
		printf("3.Visualize B+ Tree \n");
		printf("4.Exit\n");
		
		printf("\nEnter choice:\n");
		
		scanf("%d",&choice);
		switch(choice){
			case 1:
				printf("Enter an integer\n");
				scanf("%d",&(n.key));
				n.val = 1;
				root = Insertion(root,n);
	
				break;			
			
			case 2:
				printf("\nEnter the key to be deleted\n");
				scanf("%d",&(n.key));
				root = DeletionTree(root,NULL,n.key,0);
				break;
			case 3:
				traverse(root);
				break;				
			case 4:
				printf("Exit!");
				break;
			default:
				printf("Invalid choice\n");
				break;				
			
				
		}
	}
	
	return 0;
}

