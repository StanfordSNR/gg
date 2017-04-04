#include <unistd.h>
int main()
{
   execl("./hello", "./hello",  (char *) 0);
}
