#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

// ============================================================================
// FOUNDATIONAL NON-STL PRIMITIVES: MYSTRING & MYVECTOR
// Handcrafted without <string>, <vector>, <algorithm>, <sstream>, <queue>, etc.
// ============================================================================

// Custom helper string length
static int myStrLen(const char* s) {
    if (!s) return 0;
    int len = 0;
    while (s[len] != '\0') len++;
    return len;
}

static char myToLower(char c) {
    if (c >= 'A' && c <= 'Z') return (char)(c + ('a' - 'A'));
    return c;
}

template <typename T>
static void mySwap(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

// ----------------------------------------------------------------------------
// Custom String Class (Manual dynamic memory management)
// ----------------------------------------------------------------------------
class MyString {
private:
    char* data;
    int len;

public:
    MyString() : data(nullptr), len(0) {
        data = new char[1];
        data[0] = '\0';
    }

    MyString(const char* s) {
        if (!s) {
            len = 0;
            data = new char[1];
            data[0] = '\0';
        } else {
            len = myStrLen(s);
            data = new char[len + 1];
            for (int i = 0; i < len; i++) data[i] = s[i];
            data[len] = '\0';
        }
    }

    MyString(const MyString& other) {
        len = other.len;
        data = new char[len + 1];
        for (int i = 0; i <= len; i++) data[i] = other.data[i];
    }

    ~MyString() {
        delete[] data;
    }

    MyString& operator=(const MyString& other) {
        if (this != &other) {
            delete[] data;
            len = other.len;
            data = new char[len + 1];
            for (int i = 0; i <= len; i++) data[i] = other.data[i];
        }
        return *this;
    }

    MyString& operator=(const char* s) {
        delete[] data;
        if (!s) {
            len = 0;
            data = new char[1];
            data[0] = '\0';
        } else {
            len = myStrLen(s);
            data = new char[len + 1];
            for (int i = 0; i < len; i++) data[i] = s[i];
            data[len] = '\0';
        }
        return *this;
    }

    int length() const { return len; }
    bool empty() const { return len == 0; }
    const char* c_str() const { return data ? data : ""; }

    char operator[](int idx) const {
        if (idx < 0 || idx >= len) return '\0';
        return data[idx];
    }

    char& operator[](int idx) {
        return data[idx];
    }

    bool operator==(const MyString& other) const {
        if (len != other.len) return false;
        for (int i = 0; i < len; i++) {
            if (data[i] != other.data[i]) return false;
        }
        return true;
    }

    bool operator==(const char* other) const {
        if (!other) return len == 0;
        int oLen = myStrLen(other);
        if (len != oLen) return false;
        for (int i = 0; i < len; i++) {
            if (data[i] != other[i]) return false;
        }
        return true;
    }

    bool operator!=(const MyString& other) const { return !(*this == other); }
    bool operator!=(const char* other) const { return !(*this == other); }

    bool operator<(const MyString& other) const {
        int minL = len < other.len ? len : other.len;
        for (int i = 0; i < minL; i++) {
            if (data[i] < other.data[i]) return true;
            if (data[i] > other.data[i]) return false;
        }
        return len < other.len;
    }

    MyString operator+(const MyString& other) const {
        int newLen = len + other.len;
        char* buf = new char[newLen + 1];
        for (int i = 0; i < len; i++) buf[i] = data[i];
        for (int i = 0; i < other.len; i++) buf[len + i] = other.data[i];
        buf[newLen] = '\0';
        MyString res(buf);
        delete[] buf;
        return res;
    }

    MyString operator+(const char* other) const {
        int oLen = myStrLen(other);
        int newLen = len + oLen;
        char* buf = new char[newLen + 1];
        for (int i = 0; i < len; i++) buf[i] = data[i];
        for (int i = 0; i < oLen; i++) buf[len + i] = other[i];
        buf[newLen] = '\0';
        MyString res(buf);
        delete[] buf;
        return res;
    }

    MyString& operator+=(const MyString& other) {
        *this = *this + other;
        return *this;
    }

    MyString& operator+=(const char* other) {
        *this = *this + other;
        return *this;
    }

    MyString& operator+=(char c) {
        char s[2] = {c, '\0'};
        *this = *this + s;
        return *this;
    }

    int find(const char* sub, int startPos = 0) const {
        if (!sub) return -1;
        int subLen = myStrLen(sub);
        if (subLen == 0) return startPos;
        for (int i = startPos; i <= len - subLen; i++) {
            bool match = true;
            for (int j = 0; j < subLen; j++) {
                if (data[i + j] != sub[j]) {
                    match = false;
                    break;
                }
            }
            if (match) return i;
        }
        return -1;
    }

    int find(char c, int startPos = 0) const {
        for (int i = startPos; i < len; i++) {
            if (data[i] == c) return i;
        }
        return -1;
    }

    MyString substr(int start, int subLen = -1) const {
        if (start < 0 || start >= len) return MyString("");
        if (subLen < 0 || start + subLen > len) {
            subLen = len - start;
        }
        char* buf = new char[subLen + 1];
        for (int i = 0; i < subLen; i++) buf[i] = data[start + i];
        buf[subLen] = '\0';
        MyString res(buf);
        delete[] buf;
        return res;
    }

    static MyString fromInt(int n) {
        if (n == 0) return MyString("0");
        char buf[32];
        int idx = 0;
        bool neg = false;
        if (n < 0) {
            neg = true;
            n = -n;
        }
        while (n > 0) {
            buf[idx++] = (char)('0' + (n % 10));
            n /= 10;
        }
        if (neg) buf[idx++] = '-';
        // reverse
        for (int i = 0; i < idx / 2; i++) {
            char t = buf[i];
            buf[i] = buf[idx - 1 - i];
            buf[idx - 1 - i] = t;
        }
        buf[idx] = '\0';
        return MyString(buf);
    }

    static MyString fromDouble(double d, int precision = 2) {
        if (precision <= 0) {
            int rounded = (int)(d >= 0 ? (d + 0.5) : (d - 0.5));
            return fromInt(rounded);
        }
        int integerPart = (int)d;
        double frac = d - integerPart;
        if (frac < 0) frac = -frac;
        MyString s = fromInt(integerPart);
        s += ".";
        for (int i = 0; i < precision; i++) {
            frac *= 10;
            int digit = (int)frac;
            s += (char)('0' + digit);
            frac -= digit;
        }
        return s;
    }
};

// ----------------------------------------------------------------------------
// Custom Vector Template (Manual dynamic array with doubling resize)
// ----------------------------------------------------------------------------
template <typename T>
class MyVector {
private:
    T* arr;
    int cap;
    int len;

    void resize(int newCap) {
        T* newArr = new T[newCap];
        for (int i = 0; i < len; i++) {
            newArr[i] = arr[i];
        }
        delete[] arr;
        arr = newArr;
        cap = newCap;
    }

public:
    MyVector() : arr(nullptr), cap(4), len(0) {
        arr = new T[cap];
    }

    MyVector(int initialCap) : arr(nullptr), cap(initialCap > 0 ? initialCap : 4), len(0) {
        arr = new T[cap];
    }

    MyVector(const MyVector& other) {
        cap = other.cap;
        len = other.len;
        arr = new T[cap];
        for (int i = 0; i < len; i++) arr[i] = other.arr[i];
    }

    ~MyVector() {
        delete[] arr;
    }

    MyVector& operator=(const MyVector& other) {
        if (this != &other) {
            delete[] arr;
            cap = other.cap;
            len = other.len;
            arr = new T[cap];
            for (int i = 0; i < len; i++) arr[i] = other.arr[i];
        }
        return *this;
    }

    int size() const { return len; }
    bool empty() const { return len == 0; }
    void clear() { len = 0; }

    void push_back(const T& val) {
        if (len == cap) {
            resize(cap * 2);
        }
        arr[len++] = val;
    }

    void pop_back() {
        if (len > 0) len--;
    }

    T& operator[](int idx) {
        return arr[idx];
    }

    const T& operator[](int idx) const {
        return arr[idx];
    }

    T& back() {
        return arr[len - 1];
    }

    const T& back() const {
        return arr[len - 1];
    }

    void reverse() {
        for (int i = 0; i < len / 2; i++) {
            mySwap(arr[i], arr[len - 1 - i]);
        }
    }
};

// ============================================================================
// 1. DATA STRUCTURE: CUSTOM SINGLY LINKED LIST (AUDIT LOG TRAIL)
// ============================================================================
struct LogNode {
    int logId;
    MyString timestamp;
    MyString eventType;
    MyString details;
    LogNode* next;

    LogNode(int id, const MyString& ts, const MyString& type, const MyString& det)
        : logId(id), timestamp(ts), eventType(type), details(det), next(nullptr) {}
};

class AuditLinkedList {
private:
    LogNode* head;
    LogNode* tail;
    int count;

public:
    AuditLinkedList() : head(nullptr), tail(nullptr), count(0) {}

    ~AuditLinkedList() {
        LogNode* curr = head;
        while (curr) {
            LogNode* temp = curr;
            curr = curr->next;
            delete temp;
        }
    }

    void append(const MyString& ts, const MyString& type, const MyString& det) {
        count++;
        LogNode* newNode = new LogNode(count, ts, type, det);
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    MyString toJSON(int limit = 40) const {
        MyVector<LogNode*> nodes;
        LogNode* curr = head;
        while (curr) {
            nodes.push_back(curr);
            curr = curr->next;
        }

        MyString json = "[";
        int start = nodes.size() - limit;
        if (start < 0) start = 0;
        bool first = true;
        for (int i = nodes.size() - 1; i >= start; i--) {
            if (!first) json += ",";
            first = false;
            json += "{\"id\":";
            json += MyString::fromInt(nodes[i]->logId);
            json += ",\"timestamp\":\"";
            json += nodes[i]->timestamp;
            json += "\",\"type\":\"";
            json += nodes[i]->eventType;
            json += "\",\"details\":\"";
            json += nodes[i]->details;
            json += "\"}";
        }
        json += "]";
        return json;
    }

    int size() const { return count; }
};

// ============================================================================
// 2. DATA STRUCTURE: CUSTOM STACK (UNDO OPERATIONS)
// ============================================================================
struct UndoRecord {
    int orderId;
    MyString sku;
    MyString warehouseId;
    int quantity;
    MyString destination;
    MyString timestamp;
};

class UndoStack {
private:
    struct StackNode {
        UndoRecord record;
        StackNode* next;
        StackNode(const UndoRecord& rec, StackNode* nxt = nullptr) : record(rec), next(nxt) {}
    };
    StackNode* topNode;
    int count;

public:
    UndoStack() : topNode(nullptr), count(0) {}

    ~UndoStack() {
        while (!isEmpty()) pop();
    }

    bool isEmpty() const { return topNode == nullptr; }

    void push(const UndoRecord& rec) {
        topNode = new StackNode(rec, topNode);
        count++;
    }

    bool pop(UndoRecord& outRecord) {
        if (isEmpty()) return false;
        StackNode* temp = topNode;
        outRecord = temp->record;
        topNode = topNode->next;
        delete temp;
        count--;
        return true;
    }

    void pop() {
        if (!isEmpty()) {
            StackNode* temp = topNode;
            topNode = topNode->next;
            delete temp;
            count--;
        }
    }

    int size() const { return count; }

    MyString toJSON() const {
        MyString json = "[";
        StackNode* curr = topNode;
        bool first = true;
        while (curr) {
            if (!first) json += ",";
            first = false;
            json += "{\"orderId\":";
            json += MyString::fromInt(curr->record.orderId);
            json += ",\"sku\":\"";
            json += curr->record.sku;
            json += "\",\"warehouseId\":\"";
            json += curr->record.warehouseId;
            json += "\",\"quantity\":";
            json += MyString::fromInt(curr->record.quantity);
            json += ",\"destination\":\"";
            json += curr->record.destination;
            json += "\",\"timestamp\":\"";
            json += curr->record.timestamp;
            json += "\"}";
            curr = curr->next;
        }
        json += "]";
        return json;
    }
};

// ============================================================================
// 3. DATA STRUCTURE: CUSTOM FIFO QUEUE (STANDARD SHIPMENTS)
// ============================================================================
struct Order {
    int orderId;
    MyString customerName;
    MyString destination;
    MyString sku;
    MyString productName;
    int quantity;
    int priority;
    MyString status;
    MyString assignedWarehouse;
    MyString routeDetails;
    double totalCost;
    MyString createdAt;
};

class OrderQueue {
private:
    struct QueueNode {
        Order order;
        QueueNode* next;
        QueueNode(const Order& ord) : order(ord), next(nullptr) {}
    };
    QueueNode* frontNode;
    QueueNode* rearNode;
    int count;

public:
    OrderQueue() : frontNode(nullptr), rearNode(nullptr), count(0) {}

    ~OrderQueue() {
        while (!isEmpty()) dequeue();
    }

    bool isEmpty() const { return frontNode == nullptr; }

    void enqueue(const Order& ord) {
        QueueNode* newNode = new QueueNode(ord);
        if (isEmpty()) {
            frontNode = rearNode = newNode;
        } else {
            rearNode->next = newNode;
            rearNode = newNode;
        }
        count++;
    }

    bool dequeue(Order& outOrder) {
        if (isEmpty()) return false;
        QueueNode* temp = frontNode;
        outOrder = temp->order;
        frontNode = frontNode->next;
        if (!frontNode) rearNode = nullptr;
        delete temp;
        count--;
        return true;
    }

    void dequeue() {
        if (!isEmpty()) {
            QueueNode* temp = frontNode;
            frontNode = frontNode->next;
            if (!frontNode) rearNode = nullptr;
            delete temp;
            count--;
        }
    }

    int size() const { return count; }

    MyString toJSON() const {
        MyString json = "[";
        QueueNode* curr = frontNode;
        bool first = true;
        while (curr) {
            if (!first) json += ",";
            first = false;
            json += "{\"orderId\":";
            json += MyString::fromInt(curr->order.orderId);
            json += ",\"customer\":\"";
            json += curr->order.customerName;
            json += "\",\"destination\":\"";
            json += curr->order.destination;
            json += "\",\"sku\":\"";
            json += curr->order.sku;
            json += "\",\"product\":\"";
            json += curr->order.productName;
            json += "\",\"quantity\":";
            json += MyString::fromInt(curr->order.quantity);
            json += ",\"priority\":";
            json += MyString::fromInt(curr->order.priority);
            json += ",\"status\":\"";
            json += curr->order.status;
            json += "\"}";
            curr = curr->next;
        }
        json += "]";
        return json;
    }
};

// ============================================================================
// 4. DATA STRUCTURE: CUSTOM MAX-HEAP / PRIORITY QUEUE (URGENT ORDERS)
// ============================================================================
class PriorityOrderHeap {
private:
    MyVector<Order> heap;

    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[index].priority > heap[parent].priority) {
                mySwap(heap[index], heap[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    void heapifyDown(int index) {
        int size = heap.size();
        while (true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int largest = index;

            if (left < size && heap[left].priority > heap[largest].priority) {
                largest = left;
            }
            if (right < size && heap[right].priority > heap[largest].priority) {
                largest = right;
            }

            if (largest != index) {
                mySwap(heap[index], heap[largest]);
                index = largest;
            } else {
                break;
            }
        }
    }

public:
    PriorityOrderHeap() {}

    bool isEmpty() const { return heap.empty(); }
    int size() const { return heap.size(); }

    void insert(const Order& order) {
        heap.push_back(order);
        heapifyUp(heap.size() - 1);
    }

    bool extractMax(Order& maxOrder) {
        if (heap.empty()) return false;
        maxOrder = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) {
            heapifyDown(0);
        }
        return true;
    }

    MyString toJSON() const {
        MyString json = "[";
        for (int i = 0; i < heap.size(); i++) {
            if (i > 0) json += ",";
            json += "{\"orderId\":";
            json += MyString::fromInt(heap[i].orderId);
            json += ",\"customer\":\"";
            json += heap[i].customerName;
            json += "\",\"destination\":\"";
            json += heap[i].destination;
            json += "\",\"sku\":\"";
            json += heap[i].sku;
            json += "\",\"product\":\"";
            json += heap[i].productName;
            json += "\",\"quantity\":";
            json += MyString::fromInt(heap[i].quantity);
            json += ",\"priority\":";
            json += MyString::fromInt(heap[i].priority);
            json += ",\"status\":\"";
            json += heap[i].status;
            json += "\"}";
        }
        json += "]";
        return json;
    }
};

// ============================================================================
// 5. DATA STRUCTURE: CUSTOM HASH TABLE (SEPARATE CHAINING)
// ============================================================================
struct Product {
    MyString sku;
    MyString name;
    MyString category;
    double price;
    int totalStock;
    MyString primaryWarehouse;
};

class InventoryHashTable {
private:
    struct HashNode {
        MyString key;
        Product value;
        HashNode* next;
        HashNode(const MyString& k, const Product& val) : key(k), value(val), next(nullptr) {}
    };

    static const int TABLE_SIZE = 31;
    HashNode* table[TABLE_SIZE];
    int itemCount;

    int hashFunction(const MyString& key) const {
        unsigned long hashVal = 0;
        for (int i = 0; i < key.length(); i++) {
            hashVal = (hashVal * 37 + (unsigned char)key[i]) % TABLE_SIZE;
        }
        return (int)hashVal;
    }

public:
    InventoryHashTable() : itemCount(0) {
        for (int i = 0; i < TABLE_SIZE; i++) table[i] = nullptr;
    }

    ~InventoryHashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            HashNode* curr = table[i];
            while (curr) {
                HashNode* temp = curr;
                curr = curr->next;
                delete temp;
            }
        }
    }

    void insert(const Product& prod) {
        int bucket = hashFunction(prod.sku);
        HashNode* curr = table[bucket];
        while (curr) {
            if (curr->key == prod.sku) {
                curr->value = prod;
                return;
            }
            curr = curr->next;
        }
        HashNode* newNode = new HashNode(prod.sku, prod);
        newNode->next = table[bucket];
        table[bucket] = newNode;
        itemCount++;
    }

    bool search(const MyString& sku, Product& outProd) const {
        int bucket = hashFunction(sku);
        HashNode* curr = table[bucket];
        while (curr) {
            if (curr->key == sku) {
                outProd = curr->value;
                return true;
            }
            curr = curr->next;
        }
        return false;
    }

    bool updateStock(const MyString& sku, int delta) {
        int bucket = hashFunction(sku);
        HashNode* curr = table[bucket];
        while (curr) {
            if (curr->key == sku) {
                curr->value.totalStock += delta;
                return true;
            }
            curr = curr->next;
        }
        return false;
    }

    MyVector<Product> getAllProducts() const {
        MyVector<Product> list;
        for (int i = 0; i < TABLE_SIZE; i++) {
            HashNode* curr = table[i];
            while (curr) {
                list.push_back(curr->value);
                curr = curr->next;
            }
        }
        return list;
    }

    MyString toJSON() const {
        MyVector<Product> prods = getAllProducts();
        MyString json = "[";
        for (int i = 0; i < prods.size(); i++) {
            if (i > 0) json += ",";
            json += "{\"sku\":\"";
            json += prods[i].sku;
            json += "\",\"name\":\"";
            json += prods[i].name;
            json += "\",\"category\":\"";
            json += prods[i].category;
            json += "\",\"price\":";
            json += MyString::fromDouble(prods[i].price, 2);
            json += ",\"stock\":";
            json += MyString::fromInt(prods[i].totalStock);
            json += ",\"warehouse\":\"";
            json += prods[i].primaryWarehouse;
            json += "\"}";
        }
        json += "]";
        return json;
    }
};

// ============================================================================
// 6. DATA STRUCTURE: CUSTOM BINARY SEARCH TREE (BST)
// ============================================================================
struct Warehouse {
    MyString id;
    MyString name;
    MyString location;
    int currentStock;
    int capacity;
};

class WarehouseBST {
private:
    struct BSTNode {
        Warehouse data;
        BSTNode* left;
        BSTNode* right;
        BSTNode(const Warehouse& wh) : data(wh), left(nullptr), right(nullptr) {}
    };

    BSTNode* root;

    BSTNode* insertRec(BSTNode* node, const Warehouse& wh) {
        if (!node) return new BSTNode(wh);
        if (wh.currentStock < node->data.currentStock) {
            node->left = insertRec(node->left, wh);
        } else {
            node->right = insertRec(node->right, wh);
        }
        return node;
    }

    void inorderRec(BSTNode* node, MyVector<Warehouse>& list) const {
        if (!node) return;
        inorderRec(node->left, list);
        list.push_back(node->data);
        inorderRec(node->right, list);
    }

    void preorderRec(BSTNode* node, MyVector<Warehouse>& list) const {
        if (!node) return;
        list.push_back(node->data);
        preorderRec(node->left, list);
        preorderRec(node->right, list);
    }

    void postorderRec(BSTNode* node, MyVector<Warehouse>& list) const {
        if (!node) return;
        postorderRec(node->left, list);
        postorderRec(node->right, list);
        list.push_back(node->data);
    }

    void destroyRec(BSTNode* node) {
        if (!node) return;
        destroyRec(node->left);
        destroyRec(node->right);
        delete node;
    }

public:
    WarehouseBST() : root(nullptr) {}
    ~WarehouseBST() { destroyRec(root); }

    void clear() {
        destroyRec(root);
        root = nullptr;
    }

    void insert(const Warehouse& wh) {
        root = insertRec(root, wh);
    }

    MyVector<Warehouse> getInOrder() const {
        MyVector<Warehouse> list;
        inorderRec(root, list);
        return list;
    }

    MyVector<Warehouse> getPreOrder() const {
        MyVector<Warehouse> list;
        preorderRec(root, list);
        return list;
    }

    MyVector<Warehouse> getPostOrder() const {
        MyVector<Warehouse> list;
        postorderRec(root, list);
        return list;
    }

    bool findEligibleWarehouse(int requiredQty, Warehouse& bestWh) const {
        MyVector<Warehouse> sorted = getInOrder();
        for (int i = 0; i < sorted.size(); i++) {
            if (sorted[i].currentStock >= requiredQty) {
                bestWh = sorted[i];
                return true;
            }
        }
        return false;
    }

    MyString toJSON() const {
        MyVector<Warehouse> list = getInOrder();
        MyString json = "[";
        for (int i = 0; i < list.size(); i++) {
            if (i > 0) json += ",";
            json += "{\"id\":\"";
            json += list[i].id;
            json += "\",\"name\":\"";
            json += list[i].name;
            json += "\",\"location\":\"";
            json += list[i].location;
            json += "\",\"currentStock\":";
            json += MyString::fromInt(list[i].currentStock);
            json += ",\"capacity\":";
            json += MyString::fromInt(list[i].capacity);
            json += "}";
        }
        json += "]";
        return json;
    }
};

// ============================================================================
// 7. DATA STRUCTURE: CUSTOM TRIE (PREFIX TREE)
// ============================================================================
struct TrieMatch {
    MyString sku;
    MyString name;
};

class ProductTrie {
private:
    struct TrieNode {
        TrieNode* children[128];
        bool isTerminal;
        MyString fullProductName;
        MyString sku;

        TrieNode() : isTerminal(false) {
            for (int i = 0; i < 128; i++) children[i] = nullptr;
        }
    };

    TrieNode* root;

    void collectAllWords(TrieNode* node, MyVector<TrieMatch>& results) const {
        if (!node) return;
        if (node->isTerminal) {
            TrieMatch tm{node->sku, node->fullProductName};
            results.push_back(tm);
        }
        for (int i = 0; i < 128; i++) {
            if (node->children[i]) {
                collectAllWords(node->children[i], results);
            }
        }
    }

    void destroyRec(TrieNode* node) {
        if (!node) return;
        for (int i = 0; i < 128; i++) {
            if (node->children[i]) destroyRec(node->children[i]);
        }
        delete node;
    }

public:
    ProductTrie() { root = new TrieNode(); }
    ~ProductTrie() { destroyRec(root); }

    void insert(const MyString& name, const MyString& sku) {
        TrieNode* curr = root;
        for (int i = 0; i < name.length(); i++) {
            unsigned char idx = (unsigned char)myToLower(name[i]);
            if (idx >= 128) idx = 127;
            if (!curr->children[idx]) {
                curr->children[idx] = new TrieNode();
            }
            curr = curr->children[idx];
        }
        curr->isTerminal = true;
        curr->fullProductName = name;
        curr->sku = sku;
    }

    MyVector<TrieMatch> searchPrefix(const MyString& prefix) const {
        MyVector<TrieMatch> results;
        TrieNode* curr = root;
        for (int i = 0; i < prefix.length(); i++) {
            unsigned char idx = (unsigned char)myToLower(prefix[i]);
            if (idx >= 128) idx = 127;
            if (!curr->children[idx]) return results;
            curr = curr->children[idx];
        }
        collectAllWords(curr, results);
        return results;
    }
};

// ============================================================================
// 8. DATA STRUCTURE: CUSTOM GRAPH (DIJKSTRA, BFS, KRUSKAL MST WITH DSU)
// ============================================================================
struct GraphEdge {
    MyString src;
    MyString dest;
    double weight;
    MyString roadType;
};

struct DSU {
    MyVector<int> parent;
    MyVector<int> rank;

    DSU(int n) {
        for (int i = 0; i < n; i++) {
            parent.push_back(i);
            rank.push_back(0);
        }
    }

    int find(int i) {
        if (parent[i] == i) return i;
        parent[i] = find(parent[i]);
        return parent[i];
    }

    bool unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if (root_i == root_j) return false;
        if (rank[root_i] < rank[root_j]) {
            parent[root_i] = root_j;
        } else if (rank[root_i] > rank[root_j]) {
            parent[root_j] = root_i;
        } else {
            parent[root_j] = root_i;
            rank[root_i]++;
        }
        return true;
    }
};

