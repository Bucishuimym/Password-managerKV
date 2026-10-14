// TODO: 这是一个库函数示例
/* =========================================================
 * DoubleList<DataType> —— 带头结点的双向链表模板类
 *
 * 结构：哨兵头结点 first（不存数据） → 第 1 个数据结点 → … → last
 *       first->next 才是第 1 个元素；last 指向最后一个数据结点，空表时为 nullptr。
 *       有了 last，尾插 / 从尾往前找都是 O(1) 起跳，不用再走一遍链表。
 *
 * 位置约定：位序 pos 从 1 开始
 *       Insert(pos, item)    pos ∈ [1, length + 1]，pos = length + 1 表示尾插
 *       DeleteByPos(pos, out) pos ∈ [1, length]
 *   注意：myKV 里的顺序表模板用的是 0 基下标（Insert(list, count, i, item) 的 i ∈ [0, count]），
 *   接过来的时候记得把下标 +1 / -1。
 *
 * 用法：模板的实现必须跟着类一起走，所以这个文件是给别的 .cpp #include 的。
 *       在 VS 里也可以直接把它改名成 Dlist.h 再包含（工程里只有这一个文件，
 *       改完记得在"解决资源管理器"里同步文件名）。
 * ========================================================= */
#include "pch.h"
#include "framework.h"

#pragma once

#include <iostream>
#include <string>
#include <utility>
#include "DLinkedList.h"
 /* ---------------------------------------------------------
  * 字段适配层
  *
  * 链表要按 UID、按名字查，但模板不知道 DataType 里的字段叫什么名字，中间加一层翻译：
  * 默认约定 DataType 提供 GetUid() / GetName()（kvDoubleTry 里 UserGroup / pwDataGroup 的写法）。
  * 如果存的是结构体（字段直接叫 uid / name，像 myKV 里的 User / PasswordItem），
  * 就特化这个模板把字段名对上去，写法见文件末尾的示例。
  * 用不到按字段查的函数时（比如 DoubleList<int>）不特化也不影响编译——
  * 模板函数是"用到才实例化"的。
  * --------------------------------------------------------- */


/* =========================================================
 * 私有工具
 * ========================================================= */
template <typename DataType>
typename DoubleList<DataType>::Node* DoubleList<DataType>::NodeAt(int pos) const
{
    if (pos < 1 || pos > length)
        return nullptr;

    if (pos <= length / 2) {            // 位序落在前半段：从头往后走
        Node* p = first;
        for (int i = 1; i <= pos; i++)
            p = p->next;
        return p;
    }

    Node* p = last;                     // 后半段：从尾往前走
    for (int i = length; i > pos; i--)
        p = p->prev;
    return p;
}

template <typename DataType>
void DoubleList<DataType>::Unlink(Node* p)
{
    p->prev->next = p->next;            // p->prev 至少是哨兵，不会为空
    if (p->next != nullptr)
        p->next->prev = p->prev;

    length--;

    // 摘掉的是尾巴时要挪 last：删空了 last 归 nullptr，否则退回到前一个结点。
    // 少这一句就会出现 last 悬空（指向已经 delete 掉的结点），后面尾插直接崩。
    if (p->next == nullptr)
        last = (length == 0) ? nullptr : p->prev;
}

/* =========================================================
 * 构造 / 析构 / 拷贝 / 移动
 * ========================================================= */
template <typename DataType>
DoubleList<DataType>::DoubleList(int maxSize)
{
    first = new Node();                 // 造哨兵
    last = nullptr;                     // 空表：尾是空指针
    length = 0;                         // 老版本漏了这两句，last / length 里是垃圾值
    this->maxSize = maxSize;            // 形参和成员同名，要写 this->
}

template <typename DataType>
DoubleList<DataType>::DoubleList(const DataType a[], int n, int maxSize)
    : DoubleList(maxSize)               // 委托构造：先把哨兵建好，再逐个尾插
{
    for (int i = 0; i < n; i++)
        PushBack(a[i]);                 // 超过 maxSize 的自动丢掉，不会越界
}

