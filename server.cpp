#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>

using namespace muduo;
using namespace muduo::net;

class Myserver {
public:
	Myserver(EventLoop* loop, const InetAddress& listenAddr, const string& nameArg)
		:loop_(loop),server_(loop, listenAddr, nameArg) {
			server_.setConnectionCallback(std::bind(&Myserver::onConnection, this, std::placeholders::_1));
			server_.setMessageCallback(std::bind(&Myserver::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}

	void start(){
		server_.start();
	}

private:
	TcpServer server_;
	EventLoop* loop_;
	void onConnection(const TcpConnectionPtr& conn) {
		if (conn->connected()) {
			LOG_INFO << "Client connected: " << conn->peerAddress().toIpPort();
		} else {
			LOG_INFO << "Client disconnected: " << conn->peerAddress().toIpPort();
		}
	} 
	void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time) {
		string msg(buf->retrieveAllAsString());
		LOG_INFO << "Received " << msg.size() << " bytes from " << conn->peerAddress().toIpPort() << ": " << msg;
		conn->send(msg);
	}
};


int main() {
	EventLoop loop;
	InetAddress listenAddr(9877); // Listen on port 9877
	Myserver server(&loop, listenAddr, "SimpleServer");

	server.start();
	loop.loop();
	return 0;
}
