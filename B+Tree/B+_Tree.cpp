#include <stdio.h>
#include <stdlib.h>
#include <math.h> 

#define M 3
#define MaxQueueSize 100000
#define TRUE 1
#define FALSE 0;

typedef int ElementType;
typedef struct BPlusTreeNode *BPlusTree;
struct BPlusTreeNode {
	int Element_size;		//���� �� Ԫ�ظ��� 
	int Child_size;			//������� 
	ElementType Element[M + 1];		//��һ���������������� 
	BPlusTree Child[M + 2];	//�������ʱ���������һ�� 
	BPlusTree Parent;
	BPlusTree Next;
}; 

typedef struct Queue *QueueT;
struct Queue {
	BPlusTree Array[MaxQueueSize];
	int Front;
	int Rear;
	int len;
};
/*
*B+ TREE
*/ 
BPlusTree NewBPlusTree(void);
BPlusTree CreateBPlusTree (void);
BPlusTree Search (BPlusTree T, ElementType X);
int Find (BPlusTree T, ElementType X);
void Insert(BPlusTree *Root, BPlusTree T, ElementType X);
void Split(BPlusTree *Root, BPlusTree T);
BPlusTree Delete(BPlusTree T, ElementType X);
void DeleteElement(BPlusTree *Root, BPlusTree T, ElementType X);
void DeleteIndex(BPlusTree *Root, BPlusTree T);
void MergeLeafNode(BPlusTree *Root, BPlusTree T, int PosT, int PosSibling);
void MergeNoLeafNode(BPlusTree *Root, BPlusTree T, int PosT, int PosSibling);
void GiveIndex(BPlusTree *Root, BPlusTree T, int PosT, int PosSibling);
int FindLeafSibling(BPlusTree Parent, int i);
int FindIndexSibling(BPlusTree Parent, int i);
void BPlusTreePrint (BPlusTree Tree);

/*
*Queue
*/ 
QueueT NewQueue (void);
int QueueLength(QueueT Q);
void Enqueue(QueueT Q, BPlusTree obj);
BPlusTree Dequeue(QueueT Q);
void InsertSort(ElementType Array[], int N);
void InsertSortOfNode(BPlusTree Array[], int N);

int main()
{
	BPlusTree T = CreateBPlusTree();
	BPlusTreePrint(T);
	T = Delete(T, 2);
	printf("\n");
	BPlusTreePrint(T);
	T = Delete(T, 1);
	printf("\n");
	BPlusTreePrint(T);
	T = Delete(T, 3);
	printf("\n");
	BPlusTreePrint(T);
	T = Delete(T, 4);
	printf("\n");
	BPlusTreePrint(T);
	T = Delete(T, 5);
	printf("\n");
	BPlusTreePrint(T);
	T = Delete(T, 6);
	printf("\n");
	BPlusTreePrint(T);
	T = Delete(T, 7);
	printf("\n");
	BPlusTreePrint(T);
	T = Delete(T, 9);
	printf("\n");
	BPlusTreePrint(T);
	T = Delete(T, 8);
	printf("\n");
	BPlusTreePrint(T);
	// Delete(T, 26);
	// printf("\n");
	// BPlusTreePrint(T);
	// Delete(T, 25);
	// printf("\n");
	// BPlusTreePrint(T);
	// Delete(T, 20);
	// printf("\n");
	// BPlusTreePrint(T);
	// Delete(T, 17);
	// printf("\n");
	// BPlusTreePrint(T);
}

BPlusTree NewBPlusTree(void)
{
	int i;
	BPlusTree T = (BPlusTree)malloc(sizeof(BPlusTreeNode));
	T->Element_size = T->Child_size = 0;
	for(i = 0; i < M+1; i++) {	 
		T->Child[i] = NULL;			//Ҷ�ӽڵ����ӽڵ� 
		T->Element[i] = -1;			//������������-1����� 
	}
	T->Child[M+2] = NULL; 
	T->Parent = NULL;
	T->Next = NULL;
	return T;
}

