#pragma once
#ifndef RBT_TREE
#define RBT_TREE

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <utility>
#include <tuple>
#include <format>
#include <type_traits>
#include <compare>

#include "CommonTypes.h"

enum class Color { RED, BLACK };

template<typename T>
struct RBTNode
{
    T data;
    RBTNode<T>* parent;
    RBTNode<T>* left;
    RBTNode<T>* right;
    std::size_t index;
    Color color;

    RBTNode(T val, std::size_t idx)
        : data(std::move(val)), parent(nullptr), left(nullptr), right(nullptr), index(idx), color(Color::RED) {}

    // Konstruktor dedykowany dla węzła strażnika (TNULL)
    RBTNode()
        : data(), parent(nullptr), left(nullptr), right(nullptr), index(0), color(Color::BLACK) {}
};

template<typename T>
class RBT
{
private:
    RBTNode<T>* root;
    RBTNode<T>* TNULL; // Węzeł strażniczy zastępujący nullptr
    std::size_t current_size;
    std::size_t next_index;

    void initialize_TNULL();
    void left_rotate(RBTNode<T>*);
    void right_rotate(RBTNode<T>*);
    void insert_fixup(RBTNode<T>*);
    void transplant(RBTNode<T>*, RBTNode<T>*);
    void remove_fixup(RBTNode<T>*);

    void clear_node(RBTNode<T>*, bool);
    std::size_t get_height(RBTNode<T>*) const;
    void pre_order_traversal(RBTNode<T>*, std::vector<T>&) const;
    void in_order_traversal(RBTNode<T>*, std::vector<T>&) const;
    RBTNode<T>* find_minimum(RBTNode<T>*);
    void build_string_pre_order(RBTNode<T>*, std::ostringstream&) const;

public:
    RBT() : current_size(0), next_index(1) {
        initialize_TNULL();
        root = TNULL;
    }

    ~RBT() {
        clear(true);
        delete TNULL;
    }

    template <typename Comparator>
    void add(const T&, Comparator);

    template <typename Comparator>
    RBTNode<T>* find(const T&, Comparator);

    void remove(RBTNode<T>*);
    std::vector<T> pre_order() const;
    std::vector<T> in_order() const;
    void clear(bool free_data = true);
    std::size_t height() const;
    std::string to_string() const;
};

//----------------------------------------------------------------------------------
//                               MECHANIZMY RBT
//----------------------------------------------------------------------------------

//Inicjalizacja węzła NIL (tzw. strażnik)
template<typename T>
void RBT<T>::initialize_TNULL() {
    TNULL = new RBTNode<T>();       //Nowy węzeł TNULL
    TNULL->color = Color::BLACK;    //TNULL ma kolor czarny
    TNULL->left = nullptr;          //Lewe dziecko jest nullptr
    TNULL->right = nullptr;         //Prawe dziecko jests nullptr
}

//Rotacja drzewa w lewo
template<typename T>
void RBT<T>::left_rotate(RBTNode<T>* x) {
    RBTNode<T>* y = x->right;               //y wskazuje na prawe dziecko węzła x
    x->right = y->left;                     //lewe dziecko y staje się prawym dzieckiem x
    if (y->left != TNULL) {                 //jeśli lewe dziecko y istnieje (nie jest TNULL)
        y->left->parent = x;                //rodzicem lewego dziecka y staje się x
    }
    y->parent = x->parent;                  //rodzicem y staje się dotychczasowy rodzic x
    if (x->parent == TNULL) {               //jeśli x był korzeniem (nie miał rodzica)
        root = y;                           //y staje się nowym korzeniem drzewa
    }
    else if (x == x->parent->left) {        //jeśli x był lewym dzieckiem swojego rodzica
        x->parent->left = y;                //y zajmuje miejsce x jako lewe dziecko
    }
    else {
        x->parent->right = y;               //y zajmuje miejsce x jako prawe dziecko
    }
    y->left = x;                            //x staje się lewym dzieckiem y
    x->parent = y;                          //y staje się nowym rodzicem x
}


