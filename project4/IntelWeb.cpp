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
        string context;
        string key;
        string value;
        if(!(iss >> context >> key >> value))
            cout << "not format right!!!!\n";
        bool success1 = m_map_f.insert(key, value, context);
        bool success2 = m_map_r.insert(value, key, context);
        if(success1 && success2)
            cout << "success!!!\n";
    }
    return true;
}


unsigned int IntelWeb::crawl(const std::vector<std::string>& indicators,
                   unsigned int minPrevalenceToBeGood,
                   std::vector<std::string>& badEntitiesFound,
                   std::vector<InteractionTuple>& badInteractions
                   )
{
    //go through the list of indicators
    
    for(auto i = indicators.begin(); i != indicators.end(); i++)
    {
        //search up each indicator in m_map_f
        DiskMultiMap::Iterator z = m_map_f.search(*i);
        //if you find the indicator, insert it into bad_entities
        if(z.isValid())
            bad_entities.insert((*z).key);
        while(z.isValid())
        {
            string value;
            value = (*z).value;
            
            //calculate prevalence by searching through each diskmultimap and counting how many times the indicator shows up as a key
            int prevalence = 0;
            
            DiskMultiMap::Iterator x = m_map_f.search(*i);
            while(x.isValid())
            {
                prevalence++;
                ++x;
            }
            x = m_map_r.search(*i);
            while(x.isValid())
            {
                prevalence++;
                ++x;
            }
                //if prevalence is too low, insert the thing associated with the indicator into bad_entities
            if(prevalence < minPrevalenceToBeGood)
            {
                bad_entities.insert(value);
                InteractionTuple h;
                h.context = (*z).context;
                h.to = value;
                h.from = *i;
                interactions.insert(h);
            }
            ++z;
        }
        
        //do the same thing with m_map_r to find associations that go the other way
        DiskMultiMap::Iterator y = m_map_r.search(*i);
        if(y.isValid())
            bad_entities.insert((*y).key);
        while(y.isValid())
        {
            string value;
            value = (*y).value;
            
            //calculate prevalence by searching through each diskmultimap and counting how many times the indicator shows up as a key
            int prevalence = 0;
            
            DiskMultiMap::Iterator x = m_map_r.search(*i);
            while(x.isValid())
            {
                prevalence++;
                ++x;
            }
            x = m_map_r.search(*i);
            while(x.isValid())
            {
                prevalence++;
                ++x;
            }
            
            if(prevalence < minPrevalenceToBeGood)
            {
                bad_entities.insert(value);
                InteractionTuple h;
                h.context = (*z).context;
                h.to = *i;
                h.from = value;
                interactions.insert(h);
                
            }
            ++y;
        }
        
          }
    
    //we've gone through the list of indicators and found everything associated to them
    //now we want to go through the bad_entities list to find more
    bool anyFound = true;
    while(anyFound == true)
    {
        anyFound = false;
        for(auto p = bad_entities.begin(); p != bad_entities.end(); p++)
        {
            //first, if *p is an indicator, skip all this bc its already done
            bool inIndicators = false;
            for(auto u = indicators.begin(); u != indicators.end(); u++)
            {
                if((*u) == (*p))
                    inIndicators = true;
            }
            if(inIndicators == true)
                continue;
            //check for associations for each item in bad_entities
            
            //search up each indicator in m_map_f
            DiskMultiMap::Iterator z = m_map_f.search(*p);
            //if you find the indicator, insert it into bad_entities
            if(z.isValid())
                bad_entities.insert((*z).key);
            while(z.isValid())
            {
                string value;
                value = (*z).value;
                
                //calculate prevalence by searching through each diskmultimap and counting how many times the indicator shows up as a key
                int prevalence = 0;
                
                DiskMultiMap::Iterator x = m_map_f.search(*p);
                while(x.isValid())
                {
                    prevalence++;
                    ++x;
                }
                x = m_map_r.search(*p);
                while(x.isValid())
                {
                    prevalence++;
                    ++x;
                }
                //if prevalence is too low, insert the thing associated with the indicator into bad_entities
                if(prevalence < minPrevalenceToBeGood)
                {
                    auto pair = bad_entities.insert(value);
                    if(pair.second == true)
                        anyFound = true;
                    InteractionTuple h;
                    h.context = (*z).context;
                    h.to = value;
                    h.from = *p;
                    interactions.insert(h);
                }
                ++z;
            }
            
            //do the same thing with m_map_r to find associations that go the other way
            DiskMultiMap::Iterator y = m_map_r.search(*p);
            if(y.isValid())
                bad_entities.insert((*y).key);
            while(y.isValid())
            {
                string value;
                value = (*y).value;
                
                //calculate prevalence by searching through each diskmultimap and counting how many times the indicator shows up as a key
                int prevalence = 0;
                
                DiskMultiMap::Iterator x = m_map_r.search(*p);
                while(x.isValid())
                {
                    prevalence++;
                    ++x;
                }
                x = m_map_r.search(*p);
                while(x.isValid())
                {
                    prevalence++;
                    ++x;
                }
                
                if(prevalence < minPrevalenceToBeGood)
                {
                    auto pair = bad_entities.insert(value);
                    if(pair.second == true)
                        anyFound = true;
                    InteractionTuple h;
                    h.context = (*z).context;
                    h.to = *p;
                    h.from = value;
                    interactions.insert(h);
                    
                }
                ++y;
            }

            
            
        }
        
    }
    
    //transfer everything from bad_entities to badEntitiesFound
    //also transfer from interactions to badInteractions
    
    for(auto i = bad_entities.begin(); i != bad_entities.end(); i++)
    {
        badEntitiesFound.push_back(*i);
    }
    
    for(auto i = interactions.begin(); i != interactions.end(); i++)
        badInteractions.push_back(*i);
    return static_cast<unsigned int>(badEntitiesFound.size());
}




bool IntelWeb::purge(const std::string& entity)
{
    bool found = false;
    DiskMultiMap::Iterator i = m_map_f.search(entity);
    if(i.isValid())
        found = true;
    while (i.isValid())
    {
        string key = (*i).key;
        string value = (*i).value;
        string context = (*i).context;
        m_map_f.erase(key, value, context);
        ++i;
    }
    
    DiskMultiMap::Iterator j = m_map_r.search(entity);
    if(j.isValid())
        found = true;
    while (j.isValid())
    {
        string key = (*j).key;
        string value = (*j).value;
        string context = (*j).context;
        m_map_f.erase(key, value, context);
        ++j;
    }
    
    return found;
}

bool operator<(const InteractionTuple one, const InteractionTuple two)
{
    string to1 = one.to;
    string from1 = one.from;
    string context1 = one.context;
    
    string to2 = two.to;
    string from2 = two.from;
    string context2 = two.context;
    
    if(to1 < to2)
        return true;
    else if(to1 == to2 && from1 < from2)
        return true;
    else if (to1 == to2 && from1 == from2 && context1 < context2)
        return true;
    return false;
    
}





