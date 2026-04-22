// ============================================================
//  FILE: functions.cpp
//  MÔ TẢ: Cài đặt tất cả hàm cho Mini Shopee CLI
// ============================================================

#include "structures.h"
#include <iostream>
#include <iomanip>   // setw, fixed, setprecision
#include <sstream>   // stringstream (dùng để build chuỗi)
using namespace std;


// ============================================================
//  TIỆN ÍCH
// ============================================================

void printSeparator() {
    cout << string(50, '=') << endl;
}

void printHeader(const string& title) {
    printSeparator();
    cout << "   " << title << endl;
    printSeparator();
}


// ============================================================
//  BST — BINARY SEARCH TREE (Quản lý sản phẩm)
//
//  Cách BST hoạt động:
//    - id < root->id  → đi sang TRÁI
//    - id > root->id  → đi sang PHẢI
//    - id == root->id → đã tồn tại
//
//  Tìm kiếm / Insert: O(log n) trung bình
// ============================================================

// Tạo node Product mới và chèn vào BST theo id
Product* insertProduct(Product* root, int id, string name, double price, int stock) {
    // Nếu cây rỗng hoặc đến vị trí đúng → tạo node mới
    if (root == nullptr) {
        return new Product(id, name, price, stock);
    }

    if (id < root->id) {
        // ID nhỏ hơn → đệ quy sang nhánh trái
        root->left = insertProduct(root->left, id, name, price, stock);
    }
    else if (id > root->id) {
        // ID lớn hơn → đệ quy sang nhánh phải
        root->right = insertProduct(root->right, id, name, price, stock);
    }
    else {
        // ID đã tồn tại → báo lỗi, không thêm
        cout << "[!] San pham voi ID " << id << " da ton tai!\n";
    }

    return root; // Trả về root (không đổi)
}

// Tìm sản phẩm theo ID — trả về nullptr nếu không tìm thấy
Product* searchProduct(Product* root, int id) {
    // Không tìm thấy
    if (root == nullptr) return nullptr;

    // Tìm thấy đúng ID
    if (id == root->id) return root;

    // ID nhỏ hơn → tìm bên trái
    if (id < root->id)
        return searchProduct(root->left, id);

    // ID lớn hơn → tìm bên phải
    return searchProduct(root->right, id);
}

// Hiển thị toàn bộ sản phẩm theo thứ tự ID tăng dần
// Dùng Inorder Traversal: Trái → Gốc → Phải
void displayProducts(Product* root) {
    if (root == nullptr) return; // Điều kiện dừng đệ quy

    displayProducts(root->left);  // Duyệt cây trái trước

    // In thông tin node hiện tại
    cout << left
         << "  [" << setw(3)  << root->id   << "] "
         << setw(20) << root->name
         << "  Gia: " << fixed << setprecision(0)
         << setw(10) << root->price << " VND"
         << "  Kho: " << root->stock << endl;

    displayProducts(root->right); // Duyệt cây phải sau
}

// Nạp sản phẩm mẫu khi khởi động chương trình
void loadSampleProducts(Product*& root) {
    root = insertProduct(root, 101, "But bi Thien Long",   5000,  100);
    root = insertProduct(root, 205, "Vo 200 trang",        15000,  50);
    root = insertProduct(root, 310, "May tinh Casio",     250000,  20);
    root = insertProduct(root, 150, "Tui dung but",        35000,  30);
    root = insertProduct(root, 420, "Sach giao khoa Toan", 45000,  40);
    root = insertProduct(root, 180, "Hop but mau 12 cay",  60000,  25);
    root = insertProduct(root, 330, "Thuoc ke 30cm",        8000,  80);
}

// Giải phóng toàn bộ bộ nhớ BST (gọi khi kết thúc chương trình)
// Dùng Postorder: Trái → Phải → Gốc (xóa con trước, cha sau)
void freeBST(Product* root) {
    if (root == nullptr) return;
    freeBST(root->left);
    freeBST(root->right);
    delete root;
}


