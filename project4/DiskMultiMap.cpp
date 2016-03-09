#include "DiskMultiMap.h"
#include <iostream>
#include <string>
#include <functional>

DiskMultiMap::DiskMultiMap()
{
    header.reuse.curr = 0;
    header.reuse.next = 0;
    
}

DiskMultiMap::~DiskMultiMap()
{
    m_bf.close();
}

bool DiskMultiMap::createNew(const std::string& filename, unsigned int numBuckets)
{
   if(m_bf.isOpen())
       m_bf.close();
    m_bf.createNew(filename);
    header.buckets = numBuckets;
    m_bf.write(header, 0);
    BinaryFile::Offset next = sizeof(Header) + sizeof(Bucket) * numBuckets + 1;
    for(BinaryFile::Offset i = sizeof(Header)+1; i < sizeof(Bucket) * numBuckets + sizeof(Header); i += sizeof(Bucket), next += sizeof(Node))
    {
        Bucket b;
        b.list = 0;
        m_bf.write(b, i);
    }
    return true;
    
}

bool DiskMultiMap::openExisting(const std::string& filename)
{
    if(m_bf.isOpen())
        m_bf.close();
    bool success = m_bf.openExisting(filename);
    if(success)
        return true;
    return false;
}

void DiskMultiMap::close()
{
    if(!m_bf.isOpen())
        return;
    m_bf.close();
}

bool DiskMultiMap::insert(const std::string& key, const std::string& value, const std::string& context)
{
    if(key.size() > 120 || value.size() > 120 || context.size() > 120)
        return false;
    
    Node p;
    const char* thekey = key.c_str();
    for(int i = 0; i != key.size(); i++)
    {
        p.key[i] = *thekey;
        thekey++;
    }
    
    const char* thevalue = value.c_str();
    for(int i = 0; i != value.size(); i++)
    {
        p.value[i] = *thevalue;
        thevalue++;
    }
    
    const char* thecontext = context.c_str();
    for(int i = 0; i != context.size(); i++)
    {
        p.context[i] = *thecontext;
        thecontext++;
    }
    
  
    
    std::size_t hash_number = std::hash<std::string>()(key);
    cout << "hash number is " << hash_number << endl;
    hash_number = hash_number % header.buckets;
    cout << "hash number is " << hash_number << endl;
    hash_number = static_cast<int>(hash_number);
    
    
   

   BinaryFile::Offset offset = (hash_number * sizeof(Bucket)) + sizeof(Header) +1;

    Bucket bucket;
    
    m_bf.read(bucket, offset);
    BinaryFile::Offset place = bucket.list;
    
    Node last;
    
    while(place !=0)
    {

        m_bf.read(last, place);
        place = last.next;
        
    }
    
    if(header.reuse.curr != 0)
    {
        m_bf.write(p, header.reuse.curr);
        last.next = header.reuse.curr;
        header.reuse.curr = header.reuse.next;
      
    }
    else
        m_bf.write(p, m_bf.fileLength()+1);
    
    
    

    
    return true;
}