//Rotacja drzewa w prawo
template<typename T>
void RBT<T>::right_rotate(RBTNode<T>* x) {
    RBTNode<T>* y = x->left;                //y wskazuje na lewe dziecko węzła x
    x->left = y->right;                     //lewe dziecko x wskazuje na prawe dziecko y
    if (y->right != TNULL) {                //jeżeli prawe dziecko y jest różne od TNULL
        y->right->parent = x;               //rodzicem prawego dziecka staje się x
    }
    y->parent = x->parent;                  //rodzicem y staje się rodzic x
    if (x->parent == TNULL) {               //jeśli rodzic x jest równy TNULL
        root = y;                           //korzeń staje się y
    }  
    else if (x == x->parent->right) {       //jeśli x jest równy prawym dzieckiem x
        x->parent->right = y;               //prawe dziecko x jest równe y
    }
    else {                                  //w przeciwnym wypadku
        x->parent->left = y;                //lewe dziecko x jest równe y
    }
    y->right = x;                           //prawe dziecko y jest równe x
    x->parent = y;                          //rodzic x jest równy y
}


// Balansowanie drzewa po dodaniu węzła
template<typename T>
void RBT<T>::insert_fixup(RBTNode<T>* k) {
    RBTNode<T>* u;                                      //wskaźnik węzła RBT (posłuży jako wujek)
    while (k->parent->color == Color::RED) {            //rób do czasu kiedy kolor rodzica ma kolor Czerwony
        if (k->parent == k->parent->parent->right) {    //jeśli rodzic węzła k jest prawym dzieckiem dziadka
            u = k->parent->parent->left;                //u wskazuje na wujka (lewe dziecko dziadka)

            if (u->color == Color::RED) {               //jeżeli u ma kolor Czerwony
                u->color = Color::BLACK;                //to przekoloruj go na Czarny
                k->parent->color = Color::BLACK;        //przekoloruj kolor rodzica na Czarny
                k->parent->parent->color = Color::RED;  //przekoloruj kolor dziadka na Czerwony
                k = k->parent->parent;                  //k bedzie posiadał wartość dziadka
            }
            else {                                      //w przeciwnym wypadku, gdy wujek (u) jest czarny
                if (k == k->parent->left) {             //jeśli k jest lewym dzieckiem swojego rodzica
                    k = k->parent;                      //k wskazuje na rodzica
                    right_rotate(k);                    //węzeł k jest rotowany w prawo
                }
                k->parent->color = Color::BLACK;        //kolorowanie węzła rodzica na Czarno
                k->parent->parent->color = Color::RED;  //kolorowanie węzła dziadka na Czerwono
                left_rotate(k->parent->parent);         //obracanie w lewo węzła dziadka
            }
        }
        else {                                          //kiedy rodzic węzła k jest lewym dzieckiem dziadka
            u = k->parent->parent->right;               //u wskazuje na wujka (prawe dziecko dziadka)

            if (u->color == Color::RED) {               //jeśli u jest koloru czerwonego
                u->color = Color::BLACK;                //przekolorowanie węzła u na kolor czarny
                k->parent->color = Color::BLACK;        //przekolowanie węzła rodzica k na czarno
                k->parent->parent->color = Color::RED;  //przekolorowanie węzeł dziadka k na czerwono
                k = k->parent->parent;                  //wskazywanie k na dziadka
            }
            else {                                      //kiedy wujek (u) jest czarny
                if (k == k->parent->right) {            //jeśli k jest prawym dzieckiem swojego rodzica
                    k = k->parent;                      //k wskazuje na rodzica
                    left_rotate(k);                     //rotowanie k w lewo
                }
                k->parent->color = Color::BLACK;        //przekolorowanie rodzica k na Czarno
                k->parent->parent->color = Color::RED;  //przekolorowanie dziadak k na Czerwono
                right_rotate(k->parent->parent);        //rotowanie dziadka k w prawo
            }
        }

        if (k == root) {                                //jeśli k jest równe korzeniowi
            break;                                      //zakończ pętlę
        }
    }
    root->color = Color::BLACK;                         //przekoloruj korzeń na czarno
}



