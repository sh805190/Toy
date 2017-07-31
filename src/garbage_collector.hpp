#pragma once

#include <algorithm>
#include <list>

#include "literal.hpp"

template<typename T>
class GarbageCollector {
public:
  static T* Push(T* ptr) {
    ptrList.push_back(ptr);
  }

  static T* Pop(T* ptr) {
    ptrList.remove_if([ptr](T* it) -> bool {
      return it == ptr;
    });
  }

  static int GetSize() {
    return ptrList.size();
  }

  static int Collect() {
    int size = GetSize();
    //delete using a copied list
    std::list<T*> ptrCopy;
    std::for_each(ptrList.begin(), ptrList.end(), [&ptrCopy](T* ptr) {
      ptrCopy.push_back(ptr);
    });
    std::for_each(ptrCopy.begin(), ptrCopy.end(), [](T* ptr) {
      delete ptr;
    });
    ptrList.clear();
    return size;
  }

private:
  static std::list<T*> ptrList;
};

//accomodate corner-case for lFunction::block
template<>
class GarbageCollector<Literal> {
public:
  static Literal* Push(Literal* ptr) {
    ptrList.push_back(ptr);
  }

  static Literal* Pop(Literal* ptr) {
    ptrList.remove_if([ptr](Literal* it) -> bool {
      return it == ptr;
    });
  }

  static int GetSize() {
    return ptrList.size();
  }

  static int Collect() {
    int size = GetSize();
    //delete using a copied list
    std::list<Literal*> ptrCopy;
    std::for_each(ptrList.begin(), ptrList.end(), [&ptrCopy](Literal* ptr) {
      ptrCopy.push_back(ptr);
    });
    std::for_each(ptrCopy.begin(), ptrCopy.end(), [](Literal* ptr) {
      if (ptr->type == Literal::Type::LFUNCTION) {
        static_cast<lFunction*>(ptr)->block = nullptr; //cut the block loose before deletion
      }
      delete ptr;
    });
    ptrList.clear();
    return size;
  }

private:
  static std::list<Literal*> ptrList;
};

//member declarations
template<typename T>
std::list<T*> GarbageCollector<T>::ptrList;

