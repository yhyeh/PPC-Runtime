// n choose r combination
#include <vector>
#include <iostream>
#include <algorithm>

struct c_unique {
  int current;
  c_unique() {current=0;}
  int operator()() {return ++current;}
} UniqueNumber;

void myfunction (int i) {
  i -= 1;
  std::cout << i << ' ';
}

int main()
{
    int n=5;
    int r=3;

    std::vector<int> myints(r);
    std::vector<int>::iterator first = myints.begin(), last = myints.end();

    std::generate(first, last, UniqueNumber);

    std::for_each(first, last, myfunction);
    std::cout << std::endl;

    while((*first) != n-r+1){
        std::vector<int>::iterator mt = last;

        while (*(--mt) == n-(last-mt)+1);
        (*mt)++;
        while (++mt != last) *mt = *(mt-1)+1;

        std::for_each(first, last, myfunction);
        std::cout << std::endl;
    }
}