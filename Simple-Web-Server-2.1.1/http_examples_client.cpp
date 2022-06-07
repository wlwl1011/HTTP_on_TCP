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

size_t total_delay_end_time;
size_t total_delay_start_time;

// Added for the default_resource example

size_t handover_delay_end_time;
size_t handover_delay_start_time;

void default_resource_send(const HttpServer &server, const shared_ptr<HttpServer::Response> &response,
                           const shared_ptr<ifstream> &ifs);

void NetworkChange(std::string fromIface, std::string toIface) {

            //std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
            std::string del_order = "sudo route del -net 0.0.0.0 dev ";
            std::string add_order = "sudo route add -net 0.0.0.0 gw ";

            FILE* fp;
            char buf[256];
            static char iface[256];
            in_addr_t dest, gw, flag, refcnt, use, metric, mask;
            struct in_addr from_gw, to_gw;
            unsigned int to_metric;
            int ret;

            for(int i=0; i<1; i++) {

                fp = fopen("/proc/net/route", "r");

                if(fp == NULL)
                    return;

                while(fgets(buf, 255, fp)) {
                    if(!strncmp(buf, "Iface", 5)) continue;
                    ret = sscanf(buf, "%s\t%x\t%x\t%d\t%d\t%d\t%d\t%x", iface, &dest, &gw, &flag, &refcnt, &use, &metric, &mask);
                    
                    if(ret < 8) {
                        fprintf(stderr, "Line Read Error");
                        return;
                    }

                    if(dest == 0) {
                        if(strcmp(fromIface.c_str(), iface) == 0) {
                            from_gw.s_addr = gw;
                        } else if(strcmp(toIface.c_str(), iface) == 0) {
                            to_gw.s_addr = gw;
                            to_metric = metric;
                        }
                    }
        }

            fclose(fp);

        // change network info

            system(std::string(del_order + fromIface).c_str());    
            std::cout << "[quic_toy_client] Network Change from " << fromIface << " to " << toIface << std::endl;
            //cout << std::string(add_order + inet_ntoa(to_gw) + " dev " + toIface + " metric 101").c_str() << endl;
            system(std::string(add_order + inet_ntoa(to_gw) + " dev " + toIface + " metric 101").c_str());
            //cout << std::string(del_order + toIface + " metric " + std::to_string(to_metric)).c_str() << endl;
            system(std::string(del_order + toIface + " metric " + std::to_string(to_metric)).c_str());
            //cout << std::string(add_order + inet_ntoa(from_gw) + " dev " + fromIface + " metric 600").c_str() << endl;
            system(std::string(add_order + inet_ntoa(from_gw) + " dev " + fromIface + " metric 600").c_str());

    

    }

    }

void thread_function()
{
   
    this_thread::sleep_for(chrono::milliseconds(100));
    auto millisec_since_epoch_hanover_start = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count(); 
    handover_delay_start_time = millisec_since_epoch_hanover_start;
   
    NetworkChange("eth0","wlo1");
    HttpClient client2("155.230.34.228:8080", "wlo1");

    
    
    auto r2 = client2.request("GET", "/");
    handover_delay_end_time = client2.handover_delay_end_time;
    auto millisec_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    total_delay_end_time = millisec_since_epoch;// 2번 째 CLOSE에서 시간 측정
   
    client2.close();
   
    std::cout << "handover delay : " << (double)(handover_delay_end_time - handover_delay_start_time) << " miliseconds" << std::endl;
    std::cout << "total delay : " << (double)(total_delay_end_time - total_delay_start_time) << " miliseconds" << std::endl;

}

int main()
{
   // cout << "------First Connection-----"<< endl;
    HttpClient client("155.230.34.228:8080", "eth0");
   // cout << "request1" << endl;
   
    auto r1 = client.request("GET", "/test");
    thread _t1(thread_function);
    total_delay_start_time = client.total_delay_start_time; //첫 REQUEST에서 시작 재기 시작함
    std::cout << r1->status_code << std::endl;
    _t1.join();
    return 0;
}