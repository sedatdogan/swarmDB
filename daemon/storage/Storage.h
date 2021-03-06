#ifndef BLUZELLE_STORAGE_H
#define BLUZELLE_STORAGE_H

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <unordered_map>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
namespace bu = boost::uuids;

typedef bu::uuid UUID_t;
typedef vector<unsigned char> VEC_BIN_t;

struct Record
{
    time_t              timestamp_;
    VEC_BIN_t           value_;
    UUID_t              transaction_id_;

    Record():
        timestamp_(0),
        value_(0)
    {
        transaction_id_ = bu::nil_uuid();
    }

    Record(time_t timestamp, VEC_BIN_t value, UUID_t transaction_id) :
        timestamp_(timestamp),
        value_(value),
        transaction_id_(transaction_id)
    {
    }

    string value();
};

class Storage
{
private:
    unordered_map<std::string, Record>  kv_store_;
    static bu::nil_generator            nil_uuid_gen;
    static VEC_BIN_t                    nil_value;

    vector<string> keys()
    {
        vector<string> keys;

        for(const auto& p : kv_store_)
            {
            keys.emplace_back(p.first);
            }
        return keys;
    }


    VEC_BIN_t string_to_blob(const string& value)
    {
        VEC_BIN_t blob;
        blob.reserve(value.size());

        for_each
            (
            value.begin(),
            value.end(),
            [&blob](const unsigned char c)
            {
                blob.emplace_back(c);
            });
        return blob;
    }

public:
    Storage() = default;

    Storage(
        const std::string& filename
    )
    {
        cerr << "Storage does not save to: [" << filename << "] yet.\n";
    }

    ~Storage() = default;

    void
    create(
            const string& key,
            const VEC_BIN_t& value,
            const UUID_t& transaction_id
    );

    void
    create(
            const string& key,
            const string& value,
            const string& transaction_id
    )
    {
        create(
            key,
            string_to_blob(value),
            boost::lexical_cast<boost::uuids::uuid>(transaction_id)
        );
    };

    Record
    read(
            const std::string &key
    )
    {
        Storage::nil_value.resize(0);
        Record nil_record;
        auto rec = kv_store_.find(key);
        return (rec != kv_store_.end()) ? rec->second : nil_record;
    }

    void
    update(
            const std::string& key,
            const VEC_BIN_t& value
    );

    void remove(
            const std::string& key
    );
};

#endif //BLUZELLE_STORAGE_H
