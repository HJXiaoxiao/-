#include<stdio.h>
#include<stdlib.h>
#define Max 1000
#include<iostream>
#include<list>
#include<math.h>
#include<string>
#include<ctype.h>
#include <graphics.h>
#include <conio.h>
#define BLOCKTYPE 10
#define BLOCKTYPE 10
#include<windows.h >

int memory = Max;       //定义可用内存空间为1000KB
using namespace std;

typedef struct Table_items     //表示空闲分区表中的表项 
{
    int id;                 //假如 id 为-1，表示此分区是一个空闲分区。
    int base;              //指向分区的首地址
    int size;               //表示分区大小
    int status;             //表示此分区是否已经分配     0表示空闲  1表示已经分配
    int Last_visit;        //记录空闲区是不是上一次访问的过后，顺序第一个空闲区。 
}Item;
typedef Item datatype;      //空闲区表项 

typedef struct Table_list
{
    datatype* node;      //表示一个datatype类型的链表的结点
    struct Table_list* front;
    struct Table_list* next;
}List; 

struct Block
{	
	int start;
	int end;
	Block()//结构体内部函数 
	{
		start=0;
		end =0;
	}
	int operator<(Block b)
	{
		return this->end < b.start;
	}
};

struct MemBlock
{
	int num; //内存剩余多少块 ？？？
	int base;//当前块的大小 
	list<Block> memBlockList;
	MemBlock()
	{
		num = 0;
		base = 0;
		memBlockList.clear();//清空元素 
	}
};
struct Job   //分出去的内存块 
{
	int start;
	int end;
	int index;
	Job()
	{
		start=0;
		end =0;
	}
};
list<Job> jobList;//list的每个节点有三个域：前驱元素指针域、
                  //数据域和后继元素指针域。等价于双向循环链表 
int jobindex = 0;
MemBlock arr[BLOCKTYPE];
bool isMemInit = false;//是否初始化过内存

//回收所有内存
void MemEnd()
{
	int i = 0;
	for(i = 0;i<BLOCKTYPE;i++)
	{
		arr[i].base = 0;
		arr[i].memBlockList.clear();
	}
	jobList.clear();
	jobindex = 0;
	isMemInit = false;
}
 
//初始化分配内存
void MemInit( int PoolSize )
{
	
	int i = BLOCKTYPE - 1;
	int n = 0;
	int j = 0;
	int base;
	for(;PoolSize >=8 && i>=0;i--)
	{
		base = (int)pow(2.0,i+3);
		arr[i].num = 0;
		n = PoolSize/base;
		PoolSize = PoolSize%base;
		if(n > 0)
		{
			arr[i].num = n;
			for(j = n;j>0;j--)
			{
				Block block;
				arr[i].memBlockList.push_back(block);				
			}
		}
	}
	int start = 0;
	int end = 0;
	list<Block>::iterator it;
	for(i = 0;i<BLOCKTYPE;i++)
	{
		base = (int)pow(2.0,i+3);
		arr[i].base = base;
		if(arr[i].num>0)
		{
			it = arr[i].memBlockList.begin();
			while(it != arr[i].memBlockList.end())
			{				
				end = start+base-1;
				(*it).start = start;
				(*it).end = end;
				start = end+1;
				it++;
			}
		}	
 
	}
	isMemInit = true;
}
 
//合并连续内存
void mergeMem(int index)
{
	int i = index;
	if(i>=	BLOCKTYPE -1)//超过最大了
	{
		return;
	}
	if(arr[i].num > 1)
	{
		list<Block>::iterator it1 = arr[i].memBlockList.begin();//it1为迭代器，可以理解为指针，现在指向数组的一个元素 
		list<Block>::iterator it2 = arr[i].memBlockList.begin();
		it2++;
		while(it2 != arr[i].memBlockList.end())
		{
			if(it1->end == it2->start-1)
			{
				Block block;
				block.start = it1->start;
				block.end = it2->end;
				arr[i+1].memBlockList.push_back(block);
				arr[i+1].num++;
				arr[i].memBlockList.erase(it1);
				arr[i].memBlockList.erase(it2);
				arr[i+1].memBlockList.sort();
				arr[i].num--;
				arr[i].num--;
				break;
			}
			it1++;
			it2++;
		}
	}
	mergeMem(i+1);
}
 
//分割多余内存
void divdeMem(int start,int end,int index)
{
	int i = index;
	if(start > end)
	{
		//合并内存
		mergeMem(i+1);
		return;
	}
 
	Block block;
	block.end = end;
	block.start = end - arr[i].base+1;
	end = block.start -1;
	arr[i].memBlockList.push_back(block);
	arr[i].memBlockList.sort();
	arr[i].num++;
				
	divdeMem(start,end,i-1);
	
 
}
 
 
 
