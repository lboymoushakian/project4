#ifndef INTELWEB_H_
#define INTELWEB_H_

#include "InteractionTuple.h"
#include "BinaryFile.h"
#include "DiskMultiMap.h"
#include <string>
#include <vector>
#include <set>

class IntelWeb
{
public:
    IntelWeb();
    ~IntelWeb();
    bool createNew(const std::string& filePrefix, unsigned int maxDataItems);
    bool openExisting(const std::string& filePrefix);
    void close();
    bool ingest(const std::string& telemetryFile);
    unsigned int crawl(const std::vector<std::string>& indicators,
                       unsigned int minPrevalenceToBeGood,
                       std::vector<std::string>& badEntitiesFound,
                       std::vector<InteractionTuple>& badInteractions
                       );
    bool purge(const std::string& entity);
    
    
private:
    DiskMultiMap m_map_f;
    DiskMultiMap m_map_r;
    set<std::string> bad_entities;
    set<InteractionTuple> interactions;
    // Your private member declarations will go here
};


#endif // INTELWEB_H_
