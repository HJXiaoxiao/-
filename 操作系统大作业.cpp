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

int memory = Max;       //��������ڴ�ռ�Ϊ1000KB
using namespace std;

typedef struct Table_items     //��ʾ���з������еı��� 
{
    int id;                 //���� id Ϊ-1����ʾ�˷�����һ�����з�����
    int base;              //ָ��������׵�ַ
    int size;               //��ʾ������С
    int status;             //��ʾ�˷����Ƿ��Ѿ�����     0��ʾ����  1��ʾ�Ѿ�����
    int Last_visit;        //��¼�������ǲ�����һ�η��ʵĹ���˳���һ���������� 
}Item;
typedef Item datatype;      //���������� 

typedef struct Table_list
{
    datatype* node;      //��ʾһ��datatype���͵�����Ľ��
    struct Table_list* front;
    struct Table_list* next;
}List; 

struct Block
{	
	int start;
	int end;
	Block()//�ṹ���ڲ����� 
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
	int num; //�ڴ�ʣ����ٿ� ������
	int base;//��ǰ��Ĵ�С 
	list<Block> memBlockList;
	MemBlock()
	{
		num = 0;
		base = 0;
		memBlockList.clear();//���Ԫ�� 
	}
};
struct Job   //�ֳ�ȥ���ڴ�� 
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
list<Job> jobList;//list��ÿ���ڵ���������ǰ��Ԫ��ָ����
                  //������ͺ��Ԫ��ָ���򡣵ȼ���˫��ѭ������ 
int jobindex = 0;
MemBlock arr[BLOCKTYPE];
bool isMemInit = false;//�Ƿ��ʼ�����ڴ�

//���������ڴ�
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
 
//��ʼ�������ڴ�
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
 
//�ϲ������ڴ�
void mergeMem(int index)
{
	int i = index;
	if(i>=	BLOCKTYPE -1)//���������
	{
		return;
	}
	if(arr[i].num > 1)
	{
		list<Block>::iterator it1 = arr[i].memBlockList.begin();//it1Ϊ���������������Ϊָ�룬����ָ�������һ��Ԫ�� 
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
 
//�ָ�����ڴ�
void divdeMem(int start,int end,int index)
{
	int i = index;
	if(start > end)
	{
		//�ϲ��ڴ�
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
 
 
 
//�����ڴ�
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
				size = arr[i].base;//ʵ�ʷ���Ĵ�С
			}
			
			//�п��п�
			if(arr[i].num > 0)
			{
				//���п�����Ӧ���������
				Block block = arr[i].memBlockList.front();
				jobList.back().start = block.start;
				jobList.back().end = block.start+size-1;
				
				//������һ���ڴ� 
				arr[i].memBlockList.pop_front();
				arr[i].num--;
				
				//�ָ�����ڴ�
				int start = block.start + size;
				int end = block.end;
				divdeMem(start,end,i-1);
				return true;
			}
			
		}		
	}
	return false;
}
 
//�ͷ�ָ���ڴ�
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
 
//���Ҫ��ʼ�����ڴ��С
bool getRequestMem(int &n)
{
	if(isMemInit)
	{
//		cout << "�ڴ��Ѿ�����ʼ��,�����³�ʼ�������ڴ�,�������ݽ��ᶪʧ,ȷ��Ҫ������,Y/N?" << endl;
		char c[2];
		int n=0;
        InputBox(c,2,"�ڴ��Ѿ�����ʼ��,�����³�ʼ�������ڴ�,�������ݽ��ᶪʧ,ȷ��Ҫ������,1/0?");
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
//			cout << "�Ƿ�����,����������" << endl;
            HWND hWND = GetHWnd();
            SetWindowText(hWND, "Sauce");
	        MessageBox(hWND,"�Ƿ�����,����������","��ʾ��",  MB_OKCANCEL);
			return false;
		} 
		MemEnd();
	}
	char s1[5];
    InputBox(s1,5,"�����������ڴ��С");
    n=atoi(s1);
//	cout << "�����������ڴ��С" << endl;
//	cin >> n;
	return true;	
}
 