template <typename DataType>
DoubleList<DataType>::DoubleList(const DoubleList& other)
    : DoubleList(other.maxSize)
{
    for (Node* p = other.first->next; p != nullptr; p = p->next)
        PushBack(p->data);              // 深拷贝：两边各有一份结点，析构时不会双重释放
}

template <typename DataType>
DoubleList<DataType>& DoubleList<DataType>::operator=(const DoubleList& other)
{
    if (this == &other)                 // 自赋值不判，会先把自己清空再去读 other
        return *this;

    Clear();
    maxSize = other.maxSize;
    for (Node* p = other.first->next; p != nullptr; p = p->next)
        PushBack(p->data);
    return *this;
}

template <typename DataType>
DoubleList<DataType>::DoubleList(DoubleList&& other) noexcept
    : first(other.first), last(other.last), length(other.length), maxSize(other.maxSize)
{
    other.first = new Node();           // 被搬走的对象要还回一张合法的空表
    other.last = nullptr;
    other.length = 0;
}

template <typename DataType>
DoubleList<DataType>& DoubleList<DataType>::operator=(DoubleList&& other) noexcept
{
    if (this == &other)
        return *this;

    Clear();                            // 先放掉自己原来的结点
    delete first;                       // 连旧哨兵一起换掉

    first = other.first;
    last = other.last;
    length = other.length;
    maxSize = other.maxSize;

    other.first = new Node();
    other.last = nullptr;
    other.length = 0;
    return *this;
}

template <typename DataType>
DoubleList<DataType>::~DoubleList()
{
    Clear();
    delete first;                       // 哨兵也要还
    first = nullptr;
}

/* =========================================================
 * 增
 * ========================================================= */
template <typename DataType>
bool DoubleList<DataType>::PushBack(const DataType& item)
{
    if (Full())
        return false;

    Node* s = new Node(item);           // 新结点的 next 在构造函数里已经是 nullptr 了
    if (last != nullptr) {
        s->prev = last;                 // 非空表：接在原来的尾巴后面
        last->next = s;
    }
    else {
        s->prev = first;                // 空表：它同时是第 1 个结点，前驱要记成哨兵，
        first->next = s;                // 不能是 nullptr——不然删它的时候 p->prev->next 就炸了
    }
    last = s;

    length++;
    return true;
}

template <typename DataType>
bool DoubleList<DataType>::PushFront(const DataType& item)
{
    if (Full())
        return false;

    Node* s = new Node(item);
    s->prev = first;
    s->next = first->next;
    if (first->next != nullptr)
        first->next->prev = s;
    else
        last = s;                       // 本来空表，它也是尾巴
    first->next = s;

    length++;
    return true;
}

template <typename DataType>
bool DoubleList<DataType>::Insert(int pos, const DataType& item)
{
    if (Full())
        return false;
    if (pos < 1 || pos > length + 1)    // 合法位序是 1 .. length+1，尾插也算
        return false;
    if (pos == length + 1)
        return PushBack(item);          // 尾插单独走，省得再判一次空表
    if (pos == 1)
        return PushFront(item);

    Node* p = NodeAt(pos);              // pos ∈ [1, length]，一定找得到
    Node* s = new Node(item);
    s->prev = p->prev;
    s->next = p;
    p->prev->next = s;                  // 插在 p 前面：先接好两边，再改 p->prev
    p->prev = s;

    length++;
    return true;
}

/* =========================================================
 * 删
 * ========================================================= */
template <typename DataType>
bool DoubleList<DataType>::DeleteByPos(int pos, DataType& out)
{
    Node* p = NodeAt(pos);              // 位序非法时给的是 nullptr
    if (p == nullptr)
        return false;

    out = p->data;                      // 先把数据拷出来再 delete
    Unlink(p);
    delete p;
    return true;
}

