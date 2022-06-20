#pragma once

#include <cassert>
#include <iterator>

template<typename T>
class list {
private:
    template<typename DATA_TYPE>
    struct list_iterator;

    struct node_base;
    struct node;

    node_base end_;

public:
    // bidirectional iterator
    using iterator = list_iterator<T>;
    // bidirectional iterator
    using const_iterator = list_iterator<T const>;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // O(1)
    list() noexcept;

    // O(n), strong
    list(list const &);

    // O(n), strong
    list &operator=(list const &);

    // O(n)
    ~list();

    // O(1)
    bool empty() const noexcept;

    // O(1)
    T &front() noexcept;

    // O(1)
    T const &front() const noexcept;

    // O(1), strong
    void push_front(T const &);

    // O(1)
    void pop_front() noexcept;

    // O(1)
    T &back() noexcept;

    // O(1)
    T const &back() const noexcept;

    // O(1), strong
    void push_back(T const &);

    // O(1)
    void pop_back() noexcept;

    // O(1)
    iterator begin() noexcept;

    // O(1)
    const_iterator begin() const noexcept;

    // O(1)
    iterator end() noexcept;

    // O(1)
    const_iterator end() const noexcept;

    // O(1)
    reverse_iterator rbegin() noexcept;

    // O(1)
    const_reverse_iterator rbegin() const noexcept;

    // O(1)
    reverse_iterator rend() noexcept;

    // O(1)
    const_reverse_iterator rend() const noexcept;

    // O(n)
    void clear() noexcept;

    // O(1), strong
    iterator insert(const_iterator pos, T const &val);

    // O(1)
    iterator erase(const_iterator pos) noexcept;

    // O(n)
    iterator erase(const_iterator first, const_iterator last) noexcept;

    // O(1)
    void splice(const_iterator pos, list &other, const_iterator first,
                const_iterator last) noexcept;

    friend void swap(list &a, list &b) noexcept {
        a.swap(b);
    }

private:

    void swap(list &other);
    void assign_links(node_base* prev, node_base* next);

    template<typename DATA_TYPE>
    struct list_iterator {
    private:
        node_base *ptr_{nullptr};

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = DATA_TYPE;
        using pointer = DATA_TYPE *;
        using reference = DATA_TYPE &;

        list_iterator() = default;

        list_iterator(iterator const &other) : ptr_(other.ptr_) {}

        reference operator*() const {
            return ptr_->value();
        }

        pointer operator->() const {
            return &ptr_->value();
        }

        bool operator==(const_iterator const &other) const {
            return ptr_ == other.ptr_;
        }

        bool operator!=(const_iterator const &other) const {
            return ptr_ != other.ptr_;
        }

        list_iterator &operator++() &{
            ptr_ = ptr_->next_;
            return *this;
        }

        list_iterator operator++(int) &{
            list_iterator old = *this;
            ++(*this);
            return old;
        }

        list_iterator &operator--() &{
            ptr_ = ptr_->prev_;
            return *this;
        }

        list_iterator operator--(int) &{
            list_iterator old = *this;
            --(*this);
            return old;
        }

    private:
        explicit list_iterator(node_base *ptr) : ptr_(ptr) {}


        friend list;
    };
};

template<typename T>
struct list<T>::node_base {
    node_base() = default;

    T &value();

private:
    node_base *prev_{nullptr};
    node_base *next_{nullptr};

    friend list;
};

template<typename T>
struct list<T>::node : node_base {
    node(T const &value, node_base *prev, node_base *next);

private:
    T value_;

    friend node_base;
};

//NODE FUNCTIONS
template<typename T>
T &list<T>::node_base::value() {
    return static_cast<node *>(this)->value_;
}

template<typename T>
list<T>::node::node(const T &value, node_base *prev, node_base *next)
        : value_(value) {
    node_base::prev_ = prev;
    node_base::next_ = next;
}


template<typename T>
list<T>::list() noexcept {
    end_.prev_ = &end_;
    end_.next_ = &end_;
}

//SUPPORTING FUNCTIONS

template<typename T>
void list<T>::assign_links(list::node_base *prev, list::node_base *next) {
    end_.next_ = next;
    end_.prev_ = prev;
}

template<typename T>
void list<T>::swap(list<T> &other) {
    if (empty()) {
        assign_links(&other.end_, &other.end_);
    } else {
        end_.prev_->next_ = &other.end_;
        end_.next_->prev_ = &other.end_;
    }
    if (other.empty()) {
        other.assign_links(&end_, &end_);
    } else {
        other.end_.prev_->next_ = &end_;
        other.end_.next_->prev_ = &end_;
    }
    std::swap(end_, other.end_);
}

