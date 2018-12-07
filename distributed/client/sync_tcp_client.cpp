#include <iostream> //SYNC_TCP_CLIENT.CPP
#include <boost/asio.hpp>
using namespace boost::asio;
using ip::tcp;
int main() {
    io_service io;
    boost::system::error_code err;
    char buf[128];
    tcp::resolver r(io); //find endpoints from address
    tcp::resolver::query q("127.0.0.1", "10000");
    tcp::resolver::iterator server = r.resolve(q);
    tcp::socket s(io);
    s.connect(*server, err); //connect and wait
    for (;;) {
        std::cin.getline(buf,128);
        write(s,buffer(buf,strlen(buf)+1), ec);
        if(ec) break;
        size_t n = s.read_some(buffer(buf,128), ec);
        if (ec) break;
        std::cout.write(buf, n-1);
    }
}