BPlusTree CreateBPlusTree (void)
{
	int i, X, N;
	BPlusTree T = NULL;
	BPlusTree Ptr;
	scanf("%d", &N); 
	for(i = 0; i < N; i++) {
		scanf("%d", &X);
		Ptr = Search(T, X);
		if (Find(Ptr, X) != -1) {
			printf("Key %d is duplicated\n", X);
		}else {
			Insert(&T, Ptr, X);
		}
		//BPlusTreePrint(T);
		//printf("\n");
	}
	return T;
}

/*
*�ڵ����������������������ֿ����Խ��и������ 
*/
BPlusTree Search (BPlusTree T, ElementType X)
{
	if(T == NULL || T->Child_size == 0)		//������Ҷ�ڵ�ֱ�ӷ��أ��ݹ��˳������� 
		return T;
	
	int i, Position;
	for(i = 0; i < T->Element_size; i++) {		//����ֻ��һ�����䣬һ������������������䣬���Դ�1��ʼ����������һ������ʱ�������� 
		if(T->Element[i] > X) {		//�ҵ�����һ��������X�����������±����Ҫ�����������±꣬���û�м�Ϊ���һ�� 
			break;
		}
	}
	Position = i;
	return Search(T->Child[Position], X);			//�ݹ������ѯ 
}

/*
*Ԫ�����������������Ƚ��٣�ֱ�ӱ�����ѯ�������ڷ���λ�ã�δ���ڷ���-1
*/
int Find (BPlusTree T, ElementType X)
{
	if(T == NULL) {			//���� 
		return -1; 
	}
	
	int i, Position;
	for(i = 0; i < T->Element_size; i++) {
		if(T->Element[i] == X)
			return i; 
	} 
	return -1;
 }
 
void Insert(BPlusTree *Root, BPlusTree T, ElementType X)
{
	if(T == NULL) {
		*Root = NewBPlusTree();
		T = *Root;
	}
		
	T->Element[T->Element_size++] = X;
	InsertSort(T->Element, T->Element_size);
	Split(Root, T);
}