class SupplyChainGraph {
public:
    struct NodeInfo {
        MyString id;
        MyString name;
        MyString type;
        double x;
        double y;
    };

    MyVector<NodeInfo> nodes;
    MyVector<GraphEdge> edges;

    void addNode(const MyString& id, const MyString& name, const MyString& type, double x, double y) {
        NodeInfo ni{id, name, type, x, y};
        nodes.push_back(ni);
    }

    void addEdge(const MyString& src, const MyString& dest, double weight, const MyString& road = "Expressway") {
        GraphEdge e1{src, dest, weight, road};
        GraphEdge e2{dest, src, weight, road};
        edges.push_back(e1);
        edges.push_back(e2);
    }

    int getNodeIndex(const MyString& id) const {
        for (int i = 0; i < nodes.size(); i++) {
            if (nodes[i].id == id) return i;
        }
        return -1;
    }

    // Dijkstra's Shortest Path
    bool dijkstra(const MyString& startId, const MyString& endId,
                  MyVector<MyString>& path, double& totalDistance, int& nodesVisited) const {
        int n = nodes.size();
        int srcIdx = getNodeIndex(startId);
        int destIdx = getNodeIndex(endId);
        if (srcIdx == -1 || destIdx == -1) return false;

        const double INF = 1e9;
        MyVector<double> dist;
        MyVector<int> prev;
        MyVector<bool> visited;

        for (int i = 0; i < n; i++) {
            dist.push_back(INF);
            prev.push_back(-1);
            visited.push_back(false);
        }

        dist[srcIdx] = 0.0;
        nodesVisited = 0;

        for (int i = 0; i < n; i++) {
            int u = -1;
            double minDist = INF;
            for (int j = 0; j < n; j++) {
                if (!visited[j] && dist[j] < minDist) {
                    minDist = dist[j];
                    u = j;
                }
            }

            if (u == -1 || dist[u] == INF) break;
            visited[u] = true;
            nodesVisited++;

            if (u == destIdx) break;

            MyString uId = nodes[u].id;
            for (int e = 0; e < edges.size(); e++) {
                if (edges[e].src == uId) {
                    int v = getNodeIndex(edges[e].dest);
                    if (v != -1 && !visited[v]) {
                        if (dist[u] + edges[e].weight < dist[v]) {
                            dist[v] = dist[u] + edges[e].weight;
                            prev[v] = u;
                        }
                    }
                }
            }
        }

        if (dist[destIdx] == INF) return false;

        totalDistance = dist[destIdx];
        int curr = destIdx;
        while (curr != -1) {
            path.push_back(nodes[curr].id);
            curr = prev[curr];
        }
        path.reverse();
        return true;
    }