//申请内存
bool MemAlloc( int MemSize )
{
	if(MemSize > 4096 || MemSize < 1)
	{
		return false;
	}
	int size = 0;
	int i = 0;
	int n = 0;
	for(;i<BLOCKTYPE;i++)
	{
		if(MemSize <= arr[i].base)
		{
			if(0 == size)
			{
				size = arr[i].base;//实际分配的大小
			}
			
			//有空闲块
			if(arr[i].num > 0)
			{
				//空闲块链表应该是有序的
				Block block = arr[i].memBlockList.front();
				jobList.back().start = block.start;
				jobList.back().end = block.start+size-1;
				
				//分配了一块内存 
				arr[i].memBlockList.pop_front();
				arr[i].num--;
				
				//分割多余内存
				int start = block.start + size;
				int end = block.end;
				divdeMem(start,end,i-1);
				return true;
			}
			
		}		
	}
	return false;
}
 
//释放指定内存
void MemFree( int start,int end )
{
	int size = end - start +1;
	int i = 0;
	for(;i<BLOCKTYPE;i++)
	{
		if(arr[i].base == size)
		{
			Block block;
			block.start = start;
			block.end = end;
			arr[i].memBlockList.push_back(block);
			arr[i].memBlockList.sort();
			
			arr[i].num++;
			mergeMem(i);
			
		}
	}		
}
 
//获得要初始化的内存大小
bool getRequestMem(int &n)
{
	if(isMemInit)
	{
//		cout << "内存已经被初始化,将重新初始化分配内存,所有数据将会丢失,确定要继续吗,Y/N?" << endl;
		char c[2];
		int n=0;
        InputBox(c,2,"内存已经被初始化,将重新初始化分配内存,所有数据将会丢失,确定要继续吗,1/0?");
        n=atoi(c);
		if(n == 1)
		{
			
		}
		else if(n == 0)
		{
			return false;
		}
		else
		{
//			cout << "非法输入,返回主程序" << endl;
            HWND hWND = GetHWnd();
            SetWindowText(hWND, "Sauce");
	        MessageBox(hWND,"非法输入,返回主程序","提示框",  MB_OKCANCEL);
			return false;
		} 
		MemEnd();
	}
	char s1[5];
    InputBox(s1,5,"请输入分配的内存大小");
    n=atoi(s1);
//	cout << "请输入分配的内存大小" << endl;
//	cin >> n;
	return true;	
}
 
//为作业分配内存
void assignMemForJob()
{
	Job job;
	job.index = jobindex++;
	jobList.push_back(job);

    char s1[5];
    InputBox(s1,5,"请输入申请内存大小");
    int n;
    n=atoi(s1);
	if(MemAlloc(n))
	{
//		cout << "作业申请内存成功,起始地址为job" << job.index << ":" << jobList.back().start << "~" << jobList.back().end << endl;
        HWND hWND = GetHWnd();
        SetWindowText(hWND, "Sauce");
	    MessageBox(hWND,"作业申请内存成功","提示框",  MB_OKCANCEL);
	}
	else
	{
		jobindex--;
		jobList.pop_back();
		HWND hWND = GetHWnd();
        SetWindowText(hWND, "Sauce");
	    MessageBox(hWND,"当前没有足够内存,请释放一定内存后再试","提示框",  MB_OKCANCEL);
//		cout << "当前没有足够内存,请释放一定内存后再试" << endl;
	}
	
}
 
void checkMem()
{
	system("cls");
    initgraph(870, 600);
    setbkmode(TRANSPARENT);// 去掉文字背景
	IMAGE background;//定义一个图片名.
    loadimage(&background,"C:\\Users\\xiao\\Desktop\\picture\\5.png",870,600,1);//从图片文件获取图像
    putimage(0, 0, &background);//绘制图像到屏幕，图片左上角坐标为(0,0)
    setlinecolor(BLACK);
	rectangle(2,70,160,110);
	rectangle(180,70,340,110);
	rectangle(360,70,520,110);
	rectangle(540,70,700,110);
	
	rectangle(2,130,160,170);
	rectangle(180,130,340,170);
	rectangle(360,130,520,170);
	rectangle(540,130,700,170);
	outtextxy(0,10, "内存空闲块:");
	int i = 0;
	int j = 30; 
	char b[5];
	list<Block>::iterator it;	
	for(;i< BLOCKTYPE;i++)
	{
		sprintf(b, " %d" , arr[i].base);
        outtextxy(50,j,b);
        outtextxy(0,j, " :");
        outtextxy(100,j, "有");
        sprintf(b, " %d" , arr[i].num);
        outtextxy(150,j,b);
        outtextxy(200,j, "个空闲内存块    起始地址分别为:");
//		printf("%d : 有%d个空闲内存块    起始地址分别为:",arr[i].base,arr[i].num);
		for(it = arr[i].memBlockList.begin();it!=arr[i].memBlockList.end();it++)
		{
			sprintf(b, " %d" , it->start);
            outtextxy(450,j,b);
            outtextxy(500,j,"~");
            sprintf(b, " %d" , it->end);
            outtextxy(550,j,b);
//			printf("%d~%d ",it->start,it->end);
		}
        j=j+30;
	}
	
//	cout << "作业占用内存:" << endl;
	outtextxy(0,j,"作业占用内存:");
	j=j+30;
	list<Job>::iterator job = jobList.begin();
	while(job != jobList.end())
	{
		outtextxy(0,j,"作业");
		sprintf(b, " %d" , job->index);
        outtextxy(50,j,b);
        outtextxy(100,j, "大小:");
        sprintf(b, " %d" , job->end - job->start+1);
        outtextxy(150,j,b);
        outtextxy(200,j, "内存地址:");
        sprintf(b, " %d" , job->start);
        outtextxy(350,j,b);
        outtextxy(400,j,"~");
        sprintf(b, " %d" , job->end);
        outtextxy(450,j,b);
		printf("作业%d 大小:%d 内存地址:%d~%d\n",job->index,job->end - job->start+1,job->start,job->end);
		job++;
	}
}
 