// ============================================================
//  LINKED LIST — GIỎ HÀNG (Cart)
//
//  Cách hoạt động:
//    - Thêm vào cuối danh sách (tail insert)
//    - Nếu sản phẩm đã có → tăng quantity
//    - Xóa theo productId → duyệt tìm rồi bỏ qua node đó
// ============================================================

// Thêm sản phẩm vào giỏ hàng
void addToCart(Cart& cart, Product* product, int quantity) {
    // Kiểm tra xem sản phẩm đã có trong giỏ chưa
    CartItem* current = cart.head;
    while (current != nullptr) {
        if (current->productId == product->id) {
            // Đã có → chỉ tăng số lượng
            current->quantity += quantity;
            cout << "[+] Cap nhat so luong: " << product->name
                 << " x" << current->quantity << endl;
            return;
        }
        current = current->next;
    }

    // Chưa có → tạo node mới, thêm vào cuối danh sách
    CartItem* newItem = new CartItem(product->id, product->name,
                                     product->price, quantity);

    if (cart.head == nullptr) {
        // Giỏ rỗng → node mới là head
        cart.head = newItem;
    } else {
        // Duyệt đến cuối danh sách
        CartItem* tail = cart.head;
        while (tail->next != nullptr)
            tail = tail->next;
        tail->next = newItem;
    }

    cart.size++;
    cout << "[+] Da them vao gio: " << product->name
         << " x" << quantity << endl;
}

// Xóa sản phẩm khỏi giỏ hàng theo productId
void removeFromCart(Cart& cart, int productId) {
    if (cart.head == nullptr) {
        cout << "[!] Gio hang dang trong!\n";
        return;
    }

    // Trường hợp xóa node đầu tiên
    if (cart.head->productId == productId) {
        CartItem* toDelete = cart.head;
        cart.head = cart.head->next;
        delete toDelete;
        cart.size--;
        cout << "[-] Da xoa san pham khoi gio hang.\n";
        return;
    }

    // Tìm node đứng TRƯỚC node cần xóa
    CartItem* prev = cart.head;
    while (prev->next != nullptr && prev->next->productId != productId) {
        prev = prev->next;
    }

    if (prev->next == nullptr) {
        // Không tìm thấy
        cout << "[!] Khong tim thay san pham ID " << productId
             << " trong gio hang!\n";
        return;
    }

    // Bỏ qua node cần xóa, nối lại danh sách
    CartItem* toDelete = prev->next;
    prev->next = toDelete->next;
    delete toDelete;
    cart.size--;
    cout << "[-] Da xoa san pham khoi gio hang.\n";
}

// Hiển thị toàn bộ giỏ hàng
void displayCart(const Cart& cart) {
    if (cart.head == nullptr) {
        cout << "  Gio hang dang trong!\n";
        return;
    }

    cout << left
         << setw(5)  << "STT"
         << setw(22) << "Ten san pham"
         << setw(12) << "Don gia"
         << setw(8)  << "SL"
         << setw(14) << "Thanh tien" << endl;
    cout << string(61, '-') << endl;

    CartItem* current = cart.head;
    int stt = 1;
    while (current != nullptr) {
        double subtotal = current->price * current->quantity;
        cout << left
             << setw(5)  << stt++
             << setw(22) << current->productName
             << setw(12) << fixed << setprecision(0) << current->price
             << setw(8)  << current->quantity
             << setw(14) << subtotal << endl;
        current = current->next;
    }

    cout << string(61, '-') << endl;
    cout << "  TONG CONG: " << fixed << setprecision(0)
         << getCartTotal(cart) << " VND\n";
}

// Tính tổng tiền giỏ hàng
double getCartTotal(const Cart& cart) {
    double total = 0;
    CartItem* current = cart.head;
    while (current != nullptr) {
        total += current->price * current->quantity;
        current = current->next;
    }
    return total;
}