template<typename T>
list<T>::list(const list<T> &other) : list() {
    for (auto &i : other) {
        push_back(i);
    }
}

template<typename T>
list<T> &list<T>::operator=(list const &other) {
    if (this != &other) {
        list(other).swap(*this);
    }
    return *this;
}

template<typename T>
list<T>::~list() {
    erase(begin(), end());
}

template<typename T>
bool list<T>::empty() const noexcept {
    return &end_ == end_.prev_;
}

//FRONT & BACK FUNCTIONS

template<typename T>
T &list<T>::front() noexcept {
    return end_.next_->value();
}

template<typename T>
T const &list<T>::front() const noexcept {
    return end_.next_->value();
}

template<typename T>
void list<T>::push_front(T const &val) {
    insert(begin(), val);
}

template<typename T>
void list<T>::pop_front() noexcept {
    erase(begin());
}

template<typename T>
T &list<T>::back() noexcept {
    return end_.prev_->value();
}

template<typename T>
T const &list<T>::back() const noexcept {
    return end_.prev_->value();
}

template<typename T>
void list<T>::push_back(T const &val) {
    insert(end(), val);
}

template<typename T>
void list<T>::pop_back() noexcept {
    erase(std::prev(end()));
}

//ITERATORS

template<typename T>
typename list<T>::iterator list<T>::begin() noexcept {
    return iterator(end_.next_);
}

template<typename T>
typename list<T>::const_iterator list<T>::begin() const noexcept {
    return const_iterator(end_.next_);
}

template<typename T>
typename list<T>::iterator list<T>::end() noexcept {
    return iterator(&end_);
}

template<typename T>
typename list<T>::const_iterator list<T>::end() const noexcept {
    return const_iterator(const_cast<node_base *>(&end_));
}

template<typename T>
typename list<T>::reverse_iterator list<T>::rbegin() noexcept {
    return reverse_iterator(end());
}

template<typename T>
typename list<T>::const_reverse_iterator list<T>::rbegin() const noexcept {
    return const_reverse_iterator(end());
}

template<typename T>
typename list<T>::reverse_iterator list<T>::rend() noexcept {
    return reverse_iterator(begin());
}

template<typename T>
typename list<T>::const_reverse_iterator list<T>::rend() const noexcept {
    return const_reverse_iterator(begin());
}

template<typename T>
void list<T>::clear() noexcept {
    erase(begin(), end());
    assign_links(&end_, &end_);
}

template<typename T>
typename list<T>::iterator list<T>::insert(const_iterator pos, T const &val) {
    node_base *pos_ptr = pos.ptr_;
    node_base *new_node = new node(val, pos_ptr->prev_, pos_ptr);
    pos_ptr->prev_->next_ = new_node;
    pos_ptr->prev_ = new_node;
    return iterator(new_node);
}

template<typename T>
typename list<T>::iterator list<T>::erase(const_iterator pos) noexcept {
    return erase(pos, std::next(pos));
}

template<typename T>
typename list<T>::iterator list<T>::erase(const_iterator first,
                                          const_iterator last) noexcept {
    if (first != last) {
        node_base *start_ptr = first.ptr_;
        node_base *end_ptr = last.ptr_->prev_;

        end_ptr->next_->prev_ = start_ptr->prev_;
        start_ptr->prev_->next_ = end_ptr->next_;
        start_ptr->prev_ = nullptr;
        end_ptr->next_ = nullptr;

        while (end_ptr != nullptr) {
            auto t = end_ptr->prev_;
            delete static_cast<node *>(end_ptr);
            end_ptr = t;
        }
    }
    return iterator(last.ptr_);
}

template<typename T>
void list<T>::splice(const_iterator pos, list<T> &other, const_iterator first,
                     const_iterator last) noexcept {
    if (first == last) {
        return;
    }
    node_base *start_ptr = first.ptr_;
    node_base *end_ptr = last.ptr_->prev_;
    node_base *tmp_pos = pos.ptr_;

    end_ptr->next_->prev_ = start_ptr->prev_;
    start_ptr->prev_->next_ = end_ptr->next_;

    end_ptr->next_ = tmp_pos;
    start_ptr->prev_ = tmp_pos->prev_;

    tmp_pos->prev_->next_ = start_ptr;
    tmp_pos->prev_ = end_ptr;
}