    // BFS for minimum hop count
    bool bfs(const MyString& startId, const MyString& endId,
             MyVector<MyString>& path, int& hops) const {
        int n = nodes.size();
        int srcIdx = getNodeIndex(startId);
        int destIdx = getNodeIndex(endId);
        if (srcIdx == -1 || destIdx == -1) return false;

        MyVector<bool> visited;
        MyVector<int> parent;
        for (int i = 0; i < n; i++) {
            visited.push_back(false);
            parent.push_back(-1);
        }

        MyVector<int> queue;
        queue.push_back(srcIdx);
        visited[srcIdx] = true;

        bool found = false;
        int head = 0;
        while (head < queue.size()) {
            int u = queue[head++];
            if (u == destIdx) {
                found = true;
                break;
            }
            MyString uId = nodes[u].id;
            for (int e = 0; e < edges.size(); e++) {
                if (edges[e].src == uId) {
                    int v = getNodeIndex(edges[e].dest);
                    if (v != -1 && !visited[v]) {
                        visited[v] = true;
                        parent[v] = u;
                        queue.push_back(v);
                    }
                }
            }
        }

        if (!found) return false;

        int curr = destIdx;
        while (curr != -1) {
            path.push_back(nodes[curr].id);
            curr = parent[curr];
        }
        path.reverse();
        hops = path.size() - 1;
        return true;
    }

