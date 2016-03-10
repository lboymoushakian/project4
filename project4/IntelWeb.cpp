#include "IntelWeb.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

IntelWeb::IntelWeb()
{}

IntelWeb::~IntelWeb()
{
    m_map_f.close();
    m_map_r.close();
}

bool IntelWeb::createNew(const std::string& filePrefix,	unsigned int maxDataItems)
{
    m_map_f.close();
    m_map_r.close();
    std::string name1 = filePrefix + ".forward";

  
    std::string name2 = filePrefix + ".reverse";

    bool success1 =m_map_f.createNew(name1, 2 * maxDataItems);
    bool success2 = m_map_r.createNew(name2, 2 * maxDataItems);
    if(success1 && success2)
        return true;
 
    return false;
}

void IntelWeb::close()
{
    m_map_f.close();
    m_map_r.close();
}

bool IntelWeb::openExisting(const std::string& filePrefix)
{
    m_map_f.close();
    m_map_r.close();
    bool success1= false; bool success2 = false;
   if( m_map_f.openExisting(filePrefix + ".forward"))
       success1 = true;
    if(m_map_r.openExisting(filePrefix + ".reverse"))
        success2 = true;
    if(success1 && success2)
        return true;
    else
    {
        m_map_f.close();
        m_map_r.close();
        return false;
    }
}


bool IntelWeb::ingest(const std::string& telemetryFile)
{
    
    ifstream inf(telemetryFile);
    if(!inf)
        cout <<"fail to open file!!!!!\n";
    
    string line;
    while(getline(inf, line))
    {
        istringstream iss(line);
        string key;
        string value;
        string context;
        if(!(iss >> key >> value >> context))
            cout << "not format right!!!!\n";
        bool success1 = m_map_f.insert(key, value, context);
        bool success2 = m_map_r.insert(key, context, value);
        if(success1 && success2)
            cout << "success!!!\n";
    }
    return true;
}





