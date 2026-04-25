// ============================================================
//  FILE: main.cpp
//  MÔ TẢ: Menu CLI chính cho hệ thống Mini Shopee
//  Compile: g++ -std=c++17 -o shopee main.cpp functions.cpp
// ============================================================

#include "structures.h"
#include <iostream>
#include <limits>    // numeric_limits (dùng để clear input buffer)
using namespace std;


// ============================================================
//  HÀM TIỆN ÍCH — XỬ LÝ INPUT AN TOÀN
// ============================================================

// Đọc số nguyên, không crash khi nhập chữ
// Nếu nhập sai → yêu cầu nhập lại
int readInt(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail()) {
            // Nhập không phải số → xóa lỗi, bỏ qua dòng còn lại
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  [!] Vui long nhap so nguyen hop le!\n";
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
    }
}

// Đọc số nguyên dương (> 0)
int readPositiveInt(const string& prompt) {
    int value;
    while (true) {
        value = readInt(prompt);
        if (value > 0) return value;
        cout << "  [!] Vui long nhap so lon hon 0!\n";
    }
}


// ============================================================
//  CÁC CHỨC NĂNG MENU
// ============================================================

// ── MENU 1: Xem danh sách sản phẩm ──────────────────────────
void menuXemSanPham(Product* root) {
    printHeader("DANH SACH SAN PHAM");
    if (root == nullptr) {
        cout << "  Chua co san pham nao!\n";
    } else {
        displayProducts(root);
    }
    printSeparator();
}

// ── MENU 2: Thêm sản phẩm mới vào hệ thống (BST) ────────────
void menuThemSanPham(Product*& root) {
    printHeader("THEM SAN PHAM MOI");

    int    id    = readPositiveInt("  Nhap ID san pham : ");
    // Kiểm tra ID đã tồn tại chưa
    if (searchProduct(root, id) != nullptr) {
        cout << "  [!] ID " << id << " da ton tai! Vui long chon ID khac.\n";
        return;
    }

    cout << "  Nhap ten san pham: ";
    string name;
    getline(cin, name);
    if (name.empty()) {
        cout << "  [!] Ten san pham khong duoc de trong!\n";
        return;
    }

    double price;
    while (true) {
        cout << "  Nhap gia (VND)  : ";
        cin >> price;
        if (cin.fail() || price <= 0) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  [!] Gia khong hop le!\n";
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
    }

    int stock = readPositiveInt("  Nhap so luong kho: ");

    root = insertProduct(root, id, name, price, stock);
    cout << "  [OK] Da them san pham: " << name << " (ID: " << id << ")\n";
    printSeparator();
}

// ── MENU 3: Thêm sản phẩm vào giỏ hàng ─────────────────────
void menuThemVaoGio(Product* root, Cart& cart, UndoStack& undoStack) {
    printHeader("THEM VAO GIO HANG");

    if (root == nullptr) {
        cout << "  [!] Chua co san pham nao trong he thong!\n";
        return;
    }

    // Hiển thị sản phẩm để người dùng chọn
    cout << "  --- Danh sach san pham ---\n";
    displayProducts(root);
    printSeparator();

    int id = readPositiveInt("  Nhap ID san pham muon them: ");

    // Tìm sản phẩm trong BST
    Product* product = searchProduct(root, id);
    if (product == nullptr) {
        cout << "  [!] Khong tim thay san pham ID " << id << "!\n";
        return;
    }

    // Kiểm tra tồn kho
    if (product->stock == 0) {
        cout << "  [!] San pham '" << product->name << "' da het hang!\n";
        return;
    }

    cout << "  Tim thay: " << product->name
         << " | Gia: " << product->price
         << " VND | Kho: " << product->stock << endl;

    int qty = readPositiveInt("  Nhap so luong: ");

    // Kiểm tra đủ hàng không
    if (qty > product->stock) {
        cout << "  [!] So luong yeu cau (" << qty
             << ") vuot qua so luong kho (" << product->stock << ")!\n";
        return;
    }

    // Thêm vào giỏ và cập nhật tồn kho
    addToCart(cart, product, qty);
    product->stock -= qty;  // Trừ tồn kho

    // Lưu hành động vào Stack để có thể Undo
    pushAction(undoStack, "ADD_CART", product->id, qty);

    printSeparator();
}

// ── MENU 4: Xem giỏ hàng ────────────────────────────────────
void menuXemGioHang(const Cart& cart) {
    printHeader("GIO HANG CUA BAN");
    displayCart(cart);
    printSeparator();
}