//释放作业内存
void retrieveMemFromJob()
{
	char s1[5];
    InputBox(s1,5,"请输入要释放的作业编号");
    int n;
    n=atoi(s1);
	if(n < 0)
	{
//		cout << "不存在该编号的作业,作业释放内存失败" << endl;
        HWND hWND = GetHWnd();
        SetWindowText(hWND, "Sauce");
	    MessageBox(hWND,"存在该编号的作业,作业释放内存失败","提示框",  MB_OKCANCEL);
	}
	else
	{
		list<Job>::iterator it = jobList.begin();
		
		while(it != jobList.end())
		{
			if(it->index == n)
			{
				Job job = *it;
				jobList.erase(it);
				
				//释放作业内存
				MemFree(job.start,job.end);
				break;
			}
			it++;			
		}
//		cout << "作业释放内存成功" << endl;
	     HWND hWND = GetHWnd();
        SetWindowText(hWND, "Sauce");
	    MessageBox(hWND,"作业释放内存成功","提示框",  MB_OKCANCEL);	
	}
}
                     

List init(){            //初始化一个链表，创建一个头节点 
    List list;
    list.node = (datatype *)malloc(sizeof(datatype));   //申请空间 
    list.node->base = 0;    //指向分区的首地址，从0开始 
    list.node->id = -1;      //-1表示是空闲分区
    list.node->size = memory;  //初始分区大小 
    list.node->status = 0;    //表示此分区空闲          
    list.front = list.next = NULL;  //前后没有节点 
    list.node->Last_visit = 1;
    return list;    //返回初始化表 
}

datatype* input(){                 //初始化打算申请的内存分区节点
    datatype* item = (datatype *)malloc(sizeof(datatype));  //申请空间 
    char s1[5];
    InputBox(s1,5,"请输入作业号：");
    item->id=atoi(s1);
    char s2[5];
    InputBox(s2,5,"请输入所需要的内存的大小：");
    item->size=atoi(s2);
    item->Last_visit = 0;
    return item;
}

void Last_visit_swap(List *list)
{
	List* temp = list;      
    while (temp)
    {
        //如果分区未分配就要进行比较操作，并且记录差值和区的id号
        if(temp->node->Last_visit == 1 ){
            temp->node->Last_visit == 0;
         }
        temp = temp->next;
     }
}

int Last_visit_swap1(List *temp_next,List *list)
{
	List* temp = temp_next;      
    while (temp)
    {
        //如果分区未分配就要进行比较操作，并且记录差值和区的id号
        if(temp->node->status == 0 ){
            temp->node->Last_visit == 1;
            return 1;
         }
        temp = temp->next;
    }
    List* temp1 = list;      
    while (temp1)
    {
        //如果分区未分配就要进行比较操作，并且记录差值和区的id号
        if(temp1->node->status == 0 ){
            temp1->node->Last_visit == 1;
            return 1;
         }
        temp1 = temp1->next;
    }  
}

