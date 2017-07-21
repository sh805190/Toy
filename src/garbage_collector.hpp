#pragma once

#include <algorithm>
	#include <list>

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

template<typename T>
std::list<T*> GarbageCollector<T>::ptrList;