// Xóa toàn bộ giỏ hàng (sau khi thanh toán)
void clearCart(Cart& cart) {
    CartItem* current = cart.head;
    while (current != nullptr) {
        CartItem* toDelete = current;
        current = current->next;
        delete toDelete;
    }
    cart.head = nullptr;
    cart.size = 0;
}

// Kiểm tra giỏ hàng có rỗng không
bool isCartEmpty(const Cart& cart) {
    return cart.head == nullptr;
}


// ============================================================
//  STACK — UNDO (Lịch sử hành động)
//
//  Cách hoạt động (LIFO):
//    - pushAction: thêm node vào TOP của stack
//    - popAction:  lấy node từ TOP ra (hành động mới nhất)
// ============================================================

// Đẩy hành động mới lên đỉnh stack
void pushAction(UndoStack& stack, string type, int productId, int quantity) {
    ActionNode* newNode = new ActionNode(type, productId, quantity);
    newNode->next = stack.top; // Node mới trỏ vào top hiện tại
    stack.top = newNode;       // Top bây giờ là node mới
    stack.size++;
}

// Lấy hành động trên đỉnh stack ra (để undo)
// Caller có trách nhiệm delete node sau khi dùng xong
ActionNode* popAction(UndoStack& stack) {
    if (stack.top == nullptr) return nullptr;

    ActionNode* topNode = stack.top;  // Lưu lại node top
    stack.top = stack.top->next;      // Dịch top xuống 1 bậc
    stack.size--;
    topNode->next = nullptr;          // Tách hoàn toàn khỏi stack
    return topNode;
}

// Kiểm tra stack có rỗng không
bool isUndoStackEmpty(const UndoStack& stack) {
    return stack.top == nullptr;
}


// ============================================================
//  QUEUE — ĐƠN HÀNG (Order Queue)
//
//  Cách hoạt động (FIFO):
//    - enqueue: thêm vào REAR (cuối hàng đợi)
//    - display: duyệt từ FRONT → REAR
// ============================================================

// Tạo đơn hàng mới từ giỏ hàng hiện tại, thêm vào cuối Queue
void enqueueOrder(OrderQueue& queue, const Cart& cart) {
    if (isCartEmpty(cart)) {
        cout << "[!] Gio hang trong, khong the tao don hang!\n";
        return;
    }

    // Build chuỗi tóm tắt sản phẩm
    stringstream ss;
    CartItem* current = cart.head;
    while (current != nullptr) {
        ss << current->productName << " x" << current->quantity;
        if (current->next != nullptr) ss << ", ";
        current = current->next;
    }

    // Tạo node Order mới
    Order* newOrder = new Order(queue.nextId++, ss.str(), getCartTotal(cart));

    // Thêm vào cuối Queue
    if (queue.rear == nullptr) {
        // Queue rỗng → node vừa là front vừa là rear
        queue.front = queue.rear = newOrder;
    } else {
        queue.rear->next = newOrder; // Nối vào sau rear hiện tại
        queue.rear = newOrder;       // Cập nhật rear
    }

    queue.count++;
    cout << "\n[OK] Dat hang thanh cong! Ma don hang: #"
         << newOrder->orderId << endl;
    cout << "     Tong tien: " << fixed << setprecision(0)
         << newOrder->totalPrice << " VND\n";
}

// Hiển thị toàn bộ đơn hàng trong Queue (từ front đến rear)
void displayOrders(const OrderQueue& queue) {
    if (queue.front == nullptr) {
        cout << "  Chua co don hang nao!\n";
        return;
    }

    Order* current = queue.front;
    int idx = 1;
    while (current != nullptr) {
        cout << "  Don #" << current->orderId
             << " | " << current->status
             << "\n  San pham: " << current->itemsSummary
             << "\n  Tong tien: " << fixed << setprecision(0)
             << current->totalPrice << " VND\n";
        cout << string(50, '-') << endl;
        current = current->next;
        idx++;
    }
    cout << "  Tong so don hang: " << queue.count << endl;
}

// Kiểm tra Queue có rỗng không
bool isQueueEmpty(const OrderQueue& queue) {
    return queue.front == nullptr;
}
