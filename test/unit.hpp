#include <functional>

//unit test utilities
extern int passCount;
extern int failCount;

//pass unit tests here
void unit(std::string name, std::function<bool()> f);

//test function type
typedef int (*func_t)(int,char*[]);
extern func_t tests[];

//list of functions to call
int toString_test(int, char*[]);