void Split(BPlusTree *Root, BPlusTree T)
{
	if(T == NULL) return;
	if((T->Child_size == 0 && T->Element_size <= M) || 		//�ݹ�������� 
		(T->Child_size > 0 && T->Element_size < M))
	{
		return;
	}
	
	int i, Mid;
	if(T->Parent == NULL) {
		BPlusTree NewRoot = NewBPlusTree();		//���ڵ�Ҫ���ѣ�ҲҪ���Ƿ���Ҷ�ڵ㣬���£�����һ���¸�
		*Root = T->Parent = NewRoot; 
	} 
	BPlusTree Left = NewBPlusTree();	//ֱ�Ӹ��µ������ӽڵ㲻����ԭ�����ӽڵ㣬����Ҫɾ���������ݹ����鷳 
	BPlusTree Right = NewBPlusTree();
	BPlusTree Parent = T->Parent;		//�����ͷ�T�Ŀռ���������¼�丸�ڵ㣬����ִ���
	Right->Parent = Left->Parent = Parent; 	//�ǵø����½��ڵ��Parentָ��
	Left->Next = Right; Right->Next = T->Next;	//��Ȼ����Ҫ���򣬵���С��ϵ�Ѿ�ȷ��
	if(Parent->Child[0] != T) {			//T������ǵ�һ������
		for(i = 1; i < Parent->Child_size; i++) {
			if(Parent->Child[i] == T) {
				Parent->Child[i-1]->Next = Left;
			}
		}
	}
	
	if(T->Child_size == 0) {			//������ѵ���Ҷ�ڵ㣬��ô���ʱ����M+1��Ԫ�� 
		Mid = (T->Element_size + 1) / 2;
		for(i = 0; i < Mid; i++) {
			Left->Element[Left->Element_size++] = T->Element[i];		//�ǵø���Ԫ�غ��ӽڵ���� 
		}
		for(i = Mid; i < T->Element_size; i++) {
			Right->Element[Right->Element_size++] = T->Element[i]; 
		}
		
	}else {
		Mid = T->Element_size / 2;
		for(i = 0; i < Mid; i++) {
			Left->Element[Left->Element_size++] = T->Element[i];
		}
		for(i = 0; i < Mid+1; i++) {
			Left->Child[Left->Child_size++] = T->Child[i];			//����ʼ�ձ�������һ�� 
			T->Child[i]->Parent = Left;								//�ͷ���T���ǵø����ӽڵ��ҵ��µĸ��ڵ� 
		}
		for(i = Mid+1; i < T->Element_size; i++) {	//����һ���������ƣ������䲻����ʧ 
			Right->Element[Right->Element_size++] = T->Element[i]; 
		}
		for(i = Mid+1; i < T->Child_size; i++) {
			Right->Child[Right->Child_size++] = T->Child[i];
			T->Child[i]->Parent = Right; 
		}
		
	}
	
	for(i = 0; i < Parent->Child_size; i++) {
		if(T == Parent->Child[i]) {
			Parent->Child[i] = Left;
			break;
		}
	} 
	
	if(Parent->Child_size == 0) {			//�����¸������ 
		Parent->Child[0] = Left;
		Parent->Child_size++;
	}
	
	Parent->Child[Parent->Child_size++] = Right;
	Parent->Element[Parent->Element_size++] = T->Element[Mid];	//����Ҷ�ڵ㻹�Ƿ�Ҷ�ڵ㣬����T->Element[Mid]��Ϊ������ 
	InsertSort(Parent->Element, Parent->Element_size);
	InsertSortOfNode(Parent->Child, Parent->Child_size);
	
	free(T); 
	//BPlusTreePrint(*Root);
	Split(Root, Parent);		//����ֱ���ҵ���Ӧ�ò���Ľڵ�λ�ã���������ϵݹ� 
}

BPlusTree Delete(BPlusTree T, ElementType X)
{
	if(T == NULL) {
		printf("T is Empty!\n");
		return NULL;
	}
	int i, N;
	BPlusTree Ptr;
	Ptr = Search(T, X);
	if (Find(Ptr, X) != -1) {
		DeleteElement(&T, Ptr, X);
	}else {
		printf("Key %d is not found\n", X);
	}
	return T;		//����Ѵ�������T��ΪRoot�����ᵼ��ʵ�ʵ�T�޸ĳɹ�����Ϊ���ߵĵ�ַ����ͬ
}

void DeleteElement(BPlusTree *Root, BPlusTree T, ElementType X)
{
	int i;
	int Pos = Find(T, X);
	int PosT, PosSibling;
	BPlusTree Sibling;
	BPlusTree Parent;
	for(i = Pos; i < T->Element_size - 1; i++) {		//ɾ��Ҷ�ڵ��Element
		T->Element[i] = T->Element[i+1];
	}/*ע����ǰ׺�Լ��� �ԼӲ�Ӧ���Ǻ�׺�Լӣ������Խ��*/
	T->Element[--T->Element_size] = -1;					//���ڼ�1�����һ����������ѭ��������޸�

	if(T->Parent == NULL) return;		//���ڵ㣬����Ҫ����key��������ֱ��ɾ������
	if(T->Element_size > (M-1) / 2) {
		return;
	}else {
		Parent = T->Parent;
		for(i = 0; i < Parent->Child_size; i++) {
			if(Parent->Child[i] == T) {
				PosT = i;
				break;
			}
		}
		PosSibling = FindLeafSibling(Parent, PosT);
		if(PosSibling != -1) {			//����ֵܽڵ��и����Key
			Sibling = Parent->Child[PosSibling];
			if(PosT > PosSibling) {		//���ֵ�
				T->Element[T->Element_size++] = Sibling->Element[--Sibling->Element_size];
				Sibling->Element[Sibling->Element_size] = -1;
				InsertSort(T->Element, T->Element_size);
				Parent->Element[PosT - 1] = T->Element[0];		//�滻�����е�Key
			}else {						//���ֵ�
				T->Element[T->Element_size++] = Sibling->Element[0];
				for(i = 0; i < Sibling->Element_size - 1; i++) {
					Sibling->Element[i] = Sibling->Element[i+1];
				}
				Sibling->Element[--Sibling->Element_size] = -1;
				Parent->Element[PosSibling - 1] = Sibling->Element[0];		//�滻�����е�Key
			}
		}else {				//����ֵܽڵ�û�и����key
			if(i == 0) PosSibling = PosT+1;
			else PosSibling = PosT-1;
			MergeLeafNode(Root, Parent, PosT, PosSibling);
		}
	}
	DeleteIndex(Root, Parent);
	return;
}

