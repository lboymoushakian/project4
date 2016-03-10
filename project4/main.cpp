#include "DiskMultiMap.h"
#include "IntelWeb.h"
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
    map.insert("the fine" , "hi", "no");

    map.insert("the first one" , "hi", "no");
    map.insert("ho", "1" ,"3");
    map.insert("ho", "1" ,"4");
    map.insert("ho", "1" ,"3");

    DiskMultiMap::Iterator it = map.search("ho");

    string key = (*it).key;
    cout <<key <<endl;
    string context = (*it).context;
    cout <<context << endl;
    
    ++it;
    
     context = (*it).context;
    cout <<context << endl;
   
   
    
    IntelWeb iw;
    iw.createNew("what", 10);

    iw.openExisting("what");
    
    
    
}