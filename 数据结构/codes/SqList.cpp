/*

  Name: 线性表, 顺序存储结构

  Author: ycw

  Date: 09-02-23 13:12

  Description:

  实现了线性表的顺序表示:用一组地址连续的存储单元依次存储线性表的数据元素(实质上就是数组)

 */

  

#include <stdio.h>

#include <stdlib.h>

#include <iostream>

  

using namespace std;

  

#define LIST_INIT_SIZE 100  // 线性表存储空间的初始分配量

#define LISTINCREMENT 10    // 线性表存储空间每次扩容长度

#define ElemType int        // 线性表存储元素类型

#define Status int          // 方法返回值定义

#define OK 1

#define ERROR 0

  

typedef struct {

    ElemType* elem;         // 存储空间基地址

    int length;    // 线性表当前存储元素的长度

    int listsize;  // 线性表当前存储容量

} SqList;

  

// 构造一个空线性表

Status InitList(SqList& L) {

    L.elem = (ElemType*)malloc(LIST_INIT_SIZE * sizeof(ElemType));

    if (!L.elem) {

        // 容量分配失败, 抛出异常

        return 0;

    }

    // 初始化线性表的属性

    L.length = 0;

    L.listsize = LIST_INIT_SIZE;

    return 1;

}

  

// 在顺序线性表L 中第 i 个位置[之前]插人新的元素e,

Status ListInsert(SqList& L, int i, ElemType e) {

    // i 的合法值 [1, n + 1], n指元素个数

    if (i < 1 || i > L.length + 1) {

        // 比如在第n个元素之前: 插入位置在 n - 1 与 n 之间

        // 在第n + 1个元素之前: 插入位置在 n 与 n + 1 之间, 虽然压根没有n + 1

        // 这个元素 使用[之前]描述的原因推测: 可以在列表末尾插入元素

        return 0;

    }

    // 是否扩容

    if (L.length >= L.listsize) {

        ElemType* newBase = (ElemType*)realloc(

            L.elem, (L.listsize + LISTINCREMENT) * sizeof(ElemType));

        if (!newBase) {

            return 0;  // 扩容失败

        }

        L.elem = newBase;

    }

    ElemType* q = &(L.elem[i - 1]);  // q为插入位置

    ElemType* p = &(L.elem[L.length - 1]);  // p为线性表中最后一个元素的地址

    for (; p >= q; p--) {

        // 倒序原因: 不需要保存下一个元素

        *(p + 1) = *p;  // 插入元素, 指针后移

    }

    *q = e;

    L.length++;

    return 1;

}

  

// 在顺序线性表L中删除第i个元素，并用e 返回其值

Status ListDelete(SqList& L, int i, ElemType& e) {

    // i 的合法值为1~i~ListLength

    if (i < 1 || i > L.length) {

        return ERROR;

    }

    // 用了数组, 还要取指针？？？我不理解

    //  ElemType *p = &(L.elem[i - 1]);      // 被删除的元素

    //  ElemType *q = L.elem + L.length - 1; // 末尾元素

    //  e = *p;

    //  for(;p < q; p++) {

    //      *(p - 1) = *p;

    //  }

    e = L.elem[i - 1];

    for (int j = i - 1; j < L.length; j++) {

        L.elem[j - 1] = L.elem[j];

    }

    L.length--;

    return OK;

}

  

int LocateElem(SqList& L, ElemType e, Status (*compare)(ElemType, ElemType)) {

    // 在[顺序线性表中查找第1个值与e满足comapre()的元素的次序]

    // 若找到，返回其在L中的位序， 否则返回0

    int i = 1;

    ElemType* p = L.elem;

    while (i <= L.length && (*compare)(*p, e)) {

        i++;

        p++;

    }

    if (i > L.length) {

        return 0;

    }

    return i;

}

  

// 合并两个顺序线性表到第三个线性表中

void MergeList(SqList& La, SqList& Lb, SqList& Lc) {

    ElemType *pa = La.elem;

    ElemType *pb = Lb.elem;

  

    int la_len = La.length;

    int lb_len = Lb.length;

    // 因为Lc是返回列表，那之前的值就不用管了

    Lc.elem = (ElemType *)malloc((la_len + lb_len) * sizeof(ElemType));

    if (!Lc.elem) {

        cout << "MergeList(): 分配空间失败\n";

        // throw REALLOC Exception in MergeList

        return;

    }

    ElemType *pc = Lc.elem;

    Lc.listsize = la_len + lb_len;

    Lc.length = Lc.listsize;

    ElemType *pa_last = La.elem + La.length - 1;

    ElemType *pb_last = Lb.elem + Lb.length - 1;

    while (pa <= pa_last && pb <= pb_last) {

        if (*pa <= *pb) {

            *pc++ = *pa++;

        } else {

            *pc++ = *pb++;

        }

    }

    while (pa <= pa_last) {

        *pc++ = *pa++;

    }

    while (pb <= pb_last) {

        *pc++ = *pb++;

    }

}

  

// 打印顺序表

void PrintList(SqList& L) {

    for (int i = 0; i < L.length; i++) {

        cout << L.elem[i] << " ";

    }

}

  

int main() {

    cout << "开始SqList\n";

    SqList La;

    InitList(La);

    SqList Lb;

    InitList(Lb);

    SqList Lc;

    InitList(Lc);

    for (int i = 0; i < 5; i++) {

        cin >> La.elem[i];

        La.length++;

    }

    for (int i = 0; i < 5; i++) {

        cin >> Lb.elem[i];

        Lb.length++;

    }

    PrintList(La);

    PrintList(Lb);

    MergeList(La, Lb, Lc);

    cout << "打印了\n";

  

    PrintList(Lc);

    return 0;

}