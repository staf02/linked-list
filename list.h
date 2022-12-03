#pragma once

#include <cassert>
#include <iterator>

template <typename T>
class list {
private:
  template <typename DATA_TYPE>
  struct list_iterator;

  struct node_base {
    node_base() {
      prev_ = next_ = this;
    }
    node_base(node_base* prev_, node_base* next_) : prev_(prev_), next_(next_) {}

    T& value() {
      return static_cast<node*>(this)->value_;
    }

  private:
    node_base* prev_{nullptr};
    node_base* next_{nullptr};

    friend list;
  };

  struct node : node_base {
    node(T const& value, node_base* prev, node_base* next): value_(value) {
      node_base::prev_ = prev;
      node_base::next_ = next;
    }

  private:
    T value_;

    friend node_base;
  };

  node_base end_;

public:
  // bidirectional iterator
  using iterator = list_iterator<T>;
  // bidirectional iterator
  using const_iterator = list_iterator<T const>;

  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  list() noexcept : end_(&end_, &end_) {}

  list(list const& other) : list() {
    for (auto& i : other) {
      push_back(i);
    }
  }

  list& operator=(list const& other) {
    if (this != &other) {
      list(other).swap(*this);
    }
    return *this;
  }

  ~list() {
    erase(begin(), end());
  }

  // O(1)
  bool empty() const noexcept {
    return &end_ == end_.prev_;
  }

  // O(1)
  T& front() noexcept {
    return end_.next_->value();
  }

  // O(1)
  T const& front() const noexcept {
    return end_.next_->value();
  }

  // O(1), strong
  void push_front(T const& val) {
    insert(begin(), val);
  }

  // O(1)
  void pop_front() noexcept {
    erase(begin());
  }

  // O(1)
  T& back() noexcept {
    return end_.prev_->value();
  }

  // O(1)
  T const& back() const noexcept {
    return end_.prev_->value();
  }

  // O(1), strong
  void push_back(T const& val) {
    insert(end(), val);
  }

  // O(1)
  void pop_back() noexcept {
    erase(std::prev(end()));
  }

  // O(1)
  iterator begin() noexcept {
    return iterator(end_.next_);
  }

  // O(1)
  const_iterator begin() const noexcept {
    return const_iterator(end_.next_);
  }

  // O(1)
  iterator end() noexcept {
    return iterator(&end_);
  }

  // O(1)
  const_iterator end() const noexcept {
    return const_iterator(const_cast<node_base*>(&end_));
  }

  // O(1)
  reverse_iterator rbegin() noexcept {
    return reverse_iterator(end());
  }

  // O(1)
  const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(end());
  }

  // O(1)
  reverse_iterator rend() noexcept {
    return reverse_iterator(begin());
  }

  // O(1)
  const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(begin());
  }

  // O(n)
  void clear() noexcept {
    erase(begin(), end());
    assign_links(&end_, &end_);
  }

  // O(1), strong
  iterator insert(const_iterator pos, T const& val) {
    node_base* tmp = pos.ptr_;
    node_base* new_node = new node(val, tmp->prev_, tmp);
    link(new_node, new_node, pos.ptr_);
    return iterator(new_node);
  }

  // O(1)
  iterator erase(const_iterator pos) noexcept {
    return erase(pos, std::next(pos));
  }

  // O(n)
  iterator erase(const_iterator first, const_iterator last) noexcept {
    if (first != last) {
      node_base* start_ptr = first.ptr_;
      node_base* end_ptr = last.ptr_->prev_;

      cut(start_ptr, end_ptr);

      auto t = start_ptr;
      while (t != end_ptr) {
        auto tmp = t->next_;
        delete static_cast<node*>(t);
        t = tmp;
      }
      delete static_cast<node*>(t);
    }
    return iterator(last.ptr_);
  }

  // O(1)
  void splice(const_iterator pos, list& other, const_iterator first,
              const_iterator last) noexcept {
    if (first == last) {
      return;
    }
    node_base* start_pos = first.ptr_;
    node_base* end_pos = last.ptr_->prev_;
    node_base* tmp_pos = pos.ptr_;

    cut(start_pos, end_pos);
    link(start_pos, end_pos, tmp_pos);
  }

  friend void swap(list& a, list& b) noexcept {
    a.swap(b);
  }

private:
  void swap(list& other) {
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

  void assign_links(node_base* prev, node_base* next) {
    end_.next_ = next;
    end_.prev_ = prev;
  }

  static void link(node_base* start_ptr, node_base* end_ptr,
                   node_base* insert_pos) {
    end_ptr->next_ = insert_pos;
    start_ptr->prev_ = insert_pos->prev_;

    insert_pos->prev_->next_ = start_ptr;
    insert_pos->prev_ = end_ptr;
  }

  static void cut(node_base* start_ptr, node_base* end_ptr) {
    end_ptr->next_->prev_ = start_ptr->prev_;
    start_ptr->prev_->next_ = end_ptr->next_;
  }

  template <typename DATA_TYPE>
  struct list_iterator {
  private:
    node_base* ptr_{nullptr};

  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = DATA_TYPE;
    using pointer = DATA_TYPE*;
    using reference = DATA_TYPE&;

    list_iterator() = default;

    template <typename P>
    list_iterator(list_iterator<P> other,
                  std::enable_if_t<std::is_same_v<DATA_TYPE, P> ||
                                   std::is_const_v<DATA_TYPE>>* = nullptr) {
      ptr_ = other.ptr_;
    }

    reference operator*() const {
      return ptr_->value();
    }

    pointer operator->() const {
      return &ptr_->value();
    }

    bool operator==(const_iterator const& other) const {
      return ptr_ == other.ptr_;
    }

    bool operator!=(const_iterator const& other) const {
      return ptr_ != other.ptr_;
    }

    list_iterator& operator++() & {
      ptr_ = ptr_->next_;
      return *this;
    }

    list_iterator operator++(int) & {
      list_iterator old = *this;
      ++(*this);
      return old;
    }

    list_iterator& operator--() & {
      ptr_ = ptr_->prev_;
      return *this;
    }

    list_iterator operator--(int) & {
      list_iterator old = *this;
      --(*this);
      return old;
    }

  private:
    explicit list_iterator(node_base* ptr) : ptr_(ptr) {}

    friend list;
  };
};