template <typename DataType>
bool DoubleList<DataType>::DeleteByUid(int uid, DataType& out)
{
    Node* p = LocateByUid(uid);
    if (p == nullptr)
        return false;

    out = p->data;
    Unlink(p);
    delete p;
    return true;
}

template <typename DataType>
bool DoubleList<DataType>::DeleteByName(const std::string& name, DataType& out)
{
    Node* p = LocateByName(name);
    if (p == nullptr)
        return false;

    out = p->data;
    Unlink(p);
    delete p;
    return true;
}

template <typename DataType>
int DoubleList<DataType>::DeleteAllByUid(int uid)
{
    // 注销用户时用它一次清掉该用户的所有缓存数据，不用在调用方写 for + 手动跳过
    return RemoveIf([uid](const DataType& d) { return ListKey<DataType>::Uid(d) == uid; });
}

template <typename DataType>
template <typename Pred>
int DoubleList<DataType>::RemoveIf(Pred pred)
{
    int removed = 0;
    Node* p = first->next;
    while (p != nullptr) {
        Node* nextNode = p->next;       // p 可能要被删掉，先存下下一个，不然就是野指针
        if (pred(p->data)) {
            Unlink(p);
            delete p;
            removed++;
        }
        p = nextNode;
    }
    return removed;
}

template <typename DataType>
bool DoubleList<DataType>::PopBack(DataType& out)
{
    return DeleteByPos(length, out);    // 空表时 length 是 0，位序非法 → 返回 false
}

template <typename DataType>
bool DoubleList<DataType>::PopFront(DataType& out)
{
    return DeleteByPos(1, out);
}

template <typename DataType>
void DoubleList<DataType>::Clear()
{
    Node* p = first->next;
    while (p != nullptr) {
        Node* nextNode = p->next;
        delete p;
        p = nextNode;
    }
    first->next = nullptr;              // 哨兵留着，链表回到空表状态
    last = nullptr;
    length = 0;
}

/* =========================================================
 * 查
 * ========================================================= */
template <typename DataType>
typename DoubleList<DataType>::Node* DoubleList<DataType>::LocateByUid(int uid) const
{
    for (Node* p = first->next; p != nullptr; p = p->next) {
        if (ListKey<DataType>::Uid(p->data) == uid)
            return p;
    }
    return nullptr;                     // 找不到给空指针，调用方判空
}

template <typename DataType>
typename DoubleList<DataType>::Node* DoubleList<DataType>::LocateByName(const std::string& name) const
{
    for (Node* p = first->next; p != nullptr; p = p->next) {
        if (ListKey<DataType>::Name(p->data) == name)
            return p;
    }
    return nullptr;
}

template <typename DataType>
template <typename Pred>
typename DoubleList<DataType>::Node* DoubleList<DataType>::FindIf(Pred pred) const
{
    for (Node* p = first->next; p != nullptr; p = p->next) {
        if (pred(p->data))
            return p;
    }
    return nullptr;
}

template <typename DataType>
bool DoubleList<DataType>::Get(int pos, DataType& out) const
{
    Node* p = NodeAt(pos);
    if (p == nullptr)
        return false;

    out = p->data;
    return true;
}

/* =========================================================
 * 表状态
 * ========================================================= */
template <typename DataType>
int DoubleList<DataType>::Length() const
{
    return length;                      // 老版本是走一遍链表现数的，O(n)；维护好 length 就是 O(1)
}

template <typename DataType>
bool DoubleList<DataType>::Empty() const
{
    return length == 0;
}

template <typename DataType>
bool DoubleList<DataType>::Full() const
{
    return maxSize >= 0 && length >= maxSize;
}

template <typename DataType>
int DoubleList<DataType>::MaxSize() const
{
    return maxSize;
}

/* =========================================================
 * 遍历
 * ========================================================= */