void DeleteIndex(BPlusTree *Root, BPlusTree T)
{
	if(T == NULL) return;						//����
	int i;
	int PosT, PosSibling;
	BPlusTree Parent, Sibling;
	if(T->Element_size == 0 && T->Parent == NULL) {	//���ڵ㲻ͬ��������Ҷ�ڵ㣬���ٿ�����һ��Key��Ҫ����
		*Root = T->Child[0];	//���Ҫ�����㲻�ˣ�˵��ɾ����ֻʣ��һ�����ڵ㣬Ϊ��ǰ���ڵ�ĵ�һ������
		(*Root)->Parent = NULL;
		return;
	}
	if(T->Element_size > (M-1) / 2 - 1) {		//�ݹ��˳�����
		return;
	}else {
		Parent = T->Parent;
		if(Parent == NULL) return;
		for(i = 0; i < Parent->Child_size; i++) {
			if(Parent->Child[i] == T) {
				PosT = i;
				break;
			}
		}
		PosSibling = FindIndexSibling(T->Parent, PosT);
		if(PosSibling != -1) {			//����ֵܽڵ��и���
			GiveIndex(Root, Parent, PosT, PosSibling);
		}else {
			if(i == 0) PosSibling = PosT+1;
			else PosSibling = PosT-1;
			MergeNoLeafNode(Root, Parent, PosT, PosSibling);
		}
	}
	DeleteIndex(Root, Parent);
}

void MergeLeafNode(BPlusTree *Root, BPlusTree T, int PosT, int PosSibling)
{
	int i, Pos;
	BPlusTree DeleteT = T->Child[PosT];
	BPlusTree Sibling = T->Child[PosSibling];
	BPlusTree NewT = NewBPlusTree();
	for(i = 0; i < DeleteT->Element_size; i++) {
		NewT->Element[NewT->Element_size++] = DeleteT->Element[i];
	}
	for(i = 0; i < Sibling->Element_size; i++) {
		NewT->Element[NewT->Element_size++] = Sibling->Element[i];
	}
	InsertSort(NewT->Element, NewT->Element_size);
	Pos = PosT > PosSibling ? PosT : PosT+1;		//����λ��
	T->Child[Pos] = NewT;

	for(i = Pos-1; i < T->Child_size - 1; i++) {
		T->Child[i] = T->Child[i+1];
	}
	T->Child[--T->Child_size] = NULL;
	for(i = Pos-1; i < T->Element_size - 1; i++) {
		T->Element[i] = T->Element[i+1];
	}
	T->Element[--T->Element_size] = -1;
	//if(!T->Element_size) {
		//NewT->Parent = NULL;
		//*Root = NewT;
	//}else {
		NewT->Parent = T;
	//}

	free(DeleteT);
	free(Sibling);
}

