#include "server_http.hpp"
#include "client_http.hpp"

// Added for the json-example
#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <chrono>
// Added for the default_resource example
#include <fstream>
#include <boost/filesystem.hpp>
#include <vector>
#include <algorithm>
#include <ctime>
#ifdef HAVE_OPENSSL
#include "crypto.hpp"
#endif

using namespace std;
// Added for the json-example:
#include <chrono>
#include <iostream>
#include <sys/time.h>
#include <thread>

using std::cout; using std::endl;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;
typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;

size_t end_time;
size_t start_time;

// Added for the default_resource example
void default_resource_send(const HttpServer &server, const shared_ptr<HttpServer::Response> &response,
                           const shared_ptr<ifstream> &ifs);


void thread_function(HttpClient* client)
{
    this_thread::sleep_for(chrono::milliseconds(50));
    cout << "-----Start Migration-----" << endl;
    client->close();

    // HttpClient client2("192.168.81.100:6121", "192.168.34.38");
    HttpClient client2("155.230.34.228:8080", "192.168.34.38");
    auto r2 = client2.request("GET", "/");
    //cout << r2->content.rdbuf() << endl;
    client2.close();
    cout << "Second connection "<< r2->status_code << endl;
    auto millisec_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    end_time = millisec_since_epoch;// 2번 째 CLOSE에서 시간 측정;
    std::cout << "time : " << (double)(end_time - start_time) << " miliseconds" << std::endl;
}

int main()
{
    // server ip, port -> 192.168.81.100, 6121
    // client ip1 -> 192.168.113.75
    // client ip2 -> 192.168.34.38
    //  Client examples
    // HttpClient client("192.168.81.100:6121", "192.168.113.75");
    //192.168.128.100
    HttpClient client("155.230.34.228:8080", "192.168.128.100");
    auto r1 = client.request("GET", "/");
    start_time = client.start_time; //첫 REQUEST에서 시작 재기 시작함
    thread _t1(thread_function, &client);
    cout << "first connection "<< r1->status_code << endl;
    _t1.join();




    // client.close();

    // // HttpClient client2("192.168.81.100:6121", "192.168.34.38");
    // HttpClient client2("155.230.34.228:8080", "192.168.34.38");
    // auto r2 = client2.request("GET", "/");
    // //cout << r2->content.rdbuf() << endl;
    // client2.close();
    // auto millisec_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    // end_time = millisec_since_epoch;// 2번 째 CLOSE에서 시간 측정;
    // std::cout << "time : " << (double)(end_time - start_time) << "miliseconds" << std::endl;

    return 0;
}