template <typename DataType>
typename DoubleList<DataType>::Node* DoubleList<DataType>::Begin() const
{
    return first->next;                 // 空表返回 nullptr，配合 AtEnd 正好不进循环
}

template <typename DataType>
typename DoubleList<DataType>::Node* DoubleList<DataType>::Next(Node* p) const
{
    return (p == nullptr) ? nullptr : p->next;
}

template <typename DataType>
typename DoubleList<DataType>::Node* DoubleList<DataType>::Prev(Node* p) const
{
    if (p == nullptr || p->prev == first)
        return nullptr;                 // 走到第 1 个结点就停，不放哨兵出去
    return p->prev;
}

template <typename DataType>
typename DoubleList<DataType>::Node* DoubleList<DataType>::Last() const
{
    return last;
}

template <typename DataType>
bool DoubleList<DataType>::AtEnd(Node* p) const
{
    return p == nullptr;
}

template <typename DataType>
template <typename Func>
void DoubleList<DataType>::ForEach(Func f) const
{
    for (Node* p = first->next; p != nullptr; p = p->next)
        f(p->data);                     // f 里只读不改，改链表会让 p 失效
}

/* =========================================================
 * 其他
 * ========================================================= */
template <typename DataType>
void DoubleList<DataType>::Reverse()
{
    if (length < 2)
        return;                         // 空表 / 只有一个结点，不用翻

    Node* oldHead = first->next;
    Node* oldTail = last;

    for (Node* p = oldHead; p != nullptr; ) {
        Node* nextNode = p->next;
        std::swap(p->next, p->prev);    // 每个数据结点的两个指针互换，链子的方向就反过来了
        p = nextNode;
    }

    // 互换之后：原来的头结点指回了哨兵，原来的尾结点 prev 是空的，得手工接回来
    oldHead->next = nullptr;            // 它现在是新尾巴
    oldTail->prev = first;              // 它现在是新头，前面接哨兵
    first->next = oldTail;
    last = oldHead;
}

template <typename DataType>
void DoubleList<DataType>::PrintListFirst() const
{
    for (Node* p = first->next; p != nullptr; p = p->next)
        std::cout << p->data << " ";
    std::cout << std::endl;
}

template <typename DataType>
void DoubleList<DataType>::PrintListNext() const
{
    // 反向打印直接拿 last 起步（老版本先从头走到尾找尾结点，白跑一趟）
    // 走 Prev() 而不是 p->prev：第 1 个结点的前驱是哨兵，直接跟下去会把哨兵也打出来
    for (Node* p = last; p != nullptr; p = Prev(p))
        std::cout << p->data << " ";
    std::cout << std::endl;
}

/* =========================================================
 * 示例：payload 是结构体时怎么把字段名对上去
 * （把下面的 User / PasswordItem 换成你实际的类型，放到第一次用到链表之前）
 * =========================================================
 *
 * struct User { std::string name; std::string key; int uid; int rule; };
 * struct PasswordItem { std::string Username; std::string key; std::string URL; int UID; };
 *
 * template <> struct ListKey<User> {
 *     static int         Uid (const User& u) { return u.uid; }
 *     static std::string Name(const User& u) { return u.name; }
 * };
 * template <> struct ListKey<PasswordItem> {
 *     static int         Uid (const PasswordItem& p) { return p.UID; }
 *     static std::string Name(const PasswordItem& p) { return p.Username; }
 * };
 *
 * 用：
 *     DoubleList<User> users;                    // 不限容量
 *     users.PushBack(u);
 *     users.LocateByName("admin");               // 返回 Node*
 *     users.DeleteByUid(9001, out);              // out 是被删掉的那条
 *     users.DeleteAllByUid(9001);                // 级联清掉该用户全部数据
 *
 *     // 遍历（业务层的打印 / 落盘就走这条）
 *     for (auto* p = users.Begin(); !users.AtEnd(p); p = users.Next(p))
 *         std::cout << p->data.name << std::endl;
 */