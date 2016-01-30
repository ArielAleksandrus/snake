#ifndef LIST_H
#define LIST_H

typedef struct Node{
	void* value;
	struct Node* next;
	struct Node* prev;
}Node;
typedef struct List{
	int size;
	Node* first;
	Node* last;
}List;

List* newList();
Node* lnodeAt(List* l, int index);
int lsearchIndexByRef(List*l, void* ptrAddr);
void* lAt(List* l, int index);
void lclear(List* l);
void lfree(List* l);
void lprepend(List* l, void* value);
void lappend(List* l, void* value);
void linsertAt(List* l, void* value, int index);
void lremoveAt(List* l, int index);
void* lremoveNode(List* l, int index);
#endif