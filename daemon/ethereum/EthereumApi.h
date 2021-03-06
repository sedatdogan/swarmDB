#ifndef BLUZELLE_ETHEREUMAPI_H
#define BLUZELLE_ETHEREUMAPI_H


#include <string>
#include <sstream>

#include <boost/format.hpp>
#include <boost/asio.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include "EthereumToken.h"


using std::string;

class EthereumApi {
    const string host_ = "ropsten.etherscan.io";

    const string get_token_balance_by_token_contract_address_format =
            "/api?module=account&action=tokenbalance&contractaddress=%s&address=%s&tag=latest&apikey=%s";

    string address_;
    string token_;
    boost::asio::ip::tcp::resolver resolver_;
    boost::asio::ip::tcp::socket sock_;

public:
    EthereumApi(string str_addr,
                string str_token,
                boost::asio::io_service &ios)
            : address_(std::move(str_addr)),
              token_(std::move(str_token)),
              resolver_(ios), sock_(ios) {
    }

    double token_balance(const EthereumToken& t);

private:
    void connect_socket();

    void write_request(const string &target);

    string read_response();

    void close_socket();

    boost::property_tree::ptree parse_response(const string &body) const;

    template<typename T> T get_field(const boost::property_tree::ptree &tuple,
                                     const string &name) const;
};

#endif //BLUZELLE_ETHEREUMAPI_H