void MergeNoLeafNode(BPlusTree *Root, BPlusTree T, int PosT, int PosSibling)
{
	int i, Pos;
	BPlusTree DeleteT = T->Child[PosT];
	BPlusTree Sibling = T->Child[PosSibling];
	BPlusTree NewT = NewBPlusTree();
	NewT->Parent = T;
	Pos = PosT > PosSibling ? PosT : PosT+1;		//����λ��
	for(i = 0; i < DeleteT->Element_size; i++) {
		NewT->Element[NewT->Element_size++] = DeleteT->Element[i];
	}
	for(i = 0; i < Sibling->Element_size; i++) {
		NewT->Element[NewT->Element_size++] = Sibling->Element[i];
	}
	NewT->Element[NewT->Element_size++] = T->Element[Pos-1];
	InsertSort(NewT->Element, NewT->Element_size);

	for(i = 0; i < DeleteT->Child_size; i++) {
		NewT->Child[NewT->Child_size++] = DeleteT->Child[i];
		DeleteT->Child[i]->Parent = NewT;		//�ǵø����ӽڵ�ĸ��ڵ�ָ�򣬷����ͷź�����
	}
	for(i = 0; i < Sibling->Child_size; i++) {
		NewT->Child[NewT->Child_size++] = Sibling->Child[i];
		Sibling->Child[i]->Parent = NewT;
	}
	//NewT->Child[NewT->Child_size++] = T->Child[PosT-1];
	InsertSortOfNode(NewT->Child, NewT->Child_size);
	
	T->Child[Pos] = NewT;

	for(i = Pos-1; i < T->Child_size - 1; i++) {
		T->Child[i] = T->Child[i+1];
	}
	T->Child[--T->Child_size] = NULL;
	for(i = Pos-1; i < T->Element_size - 1; i++) {
		T->Element[i] = T->Element[i+1];
	}
	T->Element[--T->Element_size] = -1;

	if(!T->Element_size) {
		NewT->Parent = NULL;
		*Root = NewT;
	}else {
		NewT->Parent = T;
	}
	
	free(DeleteT);
	free(Sibling);
}

void GiveIndex(BPlusTree *Root, BPlusTree T, int PosT, int PosSibling)
{
	int i, Pos;
	BPlusTree ReceiveT = T->Child[PosT];
	BPlusTree Sibling = T->Child[PosSibling];
	if(PosT < PosSibling) {
		ReceiveT->Element[ReceiveT->Element_size++] = T->Element[PosT];
		ReceiveT->Child[ReceiveT->Child_size++] = Sibling->Child[0];
		Sibling->Child[0]->Parent = ReceiveT;		//�ǵø����ƶ��ӽڵ�ĸ��ڵ�ָ��
		InsertSort(ReceiveT->Element, ReceiveT->Element_size);
		InsertSortOfNode(ReceiveT->Child, ReceiveT->Child_size);
		T->Element[PosT] = Sibling->Element[0];
		for(i = 0; i < Sibling->Element_size; i++) {
			Sibling->Element[i] = Sibling->Element[i+1];
		}
		Sibling->Element[--Sibling->Element_size] = -1;
		for(i = 0; i < Sibling->Child_size; i++) {
			Sibling->Child[i] = Sibling->Child[i+1];
		}
		Sibling->Child[--Sibling->Child_size] = NULL;
	}else {
		ReceiveT->Element[ReceiveT->Element_size++] = T->Element[PosT];
		ReceiveT->Child[ReceiveT->Child_size++] = Sibling->Child[Sibling->Child_size]; 
		Sibling->Child[Sibling->Child_size]->Parent = ReceiveT;
		InsertSort(ReceiveT->Element, ReceiveT->Element_size);
		InsertSortOfNode(ReceiveT->Child, ReceiveT->Child_size);
		T->Element[PosT] = Sibling->Element[Sibling->Element_size];
		Sibling->Element[--Sibling->Element_size] = -1;
		Sibling->Child[--Sibling->Child_size] = NULL;
	}
}

