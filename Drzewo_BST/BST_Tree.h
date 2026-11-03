#pragma once
#ifndef BST_TREE
#define BST_TREE

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

template<typename T>
struct BSTNode
{
    T data;
    BSTNode<T>* parent;
    BSTNode<T>* left;
    BSTNode<T>* right;
    std::size_t index;

    BSTNode(T val, std::size_t idx)
        : data(std::move(val)), parent(nullptr), left(nullptr), right(nullptr), index(idx) {}
};

template<typename T>
class BST
{
private:
    BSTNode<T>* root; 
    std::size_t current_size;
    std::size_t next_index;

    void clear_node(BSTNode<T>*, bool);
    std::size_t get_height(BSTNode<T>*) const;
    void pre_order_traversal(BSTNode<T>*, std::vector<T>&) const;
    void in_order_traversal(BSTNode<T>*, std::vector<T>&) const;
    BSTNode<T>* find_minimum(BSTNode<T>*); 
    void build_string_pre_order(BSTNode<T>*, std::ostringstream&) const;

public:
    BST() : root(nullptr), current_size(0), next_index(0) {}
    ~BST() { clear(true); }

    template <typename Comparator>
    void add(const T&, Comparator);

    template <typename Comparator>
    BSTNode<T>* find(const T&, Comparator);

    void remove(BSTNode<T>*);
    std::vector<T> pre_order() const;
    std::vector<T> in_order() const;
    void clear(bool free_data = true);
    std::size_t height() const;
    std::string to_string() const;
};

//----------------------------------------------------------------------------------
//                               IMPLEMENTACJA METOD
//----------------------------------------------------------------------------------


// Usuwanie węzła
template<typename T>
void BST<T>::clear_node(BSTNode<T>* node, bool free_data) {
    if (node != nullptr) {                                  // jeśli node jest różne od nullptr
        clear_node(node->left, free_data);                  // uruchom rekurencyjnie clear node na lewym węźle
        clear_node(node->right, free_data);                 // uruchom rekurencyjnie clear node na prawym węźle
        if constexpr (std::is_pointer_v<T>) {               // warunek sprawdzający że T jesst wskaźnikiem
            if (free_data) delete node->data;               // warunek sprawdzający czy free_data jest true, then usuwanie zawartości node->data
        }
        delete node;                                        // usuwanie węzła
    }
}


//pobierz maksymalną wysokość (głębokość) drzewa.
template<typename T>
std::size_t BST<T>::get_height(BSTNode<T>* node) const {
    if (node == nullptr) return 0;                          //jeśli node wynosi nullptr to zakończ funkcje (warunek stopu)
    auto left_h = get_height(node->left);                   //przejście rekurencyjnie do lewego węzła
    auto right_h = get_height(node->right);                 //przejście rekurencyjnie do prawego węzła
    return 1 + (std::max)(left_h, right_h);                 //pobiera maksymalną wartość między left_h a right_h
}


template<typename T>
void BST<T>::pre_order_traversal(BSTNode<T>* node, std::vector<T>& result) const {
    if (node != nullptr) {
        result.push_back(node->data);
        pre_order_traversal(node->left, result);
        pre_order_traversal(node->right, result);
    }
}

template<typename T>
void BST<T>::in_order_traversal(BSTNode<T>* node, std::vector<T>& result) const {
    if (node != nullptr) {
        in_order_traversal(node->left, result);
        result.push_back(node->data);
        in_order_traversal(node->right, result);
    }
}

//Minimalny węzeł binarny
template<typename T>
BSTNode<T>* BST<T>::find_minimum(BSTNode<T>* node) {
    while (node->left != nullptr) {
        node = node->left;                  //maksymalne schodzenie w skrajne lewo drzewa binarnego
    }
    return node;                            //będzie zwrócone jak skończy się while
}