    // DFS helper for network connectivity
    bool dfsHelper(int u, int destIdx, MyVector<bool>& visited, MyVector<int>& parent) const {
        if (u == destIdx) return true;
        visited[u] = true;
        MyString uId = nodes[u].id;
        for (int e = 0; e < edges.size(); e++) {
            if (edges[e].src == uId) {
                int v = getNodeIndex(edges[e].dest);
                if (v != -1 && !visited[v]) {
                    parent[v] = u;
                    if (dfsHelper(v, destIdx, visited, parent)) return true;
                }
            }
        }
        return false;
    }

    // DFS (Depth-First Search) for route exploration
    bool dfs(const MyString& startId, const MyString& endId,
             MyVector<MyString>& path, int& hops) const {
        int n = nodes.size();
        int srcIdx = getNodeIndex(startId);
        int destIdx = getNodeIndex(endId);
        if (srcIdx == -1 || destIdx == -1) return false;

        MyVector<bool> visited;
        MyVector<int> parent;
        for (int i = 0; i < n; i++) {
            visited.push_back(false);
            parent.push_back(-1);
        }

        bool found = dfsHelper(srcIdx, destIdx, visited, parent);
        if (!found) return false;

        int curr = destIdx;
        while (curr != -1) {
            path.push_back(nodes[curr].id);
            curr = parent[curr];
        }
        path.reverse();
        hops = path.size() - 1;
        return true;
    }

    // Kruskal's MST using custom DSU
    MyVector<GraphEdge> kruskalMST(double& totalMstCost) const {
        MyVector<GraphEdge> uniqueEdges;
        for (int i = 0; i < edges.size(); i++) {
            if (edges[i].src < edges[i].dest) {
                uniqueEdges.push_back(edges[i]);
            }
        }

        // Custom Bubble/Insertion Sort for uniqueEdges by weight
        for (int i = 0; i < uniqueEdges.size() - 1; i++) {
            for (int j = 0; j < uniqueEdges.size() - i - 1; j++) {
                if (uniqueEdges[j].weight > uniqueEdges[j + 1].weight) {
                    mySwap(uniqueEdges[j], uniqueEdges[j + 1]);
                }
            }
        }

        int n = nodes.size();
        DSU dsu(n);
        MyVector<GraphEdge> mst;
        totalMstCost = 0.0;

        for (int i = 0; i < uniqueEdges.size(); i++) {
            int u = getNodeIndex(uniqueEdges[i].src);
            int v = getNodeIndex(uniqueEdges[i].dest);
            if (u != -1 && v != -1) {
                if (dsu.unite(u, v)) {
                    mst.push_back(uniqueEdges[i]);
                    totalMstCost += uniqueEdges[i].weight;
                }
            }
        }
        return mst;
    }

    MyString toJSON() const {
        MyString json = "{\"nodes\":[";
        for (int i = 0; i < nodes.size(); i++) {
            if (i > 0) json += ",";
            json += "{\"id\":\"";
            json += nodes[i].id;
            json += "\",\"name\":\"";
            json += nodes[i].name;
            json += "\",\"type\":\"";
            json += nodes[i].type;
            json += "\",\"x\":";
            json += MyString::fromDouble(nodes[i].x, 0);
            json += ",\"y\":";
            json += MyString::fromDouble(nodes[i].y, 0);
            json += "}";
        }
        json += "],\"edges\":[";
        bool first = true;
        for (int i = 0; i < edges.size(); i++) {
            if (edges[i].src < edges[i].dest) {
                if (!first) json += ",";
                first = false;
                json += "{\"src\":\"";
                json += edges[i].src;
                json += "\",\"dest\":\"";
                json += edges[i].dest;
                json += "\",\"weight\":";
                json += MyString::fromDouble(edges[i].weight, 0);
                json += ",\"road\":\"";
                json += edges[i].roadType;
                json += "\"}";
            }
        }
        json += "]}";
        return json;
    }
};

// ============================================================================
// 9. DATA STRUCTURE: INTERVAL TREE (DOCK SCHEDULING)
// ============================================================================
struct Interval {
    int low;
    int high;
    MyString truckId;
    MyString warehouseId;
};

class IntervalTree {
private:
    struct IntervalNode {
        Interval i;
        int maxHigh;
        IntervalNode *left, *right;
        IntervalNode(const Interval& val) : i(val), maxHigh(val.high), left(nullptr), right(nullptr) {}
    };

    IntervalNode* root;

    IntervalNode* insertRec(IntervalNode* node, const Interval& i) {
        if (!node) return new IntervalNode(i);
        int l = node->i.low;
        if (i.low < l) {
            node->left = insertRec(node->left, i);
        } else {
            node->right = insertRec(node->right, i);
        }
        if (node->maxHigh < i.high) {
            node->maxHigh = i.high;
        }
        return node;
    }

    bool doOverlap(const Interval& i1, const Interval& i2) const {
        return (i1.low < i2.high && i2.low < i1.high);
    }

    IntervalNode* searchOverlapRec(IntervalNode* node, const Interval& i) const {
        if (!node) return nullptr;
        if (doOverlap(node->i, i)) return node;

        if (node->left && node->left->maxHigh > i.low) {
            return searchOverlapRec(node->left, i);
        }
        return searchOverlapRec(node->right, i);
    }

    void destroyRec(IntervalNode* node) {
        if (!node) return;
        destroyRec(node->left);
        destroyRec(node->right);
        delete node;
    }

public:
    IntervalTree() : root(nullptr) {}
    ~IntervalTree() { destroyRec(root); }

    void insert(const Interval& i) {
        root = insertRec(root, i);
    }

    bool hasOverlap(const Interval& i, Interval& conflicting) const {
        IntervalNode* res = searchOverlapRec(root, i);
        if (res) {
            conflicting = res->i;
            return true;
        }
        return false;
    }
};

// ============================================================================
// 10. SORTING ALGORITHMS (MERGE SORT & QUICK SORT)
// ============================================================================
struct Supplier {
    MyString id;
    MyString name;
    MyString location;
    double rating;
    int leadTimeDays;
    double costFactor;
};

static void merge(MyVector<Supplier>& arr, int l, int m, int r, const MyString& criteria, int& comparisons) {
    int n1 = m - l + 1;
    int n2 = r - m;
    MyVector<Supplier> L;
    MyVector<Supplier> R;

    for (int i = 0; i < n1; i++) L.push_back(arr[l + i]);
    for (int j = 0; j < n2; j++) R.push_back(arr[m + 1 + j]);

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        comparisons++;
        bool condition = false;
        if (criteria == "rating") {
            condition = (L[i].rating >= R[j].rating);
        } else if (criteria == "lead_time") {
            condition = (L[i].leadTimeDays <= R[j].leadTimeDays);
        } else {
            condition = (L[i].costFactor <= R[j].costFactor);
        }

        if (condition) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

static void mergeSortHelper(MyVector<Supplier>& arr, int l, int r, const MyString& criteria, int& comparisons) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSortHelper(arr, l, m, criteria, comparisons);
        mergeSortHelper(arr, m + 1, r, criteria, comparisons);
        merge(arr, l, m, r, criteria, comparisons);
    }
}

static void runMergeSort(MyVector<Supplier>& arr, const MyString& criteria, int& comparisons) {
    comparisons = 0;
    if (!arr.empty()) {
        mergeSortHelper(arr, 0, arr.size() - 1, criteria, comparisons);
    }
}

static int partition(MyVector<Supplier>& arr, int low, int high, const MyString& criteria, int& comparisons) {
    Supplier pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        comparisons++;
        bool condition = false;
        if (criteria == "rating") {
            condition = (arr[j].rating >= pivot.rating);
        } else if (criteria == "lead_time") {
            condition = (arr[j].leadTimeDays <= pivot.leadTimeDays);
        } else {
            condition = (arr[j].costFactor <= pivot.costFactor);
        }

        if (condition) {
            i++;
            mySwap(arr[i], arr[j]);
        }
    }
    mySwap(arr[i + 1], arr[high]);
    return i + 1;
}

static void quickSortHelper(MyVector<Supplier>& arr, int low, int high, const MyString& criteria, int& comparisons) {
    if (low < high) {
        int pi = partition(arr, low, high, criteria, comparisons);
        quickSortHelper(arr, low, pi - 1, criteria, comparisons);
        quickSortHelper(arr, pi + 1, high, criteria, comparisons);
    }
}

static void runQuickSort(MyVector<Supplier>& arr, const MyString& criteria, int& comparisons) {
    comparisons = 0;
    if (!arr.empty()) {
        quickSortHelper(arr, 0, arr.size() - 1, criteria, comparisons);
    }
}

// ============================================================================
// 11. SEARCHING ALGORITHMS (LINEAR SEARCH & BINARY SEARCH)
// ============================================================================
static int linearSearch(const MyVector<Product>& list, const MyString& sku, int& steps) {
    steps = 0;
    for (int i = 0; i < list.size(); i++) {
        steps++;
        if (list[i].sku == sku) return i;
    }
    return -1;
}

