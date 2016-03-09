#include "DiskMultiMap.h"
#include <string>
#include <iostream>
#include <functional>
using namespace std;
int main()
{
    DiskMultiMap map;
    map.createNew("hi", 10);
    map.openExisting("hi");
    map.insert("the first one" , "hi", "no");
    //map.insert("the fine" , "hi", "no");
//map.insert("the first one" , "hi", "no");
   
    
    
    
}