// 首次适应算法 
int First_fit(List *list){
    datatype* item = input();              // 要对 item 的起始地址和分配状态进行初始化
    List* temp = list;                      //定义一个临时变量list* ，指向list
//    printf("%d",sizeof(temp));

    while (temp)       //链表不为空 
    {
    	// 对item 的起始地址和分配状态进行初始化，对当前分区是否空闲以及是否符大于请求分配分区的大小 ，
		// 若满足条件， 
        if(temp->node->status == 0 && temp->node->size > item->size){//如果此前的分区未分配，并且分区大小大于请求分配的大小那么此时就可以进行分配
            List *front1 = temp->front;                                          //存储当前未分配分区的 上一个分区地址
            List *next1 = temp->next;                                          //存储当前未分配分区的  下一个分区地址   
            int base = temp->node->base;                                      //记录未分配当前分区的首地址

            datatype* new_node = (datatype*)malloc(sizeof(datatype));          // 多余出来的部分要新建立一个分区
            new_node->id = -1;                                                 //然后需要对这个新的分区进行一些信息的设置
            Last_visit_swap(list);
            new_node->Last_visit=1;
            new_node->id = -1;  
            new_node->size = temp->node->size - item->size;         //新分区的大小  等于  还未分配的时的分区大小 - 请求分配的结点的大小 
            temp->node = item;                                  //对请求分配的分区结点进行分配
            temp->node->status = 1;
            new_node->status = 0;
            new_node->base = base + temp->node->size;             //新建立分区的首地址是  请求分配的分区的首地址 + 请求分配的分区的大小

            List* temp_next = (List*)malloc(sizeof(List));           //临时节点 （申请一个新的链表节点 表示下一个分区）  并且进行初始化
            temp_next->node = new_node;                             //保存下一个的分区的信息
            temp_next->front = temp_next->next = NULL;                                    

            if(front1 == NULL && next1 == NULL){                      //如果 front和next节点都是空，表明它是第一次分配分区
                temp->node->base = 0;                               //初始化首地址
                temp->next = temp_next;                     
                temp_next->front = temp;
            }
             if(front1 == NULL && next1 != NULL){                 //在第一个分区中插入新的分区
                 temp->node->base = 0;
                 temp->node->status = 1;
                temp_next->front = temp;
                temp_next->next = temp->next;
                temp->next = temp_next;
             }
            if(front1 != NULL){                      //表明不是第一次分配节点，此时需要在中间插入下一个节点
                temp->node->base = temp->front->node->base+temp->front->node->size;        //初始化首地址
                temp_next->next = temp->next;                                       //保证新插入的节点会记录原先节点的下一个节点的首地址
                temp_next->front = temp;                               // 首尾都需要保证
                temp->next = temp_next;                             //最后让所申请的分区节点的下一个节点指向  我们刚刚建立的临时节点
            }
            return 1;
        }   
        else if(temp->node->status == 0 && temp->node->size == item->size)
        {
            item->base = temp->front->node->base+temp->front->node->size;               //新插入分区的首地址  等于上一个分区的 首地址+分区的大小
            item->status = 1;                                           //表示已经分配
            temp->node = item;
            return 1;
        }
        else{
            temp = temp->next;
            continue;
        }
        temp = temp->next;    //第一个空闲区装不下，找下一个空闲区 
    }
    return 0;
}


int Best_fit(List *list){
     int min = 0;            //记录最小分区的结点的大小
     int base_min = 0;      //记录最小节点的结点的起始地址
     List* temp = list;   
     datatype* item = input();  // 要对 item 的起始地址和分配状态进行初始化
    
    while (temp)
    {
         //如果分区未分配就要进行比较操作，并且记录差值和区的id号
         if(temp->node->status == 0 && temp->node->id == -1&& temp->node->size > item->size){
             if(min == 0){          //加入min为0 表示还未找到一个可以分配的分区
                 min = temp->node->size;
                 base_min = temp->node->base;
             }
             else
             {
                 if(temp->node->size < min){      //找到一个之后，需要找出最小的分区也就是它的size最小。
                     min = temp->node->size;
                     base_min = temp->node->base;
                 }
             }
             
         }
         if(temp->node->status == 0 && temp->node->id == -1 && temp->node->size == item->size){
             int base = temp->node->base;
             temp->node = item;
             temp->node->status = 1;
             temp->node->base = base;
             Last_visit_swap(list);
             Last_visit_swap1(list,temp);
             return 1;
         }
        temp = temp->next;
     }

     //因为可能没有任何一个空间可以满足要求需要做一个判断处理   
     temp = list;
     while (temp)
     {
         if(temp->node->base == base_min && temp->node->size > item->size){

            datatype* temp_node = (datatype*)malloc(sizeof(datatype));      //会有多余的空间多出来所以需要在建立一个结点插入到链表中
            temp_node->id = -1;
            temp->node->status = 1;
            Last_visit_swap(list);
            temp_node->Last_visit=1;
            temp_node->status = 0;
            temp_node->base = base_min + item->size;
            temp_node->size = temp->node->size - item->size;

            temp->node = item;                          //对item进行完整的初始化
            temp->node->base = base_min;
            temp->node->status = 1;
            
            List* temp_list_node = (List*)malloc(sizeof(List));         //新申请一个链表的结点并且初始化
            temp_list_node->node = temp_node;
            temp_list_node->front = temp;
            temp_list_node->next = temp->next;
            if(temp->next != NULL){
                temp->next->front = temp_list_node;
            }
            temp->next = temp_list_node;
            return 1;
         }
         temp = temp->next;
     }
       
 }
 
