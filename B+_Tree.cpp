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
	int Element_size;		//索引 或 元素个数 
	int Child_size;			//区间个数 
	ElementType Element[M + 1];		//多一个来处理溢出的情况 
	BPlusTree Child[M + 2];	//区间溢出时比索引会多一个 
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
		T->Child[i] = NULL;			//叶子节点无子节点 
		T->Element[i] = -1;			//传入正数，以-1代表空 
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
*节点搜索函数，将搜索与插入分开可以进行更多操作 
*/
BPlusTree Search (BPlusTree T, ElementType X)
{
	if(T == NULL || T->Child_size == 0)		//空树或叶节点直接返回（递归退出条件） 
		return T;
	
	int i, Position;
	for(i = 0; i < T->Element_size; i++) {		//不会只有一个区间，一个索引会产生两个区间，所以从1开始，否则插入第一个区间时会有问题 
		if(T->Element[i] > X) {		//找到“第一个”大于X的索引，其下标就是要插入的区间的下标，如果没有即为最后一个 
			break;
		}
	}
	Position = i;
	return Search(T->Child[Position], X);			//递归继续查询 
}

/*
*元素搜索函数（个数比较少，直接遍历查询），存在返回位置，未存在返回-1
*/
int Find (BPlusTree T, ElementType X)
{
	if(T == NULL) {			//空树 
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
	if((T->Child_size == 0 && T->Element_size <= M) || 		//递归结束条件 
		(T->Child_size > 0 && T->Element_size < M))
	{
		return;
	}
	
	int i, Mid;
	if(T->Parent == NULL) {
		BPlusTree NewRoot = NewBPlusTree();		//根节点要分裂（也要分是否是叶节点，见下），给一个新根
		*Root = T->Parent = NewRoot; 
	} 
	BPlusTree Left = NewBPlusTree();	//直接给新的左右子节点不保留原来的子节点，否则还要删除其中数据过于麻烦 
	BPlusTree Right = NewBPlusTree();
	BPlusTree Parent = T->Parent;		//后面释放T的空间后如果不记录其父节点，会出现错误
	Right->Parent = Left->Parent = Parent; 	//记得更新新建节点的Parent指针
	Left->Next = Right; Right->Next = T->Next;	//虽然后面要排序，但大小关系已经确定
	if(Parent->Child[0] != T) {			//T如果不是第一个区间
		for(i = 1; i < Parent->Child_size; i++) {
			if(Parent->Child[i] == T) {
				Parent->Child[i-1]->Next = Left;
			}
		}
	}
	
	if(T->Child_size == 0) {			//如果分裂的是叶节点，那么溢出时会有M+1个元素 
		Mid = (T->Element_size + 1) / 2;
		for(i = 0; i < Mid; i++) {
			Left->Element[Left->Element_size++] = T->Element[i];		//记得更新元素和子节点个数 
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
			Left->Child[Left->Child_size++] = T->Child[i];			//区间始终比索引多一个 
			T->Child[i]->Parent = Left;								//释放了T，记得给其子节点找到新的父节点 
		}
		for(i = Mid+1; i < T->Element_size; i++) {	//其中一个索引上移，但区间不会消失 
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
	
	if(Parent->Child_size == 0) {			//创建新根的情况 
		Parent->Child[0] = Left;
		Parent->Child_size++;
	}
	
	Parent->Child[Parent->Child_size++] = Right;
	Parent->Element[Parent->Element_size++] = T->Element[Mid];	//无论叶节点还是非叶节点，都是T->Element[Mid]成为新索引 
	InsertSort(Parent->Element, Parent->Element_size);
	InsertSortOfNode(Parent->Child, Parent->Child_size);
	
	free(T); 
	//BPlusTreePrint(*Root);
	Split(Root, Parent);		//由于直接找到了应该插入的节点位置，因而是向上递归 
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
	return T;		//如果把传进来的T作为Root并不会导致实际的T修改成功，因为二者的地址并不同
}

void DeleteElement(BPlusTree *Root, BPlusTree T, ElementType X)
{
	int i;
	int Pos = Find(T, X);
	int PosT, PosSibling;
	BPlusTree Sibling;
	BPlusTree Parent;
	for(i = Pos; i < T->Element_size - 1; i++) {		//删除叶节点的Element
		T->Element[i] = T->Element[i+1];
	}/*注意是前缀自减， 自加才应该是后缀自加，否则会越界*/
	T->Element[--T->Element_size] = -1;					//由于减1，最后一个并不能在循环中完成修改

	if(T->Parent == NULL) return;		//根节点，不需要考虑key的数量，直接删除即可
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
		if(PosSibling != -1) {			//如果兄弟节点有富余的Key
			Sibling = Parent->Child[PosSibling];
			if(PosT > PosSibling) {		//左兄弟
				T->Element[T->Element_size++] = Sibling->Element[--Sibling->Element_size];
				Sibling->Element[Sibling->Element_size] = -1;
				InsertSort(T->Element, T->Element_size);
				Parent->Element[PosT - 1] = T->Element[0];		//替换索引中的Key
			}else {						//右兄弟
				T->Element[T->Element_size++] = Sibling->Element[0];
				for(i = 0; i < Sibling->Element_size - 1; i++) {
					Sibling->Element[i] = Sibling->Element[i+1];
				}
				Sibling->Element[--Sibling->Element_size] = -1;
				Parent->Element[PosSibling - 1] = Sibling->Element[0];		//替换索引中的Key
			}
		}else {				//如果兄弟节点没有富余的key
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
	if(T == NULL) return;						//空树
	int i;
	int PosT, PosSibling;
	BPlusTree Parent, Sibling;
	if(T->Element_size == 0 && T->Parent == NULL) {	//根节点不同于其他非叶节点，最少可以有一个Key，要是连
		*Root = T->Child[0];	//这个要求都满足不了，说明删除后只剩下一个根节点，为当前根节点的第一个孩子
		(*Root)->Parent = NULL;
		return;
	}
	if(T->Element_size > (M-1) / 2 - 1) {		//递归退出条件
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
		if(PosSibling != -1) {			//如果兄弟节点有富余
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
	Pos = PosT > PosSibling ? PosT : PosT+1;		//区间位置
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
	Pos = PosT > PosSibling ? PosT : PosT+1;		//区间位置
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
		DeleteT->Child[i]->Parent = NewT;		//记得更新子节点的父节点指向，否则释放后会出错
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
		Sibling->Child[0]->Parent = ReceiveT;		//记得更新移动子节点的父节点指向
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
		for(i = 0; i < T->Child_size; i++) {		//非叶节点的child比Element多一个 
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
