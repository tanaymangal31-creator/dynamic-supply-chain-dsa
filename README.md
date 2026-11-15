# Dynamic Supply Chain Optimization & Intelligent Delivery Network

An algorithmic framework for supply chain optimization, route planning, order dispatching, and inventory management built from scratch in **pure C++ without any Standard Template Library (Zero STL)**, paired with an interactive single-page dashboard.

> **Targeted Syllabus:** B.Tech Computer Science & Engineering (3rd Semester DSA • Course Code: 15B11CI311)  
> **Backend:** Pure C++ (`backend.cpp`) with custom memory management and handcrafted data structures  
> **Frontend:** Single-file interactive web dashboard (`index.html`) using HTML, CSS, and vanilla JavaScript  

---

## 🌟 Key Highlights & Engineering Philosophy

- **Zero STL Backend**: Implemented using `#include <iostream>`, `using namespace std;`, and `<winsock2.h>` for local socket communication. Absolutely no `<vector>`, `<string>`, `<algorithm>`, `<queue>`, `<stack>`, or `<map>`.
- **Integrated Umbrella Architecture**: Every single data structure fulfills a natural real-world requirement of a modern distribution network (such as Amazon/Flipkart logistics).
- **Single Dashboard**: Clean visual interface with live route discovery, graph structure inspection (Adjacency Matrix & List), order queues, BST traversals, and search/sort benchmarking.

---

## 🧩 Complete DSA Syllabus Mapping

| Module / Topic | Data Structure / Algorithm | Practical Supply Chain Functionality | Time & Space Complexity |
| :--- | :--- | :--- | :--- |
| **Graph Representation** | **Graph** (Adjacency List & Matrix) | Multi-tier distribution network (Suppliers &rarr; Warehouses &rarr; Transit Hubs &rarr; Consumer Cities) | Space: $O(V + E)$ |
| **Shortest Route** | **Dijkstra's Algorithm** | Computes the shortest road distance / delivery corridor between depots and destination cities | $O((V + E) \log V)$ |
| **Layered Search** | **BFS** (Breadth-First Search) | Explores closest transit hubs layer-by-layer to minimize transit hops | $O(V + E)$ |
| **Route Connectivity** | **DFS** (Depth-First Search) | Deep traversal to confirm route reachability between suppliers and customer zones | $O(V + E)$ |
| **Emergency Orders** | **Priority Queue (Binary Max-Heap)** | Critical medical & express orders jump to the top of the dispatch line ($O(1)$ peek) | Insert: $O(\log N)$, Extract-Max: $O(\log N)$ |
| **Standard Orders** | **FIFO Queue** | Processes standard retail customer orders strictly in arrival order | Enqueue: $O(1)$, Dequeue: $O(1)$ |
| **Undo Operations** | **Stack (LIFO Structure)** | Dispatched orders are pushed to stack; "Undo Dispatch" pops the last action and restores inventory | Push: $O(1)$, Pop: $O(1)$ |
| **Active Shipments** | **Singly Linked List** | Dynamic tracking chain of orders in transit (`Node -> Next -> NULL`) with dynamic insertion/deletion | Insertion: $O(1)$, Traversal: $O(N)$ |
| **Product Lookup** | **Hash Table (Chaining)** | Instant SKU key-value lookup with separate chaining collision handling | Lookup: $O(1)$ average |
| **Warehouse Tree** | **Binary Search Tree (BST)** | Organizes depots hierarchically by stock capacity with live **Inorder**, **Preorder**, and **Postorder** traversals | Search: $O(\log N)$, Inorder: $O(N)$ (sorted) |
| **Product Autocomplete**| **Trie (Prefix Tree)** | Fast prefix suggestions as users type product names | $O(L)$ where $L$ is query length |
| **Catalog Sorting** | **Merge Sort** & **Quick Sort** | Sorts inventory catalog by Price or Stock with step counters and execution benchmarking | $O(N \log N)$ |
| **Catalog Searching** | **Binary Search** vs **Linear Search** | Compares $O(N)$ sequential scan against $O(\log N)$ divide-and-conquer search | $O(\log N)$ vs $O(N)$ |

---

## 🚀 Quick Start Guide

### 1. Compile the C++ Backend (Zero STL)

Open a terminal in the project directory:

**Windows (MinGW GCC):**
```bash
g++ -std=c++14 -O2 backend.cpp -o backend.exe -lws2_32
./backend.exe
```

**Linux / macOS:**
```bash
g++ -std=c++14 -O2 backend.cpp -o backend
./backend
```

### 2. Access the Dashboard

Once the backend is running (`>>> Server listening on http://localhost:8080`):
- Open your browser and navigate to: **`http://localhost:8080/`**
- *Note:* You can also directly double-click **`index.html`** in any browser. It includes a built-in client simulation engine that functions seamlessly even offline.

---

## 🎯 Viva & Evaluation Talking Points

1. **Why is this built as one cohesive umbrella project instead of isolated mini-programs?**
   > *"In an enterprise supply chain, data structures are deeply interconnected. An order arrives and needs instant catalog lookup (Hash Table), urgent orders require priority dispatching (Max-Heap), routes require shortest-path calculations (Dijkstra), depot stock is organized hierarchically (BST), and operations require reversible rollback (Stack). Each structure is utilized exactly where its algorithmic properties shine."*

2. **Why build custom data structures without using the C++ STL?**
   > *"Implementing `MyString`, `MyVector<T>`, `MyStack<T>`, `MyQueue<T>`, `MaxHeap<T>`, `HashTable`, and `Graph` from scratch demonstrates complete mastery over manual dynamic memory allocation (`new`/`delete`), pointers, template metaprogramming, and algorithmic complexity."*

3. **How does the Max-Heap interact with the FIFO Queue?**
   > *"Orders with priority &ge; 50 (such as urgent hospital deliveries) are inserted into the Binary Max-Heap using `heapifyUp`. Standard economy orders are enqueued into the FIFO Queue. The dispatch controller always pulls from the root of the Max-Heap first ($O(1)$ access). Only when the heap is empty does it dequeue from the FIFO queue."*

---

## 📁 Repository Structure

```text
├── backend.cpp     # 100% Pure C++ backend server (Zero STL, custom DS)
├── index.html      # Responsive single-page web dashboard (HTML+CSS+JS)
├── README.md       # Project overview, DSA mapping, and viva guide
└── .gitignore      # Git ignore rules for build artifacts
```

---

## 📜 License
Developed as an academic Computer Science & Engineering 3rd Semester Data Structures project. Free for educational and learning purposes.