int Bad_fit(List *list){
     int max = 0;            //记录最大分区的结点的大小
     int base_max = 0;      //记录最大节点的结点的起始地址
     List* temp = list;   
     datatype* item = input();  // 要对 item 的起始地址和分配状态进行初始化
    
     while (temp)
     {
         //如果分区未分配就要进行比较操作，并且记录差值和区的id号
         if(temp->node->status == 0 && temp->node->id == -1&& temp->node->size > item->size){
             if(max == 0){          //加入min为0 表示还未找到一个可以分配的分区
                 max = temp->node->size;
                 base_max = temp->node->base;
             }
             else
             {
                 if(temp->node->size > max){      //找到一个之后，需要找出最大的分区也就是它的size最大。
                     max = temp->node->size;
                     base_max = temp->node->base;
                 }
             }
         }
        temp = temp->next;
     }

     //因为可能没有任何一个空间可以满足要求需要做一个判断处理   
     temp = list;
     while (temp)
     {
         if(temp->node->base == base_max && temp->node->size > item->size){
         	if(temp->node->status == 0 && temp->node->id == -1 && temp->node->size == item->size){
             int base = temp->node->base;
             temp->node = item;
             temp->node->status = 1;
             temp->node->base = base;
             Last_visit_swap(list);
             Last_visit_swap1(list,temp);
             return 1;
         }

            datatype* temp_node = (datatype*)malloc(sizeof(datatype));      //会有多余的空间多出来所以需要在建立一个结点插入到链表中
            temp_node->id = -1;
            temp->node->status = 1;
            Last_visit_swap(list);
            temp_node->Last_visit=1;
            temp_node->status = 0;
            temp_node->base = base_max + item->size;
            temp_node->size = temp->node->size - item->size;

            temp->node = item;                          //对item进行完整的初始化
            temp->node->base = base_max;
            temp->node->status = 1;
            
            List* temp_list_node = (List*)malloc(sizeof(List));         //新申请一个链表的结点并且初始化
            temp_list_node->node = temp_node;
            temp_list_node->front = temp;
            temp_list_node->next = temp->next;
            if(temp->next != NULL){
                temp->next->front = temp_list_node;
            }
            temp->next = temp_list_node;
            return 1;
         }
         temp = temp->next;
     }
       
 }
 
int Last_fit(List *list){
    datatype* item = input();              // 要对 item 的起始地址和分配状态进行初始化
    List* temp = list;                      //定义一个临时变量list* ，指向list
    while (temp->node->Last_visit !=1) 
    	{
    		temp=temp->next;
		}
    while (temp)       //链表不为空 
    {
        if(temp->node->status == 0 && temp->node->Last_visit ==1 &&temp->node->size > item->size){
		//如果此前的分区未分配，并且分区大小大于请求分配的大小那么此时就可以进行分配
            List *front1 = temp->front;                                          //存储当前未分配分区的 上一个分区地址
            List *next1 = temp->next;                                          //存储当前未分配分区的  下一个分区地址   
            int base = temp->node->base;                                      //记录未分配当前分区的首地址

            datatype* new_node = (datatype*)malloc(sizeof(datatype));          // 多余出来的部分要新建立一个分区
            new_node->id = -1;                                                 //然后需要对这个新的分区进行一些信息的设置
            Last_visit_swap(list);
            new_node->Last_visit=1;
            new_node->id = -1;  
            new_node->size = temp->node->size - item->size;         //新分区的大小  等于  还未分配的时的分区大小 - 请求分配的结点的大小 
            temp->node = item;                                  //对请求分配的分区结点进行分配
            temp->node->status = 1;
            new_node->status = 0;
            new_node->base = base + temp->node->size;             //新建立分区的首地址是  请求分配的分区的首地址 + 请求分配的分区的大小

            List* temp_next = (List*)malloc(sizeof(List));           //临时节点 （申请一个新的链表节点 表示下一个分区）  并且进行初始化
            temp_next->node = new_node;                             //保存下一个的分区的信息
            temp_next->front = temp_next->next = NULL;                                    

            if(front1 == NULL && next1 == NULL){                      //如果 front和next节点都是空，表明它是第一次分配分区
                temp->node->base = 0;                               //初始化首地址
                temp->next = temp_next;                     
                temp_next->front = temp;
            }
             if(front1 == NULL && next1 != NULL){                 //在第一个分区中插入新的分区
                 temp->node->base = 0;
                 temp->node->status = 1;
                temp_next->front = temp;
                temp_next->next = temp->next;
                temp->next = temp_next;
             }
            if(front1 != NULL){                      //表明不是第一次分配节点，此时需要在中间插入下一个节点
                temp->node->base = temp->front->node->base+temp->front->node->size;        //初始化首地址
                temp_next->next = temp->next;                                       //保证新插入的节点会记录原先节点的下一个节点的首地址
                temp_next->front = temp;                               // 首尾都需要保证
                temp->next = temp_next;                             //最后让所申请的分区节点的下一个节点指向  我们刚刚建立的临时节点
            }
            return 1;
        }   
        else if(temp->node->status == 0 && temp->node->size == item->size && temp->node->Last_visit ==1)
        {
            item->base = temp->front->node->base+temp->front->node->size;               //新插入分区的首地址  等于上一个分区的 首地址+分区的大小
            item->status = 1;                                           //表示已经分配
            temp->node = item;
            Last_visit_swap(list);
            Last_visit_swap1(list,temp);
            return 1;
        }
        temp = temp->next;    //第一个空闲区装不下，找下一个空闲区 
        Last_visit_swap(list);
        Last_visit_swap1(list,temp);
    }
    return 0;
}
 