//Ϊ��ҵ�����ڴ�
void assignMemForJob()
{
	Job job;
	job.index = jobindex++;
	jobList.push_back(job);

    char s1[5];
    InputBox(s1,5,"�����������ڴ��С");
    int n;
    n=atoi(s1);
	if(MemAlloc(n))
	{
//		cout << "��ҵ�����ڴ�ɹ�,��ʼ��ַΪjob" << job.index << ":" << jobList.back().start << "~" << jobList.back().end << endl;
        HWND hWND = GetHWnd();
        SetWindowText(hWND, "Sauce");
	    MessageBox(hWND,"��ҵ�����ڴ�ɹ�","��ʾ��",  MB_OKCANCEL);
	}
	else
	{
		jobindex--;
		jobList.pop_back();
		HWND hWND = GetHWnd();
        SetWindowText(hWND, "Sauce");
	    MessageBox(hWND,"��ǰû���㹻�ڴ�,���ͷ�һ���ڴ������","��ʾ��",  MB_OKCANCEL);
//		cout << "��ǰû���㹻�ڴ�,���ͷ�һ���ڴ������" << endl;
	}
	
}
 
void checkMem()
{
	system("cls");
    initgraph(870, 600);
    setbkmode(TRANSPARENT);// ȥ�����ֱ���
	IMAGE background;//����һ��ͼƬ��.
    loadimage(&background,"C:\\Users\\xiao\\Desktop\\picture\\5.png",870,600,1);//��ͼƬ�ļ���ȡͼ��
    putimage(0, 0, &background);//����ͼ����Ļ��ͼƬ���Ͻ�����Ϊ(0,0)
    setlinecolor(BLACK);
	rectangle(2,70,160,110);
	rectangle(180,70,340,110);
	rectangle(360,70,520,110);
	rectangle(540,70,700,110);
	
	rectangle(2,130,160,170);
	rectangle(180,130,340,170);
	rectangle(360,130,520,170);
	rectangle(540,130,700,170);
	outtextxy(0,10, "�ڴ���п�:");
	int i = 0;
	int j = 30; 
	char b[5];
	list<Block>::iterator it;	
	for(;i< BLOCKTYPE;i++)
	{
		sprintf(b, " %d" , arr[i].base);
        outtextxy(50,j,b);
        outtextxy(0,j, " :");
        outtextxy(100,j, "��");
        sprintf(b, " %d" , arr[i].num);
        outtextxy(150,j,b);
        outtextxy(200,j, "�������ڴ��    ��ʼ��ַ�ֱ�Ϊ:");
//		printf("%d : ��%d�������ڴ��    ��ʼ��ַ�ֱ�Ϊ:",arr[i].base,arr[i].num);
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
	
//	cout << "��ҵռ���ڴ�:" << endl;
	outtextxy(0,j,"��ҵռ���ڴ�:");
	j=j+30;
	list<Job>::iterator job = jobList.begin();
	while(job != jobList.end())
	{
		outtextxy(0,j,"��ҵ");
		sprintf(b, " %d" , job->index);
        outtextxy(50,j,b);
        outtextxy(100,j, "��С:");
        sprintf(b, " %d" , job->end - job->start+1);
        outtextxy(150,j,b);
        outtextxy(200,j, "�ڴ��ַ:");
        sprintf(b, " %d" , job->start);
        outtextxy(350,j,b);
        outtextxy(400,j,"~");
        sprintf(b, " %d" , job->end);
        outtextxy(450,j,b);
		printf("��ҵ%d ��С:%d �ڴ��ַ:%d~%d\n",job->index,job->end - job->start+1,job->start,job->end);
		job++;
	}
}
 
//�ͷ���ҵ�ڴ�
void retrieveMemFromJob()
{
	char s1[5];
    InputBox(s1,5,"������Ҫ�ͷŵ���ҵ���");
    int n;
    n=atoi(s1);
	if(n < 0)
	{
//		cout << "�����ڸñ�ŵ���ҵ,��ҵ�ͷ��ڴ�ʧ��" << endl;
        HWND hWND = GetHWnd();
        SetWindowText(hWND, "Sauce");
	    MessageBox(hWND,"���ڸñ�ŵ���ҵ,��ҵ�ͷ��ڴ�ʧ��","��ʾ��",  MB_OKCANCEL);
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
				
				//�ͷ���ҵ�ڴ�
				MemFree(job.start,job.end);
				break;
			}
			it++;			
		}
//		cout << "��ҵ�ͷ��ڴ�ɹ�" << endl;
	     HWND hWND = GetHWnd();
        SetWindowText(hWND, "Sauce");
	    MessageBox(hWND,"��ҵ�ͷ��ڴ�ɹ�","��ʾ��",  MB_OKCANCEL);	
	}
}
                     

