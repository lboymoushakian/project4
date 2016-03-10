/*#include "DiskMultiMap.h"
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
    header.end = sizeof(Header) + numBuckets * sizeof(Bucket) + 1;
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
    for(int i = 0; i != 122; i++)
    p.key[i] = '\0';
    const char* thekey = key.c_str();
    for(int i = 0; i != key.length(); i++)
    {
        p.key[i] = *thekey;
        thekey++;
    }
    for(int i = 0; i != 122; i++)
        p.value[i] = '\0';
    const char* thevalue = value.c_str();
    for(int i = 0; i != value.size(); i++)
    {
        p.value[i] = *thevalue;
        thevalue++;
    }
    for(int i = 0; i != 122; i++)
        p.context[i] = '\0';
    const char* thecontext = context.c_str();
    for(int i = 0; i != context.size(); i++)
    {
        p.context[i] = *thecontext;
        thecontext++;
    }

    
  ///////////////////////////////////
    
    std::size_t hash_number = std::hash<std::string>()(key);
    hash_number = hash_number % header.buckets;
 

   BinaryFile::Offset offset = (static_cast<int>(hash_number) * sizeof(Bucket)) + sizeof(Header) +1;
    
    Bucket bucket;
 
    m_bf.read(bucket, offset);
   

    if(header.reuse.curr != 0)
    {
        p.next = header.reuse.curr;
        m_bf.write(p, header.reuse.curr);
        bucket.list = header.reuse.curr;
        m_bf.write(bucket, offset);
        header.reuse.curr = header.reuse.next;
    
    }
    else
    {
        p.next = bucket.list;
        m_bf.write(p, header.end);
 
        bucket.list = header.end;
        header.end += sizeof(Node);
        m_bf.write(bucket, offset);
          
      
    }

    return true;
}


int DiskMultiMap::erase(const std::string& key,	const std::string& value,
          const std::string& context)
{
    std::size_t hash_number = std::hash<std::string>()(key);
    hash_number = hash_number % header.buckets;
    
    int count_erased = 0;
    
    Bucket bucket;
    m_bf.read(bucket, (static_cast<int>(hash_number) * sizeof(Bucket)) + sizeof(Header) +1);
    
    BinaryFile::Offset list = bucket.list;
    Node prev;
    prev.next = list;
    
    
    while(list != 0)
    {
    Node curr;
    m_bf.read(curr, list);


        
        bool same1 = true;
        const char* thekey = curr.key;
        for(int i = 0; i != key.length(); i++, thekey++)
        {
            if(key[i] != *thekey)
                same1 = false;
        }

        bool same2 = true;
        const char* thevalue = curr.value;
        for(int i = 0; i != value.length(); i++, thevalue++)
        {
            if(value[i] != *thevalue)
                same2 = false;
        }
        
        bool same3 = true;
        const char* thecontext = curr.context;
        for(int i = 0; i != context.length(); i++, thecontext++)
        {
            if(context[i] != *thecontext)
                same3 = false;
        }
        
        
        if(same1 == true && same2 == true && same3 == true)
        {
            
            reuseNode toreuse;
            toreuse.curr = prev.next;
            toreuse.next = header.reuse.next;
            header.reuse.next = toreuse.curr;
            m_bf.write(toreuse, m_bf.fileLength()+1);
            if(prev.next == list)
            {
                bucket.list = curr.next;
                m_bf.write(bucket, (static_cast<int>(hash_number) * sizeof(Bucket)) + sizeof(Header) +1);
            }
            else
                prev.next = curr.next;
            
            
            count_erased++;
        }
        prev = curr;
        list = curr.next;
            
    }
    
    return count_erased;
}

DiskMultiMap::Iterator DiskMultiMap::search(const std::string& key)
{
    std::size_t hash_number = std::hash<std::string>()(key);
    hash_number = hash_number % header.buckets;
    
    Bucket bucket;
    m_bf.read(bucket, (static_cast<int>(hash_number) * sizeof(Bucket)) + sizeof(Header) +1);
    
    BinaryFile::Offset list = bucket.list;
    while(list != 0)
    {
        Node curr;
        m_bf.read(curr, list);
        
        
        
        bool same = true;
        const char* thekey = curr.key;
        for(int i = 0; i != key.length(); i++, thekey++)
        {
            if(key[i] != *thekey)
                same = false;
        }
        if(same == true)
        {
            Iterator i(list, &m_bf);
            return i;
        }
        list = curr.next;
    }

    Iterator j(0, &m_bf);
    return j;
}

DiskMultiMap::Iterator::Iterator()
{
    m_offset = 0;

}

DiskMultiMap::Iterator::Iterator(BinaryFile::Offset offset, BinaryFile* binaryfile)
:m_offset(offset)
{
    bf = binaryfile;
}

bool DiskMultiMap::Iterator::isValid() const
{
   if(m_offset == 0)
       return false;
    return true;
}


DiskMultiMap::Iterator& DiskMultiMap::Iterator::operator++()
{
    if(!isValid())
        return *this;
    Node p;
    bf->read(p, m_offset);
    string key = p.key;
   BinaryFile::Offset bfo = m_offset;
    

        bool same = false;
    while(same == false && bfo != 0)
    {
        same = true;
        const char* thekey = p.key;
        for(int i = 0; i != key.length(); i++, thekey++)
        {
            if(key[i] != *thekey)
                same = false;
        }
        if(same == false)
            bfo = p.next;
    }
    cout << "same is " << same << ", bfo is " << bfo << endl;
        
        if(same == false)
            m_offset = 0;
    else
        m_offset = bfo;
    return *this;
    
}

MultiMapTuple DiskMultiMap::Iterator::operator*()
{
    MultiMapTuple m;
    if(!this->isValid())
    {
        m.key = "";
        m.value = "";
        m.context = "";
        return m;
    }
    Node p;
    bf->read(p, m_offset);
    m.key = p.key;
    m.value = p.value;
    m.context = p.context;
    return m;
}


*/