int Momory_recycle(List *list){             //回收表 
    List* temp = list;                      //申请一个链表节点 指向list 的头节点
    int number;                             //用于存放要释放的节点的分区号
    char s2[5];
    InputBox(s2,5,"请输入需要回收的ID号：");
    number=atoi(s2);
    while (temp)
    {   
        if(temp->node->id == number)            //首先找到 节点id = number 的节点，然后分四种情况讨论 
        {   
            // 一、 要回收的是第一个结点
            if(temp->front == NULL){
                temp->node->status = 0;
                Last_visit_swap(list);
                temp->node->Last_visit=1;
                temp->node->id = -1;
                if(temp->next == NULL){
                    temp->node->size = temp->node->size + temp->next->node->size;
                    temp->next = temp->next;
                    return 1;
                }
                
            if(temp->next->node->id == -1 && temp->next->node->status == 0){
                List* next = temp->next;
                // 此时来判断 temp->next 是否是系统的最后一个结点
                // 此时只将当前节点 和下一个结点合并就可以了
                //即 首地址不变，   分区状态 和 分区id进行变化  
                temp->node->size = temp->node->size + next->node->size;
                temp->node->status = 0;
                temp->node->id = -1;
                temp->next = next->next;
                if(next->next == NULL){
                    free(next);
                    return 1;
                }
                //如果不是最后一个结点的话就会多一个步骤
                // 让 next->next->front 指向上一个结点
                else
                {
                    next->next->front = temp;
                    free(next);    
                    return 1;
                }       
            }
                
                return 1;
            }
            
            if(temp->front->node->id != -1 && temp->front->node->status != 0 && temp->next== NULL ){
                temp->node->status = 0;
                Last_visit_swap(list);
                temp->node->Last_visit=1;
                temp->node->id = -1;
                return 1;
            }
            
            if(temp->front->node->status == 0 && temp->next== NULL ){
                List* front = temp->front;
                front->next = temp;
                front->node->size += temp->node->size;
                free(temp);
                Last_visit_swap(list);
                front->node->Last_visit=1;
                return 1;
            }
              //二、 前后都没有空闲的分区
            //最简单，   直接改变 分区的 id 和 分区的状态就可以了。
            // 如果回收第一个分区的话 必须要先进行处理，如果不先进行处理 ，判断 temp->front->node->id != -1 会报一个段错误。因为temp-》front 此时指向的是null  
            if(temp->front->node->id != -1 && temp->front->node->status != 0 && temp->next->node->id != -1 && temp->next->node->status != 0){
                temp->node->status = 0;
                Last_visit_swap(list);
                temp->node->Last_visit=1;
                temp->node->id = -1;
                return 1;
            }

            //三、要回收的节点    前面和后面都是空闲的
            // 将三个空闲区合并到一起，起始地址为前面的分区的起始地址， 大小为三个空闲区大小之和
            //还需要做一个判断，如果
            if(temp->front->node->id == -1 && temp->front->node->status == 0 && temp->next->node->id == -1 && temp->next->node->status == 0){
                List* front = temp->front;
                List* next = temp->next;
                front->node->size = front->node->size + temp->node->size + next->node->size;    
                front->next = next->next;
                Last_visit_swap(list);
                front->node->Last_visit=1;
                 if(next->next == NULL){
                    free(temp);
                    return 1;
                }
                //如果不是最后一个结点的话就会多一个步骤
                // 让 next->next->front 指向上一个结点
                else
                {
                    
                    next->next->front = front;
                    free(temp);  
                    return 1;
                }       
                return 1;
            }
            // 四、 要回收的节点  前面的节点是空闲的
            //合并后的分区起始地址为前一个结点， 分区大小为前一个节点 与 当前节点之和。
            if(temp->front->node->id == -1 && temp->front->node->status == 0){
                List* front = temp->front;
                front->next = temp->next;
                temp->next->front = front;
                front->node->size += temp->node->size;
                free(temp);
                Last_visit_swap(list);
                front->node->Last_visit=1;
                return 1;
            }
            //五、 要回收的节点    后面的额节点是空闲的
            //合并后的分区首地址为当前节点 ，  分区大小为当前节点 与 当前节点的下一个结点大小之和。
            // 这个需要多一个步骤， 改变分区的 id 和  分区的状态。
            // 还要注意一点：  当要回收的空间是和  系统最后的空闲区相邻时 ， temp->next->next 指向的是null；

            if(temp->next->node->id == -1 && temp->next->node->status == 0){
                List* next = temp->next;
                // 此时来判断 temp->next 是否是系统的最后一个结点
                // 此时只将当前节点 和下一个结点合并就可以了
                //即 首地址不变，   分区状态 和 分区id进行变化  
                temp->node->size = temp->node->size + next->node->size;
                temp->node->status = 0;
                temp->node->id = -1;
                temp->next = next->next;
                Last_visit_swap(list);
                temp->node->Last_visit=1;
                if(next->next == NULL){
                    free(next);
                    return 1;
                }
                //如果不是最后一个结点的话就会多一个步骤
                // 让 next->next->front 指向上一个结点
                else
                {
                    next->next->front = temp;
                    free(next);    
                    return 1;
                }       
            }
        }
        temp = temp->next;
    }
    return 0;
 }
 
