#include "list.h"
#include <stdlib.h>
List* newList(){
	List* l = malloc(sizeof(List));
	l->size = 0;
	l->first = NULL;
	l->last = NULL;
}
void* lAt(List* l, int index){
	Node* n = lnodeAt(l, index);
	if(n != NULL)
		return n->value;
	else
		return NULL;
}
void lclear(List* l){
	if(l == NULL)
		return;

	while(l->size > 0){
		lremoveAt(l, 0);
	}
}
void lfree(List* l){
	if(l == NULL)
		return;
	
	Node* cur = l->first;
	Node* aux = NULL;
	while(cur != NULL){
		aux = cur;
		cur = cur->next;
		free(aux);
	}
	free(l);
}
int lsearchIndexByRef(List* l, void* ptrAddr){
	int i;
	void* aux;
	for(i = 0; i < l->size; i++){
		aux = lAt(l, i);
		if(aux == ptrAddr)
			return i;
	}
	
	return -1;
}
Node* lnodeAt(List* l, int index){
	Node* aux = NULL;

	if(index < 0 || index >= l->size)
		return aux;

	int i;
	if(index > l->size / 2){
		aux = l->last;
		for(i = 0; i < l->size - index - 1; i++)
			aux = aux->prev;
	} else {
		aux = l->first;
		for(i = 0; i < index; i++)
			aux = aux->next;
	}

	return aux;
}
void lprepend(List* l, void* value){
	linsertAt(l, value, 0);
}
void lappend(List* l, void* value){
	linsertAt(l, value, l->size);
}
void linsertAt(List* l, void* value, int index){
	if(index < 0 || index > l->size)
		return;

	Node* toInsert = malloc(sizeof(Node));
	toInsert->value = value;
	toInsert->prev = NULL;
	toInsert->next = NULL;

	if(index == 0){
		if(l->first != NULL){
			l->first->prev = toInsert;
			toInsert->next = l->first;
		} else { // lista vazia.
			l->first = toInsert;
			l->last = toInsert;
		}
		l->first = toInsert;
		l->size++;
		return;
	} else if(index == l->size){
		toInsert->prev = l->last;
		l->last->next = toInsert;
		l->last = toInsert;
		l->size++;
		return;
	}

	Node* aux = NULL;
	int i;
	if(index > l->size / 2){
		aux = l->last;
		for(i = 0; i < l->size - index + 1; i++)
			aux = aux->prev;
	} else {
		aux = l->first;
		for(i = 0; i < index - 1; i++)
			aux = aux->next;
	}

	toInsert->next = aux->next;
	toInsert->prev = aux;
	aux->next->prev = toInsert;
	aux->next = toInsert;
	l->size++;
}
void lremoveAt(List* l, int index){
	void* value = lremoveNode(l, index);
	if(value != NULL)
		free(value);
}
void* lremoveNode(List* l, int index){
	Node* aux = lnodeAt(l, index);
	
	if(aux == NULL)
		return;
	
	Node* prev = aux->prev;
	Node* next = aux->next;
	
	void* value = aux->value;
	if(aux == NULL)
		return;
	
	if(prev == NULL)
		l->first = next;
	else
		prev->next = next;
	
	if(next == NULL)
		l->last = prev;
	else
		next->prev = prev;
	
	free(aux);
	l->size--;
	return value;
}