// ── MENU 5: Thanh toán ──────────────────────────────────────
void menuThanhToan(Cart& cart, OrderQueue& orderQueue, UndoStack& undoStack) {
    printHeader("THANH TOAN");

    if (isCartEmpty(cart)) {
        cout << "  [!] Gio hang trong! Vui long them san pham truoc.\n";
        return;
    }

    // Hiển thị lại giỏ hàng trước khi xác nhận
    cout << "  Gio hang hien tai:\n";
    displayCart(cart);

    cout << "\n  Ban co chac chan muon thanh toan? (y/n): ";
    char confirm;
    cin >> confirm;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (confirm != 'y' && confirm != 'Y') {
        cout << "  [!] Da huy thanh toan.\n";
        return;
    }

    // Tạo đơn hàng và đưa vào Queue
    enqueueOrder(orderQueue, cart);

    // Xóa giỏ hàng sau khi đặt hàng
    clearCart(cart);

    // Xóa lịch sử undo (vì giỏ đã reset, undo không còn ý nghĩa)
    while (!isUndoStackEmpty(undoStack)) {
        ActionNode* node = popAction(undoStack);
        delete node;
    }

    cout << "  [OK] Cam on ban da mua hang!\n";
    printSeparator();
}

// ── MENU 6: Undo thao tác gần nhất ──────────────────────────
void menuUndo(Cart& cart, Product* root, UndoStack& undoStack) {
    printHeader("UNDO THAO TAC GAN NHAT");

    if (isUndoStackEmpty(undoStack)) {
        cout << "  [!] Khong co thao tac nao de undo!\n";
        return;
    }

    // Lấy hành động gần nhất từ Stack
    ActionNode* action = popAction(undoStack);

    if (action->type == "ADD_CART") {
        cout << "  [UNDO] Hoan tac: Da them san pham ID "
             << action->productId << " x" << action->quantity
             << " vao gio hang.\n";

        // Xóa khỏi giỏ hàng
        removeFromCart(cart, action->productId);

        // Hoàn lại tồn kho
        Product* product = searchProduct(root, action->productId);
        if (product != nullptr) {
            product->stock += action->quantity;
            cout << "  [OK] Da hoan lai " << action->quantity
                 << " vao kho san pham ID " << action->productId << ".\n";
        }
    }

    delete action; // Giải phóng bộ nhớ
    printSeparator();
}

// ── MENU 7: Xem danh sách đơn hàng ──────────────────────────
void menuXemDonHang(const OrderQueue& orderQueue) {
    printHeader("DANH SACH DON HANG");
    displayOrders(orderQueue);
    printSeparator();
}


// ============================================================
//  MAIN — MENU CHÍNH
// ============================================================
int main() {
    // Khởi tạo các cấu trúc dữ liệu
    Product*    productRoot = nullptr;  // BST rỗng
    Cart        cart;                   // Giỏ hàng rỗng
    OrderQueue  orderQueue;             // Hàng đợi đơn hàng rỗng
    UndoStack   undoStack;              // Stack undo rỗng

    // Nạp dữ liệu mẫu vào BST
    loadSampleProducts(productRoot);

    cout << "\n";
    printHeader("CHAO MUNG DEN VOI MINI SHOPEE CLI");
    cout << "  He thong da san sang! Co " << "7" << " san pham mau.\n";
    printSeparator();

    int choice = 0;

    // Vòng lặp menu chính
    do {
        cout << "\n";
        printHeader("MENU CHINH");
        cout << "  1. Xem danh sach san pham\n";
        cout << "  2. Them san pham moi (BST)\n";
        cout << "  3. Them san pham vao gio hang\n";
        cout << "  4. Xem gio hang\n";
        cout << "  5. Thanh toan\n";
        cout << "  6. Undo thao tac gan nhat\n";
        cout << "  7. Xem danh sach don hang\n";
        cout << "  0. Thoat\n";
        printSeparator();

        choice = readInt("  Chon chuc nang (0-7): ");

        cout << "\n";

        switch (choice) {
            case 1:
                menuXemSanPham(productRoot);
                break;
            case 2:
                menuThemSanPham(productRoot);
                break;
            case 3:
                menuThemVaoGio(productRoot, cart, undoStack);
                break;
            case 4:
                menuXemGioHang(cart);
                break;
            case 5:
                menuThanhToan(cart, orderQueue, undoStack);
                break;
            case 6:
                menuUndo(cart, productRoot, undoStack);
                break;
            case 7:
                menuXemDonHang(orderQueue);
                break;
            case 0:
                cout << "  Tam biet! Hen gap lai.\n\n";
                break;
            default:
                cout << "  [!] Lua chon khong hop le! Vui long chon 0-7.\n";
                break;
        }

    } while (choice != 0);

    // Giải phóng toàn bộ bộ nhớ trước khi thoát
    freeBST(productRoot);
    clearCart(cart);

    // Giải phóng Queue
    Order* curr = orderQueue.front;
    while (curr != nullptr) {
        Order* tmp = curr;
        curr = curr->next;
        delete tmp;
    }

    // Giải phóng Stack
    while (!isUndoStackEmpty(undoStack)) {
        ActionNode* node = popAction(undoStack);
        delete node;
    }

    return 0;
}