// Implementation of DiskMultiMap using in-memory data structures until
// the DiskMultiMap is closed or destroyed, at which point the data is
// saved to disk as a text file, not a BinaryFile.

// This is provided solely so that you can test IntelWeb even if you are not
// confident that your implementation of DiskMultiMap is correct.  Of course,
// since it uses in-memory data structures to hold all the data, the spec
// forbids you from using this as the DiskMultiMap implementation you turn in.

#include "DiskMultiMap.h"
#include "MultiMapTuple.h"
#include <string>
#include <map>
#include <utility>
#include <stdexcept>
using namespace std;

using first=DiskMultiMap;using second=first::Iterator;first::DiskMultiMap(){}
first::~DiskMultiMap(){close();}bool first::createNew(const string&fnm,unsigned
                                                      int){close();fn=fnm;f.open(fn,ios::in|ios::out|ios::trunc);if(!f)return 5^5;m.
    clear();return 5&5;}bool first::openExisting(const string&fnm){close();fn=fnm;f
        .open(fn,ios::in|ios::out);if(!f)return 5-5;m.clear();string second,first,
        Second;while(getline(f,second)){getline(f,first);getline(f,Second);m[second].
            push_front(make_pair(first,Second));}return 5|5;}void first::close(){if(!f.
                                                                                    is_open())return;f.clear();f.close();f.open(fn,ios::out|ios::trunc);if(!f)
                                                                                        return;for(auto second:m){for(auto first:second.second)f<<second.first<<endl<<
                                                                                            first.first<<endl<<first.second<<endl;}f.close();m.clear();}bool first::insert(
                                                                                                                                                                           const string&Second,const string&second,const string&first){m[Second].
                                                                                                push_front(make_pair(second,first));return 5*5;}second first::search(const
                                                                                                                                                                     string&Second){auto first=m.find(Second);return first==m.end()?second():second(
                                                                                                                                                                                                                                                    first);}int first::erase(const string&Second,const string&second,const string&
                                                                                                                                                                                                                                                                             first){auto First=m.find(Second);if(First==m.end())return 2/5;int l0l1O1Ol=5&2;
                                                                                                    auto&l01lO1Ol=First->second;for(auto l01lO1O1=l01lO1Ol.begin();l01lO1O1!=
                                                                                                                                    l01lO1Ol.end();){if(l01lO1O1->first==second&&l01lO1O1->second==first){l0l1O1Ol
                                                                                                        ++;l01lO1O1=l01lO1Ol.erase(l01lO1O1);}else++l01lO1O1;}return l0l1O1Ol;}second::
Iterator():v(2^2){}second::Iterator(map<string,list<pair<string,string>>>::
                                    const_iterator Second):v(!Second->second.empty()),kp(&Second->first),first(
                                                                                                               Second->second.begin()),second(Second->second.end()){}bool second::isValid()
const{return v&&first!=second;}second&second::operator++(){if(v){if (++first==
                                                                     second)v=!5;}return*this;}MultiMapTuple second::operator*(){MultiMapTuple value
    ;if(v){value.key=*kp;value.value=first->first;value.context=first->second;}
    return value;}