List init(){            //��ʼ��һ����������һ��ͷ�ڵ� 
    List list;
    list.node = (datatype *)malloc(sizeof(datatype));   //����ռ� 
    list.node->base = 0;    //ָ��������׵�ַ����0��ʼ 
    list.node->id = -1;      //-1��ʾ�ǿ��з���
    list.node->size = memory;  //��ʼ������С 
    list.node->status = 0;    //��ʾ�˷�������          
    list.front = list.next = NULL;  //ǰ��û�нڵ� 
    list.node->Last_visit = 1;
    return list;    //���س�ʼ���� 
}

datatype* input(){                 //��ʼ������������ڴ�����ڵ�
    datatype* item = (datatype *)malloc(sizeof(datatype));  //����ռ� 
    char s1[5];
    InputBox(s1,5,"��������ҵ�ţ�");
    item->id=atoi(s1);
    char s2[5];
    InputBox(s2,5,"����������Ҫ���ڴ�Ĵ�С��");
    item->size=atoi(s2);
    item->Last_visit = 0;
    return item;
}

void Last_visit_swap(List *list)
{
	List* temp = list;      
    while (temp)
    {
        //�������δ�����Ҫ���бȽϲ��������Ҽ�¼��ֵ������id��
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
        //�������δ�����Ҫ���бȽϲ��������Ҽ�¼��ֵ������id��
        if(temp->node->status == 0 ){
            temp->node->Last_visit == 1;
            return 1;
         }
        temp = temp->next;
    }
    List* temp1 = list;      
    while (temp1)
    {
        //�������δ�����Ҫ���бȽϲ��������Ҽ�¼��ֵ������id��
        if(temp1->node->status == 0 ){
            temp1->node->Last_visit == 1;
            return 1;
         }
        temp1 = temp1->next;
    }  
}

// �״���Ӧ�㷨 
int First_fit(List *list){
    datatype* item = input();              // Ҫ�� item ����ʼ��ַ�ͷ���״̬���г�ʼ��
    List* temp = list;                      //����һ����ʱ����list* ��ָ��list
//    printf("%d",sizeof(temp));

    while (temp)       //����Ϊ�� 
    {
    	// ��item ����ʼ��ַ�ͷ���״̬���г�ʼ�����Ե�ǰ�����Ƿ�����Լ��Ƿ�����������������Ĵ�С ��
		// ������������ 
        if(temp->node->status == 0 && temp->node->size > item->size){//�����ǰ�ķ���δ���䣬���ҷ�����С�����������Ĵ�С��ô��ʱ�Ϳ��Խ��з���
            List *front1 = temp->front;                                          //�洢��ǰδ��������� ��һ��������ַ
            List *next1 = temp->next;                                          //�洢��ǰδ���������  ��һ��������ַ   
            int base = temp->node->base;                                      //��¼δ���䵱ǰ�������׵�ַ

            datatype* new_node = (datatype*)malloc(sizeof(datatype));          // ��������Ĳ���Ҫ�½���һ������
            new_node->id = -1;                                                 //Ȼ����Ҫ������µķ�������һЩ��Ϣ������
            Last_visit_swap(list);
            new_node->Last_visit=1;
            new_node->id = -1;  
            new_node->size = temp->node->size - item->size;         //�·����Ĵ�С  ����  ��δ�����ʱ�ķ�����С - �������Ľ��Ĵ�С 
            temp->node = item;                                  //���������ķ��������з���
            temp->node->status = 1;
            new_node->status = 0;
            new_node->base = base + temp->node->size;             //�½����������׵�ַ��  �������ķ������׵�ַ + �������ķ����Ĵ�С

            List* temp_next = (List*)malloc(sizeof(List));           //��ʱ�ڵ� ������һ���µ�����ڵ� ��ʾ��һ��������  ���ҽ��г�ʼ��
            temp_next->node = new_node;                             //������һ���ķ�������Ϣ
            temp_next->front = temp_next->next = NULL;                                    

            if(front1 == NULL && next1 == NULL){                      //��� front��next�ڵ㶼�ǿգ��������ǵ�һ�η������
                temp->node->base = 0;                               //��ʼ���׵�ַ
                temp->next = temp_next;                     
                temp_next->front = temp;
            }
             if(front1 == NULL && next1 != NULL){                 //�ڵ�һ�������в����µķ���
                 temp->node->base = 0;
                 temp->node->status = 1;
                temp_next->front = temp;
                temp_next->next = temp->next;
                temp->next = temp_next;
             }
            if(front1 != NULL){                      //�������ǵ�һ�η���ڵ㣬��ʱ��Ҫ���м������һ���ڵ�
                temp->node->base = temp->front->node->base+temp->front->node->size;        //��ʼ���׵�ַ
                temp_next->next = temp->next;                                       //��֤�²���Ľڵ���¼ԭ�Ƚڵ����һ���ڵ���׵�ַ
                temp_next->front = temp;                               // ��β����Ҫ��֤
                temp->next = temp_next;                             //�����������ķ����ڵ����һ���ڵ�ָ��  ���Ǹոս�������ʱ�ڵ�
            }
            return 1;
        }   
        else if(temp->node->status == 0 && temp->node->size == item->size)
        {
            item->base = temp->front->node->base+temp->front->node->size;               //�²���������׵�ַ  ������һ�������� �׵�ַ+�����Ĵ�С
            item->status = 1;                                           //��ʾ�Ѿ�����
            temp->node = item;
            return 1;
        }
        else{
            temp = temp->next;
            continue;
        }
        temp = temp->next;    //��һ��������װ���£�����һ�������� 
    }
    return 0;
}


