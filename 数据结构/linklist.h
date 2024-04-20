typedef int data_t; 
typedef struct node 
{ 
	data_t data; 
	struct node *next; 
}listnode, *linklist;

linklist list_create();										//链表的创建 
int list_head_insert(linklist H, data_t value);				//链表头部插入 
int list_tail_insert(linklist H, data_t value);				//在尾部插入元素
linklist list_get(linklist H, int pos);						//获取某一结点的值 
int list_insert(linklist H, data_t value, int pos);			//在链表某一位置插入元素
int list_order_insert(linklist H,data_t value);				//链表的有序插入
int list_delete(linklist H,int pos);						//链表元素的删除
int list_show(linklist H);									//遍历打印链表每个元素的值
void list_reverse(linklist H);								//链表的倒置
void list_sort(linklist H);                                 //链表的排序
linklist list_free(linklist H); 							//链表空间的释放
linklist list_adjmax(linklist H, data_t *value);            //求相邻结点的最大值
int list_merge(linklist H1, linklist H2) 					//两个链表的合并