static int binarySearch(const MyVector<Product>& sortedList, const MyString& sku, int& steps) {
    steps = 0;
    int low = 0, high = sortedList.size() - 1;
    while (low <= high) {
        steps++;
        int mid = low + (high - low) / 2;
        if (sortedList[mid].sku == sku) {
            return mid;
        } else if (sortedList[mid].sku < sku) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1;
}

// ============================================================================
// 12. CENTRAL SUPPLY CHAIN MANAGER (BUSINESS & SIMULATION LOGIC)
// ============================================================================
class SupplyChainSystem {
public:
    AuditLinkedList auditLog;
    UndoStack undoStack;
    OrderQueue normalQueue;
    PriorityOrderHeap priorityHeap;
    InventoryHashTable inventory;
    WarehouseBST warehouseBST;
    ProductTrie productTrie;
    SupplyChainGraph graph;
    IntervalTree dockScheduler;
    MyVector<Supplier> suppliers;
    MyVector<Warehouse> warehouses;
    int nextOrderId;

    SupplyChainSystem() : nextOrderId(1001) {
        initNetwork();
        initCatalogAndInventory();
        initSuppliers();
        initIntervalTree();
        initOrders();
    }

    void initOrders() {
        placeOrder("Apollo Emergency Hospital", "MUMBAI", "TABLET-AIR", 5, 95);
        placeOrder("Dr. Nair Clinic", "PUNE", "PHONE-X", 2, 90);
        placeOrder("Rohan Sharma", "DELHI", "LAPTOP-01", 1, 30);
        placeOrder("Pooja Verma", "BANGALORE", "HEADPHONE-NC", 2, 30);
        placeOrder("Amit Patel", "HYDERABAD", "MOUSE-WL", 4, 30);
    }

    void initNetwork() {
        graph.addNode("SUP-PUNE", "Foxconn Pune Facility", "SUPPLIER", 180, 260);
        graph.addNode("SUP-BLR", "Tata Micro Bangalore", "SUPPLIER", 250, 480);
        graph.addNode("SUP-NOIDA", "Samsung Noida Hub", "SUPPLIER", 300, 70);

        graph.addNode("WH-BHIWANDI", "Bhiwandi Central Warehouse", "WAREHOUSE", 150, 310);
        graph.addNode("WH-GURGAON", "Gurgaon Logistics Depot", "WAREHOUSE", 280, 110);
        graph.addNode("WH-CHENNAI", "Chennai Coastal Warehouse", "WAREHOUSE", 330, 490);

        graph.addNode("DC-NAGPUR", "Nagpur Central Transit Hub", "DISTRIBUTION_CENTER", 340, 280);
        graph.addNode("DC-AHMEDABAD", "Ahmedabad Freight Terminal", "DISTRIBUTION_CENTER", 140, 190);
        graph.addNode("DC-HYDERABAD", "Hyderabad Cargo Hub", "DISTRIBUTION_CENTER", 300, 380);

        graph.addNode("HUB-MUMBAI", "Mumbai Delivery Hub", "HUB", 110, 340);
        graph.addNode("HUB-DELHI", "Delhi NCR Hub", "HUB", 320, 130);
        graph.addNode("HUB-BLR", "Bangalore Tech Hub", "HUB", 240, 520);
        graph.addNode("HUB-KOLKATA", "Kolkata Eastern Hub", "HUB", 510, 260);

        graph.addNode("MUMBAI", "Mumbai Consumer Zone", "CUSTOMER", 80, 360);
        graph.addNode("DELHI", "Delhi Consumer Zone", "CUSTOMER", 340, 90);
        graph.addNode("BANGALORE", "Bangalore Consumer Zone", "CUSTOMER", 230, 560);
        graph.addNode("HYDERABAD", "Hyderabad Consumer Zone", "CUSTOMER", 330, 390);
        graph.addNode("KOLKATA", "Kolkata Consumer Zone", "CUSTOMER", 540, 240);
        graph.addNode("PUNE", "Pune Consumer Zone", "CUSTOMER", 170, 370);
        graph.addNode("CHENNAI", "Chennai Consumer Zone", "CUSTOMER", 360, 520);

        graph.addEdge("SUP-PUNE", "WH-BHIWANDI", 140, "Expressway");
        graph.addEdge("SUP-PUNE", "DC-NAGPUR", 710, "National Highway");
        graph.addEdge("SUP-BLR", "WH-CHENNAI", 340, "Freight Corridor");
        graph.addEdge("SUP-BLR", "HUB-BLR", 35, "City Bypass");
        graph.addEdge("SUP-NOIDA", "WH-GURGAON", 60, "Expressway");

        graph.addEdge("WH-BHIWANDI", "DC-AHMEDABAD", 520, "Golden Quadrilateral");
        graph.addEdge("WH-BHIWANDI", "HUB-MUMBAI", 45, "Thane Expressway");
        graph.addEdge("WH-BHIWANDI", "DC-NAGPUR", 790, "Samruddhi Mahamarg");
        graph.addEdge("WH-GURGAON", "HUB-DELHI", 30, "NH-48");
        graph.addEdge("WH-GURGAON", "DC-AHMEDABAD", 880, "Western Freight Corridor");
        graph.addEdge("WH-GURGAON", "DC-NAGPUR", 950, "Grand Trunk Road");
        graph.addEdge("WH-CHENNAI", "HUB-BLR", 350, "NH-44");
        graph.addEdge("WH-CHENNAI", "DC-HYDERABAD", 630, "East Coast Road");

        graph.addEdge("DC-NAGPUR", "DC-HYDERABAD", 500, "NH-44");
        graph.addEdge("DC-NAGPUR", "HUB-KOLKATA", 970, "Eastern Corridor");
        graph.addEdge("DC-AHMEDABAD", "HUB-MUMBAI", 530, "Expressway");
        graph.addEdge("DC-AHMEDABAD", "HUB-DELHI", 900, "Expressway");
        graph.addEdge("DC-HYDERABAD", "HUB-BLR", 570, "NH-44");

        graph.addEdge("HUB-MUMBAI", "MUMBAI", 15, "Local Arterial");
        graph.addEdge("HUB-MUMBAI", "PUNE", 150, "Mumbai-Pune Expressway");
        graph.addEdge("HUB-DELHI", "DELHI", 20, "Ring Road");
        graph.addEdge("HUB-BLR", "BANGALORE", 20, "Hosur Elevated Road");
        graph.addEdge("DC-HYDERABAD", "HYDERABAD", 25, "Outer Ring Road");
        graph.addEdge("WH-CHENNAI", "CHENNAI", 25, "GST Road");
        graph.addEdge("HUB-KOLKATA", "KOLKATA", 20, "VIP Road");
    }

    void initCatalogAndInventory() {
        Product p1{"LAPTOP-01", "Laptop XPS 15", "Electronics", 1250.0, 45, "WH-BHIWANDI"};
        Product p2{"PHONE-X", "Smartphone Ultra", "Electronics", 899.0, 120, "WH-BHIWANDI"};
        Product p3{"TABLET-AIR", "Tablet Air 11-inch", "Electronics", 549.0, 75, "WH-CHENNAI"};
        Product p4{"HEADPHONE-NC", "Noise Cancelling Headphones", "Audio", 199.0, 210, "WH-GURGAON"};
        Product p5{"SMARTWATCH-4", "Fitness Smartwatch v4", "Wearables", 149.0, 160, "WH-CHENNAI"};
        Product p6{"ROUTER-AX", "WiFi 6 Gigabyte Router", "Networking", 89.0, 85, "WH-GURGAON"};
        Product p7{"MONITOR-4K", "Ultra HD 27-inch Monitor", "Displays", 340.0, 40, "WH-GURGAON"};
        Product p8{"KEYBOARD-RGB", "Mechanical Gaming Keyboard", "Peripherals", 75.0, 95, "WH-BHIWANDI"};
        Product p9{"MOUSE-WL", "Wireless Ergonomic Mouse", "Peripherals", 28.0, 180, "WH-CHENNAI"};
        Product p10{"PHONE-CASE", "Phone Armor Case", "Accessories", 15.0, 320, "WH-CHENNAI"};

        inventory.insert(p1); productTrie.insert(p1.name, p1.sku);
        inventory.insert(p2); productTrie.insert(p2.name, p2.sku);
        inventory.insert(p3); productTrie.insert(p3.name, p3.sku);
        inventory.insert(p4); productTrie.insert(p4.name, p4.sku);
        inventory.insert(p5); productTrie.insert(p5.name, p5.sku);
        inventory.insert(p6); productTrie.insert(p6.name, p6.sku);
        inventory.insert(p7); productTrie.insert(p7.name, p7.sku);
        inventory.insert(p8); productTrie.insert(p8.name, p8.sku);
        inventory.insert(p9); productTrie.insert(p9.name, p9.sku);
        inventory.insert(p10); productTrie.insert(p10.name, p10.sku);

        Warehouse w1{"WH-BHIWANDI", "Bhiwandi Central Warehouse", "Maharashtra", 620, 1000};
        Warehouse w2{"WH-GURGAON", "Gurgaon Logistics Depot", "Haryana", 410, 800};
        Warehouse w3{"WH-CHENNAI", "Chennai Coastal Warehouse", "Tamil Nadu", 710, 1200};

        warehouses.push_back(w1);
        warehouses.push_back(w2);
        warehouses.push_back(w3);

        refreshWarehouseBST();
        auditLog.append("SYSTEM_START", "INFO", "Supply Chain Optimization Engine initialized (Zero STL)");
    }

    void refreshWarehouseBST() {
        warehouseBST.clear();
        for (int i = 0; i < warehouses.size(); i++) {
            warehouseBST.insert(warehouses[i]);
        }
    }

    void initSuppliers() {
        suppliers.push_back({"SUP-PUNE", "Foxconn Pune Facility", "Pune", 4.9, 2, 1.05});
        suppliers.push_back({"SUP-BLR", "Tata Micro Bangalore", "Bangalore", 4.8, 3, 1.12});
        suppliers.push_back({"SUP-NOIDA", "Samsung Noida Hub", "Noida", 4.5, 4, 0.98});
        suppliers.push_back({"SUP-HYD", "Bharat Semiconductor Fab", "Hyderabad", 4.7, 3, 1.02});
        suppliers.push_back({"SUP-AHM", "Adani Logistics Allied", "Ahmedabad", 4.3, 5, 0.94});
    }

    void initIntervalTree() {
        dockScheduler.insert({9, 11, "TRUCK-KA01", "WH-BHIWANDI"});
        dockScheduler.insert({12, 14, "TRUCK-MH04", "WH-BHIWANDI"});
        dockScheduler.insert({15, 17, "TRUCK-DL09", "WH-GURGAON"});
    }

    int placeOrder(const MyString& customer, const MyString& destination, const MyString& sku, int qty, int priority) {
        Product prod;
        if (!inventory.search(sku, prod)) {
            return -1;
        }

        Order order;
        order.orderId = nextOrderId++;
        order.customerName = customer;
        order.destination = destination;
        order.sku = sku;
        order.productName = prod.name;
        order.quantity = qty;
        order.priority = priority;
        order.status = "PENDING";
        order.assignedWarehouse = prod.primaryWarehouse;
        order.totalCost = prod.price * qty;
        order.createdAt = "Now";

        if (priority >= 50) {
            priorityHeap.insert(order);
            MyString msg = "High priority order #";
            msg += MyString::fromInt(order.orderId);
            msg += " ("; msg += customer; msg += ") inserted into Max-Heap with score ";
            msg += MyString::fromInt(priority);
            auditLog.append("ORDER_QUEUED_HEAP", "PRIORITY_HEAP", msg);
        } else {
            normalQueue.enqueue(order);
            MyString msg = "Standard order #";
            msg += MyString::fromInt(order.orderId);
            msg += " ("; msg += customer; msg += ") enqueued to FIFO Queue";
            auditLog.append("ORDER_QUEUED_FIFO", "FIFO_QUEUE", msg);
        }

        return order.orderId;
    }

    bool processNextOrder(Order& processed, MyString& dsaLog) {
        MyString log = "";
        bool hasOrder = false;

        if (!priorityHeap.isEmpty()) {
            priorityHeap.extractMax(processed);
            log += "1. [Max-Heap] Extracted order #";
            log += MyString::fromInt(processed.orderId);
            log += " (Priority: ";
            log += MyString::fromInt(processed.priority);
            log += ").\n";
            hasOrder = true;
        } else if (!normalQueue.isEmpty()) {
            normalQueue.dequeue(processed);
            log += "1. [FIFO Queue] Dequeued standard order #";
            log += MyString::fromInt(processed.orderId);
            log += ".\n";
            hasOrder = true;
        }

        if (!hasOrder) {
            dsaLog = "No pending orders in Queue or Max-Heap!";
            return false;
        }

        Product prod;
        if (!inventory.search(processed.sku, prod)) {
            log += "2. [Hash Table] ERROR: Product SKU not found!\n";
            dsaLog = log;
            return false;
        }
        log += "2. [Hash Table] Found ";
        log += prod.name;
        log += " in O(1) time. Current stock: ";
        log += MyString::fromInt(prod.totalStock);
        log += ".\n";

        if (prod.totalStock < processed.quantity) {
            log += "2. [Hash Table] Insufficient inventory. Order delayed.\n";
            dsaLog = log;
            return false;
        }

        Warehouse bestWh;
        if (warehouseBST.findEligibleWarehouse(processed.quantity, bestWh)) {
            processed.assignedWarehouse = bestWh.id;
            log += "3. [BST] Traversed tree to allocate best warehouse: ";
            log += bestWh.name;
            log += " (Stock: ";
            log += MyString::fromInt(bestWh.currentStock);
            log += ").\n";
        } else {
            processed.assignedWarehouse = prod.primaryWarehouse;
            log += "3. [BST] Fallback to primary warehouse.\n";
        }

        MyVector<MyString> route;
        double routeDistance = 0.0;
        int visitedNodes = 0;
        bool routeFound = graph.dijkstra(processed.assignedWarehouse, processed.destination, route, routeDistance, visitedNodes);

        if (routeFound) {
            MyString routeStr = "";
            for (int i = 0; i < route.size(); i++) {
                if (i > 0) routeStr += " -> ";
                routeStr += route[i];
            }
            processed.routeDetails = routeStr;
            log += "4. [Graph Dijkstra] Computed optimal route with ";
            log += MyString::fromInt(visitedNodes);
            log += " nodes inspected: ";
            log += processed.routeDetails;
            log += " (Distance: ";
            log += MyString::fromDouble(routeDistance, 0);
            log += " km).\n";
        } else {
            processed.routeDetails = processed.assignedWarehouse + " -> Direct Transport -> " + processed.destination;
            log += "4. [Graph Dijkstra] Direct destination route selected.\n";
        }

        inventory.updateStock(processed.sku, -processed.quantity);
        for (int i = 0; i < warehouses.size(); i++) {
            if (warehouses[i].id == processed.assignedWarehouse) {
                warehouses[i].currentStock -= processed.quantity;
                break;
            }
        }
        refreshWarehouseBST();
        log += "5. [Hash Table & BST] Deducted ";
        log += MyString::fromInt(processed.quantity);
        log += " units. Hash table & BST updated.\n";

        UndoRecord undoRec;
        undoRec.orderId = processed.orderId;
        undoRec.sku = processed.sku;
        undoRec.warehouseId = processed.assignedWarehouse;
        undoRec.quantity = processed.quantity;
        undoRec.destination = processed.destination;
        undoRec.timestamp = "Dispatched";
        undoStack.push(undoRec);
        log += "6. [Undo Stack] Pushed dispatch action to Undo Stack (Depth: ";
        log += MyString::fromInt(undoStack.size());
        log += ").\n";

        processed.status = "DISPATCHED";
        MyString aMsg = "Order #";
        aMsg += MyString::fromInt(processed.orderId);
        aMsg += " dispatched via ";
        aMsg += processed.routeDetails;
        auditLog.append("ORDER_DISPATCHED", "PIPELINE", aMsg);
        log += "7. [Linked List] Logged transaction to audit trail.\n";

        dsaLog = log;
        return true;
    }

    bool undoLastDispatch(MyString& undoLog) {
        if (undoStack.isEmpty()) {
            undoLog = "Undo Stack is empty! No actions to revert.";
            return false;
        }

        UndoRecord rec;
        undoStack.pop(rec);

        inventory.updateStock(rec.sku, rec.quantity);

        for (int i = 0; i < warehouses.size(); i++) {
            if (warehouses[i].id == rec.warehouseId) {
                warehouses[i].currentStock += rec.quantity;
                break;
            }
        }
        refreshWarehouseBST();

        MyString res = "[Stack Pop] Reverted order #";
        res += MyString::fromInt(rec.orderId);
        res += ". Restored ";
        res += MyString::fromInt(rec.quantity);
        res += " units of ";
        res += rec.sku;
        res += " to warehouse ";
        res += rec.warehouseId;
        res += ".";
        undoLog = res;

        auditLog.append("UNDO_OPERATION", "STACK", undoLog);
        return true;
    }
};

// ============================================================================
// 13. EMBEDDED LIGHTWEIGHT HTTP REST API SERVER (NON-STL SOCKET ENGINE)
// ============================================================================
static MyString extractJsonString(const MyString& json, const char* key) {
    MyString pat = "\"";
    pat += key;
    pat += "\"";
    int keyPos = json.find(pat.c_str());
    if (keyPos == -1) return MyString("");
    int colonPos = json.find(':', keyPos);
    if (colonPos == -1) return MyString("");
    int firstQuote = json.find('\"', colonPos + 1);
    if (firstQuote == -1) return MyString("");
    int secondQuote = json.find('\"', firstQuote + 1);
    if (secondQuote == -1) return MyString("");
    return json.substr(firstQuote + 1, secondQuote - firstQuote - 1);
}

static int extractJsonInt(const MyString& json, const char* key, int defaultVal = 0) {
    MyString pat = "\"";
    pat += key;
    pat += "\"";
    int keyPos = json.find(pat.c_str());
    if (keyPos == -1) return defaultVal;
    int colonPos = json.find(':', keyPos);
    if (colonPos == -1) return defaultVal;
    int pos = colonPos + 1;
    while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\"' || json[pos] == '\t' || json[pos] == '\r' || json[pos] == '\n')) pos++;
    int endPos = pos;
    while (endPos < json.length() && ((json[endPos] >= '0' && json[endPos] <= '9') || json[endPos] == '-')) endPos++;
    if (endPos == pos) return defaultVal;
    int res = 0;
    bool neg = false;
    int p = pos;
    if (json[p] == '-') { neg = true; p++; }
    while (p < endPos) {
        res = res * 10 + (json[p] - '0');
        p++;
    }
    return neg ? -res : res;
}

static MyString getQueryParam(const MyString& query, const char* key) {
    MyString pat = key;
    pat += "=";
    int pos = query.find(pat.c_str());
    if (pos == -1) return MyString("");
    pos += pat.length();
    int endPos = query.find('&', pos);
    if (endPos == -1) endPos = query.length();
    return query.substr(pos, endPos - pos);
}

void sendHttpResponse(SOCKET clientSocket, int statusCode, const char* contentType, const char* body, int bodyLen) {
    char header[512];
    sprintf(header, "HTTP/1.1 %d OK\r\nContent-Type: %s; charset=utf-8\r\nContent-Length: %d\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: GET, POST, OPTIONS\r\nAccess-Control-Allow-Headers: Content-Type\r\nCache-Control: no-cache, no-store, must-revalidate\r\nConnection: close\r\n\r\n", statusCode, contentType, bodyLen);

    int hLen = myStrLen(header);
    int hSent = 0;
    while (hSent < hLen) {
        int s = send(clientSocket, header + hSent, hLen - hSent, 0);
        if (s <= 0) break;
        hSent += s;
    }

    if (body && bodyLen > 0) {
        int bSent = 0;
        while (bSent < bodyLen) {
            int s = send(clientSocket, body + bSent, bodyLen - bSent, 0);
            if (s <= 0) break;
            bSent += s;
        }
    }
}

void sendHttpResponse(SOCKET clientSocket, int statusCode, const char* contentType, const MyString& body) {
    sendHttpResponse(clientSocket, statusCode, contentType, body.c_str(), body.length());
}

SupplyChainSystem* g_system = nullptr;

void handleClientRequest(SOCKET clientSocket) {
    char buffer[4096];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived <= 0) {
        closesocket(clientSocket);
        return;
    }
    buffer[bytesReceived] = '\0';
    MyString request(buffer);

    // Parse method and path
    int firstSpace = request.find(' ');
    if (firstSpace == -1) {
        closesocket(clientSocket);
        return;
    }
    MyString method = request.substr(0, firstSpace);
    int secondSpace = request.find(' ', firstSpace + 1);
    if (secondSpace == -1) {
        closesocket(clientSocket);
        return;
    }
    MyString url = request.substr(firstSpace + 1, secondSpace - firstSpace - 1);

    if (method == "OPTIONS") {
        sendHttpResponse(clientSocket, 200, "text/plain", MyString("OK"));
        closesocket(clientSocket);
        return;
    }

    MyString path = url;
    MyString query = "";
    int qPos = url.find('?');
    if (qPos != -1) {
        path = url.substr(0, qPos);
        query = url.substr(qPos + 1);
    }

    if (path == "/favicon.ico") {
        sendHttpResponse(clientSocket, 204, "image/x-icon", MyString(""));
        closesocket(clientSocket);
        return;
    }

    // Ensure entire HTTP Body is read if Content-Length exists
    int headerEnd = request.find("\r\n\r\n");
    int headerSepLen = 4;
    if (headerEnd == -1) {
        headerEnd = request.find("\n\n");
        headerSepLen = 2;
    }

    if (headerEnd != -1) {
        int clPos = request.find("Content-Length:");
        if (clPos == -1) clPos = request.find("content-length:");
        if (clPos != -1) {
            int valStart = clPos + 15;
            while (valStart < request.length() && (request[valStart] == ' ' || request[valStart] == '\t')) {
                valStart++;
            }
            int expLen = 0;
            while (valStart < request.length() && (request[valStart] >= '0' && request[valStart] <= '9')) {
                expLen = expLen * 10 + (request[valStart] - '0');
                valStart++;
            }
            int currLen = request.length() - (headerEnd + headerSepLen);
            while (currLen < expLen) {
                int n = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
                if (n <= 0) break;
                buffer[n] = '\0';
                request += buffer;
                currLen += n;
            }
        }
    }

    // 1. Serve index.html
    if (path == "/" || path == "/index.html") {
        FILE* f = fopen("index.html", "rb");
        if (!f) {
            char exePath[MAX_PATH];
            GetModuleFileNameA(NULL, exePath, MAX_PATH);
            int lastSlash = -1;
            for (int i = 0; exePath[i] != '\0'; i++) {
                if (exePath[i] == '\\' || exePath[i] == '/') lastSlash = i;
            }
            if (lastSlash != -1) {
                exePath[lastSlash + 1] = '\0';
                MyString resolvedPath(exePath);
                resolvedPath += "index.html";
                f = fopen(resolvedPath.c_str(), "rb");
            }
        }
        if (!f) f = fopen("D:\\projects\\dynamic-supply-chain-dsa\\index.html", "rb");
        if (!f) f = fopen("C:\\Users\\Omen\\.gemini\\antigravity\\scratch\\supply_chain_dsa\\index.html", "rb");
        if (!f) f = fopen("C:\\Users\\Omen\\index.html", "rb");
        if (!f) f = fopen("C:\\Users\\Omen\\.gemini\\antigravity\\brain\\d26ed44f-d54b-4b62-99d4-4b604bd0e3b3\\dashboard.html", "rb");
        if (!f) f = fopen("supply_chain_dsa/index.html", "rb");
        if (f) {
            fseek(f, 0, SEEK_END);
            long fsize = ftell(f);
            fseek(f, 0, SEEK_SET);
            char* fileBuf = new char[fsize + 1];
            fread(fileBuf, 1, fsize, f);
            fileBuf[fsize] = '\0';
            fclose(f);
            sendHttpResponse(clientSocket, 200, "text/html", fileBuf, (int)fsize);
            delete[] fileBuf;
        } else {
            const char* fallbackHtml = "<!DOCTYPE html><html><body style='font-family:sans-serif;padding:40px;background:#0f172a;color:#f8fafc;text-align:center;'><h2>C++ Backend Running on Port 8080</h2><p>Please ensure index.html is placed in the project folder.</p></body></html>";
            sendHttpResponse(clientSocket, 200, "text/html", fallbackHtml, myStrLen(fallbackHtml));
        }
    }
    // 2. GET /api/state
    else if (path == "/api/state") {
        MyString json = "{\"inventory\":";
        json += g_system->inventory.toJSON();
        json += ",\"warehouses\":";
        json += g_system->warehouseBST.toJSON();
        json += ",\"network\":";
        json += g_system->graph.toJSON();
        json += ",\"priorityHeap\":";
        json += g_system->priorityHeap.toJSON();
        json += ",\"normalQueue\":";
        json += g_system->normalQueue.toJSON();
        json += ",\"undoStack\":";
        json += g_system->undoStack.toJSON();
        json += ",\"auditLog\":";
        json += g_system->auditLog.toJSON();
        json += ",\"stats\":{\"totalProducts\":";
        json += MyString::fromInt(g_system->inventory.getAllProducts().size());
        json += ",\"priorityOrdersCount\":";
        json += MyString::fromInt(g_system->priorityHeap.size());
        json += ",\"normalOrdersCount\":";
        json += MyString::fromInt(g_system->normalQueue.size());
        json += ",\"undoStackCount\":";
        json += MyString::fromInt(g_system->undoStack.size());
        json += ",\"logEntriesCount\":";
        json += MyString::fromInt(g_system->auditLog.size());
        json += "}}";
        sendHttpResponse(clientSocket, 200, "application/json", json);
    }
    // 3. GET /api/trie
    else if (path == "/api/trie") {
        MyString prefix = getQueryParam(query, "prefix");
        MyVector<TrieMatch> matches = g_system->productTrie.searchPrefix(prefix);
        MyString json = "[";
        for (int i = 0; i < matches.size(); i++) {
            if (i > 0) json += ",";
            json += "{\"sku\":\"";
            json += matches[i].sku;
            json += "\",\"name\":\"";
            json += matches[i].name;
            json += "\"}";
        }
        json += "]";
        sendHttpResponse(clientSocket, 200, "application/json", json);
    }
    // 4. GET /api/route
    else if (path == "/api/route") {
        MyString from = getQueryParam(query, "from");
        MyString to = getQueryParam(query, "to");
        MyString algo = getQueryParam(query, "algo");
        if (algo.empty()) algo = "dijkstra";

        MyVector<MyString> pathNodes;
        double totalDistance = 0.0;
        int metric = 0;
        bool found = false;

        if (algo == "dijkstra") {
            found = g_system->graph.dijkstra(from, to, pathNodes, totalDistance, metric);
        } else if (algo == "dfs") {
            found = g_system->graph.dfs(from, to, pathNodes, metric);
            totalDistance = metric * 160.0;
        } else {
            found = g_system->graph.bfs(from, to, pathNodes, metric);
            totalDistance = metric * 150.0;
        }

        MyString json = "{\"found\":";
        json += (found ? "true" : "false");
        json += ",\"algorithm\":\"";
        json += algo;
        json += "\",\"from\":\"";
        json += from;
        json += "\",\"to\":\"";
        json += to;
        json += "\",\"distance\":";
        json += MyString::fromDouble(totalDistance, 0);
        json += ",\"metric\":";
        json += MyString::fromInt(metric);
        json += ",\"executionTimeMicros\":0";
        json += ",\"path\":[";
        for (int i = 0; i < pathNodes.size(); i++) {
            if (i > 0) json += ",";
            json += "\"";
            json += pathNodes[i];
            json += "\"";
        }
        json += "]}";
        sendHttpResponse(clientSocket, 200, "application/json", json);
    }
    // 5. GET /api/mst
    else if (path == "/api/mst") {
        double totalCost = 0.0;
        MyVector<GraphEdge> mstEdges = g_system->graph.kruskalMST(totalCost);
        MyString json = "{\"totalCost\":";
        json += MyString::fromDouble(totalCost, 0);
        json += ",\"edges\":[";
        for (int i = 0; i < mstEdges.size(); i++) {
            if (i > 0) json += ",";
            json += "{\"src\":\"";
            json += mstEdges[i].src;
            json += "\",\"dest\":\"";
            json += mstEdges[i].dest;
            json += "\",\"weight\":";
            json += MyString::fromDouble(mstEdges[i].weight, 0);
            json += ",\"road\":\"";
            json += mstEdges[i].roadType;
            json += "\"}";
        }
        json += "]}";
        sendHttpResponse(clientSocket, 200, "application/json", json);
    }
    // 5b. GET /api/bst/traversal
    else if (path == "/api/bst/traversal") {
        MyString type = getQueryParam(query, "type");
        if (type.empty()) type = "inorder";

        MyVector<Warehouse> list;
        if (type == "preorder") list = g_system->warehouseBST.getPreOrder();
        else if (type == "postorder") list = g_system->warehouseBST.getPostOrder();
        else list = g_system->warehouseBST.getInOrder();

        MyString json = "{\"type\":\"";
        json += type;
        json += "\",\"warehouses\":[";
        for (int i = 0; i < list.size(); i++) {
            if (i > 0) json += ",";
            json += "{\"id\":\"";
            json += list[i].id;
            json += "\",\"name\":\"";
            json += list[i].name;
            json += "\",\"stock\":";
            json += MyString::fromInt(list[i].currentStock);
            json += ",\"capacity\":";
            json += MyString::fromInt(list[i].capacity);
            json += "}";
        }
        json += "]}";
        sendHttpResponse(clientSocket, 200, "application/json", json);
    }
    // 6. GET /api/sort
    else if (path == "/api/sort") {
        MyString criteria = getQueryParam(query, "by");
        MyString algo = getQueryParam(query, "algo");
        if (criteria.empty()) criteria = "rating";
        if (algo.empty()) algo = "merge";

        MyVector<Supplier> copyList = g_system->suppliers;
        int comparisons = 0;

        if (algo == "quick") {
            runQuickSort(copyList, criteria, comparisons);
        } else {
            runMergeSort(copyList, criteria, comparisons);
        }

        MyString json = "{\"algorithm\":\"";
        json += algo;
        json += "\",\"criteria\":\"";
        json += criteria;
        json += "\",\"comparisons\":";
        json += MyString::fromInt(comparisons);
        json += ",\"timeMicros\":0,\"suppliers\":[";
        for (int i = 0; i < copyList.size(); i++) {
            if (i > 0) json += ",";
            json += "{\"id\":\"";
            json += copyList[i].id;
            json += "\",\"name\":\"";
            json += copyList[i].name;
            json += "\",\"location\":\"";
            json += copyList[i].location;
            json += "\",\"rating\":";
            json += MyString::fromDouble(copyList[i].rating, 1);
            json += ",\"leadTime\":";
            json += MyString::fromInt(copyList[i].leadTimeDays);
            json += ",\"costFactor\":";
            json += MyString::fromDouble(copyList[i].costFactor, 2);
            json += "}";
        }
        json += "]}";
        sendHttpResponse(clientSocket, 200, "application/json", json);
    }
    // 7. GET /api/search
    else if (path == "/api/search") {
        MyString sku = getQueryParam(query, "sku");
        MyString algo = getQueryParam(query, "algo");
        if (algo.empty()) algo = "binary";

        MyVector<Product> prods = g_system->inventory.getAllProducts();
        // Bubble sort prods by SKU for binary search
        for (int i = 0; i < prods.size() - 1; i++) {
            for (int j = 0; j < prods.size() - i - 1; j++) {
                if (prods[j + 1].sku < prods[j].sku) {
                    mySwap(prods[j], prods[j + 1]);
                }
            }
        }

        int steps = 0;
        int idx = -1;
        if (algo == "linear") {
            idx = linearSearch(prods, sku, steps);
        } else {
            idx = binarySearch(prods, sku, steps);
        }

        MyString json = "{\"found\":";
        json += (idx != -1 ? "true" : "false");
        json += ",\"algorithm\":\"";
        json += algo;
        json += "\",\"steps\":";
        json += MyString::fromInt(steps);
        json += ",\"timeNanos\":0,";
        if (idx != -1) {
            json += "\"product\":{\"sku\":\"";
            json += prods[idx].sku;
            json += "\",\"name\":\"";
            json += prods[idx].name;
            json += "\",\"price\":";
            json += MyString::fromDouble(prods[idx].price, 2);
            json += ",\"stock\":";
            json += MyString::fromInt(prods[idx].totalStock);
            json += "}";
        } else {
            json += "\"product\":null";
        }
        json += "}";
        sendHttpResponse(clientSocket, 200, "application/json", json);
    }
    // 8. POST /api/order
    else if (path == "/api/order" && method == "POST") {
        int bPos = request.find("\r\n\r\n");
        MyString body = (bPos != -1) ? request.substr(bPos + 4) : MyString("");

        MyString customer = extractJsonString(body, "customer");
        MyString destination = extractJsonString(body, "destination");
        MyString sku = extractJsonString(body, "sku");
        int qty = extractJsonInt(body, "quantity", 1);
        int priority = extractJsonInt(body, "priority", 30);

        if (customer.empty()) customer = "Retail Customer";
        if (destination.empty()) destination = "MUMBAI";
        if (sku.empty()) sku = "LAPTOP-01";
        if (qty <= 0) qty = 1;

        int orderId = g_system->placeOrder(customer, destination, sku, qty, priority);
        MyString json = "{\"success\":";
        if (orderId != -1) {
            json += "true,\"orderId\":";
            json += MyString::fromInt(orderId);
            json += ",\"message\":\"Order placed successfully!\"}";
        } else {
            json += "false,\"message\":\"Invalid Product SKU.\"}";
        }
        sendHttpResponse(clientSocket, 200, "application/json", json);
    }
    // 9. POST /api/dispatch or /api/process-order
    else if ((path == "/api/dispatch" || path == "/api/process-order") && method == "POST") {
        Order processed;
        MyString dsaLog;
        bool success = g_system->processNextOrder(processed, dsaLog);

        MyString json = "{\"success\":";
        json += (success ? "true" : "false");
        json += ",\"dsaLog\":\"";
        for (int i = 0; i < dsaLog.length(); i++) {
            if (dsaLog[i] == '\n') json += "\\n";
            else if (dsaLog[i] == '\"') json += "\\\"";
            else if (dsaLog[i] == '\\') json += "\\\\";
            else json += dsaLog[i];
        }
        json += "\",";
        if (success) {
            json += "\"order\":{\"id\":";
            json += MyString::fromInt(processed.orderId);
            json += ",\"customer\":\"";
            json += processed.customerName;
            json += "\",\"destination\":\"";
            json += processed.destination;
            json += "\",\"sku\":\"";
            json += processed.sku;
            json += "\",\"quantity\":";
            json += MyString::fromInt(processed.quantity);
            json += ",\"priority\":";
            json += MyString::fromInt(processed.priority);
            json += ",\"assignedWarehouse\":\"";
            json += processed.assignedWarehouse;
            json += "\",\"route\":\"";
            json += processed.routeDetails;
            json += "\"}";
        } else {
            json += "\"order\":null";
        }
        json += "}";
        sendHttpResponse(clientSocket, 200, "application/json", json);
    }
    // 10. POST /api/undo
    else if (path == "/api/undo" && method == "POST") {
        MyString undoLog;
        bool success = g_system->undoLastDispatch(undoLog);
        MyString json = "{\"success\":";
        json += (success ? "true" : "false");
        json += ",\"message\":\"";
        json += undoLog;
        json += "\"}";
        sendHttpResponse(clientSocket, 200, "application/json", json);
    }
    // 11. POST /api/schedule-dock
    else if (path == "/api/schedule-dock" && method == "POST") {
        int bPos = request.find("\r\n\r\n");
        MyString body = (bPos != -1) ? request.substr(bPos + 4) : MyString("");

        int startHour = extractJsonInt(body, "startHour", 10);
        int endHour = extractJsonInt(body, "endHour", 12);
        MyString truckId = extractJsonString(body, "truckId");
        if (truckId.empty()) truckId = "TRUCK-X99";

        Interval req{startHour, endHour, truckId, "WH-BHIWANDI"};
        Interval conflict;
        bool hasCollision = g_system->dockScheduler.hasOverlap(req, conflict);

        if (!hasCollision) {
            g_system->dockScheduler.insert(req);
        }

        MyString json = "{\"booked\":";
        json += (!hasCollision ? "true" : "false");
        json += ",\"startHour\":";
        json += MyString::fromInt(startHour);
        json += ",\"endHour\":";
        json += MyString::fromInt(endHour);
        json += ",\"truckId\":\"";
        json += truckId;
        json += "\",";
        if (hasCollision) {
            json += "\"conflict\":{\"truckId\":\"";
            json += conflict.truckId;
            json += "\",\"startHour\":";
            json += MyString::fromInt(conflict.low);
            json += ",\"endHour\":";
            json += MyString::fromInt(conflict.high);
            json += "}";
        } else {
            json += "\"conflict\":null";
        }
        json += "}";
        sendHttpResponse(clientSocket, 200, "application/json", json);
    } else {
        sendHttpResponse(clientSocket, 404, "text/plain", MyString("404 Not Found"));
    }

    closesocket(clientSocket);
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================
int main() {
    cout << "================================================================\n";
    cout << "  DYNAMIC SUPPLY CHAIN OPTIMIZATION & DISTRIBUTION SYSTEM       \n";
    cout << "  [100% PURE C++ IMPLEMENTATION - ZERO STL HEADERS USED]        \n";
    cout << "================================================================\n";
    cout << " Custom Data Structures Active:\n";
    cout << "  [+] MyString (Dynamic char* with manual buffer management)\n";
    cout << "  [+] MyVector<T> (Dynamic auto-resizing array)\n";
    cout << "  [+] Custom Singly Linked List (Transaction Audit Trail)\n";
    cout << "  [+] Custom Stack (Undo & Inventory Rollback)\n";
    cout << "  [+] Custom FIFO Queue (Standard Customer Orders)\n";
    cout << "  [+] Custom Max-Heap / Priority Queue (Emergency Orders)\n";
    cout << "  [+] Custom Separate Chaining Hash Table (O(1) Product Inventory)\n";
    cout << "  [+] Custom Binary Search Tree (Warehouse Capacity Management)\n";
    cout << "  [+] Custom Trie (128-Child ASCII Product Name Autocomplete)\n";
    cout << "  [+] Custom Graph (Adjacency List + Node Relaxation)\n";
    cout << "  [+] Custom Dijkstra Shortest Path Algorithm\n";
    cout << "  [+] Custom Breadth-First Search (BFS)\n";
    cout << "  [+] Custom Kruskal's MST with Disjoint Set Union (DSU)\n";
    cout << "  [+] Custom Interval Tree (Warehouse Dock Overlap Detection)\n";
    cout << "  [+] Custom Merge Sort & Quick Sort Algorithms\n";
    cout << "  [+] Custom Linear & Binary Search Algorithms\n";
    cout << "----------------------------------------------------------------\n";

    g_system = new SupplyChainSystem();

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Failed to initialize Winsock.\n";
        return 1;
    }
#endif

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Failed to create socket.\n";
        return 1;
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    int PORT = 8080;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Bind failed on port " << PORT << ". Address may be in use.\n";
        closesocket(serverSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    if (listen(serverSocket, 10) == SOCKET_ERROR) {
        cerr << "Listen failed.\n";
        closesocket(serverSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    cout << "\n>>> Server listening on http://localhost:" << PORT << "\n";
    cout << ">>> Open http://localhost:" << PORT << " in your web browser!\n";
    cout << ">>> Press Ctrl+C in terminal to stop server.\n\n";

    while (true) {
        sockaddr_in clientAddr;
        int clientLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);
        if (clientSocket != INVALID_SOCKET) {
            DWORD timeout = 2500;
            setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
            setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
            handleClientRequest(clientSocket);
        }
    }

    closesocket(serverSocket);
#ifdef _WIN32
    WSACleanup();
#endif
    delete g_system;
    return 0;
}