int Best_fit(List *list){
     int min = 0;            //��¼��С�����Ľ��Ĵ�С
     int base_min = 0;      //��¼��С�ڵ�Ľ�����ʼ��ַ
     List* temp = list;   
     datatype* item = input();  // Ҫ�� item ����ʼ��ַ�ͷ���״̬���г�ʼ��
    
    while (temp)
    {
         //�������δ�����Ҫ���бȽϲ��������Ҽ�¼��ֵ������id��
         if(temp->node->status == 0 && temp->node->id == -1&& temp->node->size > item->size){
             if(min == 0){          //����minΪ0 ��ʾ��δ�ҵ�һ�����Է���ķ���
                 min = temp->node->size;
                 base_min = temp->node->base;
             }
             else
             {
                 if(temp->node->size < min){      //�ҵ�һ��֮����Ҫ�ҳ���С�ķ���Ҳ��������size��С��
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

     //��Ϊ����û���κ�һ���ռ��������Ҫ����Ҫ��һ���жϴ���   
     temp = list;
     while (temp)
     {
         if(temp->node->base == base_min && temp->node->size > item->size){

            datatype* temp_node = (datatype*)malloc(sizeof(datatype));      //���ж���Ŀռ�����������Ҫ�ڽ���һ�������뵽������
            temp_node->id = -1;
            temp->node->status = 1;
            Last_visit_swap(list);
            temp_node->Last_visit=1;
            temp_node->status = 0;
            temp_node->base = base_min + item->size;
            temp_node->size = temp->node->size - item->size;

            temp->node = item;                          //��item���������ĳ�ʼ��
            temp->node->base = base_min;
            temp->node->status = 1;
            
            List* temp_list_node = (List*)malloc(sizeof(List));         //������һ������Ľ�㲢�ҳ�ʼ��
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
     int max = 0;            //��¼�������Ľ��Ĵ�С
     int base_max = 0;      //��¼���ڵ�Ľ�����ʼ��ַ
     List* temp = list;   
     datatype* item = input();  // Ҫ�� item ����ʼ��ַ�ͷ���״̬���г�ʼ��
    
     while (temp)
     {
         //�������δ�����Ҫ���бȽϲ��������Ҽ�¼��ֵ������id��
         if(temp->node->status == 0 && temp->node->id == -1&& temp->node->size > item->size){
             if(max == 0){          //����minΪ0 ��ʾ��δ�ҵ�һ�����Է���ķ���
                 max = temp->node->size;
                 base_max = temp->node->base;
             }
             else
             {
                 if(temp->node->size > max){      //�ҵ�һ��֮����Ҫ�ҳ����ķ���Ҳ��������size���
                     max = temp->node->size;
                     base_max = temp->node->base;
                 }
             }
         }
        temp = temp->next;
     }

     //��Ϊ����û���κ�һ���ռ��������Ҫ����Ҫ��һ���жϴ���   
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

            datatype* temp_node = (datatype*)malloc(sizeof(datatype));      //���ж���Ŀռ�����������Ҫ�ڽ���һ�������뵽������
            temp_node->id = -1;
            temp->node->status = 1;
            Last_visit_swap(list);
            temp_node->Last_visit=1;
            temp_node->status = 0;
            temp_node->base = base_max + item->size;
            temp_node->size = temp->node->size - item->size;

            temp->node = item;                          //��item���������ĳ�ʼ��
            temp->node->base = base_max;
            temp->node->status = 1;
            
            List* temp_list_node = (List*)malloc(sizeof(List));         //������һ������Ľ�㲢�ҳ�ʼ��
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
    datatype* item = input();              // Ҫ�� item ����ʼ��ַ�ͷ���״̬���г�ʼ��
    List* temp = list;                      //����һ����ʱ����list* ��ָ��list
    while (temp->node->Last_visit !=1) 
    	{
    		temp=temp->next;
		}
    while (temp)       //����Ϊ�� 
    {
        if(temp->node->status == 0 && temp->node->Last_visit ==1 &&temp->node->size > item->size){
		//�����ǰ�ķ���δ���䣬���ҷ�����С�����������Ĵ�С��ô��ʱ�Ϳ��Խ��з���
            List *front1 = temp->front;                                          //�洢��ǰδ��������� ��һ��������ַ
            List *next1 = temp->next;                                          //�洢��ǰδ���������  ��һ��������ַ   
            int base = temp->node->base;                                      //��¼δ���䵱ǰ�������׵�ַ

            datatype* new_node = (datatype*)malloc(sizeof(datatype));          // ��������Ĳ���Ҫ�½���һ������
            new_node->id = -1;                                                 //Ȼ����Ҫ������µķ�������һЩ��Ϣ������
            Last_visit_swap(list);
            new_node->Last_visit=1;
            new_node->id = -1;  
            new_node->size = temp->node->size - item->size;         //�·����Ĵ�С  ����  ��δ�����ʱ�ķ�����С - �������Ľ��Ĵ�С 
            temp->node = item;                                  //���������ķ��������з���
            temp->node->status = 1;
            new_node->status = 0;
            new_node->base = base + temp->node->size;             //�½����������׵�ַ��  �������ķ������׵�ַ + �������ķ����Ĵ�С

            List* temp_next = (List*)malloc(sizeof(List));           //��ʱ�ڵ� ������һ���µ�����ڵ� ��ʾ��һ��������  ���ҽ��г�ʼ��
            temp_next->node = new_node;                             //������һ���ķ�������Ϣ
            temp_next->front = temp_next->next = NULL;                                    

            if(front1 == NULL && next1 == NULL){                      //��� front��next�ڵ㶼�ǿգ��������ǵ�һ�η������
                temp->node->base = 0;                               //��ʼ���׵�ַ
                temp->next = temp_next;                     
                temp_next->front = temp;
            }
             if(front1 == NULL && next1 != NULL){                 //�ڵ�һ�������в����µķ���
                 temp->node->base = 0;
                 temp->node->status = 1;
                temp_next->front = temp;
                temp_next->next = temp->next;
                temp->next = temp_next;
             }
            if(front1 != NULL){                      //�������ǵ�һ�η���ڵ㣬��ʱ��Ҫ���м������һ���ڵ�
                temp->node->base = temp->front->node->base+temp->front->node->size;        //��ʼ���׵�ַ
                temp_next->next = temp->next;                                       //��֤�²���Ľڵ���¼ԭ�Ƚڵ����һ���ڵ���׵�ַ
                temp_next->front = temp;                               // ��β����Ҫ��֤
                temp->next = temp_next;                             //�����������ķ����ڵ����һ���ڵ�ָ��  ���Ǹոս�������ʱ�ڵ�
            }
            return 1;
        }   
        else if(temp->node->status == 0 && temp->node->size == item->size && temp->node->Last_visit ==1)
        {
            item->base = temp->front->node->base+temp->front->node->size;               //�²���������׵�ַ  ������һ�������� �׵�ַ+�����Ĵ�С
            item->status = 1;                                           //��ʾ�Ѿ�����
            temp->node = item;
            Last_visit_swap(list);
            Last_visit_swap1(list,temp);
            return 1;
        }
        temp = temp->next;    //��һ��������װ���£�����һ�������� 
        Last_visit_swap(list);
        Last_visit_swap1(list,temp);
    }
    return 0;
}
 

int Momory_recycle(List *list){             //���ձ� 
    List* temp = list;                      //����һ������ڵ� ָ��list ��ͷ�ڵ�
    int number;                             //���ڴ��Ҫ�ͷŵĽڵ�ķ�����
    char s2[5];
    InputBox(s2,5,"��������Ҫ���յ�ID�ţ�");
    number=atoi(s2);
    while (temp)
    {   
        if(temp->node->id == number)            //�����ҵ� �ڵ�id = number �Ľڵ㣬Ȼ�������������� 
        {   
            // һ�� Ҫ���յ��ǵ�һ�����
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
                // ��ʱ���ж� temp->next �Ƿ���ϵͳ�����һ�����
                // ��ʱֻ����ǰ�ڵ� ����һ�����ϲ��Ϳ�����
                //�� �׵�ַ���䣬   ����״̬ �� ����id���б仯  
                temp->node->size = temp->node->size + next->node->size;
                temp->node->status = 0;
                temp->node->id = -1;
                temp->next = next->next;
                if(next->next == NULL){
                    free(next);
                    return 1;
                }
                //����������һ�����Ļ��ͻ��һ������
                // �� next->next->front ָ����һ�����
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
              //���� ǰ��û�п��еķ���
            //��򵥣�   ֱ�Ӹı� ������ id �� ������״̬�Ϳ����ˡ�
            // ������յ�һ�������Ļ� ����Ҫ�Ƚ��д���������Ƚ��д��� ���ж� temp->front->node->id != -1 �ᱨһ���δ�����Ϊtemp-��front ��ʱָ�����null  
            if(temp->front->node->id != -1 && temp->front->node->status != 0 && temp->next->node->id != -1 && temp->next->node->status != 0){
                temp->node->status = 0;
                Last_visit_swap(list);
                temp->node->Last_visit=1;
                temp->node->id = -1;
                return 1;
            }

            //����Ҫ���յĽڵ�    ǰ��ͺ��涼�ǿ��е�
            // �������������ϲ���һ����ʼ��ַΪǰ��ķ�������ʼ��ַ�� ��СΪ������������С֮��
            //����Ҫ��һ���жϣ����
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
                //����������һ�����Ļ��ͻ��һ������
                // �� next->next->front ָ����һ�����
                else
                {
                    
                    next->next->front = front;
                    free(temp);  
                    return 1;
                }       
                return 1;
            }
            // �ġ� Ҫ���յĽڵ�  ǰ��Ľڵ��ǿ��е�
            //�ϲ���ķ�����ʼ��ַΪǰһ����㣬 ������СΪǰһ���ڵ� �� ��ǰ�ڵ�֮�͡�
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
            //�塢 Ҫ���յĽڵ�    ����Ķ�ڵ��ǿ��е�
            //�ϲ���ķ����׵�ַΪ��ǰ�ڵ� ��  ������СΪ��ǰ�ڵ� �� ��ǰ�ڵ����һ������С֮�͡�
            // �����Ҫ��һ�����裬 �ı������ id ��  ������״̬��
            // ��Ҫע��һ�㣺  ��Ҫ���յĿռ��Ǻ�  ϵͳ���Ŀ���������ʱ �� temp->next->next ָ�����null��

            if(temp->next->node->id == -1 && temp->next->node->status == 0){
                List* next = temp->next;
                // ��ʱ���ж� temp->next �Ƿ���ϵͳ�����һ�����
                // ��ʱֻ����ǰ�ڵ� ����һ�����ϲ��Ϳ�����
                //�� �׵�ַ���䣬   ����״̬ �� ����id���б仯  
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
                //����������һ�����Ļ��ͻ��һ������
                // �� next->next->front ָ����һ�����
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
    loadimage(&background,"C:\\Users\\xiao\\Desktop\\picture\\5.png",870,600,1);//��ͼƬ�ļ���ȡͼ��
    putimage(0, 0, &background);//����ͼ����Ļ��ͼƬ���Ͻ�����Ϊ(0,0)
    outtextxy(0, 0,"--------------------------------------��̬����ģ��--------------------------------------" );
    outtextxy(0, 40, "��ѡ��������еĲ�����");
    outtextxy(0, 80, "  �״���Ӧ�㷨      �����Ӧ�㷨      �´���Ӧ�㷨     ���ٷ����㷨");
    outtextxy(0, 140, "  ���Ӧ�㷨      �ڴ����          ��ʾ�ڴ�״��     �˳�");
    outtextxy(0, 200,"--------------------------------------��̬����ģ��--------------------------------------" );
    int  a=230;
    char b[5];
    while (temp)
    {
        if(temp->node->status == 0 && temp->node->id == -1){
        	outtextxy(0,a, "�����ţ�FREE");
        	outtextxy(150,a, "��ʼ��ַ��");
        	sprintf(b, " %d" , temp->node->base);
        	outtextxy(250,a,b);
        	outtextxy(300,a, "�յ��ַ��");
        	sprintf(b, " %d" , (temp->node->size) - 1);
        	outtextxy(400,a,b);
        	outtextxy(450,a, "�ڴ��С��");
        	sprintf(b, " %d" , temp->node->size);
        	outtextxy(550,a, b);
        	outtextxy(600,a, "����״̬������");
            a=a+30;
        }
        else
        {
        	outtextxy(0,a, "�����ţ�");
        	sprintf(b, " %d" , temp->node->id);
        	outtextxy(100,a, b);
        	// ------------------------------------ 
        	outtextxy(150,a, "��ʼ��ַ��");
        	sprintf(b, " %d" , temp->node->base);
        	outtextxy(250,a, b);
        	// ------------------------------------  
        	outtextxy(300,a, "�յ��ַ��");
        	sprintf(b, " %d" , (temp->node->size) - 1);
        	outtextxy(400,a, b);
        	// ------------------------------------  
        	outtextxy(450,a, "�ڴ��С��");
        	sprintf(b, " %d" , temp->node->size);
        	outtextxy(550,a, b);
        	outtextxy(600,a, "����״̬���ѷ���");
           a=a+30;
        }

        temp = temp->next;
    }

}

void dialog1(){
	HWND hWND = GetHWnd(); // ��ô��ھ��
    SetWindowText(hWND, "Sauce");
	MessageBox(hWND, "����ʧ��","��ʾ��", MB_OKCANCEL);
}

void dialog2(){
	HWND hWND = GetHWnd();
    SetWindowText(hWND, "Sauce");
	MessageBox(hWND,"����ɹ�","��ʾ��",  MB_OKCANCEL);
}

void dialog3(){
	HWND hWND = GetHWnd();
    SetWindowText(hWND, "Sauce");
	MessageBox(hWND,"���ճɹ�","��ʾ��",  MB_OKCANCEL);
}

void dialog4(){
	HWND hWND = GetHWnd();
    SetWindowText(hWND, "Sauce");
	MessageBox(hWND,"����ʧ��","��ʾ��",  MB_OKCANCEL);
}


int main(){
    List list = init();   //�������� 
    int select;           //��¼�㷨ѡ�� 
    int insert_state;     //��¼�״���Ӧ�㷨�����Ƿ�ɹ�------�Ƿ����ɹ� 
	int recycle_state;    //��¼�����Ƿ�ɹ� 
    int insert_state_best;//��¼�����Ӧ�㷨�����Ƿ�ɹ� 
    int insert_state_bad;//��¼���Ӧ�㷨�����Ƿ�ɹ� 
    int insert_state_Last;  //��¼�´������㷨�����Ƿ�ɹ� 
    int choice = -1;
	int size = 0;
	int x=1;
	char key;
	MOUSEMSG m;//���������� m
	initgraph(870, 600);   // ��ʼ��ͼ�ν��棨���崰�ڴ�С�� 
	settextstyle(20,0,"����");//�������ִ�С ��ʽ 
	setbkmode(TRANSPARENT);// ȥ�����ֱ���
    settextcolor(WHITE);   // �����ı���ɫ 
    IMAGE background;//����һ��ͼƬ��.
        loadimage(&background,"C:\\Users\\xiao\\Desktop\\picture\\5.png",870,600,1);//��ͼƬ�ļ���ȡͼ��
    	putimage(0, 0, &background);//����ͼ����Ļ��ͼƬ���Ͻ�����Ϊ(0,0)
        outtextxy(0, 0,"--------------------------------------��̬����ģ��--------------------------------------" );
        outtextxy(0, 40, "��ѡ��������еĲ�����");
        outtextxy(0, 80, "  �״���Ӧ�㷨      �����Ӧ�㷨      �´���Ӧ�㷨     ���ٷ����㷨");
        outtextxy(0, 140, "  ���Ӧ�㷨      �ڴ����          ��ʾ�ڴ�״��     �˳�");
        outtextxy(0, 200,"--------------------------------------��̬����ģ��--------------------------------------" );
    	while(x){
		m = GetMouseMsg();  // ��ȡһ�������Ϣ
		if(m.x>=1 && m.x<= 160 && m.y >=70&& m.y<=110){//�������λ�� �Ƿ��������� a
			setlinecolor(RED);//����� �����µı߿�Ϊ��ɫ
			rectangle(2,70,160,110);//���µı߿�
			//�������� ���
			if(m.uMsg==WM_LBUTTONDOWN){
                insert_state = First_fit(&list);
	            if(insert_state == 0){  // ʧ�� 
	                dialog1();
	            }
	            else {   // �ɹ� 
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
                InputBox(s1,5,"�����ڴ����㷨����ģ��\n1.��ʼ���ڴ����\n2.��ҵ���������ڴ�\n3.��ҵ�ͷ��ڴ�\n4.�鿴��ǰ�ڴ�ʹ�����\n0.�˳�ģ�����������ѡ��");
                choice=atoi(s1);  // ���ַ���ת������������һ������ 
				switch(choice)
				{
					case 1:				
						if(getRequestMem(size))
						{
							MemInit(size);
							cout << "��ʼ���ڴ�ɹ�" << endl;
						}
						else
						{
							cout << "��ʼ���ڴ�ʧ��" << endl;
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
						settextstyle(20,0,"����");//�������ִ�С ��ʽ 
						setbkmode(TRANSPARENT);// ȥ�����ֱ���
					    settextcolor(WHITE);
					    IMAGE background;//����һ��ͼƬ��.
				        loadimage(&background,"C:\\Users\\xiao\\Desktop\\picture\\5.png",870,600,1);//��ͼƬ�ļ���ȡͼ��
				    	putimage(0, 0, &background);//����ͼ����Ļ��ͼƬ���Ͻ�����Ϊ(0,0)
				        outtextxy(0, 0,"--------------------------------------��̬����ģ��--------------------------------------" );
				        outtextxy(0, 40, "��ѡ��������еĲ�����");
				        outtextxy(0, 80, "  �״���Ӧ�㷨      �����Ӧ�㷨      �´���Ӧ�㷨     ���ٷ����㷨");
				        outtextxy(0, 140, "  ���Ӧ�㷨      �ڴ����          ��ʾ�ڴ�״��     �˳�");
				        outtextxy(0, 200,"--------------------------------------��̬����ģ��--------------------------------------" );
						break; 
					}
				}
			}
		}
		//
		else if(m.x>=2 && m.x<= 160 && m.y >=130&& m.y<=170){//
			setlinecolor(RED);
			rectangle(2,130,160,170);
			//������г�����
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
		}else if(m.x>=360 && m.x<= 520 && m.y >=130&& m.y<=170){//��ʾ�ڴ� 
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
		//һ����겻����Ӧλ�� ��������ɫ�߿� ����֮ǰ�ĺ�ɫ�߿�
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