template<typename T>
void RBT<T>::transplant(RBTNode<T>* u, RBTNode<T>* v) {
    if (u->parent == TNULL) {
        root = v;
    }
    else if (u == u->parent->left) {
        u->parent->left = v;
    }
    else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

//balansowanie drzewa po usunięciu węzła
template<typename T>
void RBT<T>::remove_fixup(RBTNode<T>* x) {
    RBTNode<T>* s;                                             // pusty węzeł s
    while (x != root && x->color == Color::BLACK) {             // działaj o czasu kiedy root jesst różny od x i kolor x jest CZARNY
        if (x == x->parent->left) {                             // jeśli x jest równy lewemu dzieku rodzica x
            s = x->parent->right;                               // s przejmuje wartość prawego dziecka rodzica x
            if (s->color == Color::RED) {                       // jeśli kolor s jest równy RED
                s->color = Color::BLACK;                        // zmieniamy kolor na CZARNY
                x->parent->color = Color::RED;                  // a kolor rodzica na RED
                left_rotate(x->parent);                         // rotujemy x->parent
                s = x->parent->right;                           // s ponownie przejmuje wartość prawego dziecka rodzica x
            }
            if (s->left->color == Color::BLACK && s->right->color == Color::BLACK) {   // jeśli lewe dziecko s ma kolor CZARNY i prawe ma kolor CZARNY
                s->color = Color::RED; // rodzic zmienia na kolor czerwony
                x = x->parent;  // x wskazuje na rodzica
            }
            else {
                if (s->right->color == Color::BLACK) {  // jeśli prawe dziecko s ma kolor CZARNY
                    s->left->color = Color::BLACK;      // zmieniamy kolor lewego dziecka na CZARNY
                    s->color = Color::RED;              // zmieniamy kolor rodzica na CZERWONY
                    right_rotate(s);                    // rotujemy na prawo węzeł s
                    s = x->parent->right;               // s wskazuje na prawego potomka rodzica x
                }
                s->color = x->parent->color;            // kolor węzła s przejmuje kolor rodzica x
                x->parent->color = Color::BLACK;        // rodzic x jest kolorowany na czarno
                s->right->color = Color::BLACK;         // prawe dziecko s jest kolorowane na czarno
                left_rotate(x->parent);                 // rodzic x jest rotowany na lewo
                x = root;                               // x jest równy korzeniowi
            }
        }
        else {
            s = x->parent->left;
            if (s->color == Color::RED) {
                s->color = Color::BLACK;
                x->parent->color = Color::RED;
                right_rotate(x->parent);
                s = x->parent->left;
            }
            if (s->right->color == Color::BLACK && s->left->color == Color::BLACK) {
                s->color = Color::RED;
                x = x->parent;
            }
            else {
                if (s->left->color == Color::BLACK) {
                    s->right->color = Color::BLACK;
                    s->color = Color::RED;
                    left_rotate(s);
                    s = x->parent->left;
                }
                s->color = x->parent->color;
                x->parent->color = Color::BLACK;
                s->left->color = Color::BLACK;
                right_rotate(x->parent);
                x = root;
            }
        }
    }
    x->color = Color::BLACK;
}

//----------------------------------------------------------------------------------
//                               IMPLEMENTACJA METOD
//----------------------------------------------------------------------------------

template<typename T>
void RBT<T>::clear_node(RBTNode<T>* node, bool free_data) {
    if (node != TNULL && node != nullptr) {
        clear_node(node->left, free_data);
        clear_node(node->right, free_data);
        if constexpr (std::is_pointer_v<T>) {
            if (free_data) delete node->data;
        }
        delete node;
    }
}

template<typename T>
std::size_t RBT<T>::get_height(RBTNode<T>* node) const {
    if (node == TNULL || node == nullptr) return 0;
    auto left_h = get_height(node->left);
    auto right_h = get_height(node->right);
    return 1 + std::max(left_h, right_h);
}

template<typename T>
void RBT<T>::pre_order_traversal(RBTNode<T>* node, std::vector<T>& result) const {
    if (node != TNULL) {
        result.push_back(node->data);
        pre_order_traversal(node->left, result);
        pre_order_traversal(node->right, result);
    }
}

template<typename T>
void RBT<T>::in_order_traversal(RBTNode<T>* node, std::vector<T>& result) const {
    if (node != TNULL) {
        in_order_traversal(node->left, result);
        result.push_back(node->data);
        in_order_traversal(node->right, result);
    }
}

template<typename T>
RBTNode<T>* RBT<T>::find_minimum(RBTNode<T>* node) {
    while (node->left != TNULL) {
        node = node->left;
    }
    return node;
}

template<typename T>
void RBT<T>::build_string_pre_order(RBTNode<T>* node, std::ostringstream& oss) const
{
    if (node != TNULL) {
        oss << "(" << node->index << ": [p: ";

        if (node->parent != TNULL) oss << node->parent->index;
        else oss << "NULL";

        oss << ", l: ";
        if (node->left != TNULL) oss << node->left->index;
        else oss << "NULL";

        oss << ", r: ";
        if (node->right != TNULL) oss << node->right->index;
        else oss << "NULL";

        oss << "], color: " << (node->color == Color::RED ? "RED" : "BLACK");
        oss << ", data: ";

        if constexpr (std::is_pointer_v<T>) {
            if (node->data) oss << *(node->data);
            else oss << "NULL";
        }
        else {
            oss << node->data;
        }

        oss << "),\n";

        build_string_pre_order(node->left, oss);
        build_string_pre_order(node->right, oss);
    }
}


//dodanie węzła z odpowiednimi zasadami z RBT
template<typename T>
template <typename Comparator>
void RBT<T>::add(const T& data, Comparator cmp) {
    auto* new_node = new RBTNode<T>(data, next_index++);
    new_node->left = TNULL;
    new_node->right = TNULL;

    RBTNode<T>* current = root;
    RBTNode<T>* parent = TNULL;

    while (current != TNULL) {
        parent = current;
        if (cmp(data, current->data) < 0) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }

    new_node->parent = parent;
    if (parent == TNULL) {
        root = new_node;
    }
    else if (cmp(data, parent->data) < 0) {
        parent->left = new_node;
    }
    else {
        parent->right = new_node;
    }

    if (new_node->parent == TNULL) {
        new_node->color = Color::BLACK;
        current_size++;
        return;
    }

    if (new_node->parent->parent == TNULL) {
        current_size++;
        return;
    }

    insert_fixup(new_node);
    current_size++;
}

template<typename T>
template <typename Comparator>
RBTNode<T>* RBT<T>::find(const T& data, Comparator cmp) {
    RBTNode<T>* current = root;
    while (current != TNULL) {
        auto cmp_result = cmp(data, current->data);
        if (cmp_result == 0) {
            return current;
        }
        else if (cmp_result < 0) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }
    return nullptr;
}

//usuwanie węzła zgodnie z zasadami RBT
template<typename T>
void RBT<T>::remove(RBTNode<T>* z) {
    if (z == TNULL || z == nullptr) return;

    RBTNode<T>* y = z;
    RBTNode<T>* x;
    Color y_original_color = y->color;

    if (z->left == TNULL) {
        x = z->right;
        transplant(z, z->right);
    }
    else if (z->right == TNULL) {
        x = z->left;
        transplant(z, z->left);
    }
    else {
        y = find_minimum(z->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        }
        else {
            transplant(y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    delete z;
    current_size--;

    if (y_original_color == Color::BLACK) {
        remove_fixup(x);
    }
}

template<typename T>
std::vector<T> RBT<T>::pre_order() const {
    std::vector<T> result;
    pre_order_traversal(root, result);
    return result;
}

template<typename T>
std::vector<T> RBT<T>::in_order() const {
    std::vector<T> result;
    in_order_traversal(root, result);
    return result;
}

template<typename T>
void RBT<T>::clear(bool free_data) {
    clear_node(root, free_data);
    root = TNULL;
    current_size = 0;
    next_index = 1;
}

template<typename T>
std::size_t RBT<T>::height() const {
    return get_height(root);
}

template<typename T>
std::string RBT<T>::to_string() const {
    std::ostringstream oss;

    oss << "red-black tree:\n";
    oss << "size: " << current_size << "\n";
    oss << "height: " << height() << "\n";
    oss << "{\n";

    build_string_pre_order(root, oss);

    oss << "}\n";

    return oss.str();
}

#endif // !RBT_TREE