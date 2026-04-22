#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <string>
using namespace std;

// ============================================================
//  FILE: structures.h
//  MÔ TẢ: Khai báo toàn bộ struct và prototype hàm
//         cho hệ thống Mini Shopee CLI
// ============================================================


// ============================================================
//  1. STRUCT: Product  (dùng cho BST)
//     Mỗi node trong BST là một sản phẩm.
//     BST được sắp xếp theo `id` → tìm kiếm O(log n)
// ============================================================
struct Product {
    int    id;       // Mã sản phẩm (dùng làm key của BST)
    string name;     // Tên sản phẩm
    double price;    // Giá (VND)
    int    stock;    // Số lượng tồn kho

    // Con trỏ trái / phải của BST
    Product* left;
    Product* right;

    // Constructor: khởi tạo node mới, left/right = nullptr
    Product(int i, string n, double p, int s)
        : id(i), name(n), price(p), stock(s),
          left(nullptr), right(nullptr) {}
};


// ============================================================
//  2. STRUCT: CartItem  (dùng cho Linked List – giỏ hàng)
//     Mỗi node là 1 sản phẩm đã thêm vào giỏ.
//     Linked List cho phép thêm/xóa linh hoạt, không cố định size.
// ============================================================
struct CartItem {
    int    productId;    // ID sản phẩm (để tìm lại trong BST)
    string productName;  // Lưu tên để hiển thị nhanh
    double price;        // Giá tại thời điểm thêm vào giỏ
    int    quantity;     // Số lượng

    CartItem* next;      // Con trỏ đến phần tử kế tiếp

    // Constructor
    CartItem(int id, string name, double p, int qty)
        : productId(id), productName(name), price(p), quantity(qty),
          next(nullptr) {}
};


// ============================================================
//  3. STRUCT: Order  (dùng cho Queue – hàng đợi đơn hàng)
//     Mỗi node là 1 đơn hàng đã được đặt.
//     Queue đảm bảo FIFO: đơn nào đặt trước xử lý trước.
// ============================================================
struct Order {
    int    orderId;      // Mã đơn hàng (tự tăng)
    string itemsSummary; // Tóm tắt sản phẩm trong đơn, vd: "Bút x2, Vở x1"
    double totalPrice;   // Tổng tiền
    string status;       // Trạng thái: "Đang xử lý" / "Hoàn thành"

    Order* next;         // Con trỏ đến đơn hàng kế tiếp trong Queue

    // Constructor
    Order(int id, string summary, double total)
        : orderId(id), itemsSummary(summary), totalPrice(total),
          status("Đang xử lý"), next(nullptr) {}
};


// ============================================================
//  4. STRUCT: ActionNode  (dùng cho Stack – undo)
//     Mỗi node lưu 1 hành động vừa thực hiện.
//     Stack LIFO: hành động cuối cùng được undo trước.
// ============================================================
struct ActionNode {
    string type;       // Loại hành động: "ADD_CART" hoặc "REMOVE_CART"
    int    productId;  // ID sản phẩm liên quan
    int    quantity;   // Số lượng đã thêm/xóa

    ActionNode* next;  // Con trỏ đến hành động trước đó (bên dưới stack)

    // Constructor
    ActionNode(string t, int id, int qty)
        : type(t), productId(id), quantity(qty), next(nullptr) {}
};


// ============================================================
//  5. STRUCT: Cart  (bọc Linked List giỏ hàng)
//     Lưu con trỏ head và đếm số phần tử.
// ============================================================
struct Cart {
    CartItem* head;  // Đầu danh sách
    int       size;  // Số loại sản phẩm trong giỏ

    Cart() : head(nullptr), size(0) {}
};


// ============================================================
//  6. STRUCT: OrderQueue  (bọc Queue đơn hàng)
//     front → lấy ra (dequeue) | rear → thêm vào (enqueue)
// ============================================================
struct OrderQueue {
    Order* front;   // Đầu hàng đợi (xử lý trước)
    Order* rear;    // Cuối hàng đợi (thêm vào sau)
    int    count;   // Tổng số đơn hàng
    int    nextId;  // ID tự tăng cho đơn hàng mới

    OrderQueue() : front(nullptr), rear(nullptr), count(0), nextId(1) {}
};


// ============================================================
//  7. STRUCT: UndoStack  (bọc Stack hành động)
// ============================================================
struct UndoStack {
    ActionNode* top;  // Đỉnh stack (hành động mới nhất)
    int         size; // Số hành động đang lưu

    UndoStack() : top(nullptr), size(0) {}
};


// ============================================================
//  PROTOTYPE — Khai báo hàm (cài đặt trong functions.cpp)
// ============================================================

// --- BST: Quản lý sản phẩm ---
Product* insertProduct(Product* root, int id, string name, double price, int stock);
Product* searchProduct(Product* root, int id);
void     displayProducts(Product* root);
void     loadSampleProducts(Product*& root);
void     freeBST(Product* root);  // Giải phóng bộ nhớ BST

// --- Linked List: Giỏ hàng ---
void   addToCart(Cart& cart, Product* product, int quantity);
void   removeFromCart(Cart& cart, int productId);
void   displayCart(const Cart& cart);
void   clearCart(Cart& cart);
double getCartTotal(const Cart& cart);
bool   isCartEmpty(const Cart& cart);

// --- Stack: Undo ---
void        pushAction(UndoStack& stack, string type, int productId, int quantity);
ActionNode* popAction(UndoStack& stack);
bool        isUndoStackEmpty(const UndoStack& stack);

// --- Queue: Đơn hàng ---
void enqueueOrder(OrderQueue& queue, const Cart& cart);
void displayOrders(const OrderQueue& queue);
bool isQueueEmpty(const OrderQueue& queue);

// --- Tiện ích ---
void printSeparator();   // In dòng kẻ ngang cho đẹp
void printHeader(const string& title);  // In tiêu đề menu

#endif // STRUCTURES_H