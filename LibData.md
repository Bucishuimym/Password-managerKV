

# 可供调用的函数接口

## 公开接口（public）
```cpp

    explicit DoubleList(int maxSize = -1);                                     // 构造：可选限制最大容量，默认不限制
    DoubleList(const DataType a[], int n, int maxSize = -1);                   // 从数组构造，按顺序将数组元素尾插入链表
    DoubleList(const DoubleList& other);                                       // 拷贝构造：深拷贝所有结点数据
    DoubleList& operator=(const DoubleList& other);                            // 拷贝赋值：清空当前并拷贝 other 的数据 
    DoubleList(DoubleList&& other) noexcept;                                   // 移动构造：接管 other 的内部资源
    DoubleList& operator=(DoubleList&& other) noexcept;                        // 移动赋值：释放当前资源并接管 other
    ~DoubleList();                                                             // 析构：释放所有结点及哨兵

    bool PushBack(const DataType& item);                                       // 在链表尾部插入元素，成功返回 true（受 maxSize 限制）    
    bool PushFront(const DataType& item);                                      // 在链表头部（第 1 位）插入元素，成功返回 true
    bool Insert(int pos, const DataType& item);                                // 在位序 pos（1 基）处插入元素，pos 可为 length+1 表示尾插
    bool DeleteByPos(int pos, DataType& out);                                  // 删除位于 pos 的元素并通过 out 返回被删除的数据，成功返回 true        
    bool DeleteByUid(int uid, DataType& out);                                  // 删除首个匹配给定 uid 的元素并通过 out 返回，成功返回 true
    bool DeleteByName(const std::string& name, DataType& out);                 // 删除首个匹配给定 name 的元素并通过 out 返回，成功返回 true
    int  DeleteAllByUid(int uid);                                              // 删除所有匹配 uid 的元素，返回删除的条目数（级联删除）

    template <typename Pred>
    int  RemoveIf(Pred pred);                                                  // 模板方法：对每个元素调用 pred(data)，若为真则删除，返回删除数量
    bool PopBack(DataType& out);                                               // 弹出并返回尾部元素，成功返回 true
    bool PopFront(DataType& out);                                              // 弹出并返回头部元素，成功返回 true
    void Clear();                                                              // 清空所有数据结点，但保留哨兵头结点，重置 length 与 last                                     

    Node* LocateByUid(int uid) const;                                          // 查找首个 uid 匹配的结点指针，找不到返回 nullptr
    Node* LocateByName(const std::string& name) const;                         // 查找首个 name 匹配的结点指针，找不到返回 nullptr
    template <typename Pred>
    Node* FindIf(Pred pred) const;                                             // 通用查找：若 pred(data) 为真则返回该结点指针，否则 nullptr
    bool  Get(int pos, DataType& out) const;                                   // 按位序获取元素值到 out，成功返回 true（链表访问为 O(n)）             
    int  Length() const;                                                       // 返回当前元素个数，O(1)                            
    bool Empty() const;                                                        // 判断是否为空
    bool Full() const;                                                         // 判断是否已达容量上限（当 maxSize >= 0 时）
    int  MaxSize() const;                                                      // 返回配置的最大容量（-1 表示不限制）

    Node* Begin() const;                                                       // 返回第 1 个数据结点指针；空表返回 nullptr                 
    Node* Next(Node* p) const;                                                 // 返回 p 的下一个结点指针（或 nullptr）
    Node* Prev(Node* p) const;                                                 // 返回 p 的上一个数据结点指针；到头返回 nullptr（不会暴露哨兵）                          
    Node* Last() const;                                                        // 返回尾部最后一个数据结点指针；空表返回 nullptr                                 
    bool  AtEnd(Node* p) const;                                                // 判断结点指针是否到达末尾（nullptr 视为到尾）                         

    template <typename Func>
    void  ForEach(Func f) const;                                               // 对每个数据结点执行函数 f（只读遍历，f 不应修改链表结构）                          
    void Reverse();                                                            // 就地反转链表方向（不改变数据，仅调整指针）                                     
    void PrintListFirst() const;                                               // 从头到尾打印链表内容（调试用）                       
    void PrintListNext() const;                                                // 从尾到头打印链表内容（调试用） 

```
## 私有接口（private）

```cpp

    Node* first;        // 哨兵头结点
    Node* last;         // 最后一个数据结点，空表为 nullptr
    int   length;       // 数据结点个数
    int   maxSize;      // 容量上限，-1 表示不限

    Node* NodeAt(int pos) const;                                               // 根据位序 pos（1 基）返回对应的数据结点指针；pos 非法时返回 nullptr
    void  Unlink(Node* p);                                                     // 从链上摘除结点 p（不释放内存），并更新 length 与 last 指针

```

---

## 字段适配器

```cpp

struct ListKey
{
    static int Uid(const DataType& d) { return d.GetUid(); }                   // 返回数据项的唯一整数标识（UID），默认调用 DataType::GetUid()
    static std::string Name(const DataType& d) { return d.GetName(); }         // 返回数据项的名字字符串，默认调用 DataType::GetName()
};

```