//Łańcuchowe pokazanie drzewa binarnego
template<typename T>
void BST<T>::build_string_pre_order(BSTNode<T>* node, std::ostringstream& oss) const
{
    if (node != nullptr) {
        oss << "(" << node->index << ": [p: ";

        if (node->parent != nullptr) oss << node->parent->index;
        else oss << "NULL";

        oss << ", l: ";
        if (node->left != nullptr) oss << node->left->index;
        else oss << "NULL";

        oss << ", r: ";
        if (node->right != nullptr) oss << node->right->index;
        else oss << "NULL";

        oss << "], data: ";

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


//dodanie nowego węzła po danej wartości
template<typename T>
template <typename Comparator>
void BST<T>::add(const T& data, Comparator cmp) {
    auto* new_node = new BSTNode<T>(data, next_index++);    // utworzenie nowego węzła new_node

    if (root == nullptr) {                                  // jeżeli root jest równy nullptr
        root = new_node;                                    // root przemuje wartość new_node
        current_size++;                                     // zwiększenie rozmiaru drzewa
        return;                                             // zakończenie dodawania
    }

    BSTNode<T>* current = root;                             // przejęcie węzła root do current
    BSTNode<T>* parent = nullptr;                           // przejęcie wartości nullptr do parent

    while (current != nullptr) {                            // działaj do czasu kiedy current jest różne od nullptr 
        parent = current;                                   // parent przejmuje węzeł current
        if (cmp(data, current->data) < 0) {                 // jeśli data > current->data
            current = current->left;                        // przejdź do lewego węzła
        }
        else {                                              // jeśli data < current->data
            current = current->right;                       // przejdź do prawego węzła
        }
    }

    new_node->parent = parent;                              //rodzic new_node przejmuje węzeł parent
    if (cmp(data, parent->data) < 0) {                      //jeśli data < parent->data
        parent->left = new_node;                            //lewe dziecko rodzica przejmuje wartość new_node
    }
    else {                                                  //jeśli data > parent->data
        parent->right = new_node;                           //prawe dziecko rodzica przejmuje wartość new_node
    }
    current_size++;                                         //zwiększenie rozmiaru drzewa
}


//Szukanie danego węzła po wartości
template<typename T>
template <typename Comparator>
BSTNode<T>* BST<T>::find(const T& data, Comparator cmp) {
    BSTNode<T>* current = root;                                     //utworzenie węzła current który wskazuje na root
    while (current != nullptr) {                                    //działaj do puki current jest różny od nullptr
        auto cmp_result = cmp(data, current->data);                 //przypisanie wartości komparatora (data,current->data)
        if (cmp_result == 0) {                                      //jeśsli data == current->data
            return current;                                         //zwrócenie wartości
        }
        else if (cmp_result < 0) {                                  //jeśli data < current->data
            current = current->left;                                //przejście do lewego dziecka
        }
        else {                                                      //jeśli data > current->data
            current = current->right;                               //przejście do prawego węzła
        }
    }
    return nullptr;                                                 //zwrócenie nullptr gdy nie będzie znalezionego węzła
}


//Usuwanie danego węzła po wartości
template<typename T>
void BST<T>::remove(BSTNode<T>* node) {
    if (node == nullptr) return;                                            //nie można usunąć pustego węzła

    if (node->left == nullptr && node->right == nullptr) {                  //jeśli prawy i lewy węzeł jest nullptr
        if (node->parent != nullptr) {                                      //jeśli rodzic zawiera wartość
            if (node->parent->left == node)                                 //jeśli lewe dziecko rodzica node jesst równe node
                node->parent->left = nullptr;                               //lewe dziecko rodzica node jest równe nullptr
            else node->parent->right = nullptr;                             // w innym wypadku prawe dziecko rodzica node równe nullptr
        }
        else {                                                              //w innym wypadku root jest równy nullptr
            root = nullptr;
        }
        delete node;                                                        //usuwanie węzła
        current_size--;                                                     //zmniejszenie rozmiaru
    }
    else if (node->left != nullptr && node->right != nullptr) {             //jeśli oba dzieci posiadają wartość
        auto* successor = find_minimum(node->right);                        //przejmij minimalną wartość drzewa (rozpoczęte od prawego dziecka node)
        node->data = successor->data;                                       //przejmij wartość successor do node
        remove(successor);                                                  //rozpocznij rekurencyjnie funkcje z węzłem successor
    }
    else {                                                                  //kiedy lewy lub prawy jest nullptr
        auto* child = (node->left != nullptr) ? node->left : node->right;   //child jest równy node->left jeśli nie jest nullptr (w innym wypadku node->right)
        if (node->parent != nullptr) {                                      //jeśli node rodzic jest różny od nullptr
            if (node->parent->left == node)                                 //jeśli rodzic node lewego dziecka jest równy node
                node->parent->left = child;                                 //przypisanie do lewego dziecka rodzica node child
            else                                                            //w przeciwnym wypadku przypisanie jest do prawego
                node->parent->right = child;
        }
        else {
            root = child;                                                   //przypisanie do dziecka roota
        }
        child->parent = node->parent;                                       //przypisanie rodzica dziecka do rodzica node
        delete node;                                                        //usuniecie noda
        current_size--;                                                     //zmniejszenie rozmiaru
    }
}

template<typename T>
std::vector<T> BST<T>::pre_order() const {
    std::vector<T> result;
    pre_order_traversal(root, result);
    return result;
}

template<typename T>
std::vector<T> BST<T>::in_order() const {
    std::vector<T> result;
    in_order_traversal(root, result);
    return result;
}

//wyczyszcenie drzewa binarnego (funkcja jests uruchamiana poprzez destruktor)
template<typename T>
void BST<T>::clear(bool free_data) {
    clear_node(root, free_data);
    root = nullptr;
    current_size = 0;
    next_index = 0;
}


template<typename T>
std::size_t BST<T>::height() const {
    return get_height(root);
}


template<typename T>
std::string BST<T>::to_string() const {
    std::ostringstream oss;

    oss << "binary search tree:\n";
    oss << "size: " << current_size << "\n";
    oss << "height: " << height() << "\n";
    oss << "{\n";

    build_string_pre_order(root, oss);

    oss << "}\n";

    return oss.str();
}

#endif // !BST_TREE