void Momery_state(List *list){
    List* temp = list;
    clearrectangle(0,230,870,600);
    IMAGE background;
    loadimage(&background,"C:\\Users\\xiao\\Desktop\\picture\\5.png",870,600,1);//从图片文件获取图像
    putimage(0, 0, &background);//绘制图像到屏幕，图片左上角坐标为(0,0)
    outtextxy(0, 0,"--------------------------------------动态分区模拟--------------------------------------" );
    outtextxy(0, 40, "请选择你想进行的操作：");
    outtextxy(0, 80, "  首次适应算法      最佳适应算法      下次适应算法     快速分配算法");
    outtextxy(0, 140, "  最坏适应算法      内存回收          显示内存状况     退出");
    outtextxy(0, 200,"--------------------------------------动态分区模拟--------------------------------------" );
    int  a=230;
    char b[5];
    while (temp)
    {
        if(temp->node->status == 0 && temp->node->id == -1){
        	outtextxy(0,a, "分区号：FREE");
        	outtextxy(150,a, "起始地址：");
        	sprintf(b, " %d" , temp->node->base);
        	outtextxy(250,a,b);
        	outtextxy(300,a, "终点地址：");
        	sprintf(b, " %d" , (temp->node->size) - 1);
        	outtextxy(400,a,b);
        	outtextxy(450,a, "内存大小：");
        	sprintf(b, " %d" , temp->node->size);
        	outtextxy(550,a, b);
        	outtextxy(600,a, "分区状态：空闲");
            a=a+30;
        }
        else
        {
        	outtextxy(0,a, "分区号：");
        	sprintf(b, " %d" , temp->node->id);
        	outtextxy(100,a, b);
        	// ------------------------------------ 
        	outtextxy(150,a, "起始地址：");
        	sprintf(b, " %d" , temp->node->base);
        	outtextxy(250,a, b);
        	// ------------------------------------  
        	outtextxy(300,a, "终点地址：");
        	sprintf(b, " %d" , (temp->node->size) - 1);
        	outtextxy(400,a, b);
        	// ------------------------------------  
        	outtextxy(450,a, "内存大小：");
        	sprintf(b, " %d" , temp->node->size);
        	outtextxy(550,a, b);
        	outtextxy(600,a, "分区状态：已分配");
           a=a+30;
        }

        temp = temp->next;
    }

}

void dialog1(){
	HWND hWND = GetHWnd(); // 获得窗口句柄
    SetWindowText(hWND, "Sauce");
	MessageBox(hWND, "分配失败","提示框", MB_OKCANCEL);
}

void dialog2(){
	HWND hWND = GetHWnd();
    SetWindowText(hWND, "Sauce");
	MessageBox(hWND,"分配成功","提示框",  MB_OKCANCEL);
}

void dialog3(){
	HWND hWND = GetHWnd();
    SetWindowText(hWND, "Sauce");
	MessageBox(hWND,"回收成功","提示框",  MB_OKCANCEL);
}

void dialog4(){
	HWND hWND = GetHWnd();
    SetWindowText(hWND, "Sauce");
	MessageBox(hWND,"回收失败","提示框",  MB_OKCANCEL);
}


