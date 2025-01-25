#pragma once

#if 0

// Custom new operator. Do your memory logging here.
void* operator new(size_t size, char* file, unsigned int line) {
  void* x = malloc(size);
  std::cout << "Allocated " << size << " byte(s) at address " << x << " in " << file
       << ":" << line << std::endl;
  return x;
}

// You must override the default delete operator to detect all deallocations
void operator delete(void* p) {
  free(p);
  std::cout << "Freed memory at address " << p << std::endl;
}

// You also should provide an overload with the same arguments as your
// placement new. This would be called in case the constructor of the
// created object would throw.
void operator delete(void* p, char* file, unsigned int line) {
  free(p);
  std::cout << "Freed memory at address " << p << std::endl;
}

#define new new(__FILE__, __LINE__)

#endif


namespace UtilDebug {

}