int FindLeafSibling(BPlusTree Parent, int i)
{
	if(i == 0) {
		if(Parent->Child[i+1]->Element_size > (M-1) / 2 + 1) return i+1;
	}else if(i == Parent->Child_size - 1) {
		if(Parent->Child[i-1]->Element_size > (M-1) / 2 + 1) return i-1;
	}else {
		if(Parent->Child[i-1]->Element_size > (M-1) / 2 + 1) return i-1;
		else if(Parent->Child[i+1]->Element_size > (M-1) / 2 + 1) return i+1;
	}
	return -1;
}

int FindIndexSibling(BPlusTree Parent, int i)
{
	if(i == 0) {
		if(Parent->Child[i+1]->Element_size > (M-1) / 2) return i+1;
	}else if(i == Parent->Child_size - 1) {
		if(Parent->Child[i-1]->Element_size > (M-1) / 2) return i-1;
	}else {
		if(Parent->Child[i-1]->Element_size > (M-1) / 2) return i-1;
		else if(Parent->Child[i+1]->Element_size >= (M-1) / 2) return i+1;
	}
	return -1;
}

void InsertSort(ElementType Array[], int N)
{
	int i, j, P, Tmp;
	for ( P = 1; P < N; P++ ) { 
		Tmp = Array[ P ];
		for ( j = P; j > 0 && Array[ j - 1 ] > Tmp; j-- ) {
			Array[ j ] = Array[ j - 1 ]; 
		}
		Array[ j ] = Tmp;
	}
}

void InsertSortOfNode(BPlusTree Array[], int N)
{
	int i, j, P;
	BPlusTree Tmp;
	for ( P = 1; P < N; P++ ) { 
		Tmp = Array[ P ];
		for ( j = P; j > 0 && Array[ j - 1 ]->Element[0] > Tmp->Element[0]; j-- ) {
			Array[ j ] = Array[ j - 1 ]; 
		}
		Array[ j ] = Tmp;
	}
}

void BPlusTreePrint (BPlusTree Tree)
{
	int i, Mark = 1;
	BPlusTree T;
	QueueT DQ, EQ;
	QueueT Q1 = NewQueue();
	QueueT Q2 = NewQueue();
	Enqueue(Q1, Tree);
	while(QueueLength(Q1) || QueueLength(Q2)) {
		if(Mark) {
			DQ = Q1;
			EQ = Q2;
		}
		else {
			DQ = Q2;
			EQ = Q1;
		}
		T = Dequeue(DQ);
		
		for(i = 0 ; i < T->Element_size; i++) {
			if(i == 0) printf("[");
			printf("%d", T->Element[i]);
			if(i < T->Element_size - 1) {
				printf(","); 
			}else printf("]");
		}
		if(DQ == Q1 && QueueLength(Q1) == 0) {
			printf("\n");
			Mark = !Mark;
		}if(DQ == Q2 && QueueLength(Q2) == 0) {
			printf("\n");
			Mark = !Mark;
		}
		for(i = 0; i < T->Child_size; i++) {		//��Ҷ�ڵ��child��Element��һ�� 
			Enqueue(EQ, T->Child[i]);
		}
	}
}
	
QueueT NewQueue (void) 
{
	QueueT Q = (QueueT)malloc(sizeof(struct Queue));
	Q->Front = 	Q->Rear = 0;
	Q->len = 0;
	return Q;
}	

int QueueLength(QueueT Q)
{
    return Q->len;
}

void Enqueue(QueueT Q, BPlusTree obj)
{
    if (Q->len == MaxQueueSize) {
        printf("Enqueue called on a full queue");
        return;
    }
    Q->Array[(Q->Rear++) % MaxQueueSize] = obj;
    Q->len++;
    return;
}

BPlusTree Dequeue(QueueT Q)
{
    BPlusTree result;
    int i;

    if (Q->len == 0) {
    	printf("Dequeue of empty queue");
    	return NULL;
	}
    result = Q->Array[(Q->Front++) % MaxQueueSize];
    Q->len--;
    return result;
}