int main(){
    List list = init();   //创建链表 
    int select;           //记录算法选择 
    int insert_state;     //记录首次适应算法插入是否成功------是否插入成功 
	int recycle_state;    //记录回收是否成功 
    int insert_state_best;//记录最佳适应算法插入是否成功 
    int insert_state_bad;//记录最坏适应算法插入是否成功 
    int insert_state_Last;  //记录下次适配算法插入是否成功 
    int choice = -1;
	int size = 0;
	int x=1;
	char key;
	MOUSEMSG m;//定义鼠标变量 m
	initgraph(870, 600);   // 初始化图形界面（定义窗口大小） 
	settextstyle(20,0,"楷体");//设置文字大小 格式 
	setbkmode(TRANSPARENT);// 去掉文字背景
    settextcolor(WHITE);   // 设置文本颜色 
    IMAGE background;//定义一个图片名.
        loadimage(&background,"C:\\Users\\xiao\\Desktop\\picture\\5.png",870,600,1);//从图片文件获取图像
    	putimage(0, 0, &background);//绘制图像到屏幕，图片左上角坐标为(0,0)
        outtextxy(0, 0,"--------------------------------------动态分区模拟--------------------------------------" );
        outtextxy(0, 40, "请选择你想进行的操作：");
        outtextxy(0, 80, "  首次适应算法      最佳适应算法      下次适应算法     快速分配算法");
        outtextxy(0, 140, "  最坏适应算法      内存回收          显示内存状况     退出");
        outtextxy(0, 200,"--------------------------------------动态分区模拟--------------------------------------" );
    	while(x){
		m = GetMouseMsg();  // 获取一个鼠标消息
		if(m.x>=1 && m.x<= 160 && m.y >=70&& m.y<=110){//检测鼠标的位置 是否满足条件 a
			setlinecolor(RED);//满足后 设置新的边框为红色
			rectangle(2,70,160,110);//画新的边框
			//如果点击了 年份
			if(m.uMsg==WM_LBUTTONDOWN){
                insert_state = First_fit(&list);
	            if(insert_state == 0){  // 失败 
	                dialog1();
	            }
	            else {   // 成功 
	                dialog2();
	            }     
			}
		}else if(m.x>=180 && m.x<= 340 && m.y >=70&& m.y<=110){
			setlinecolor(RED);
			rectangle(180,70,340,110);
			if(m.uMsg==WM_LBUTTONDOWN){
            insert_state_best = Best_fit(&list);
	            if(insert_state_best == 1){
	                dialog2();
	            }    
	            else  {
	                   dialog1();
	            }   
			}
		}else if(m.x>=360 && m.x<= 520 && m.y >=70&& m.y<=110){
			setlinecolor(RED);
			rectangle(360,70,520,110);
			if(m.uMsg==WM_LBUTTONDOWN){
                insert_state_Last = Last_fit(&list);
	            if(insert_state_Last == 0){
	                  dialog1();
	            }
	            else {
	                  dialog2();
	            }     
			}
		}else if(m.x>=540 && m.x<= 700 && m.y >=70&& m.y<=110){
			setlinecolor(RED);
			rectangle(540,70,700,110);
			if(m.uMsg==WM_LBUTTONDOWN){
				int y=1;
		     while(y)
			{
				char s1[5];
                InputBox(s1,5,"空闲内存伙伴算法程序模拟\n1.初始化内存分配\n2.作业分配申请内存\n3.作业释放内存\n4.查看当前内存使用情况\n0.退出模拟程序请输入选择：");
                choice=atoi(s1);  // 把字符串转换成整型数的一个函数 
				switch(choice)
				{
					case 1:				
						if(getRequestMem(size))
						{
							MemInit(size);
							cout << "初始化内存成功" << endl;
						}
						else
						{
							cout << "初始化内存失败" << endl;
						}			
						break;
					case 2:
						assignMemForJob();
						break;
					case 3:
						retrieveMemFromJob();
						break;
					case 4:
						checkMem();
						break;
					case 0:
						y=0; 
						settextstyle(20,0,"楷体");//设置文字大小 格式 
						setbkmode(TRANSPARENT);// 去掉文字背景
					    settextcolor(WHITE);
					    IMAGE background;//定义一个图片名.
				        loadimage(&background,"C:\\Users\\xiao\\Desktop\\picture\\5.png",870,600,1);//从图片文件获取图像
				    	putimage(0, 0, &background);//绘制图像到屏幕，图片左上角坐标为(0,0)
				        outtextxy(0, 0,"--------------------------------------动态分区模拟--------------------------------------" );
				        outtextxy(0, 40, "请选择你想进行的操作：");
				        outtextxy(0, 80, "  首次适应算法      最佳适应算法      下次适应算法     快速分配算法");
				        outtextxy(0, 140, "  最坏适应算法      内存回收          显示内存状况     退出");
				        outtextxy(0, 200,"--------------------------------------动态分区模拟--------------------------------------" );
						break; 
					}
				}
			}
		}
		//
		else if(m.x>=2 && m.x<= 160 && m.y >=130&& m.y<=170){//
			setlinecolor(RED);
			rectangle(2,130,160,170);
			//点击了列出所有
			if(m.uMsg==WM_LBUTTONDOWN){
		        insert_state_bad = Bad_fit(&list);
			    if(insert_state_bad == 1){
			           dialog2();
			    }    
			    else  {
			            dialog1();
			    } 
			}
		}else if(m.x>=180 && m.x<= 340 && m.y >=130&& m.y<=170){//
			setlinecolor(RED);
			rectangle(180,130,340,170);
			if(m.uMsg==WM_LBUTTONDOWN){
	            recycle_state = Momory_recycle(&list);
	            if(recycle_state == 1){
	                dialog3();
	            }
	            else{
	                dialog4();
	            }
			}
		}else if(m.x>=360 && m.x<= 520 && m.y >=130&& m.y<=170){//显示内存 
			setlinecolor(RED);
			rectangle(360,130,520,170);
			if(m.uMsg==WM_LBUTTONDOWN){
                Momery_state(&list);
			}
		}else if(m.x>=540 && m.x<= 700 && m.y >=130&& m.y<=170){//
			setlinecolor(RED);
			rectangle(540,130,700,170);
			if(m.uMsg==WM_LBUTTONDOWN){
                x=0;
			}
		}
		//一旦鼠标不在相应位置 将画出白色边框 覆盖之前的红色边框
		else {
			setlinecolor(WHITE);
			rectangle(2,70,160,110);
			rectangle(180,70,340,110);
			rectangle(360,70,520,110);
			rectangle(540,70,700,110);
			
			rectangle(2,130,160,170);
			rectangle(180,130,340,170);
			rectangle(360,130,520,170);
			rectangle(540,130,700,170);
		}
	}
    closegraph();
}
