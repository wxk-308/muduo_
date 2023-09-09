#include <muduo/net/TcpClient.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>

using namespace muduo;
using namespace muduo::net;

class Myclient{
public:
	Myclient(EventLoop* loop, const InetAddress& serverAddr, const string& nameArg)
		:loop_(loop),client_(loop, serverAddr, nameArg){
		client_.setConnectionCallback(std::bind(&Myclient::onConnection, this, std::placeholders::_1));
		client_.setMessageCallback(std::bind(&Myclient::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}
	void connect(){
		client_.connect();
	}
private:
	EventLoop* loop_;
	TcpClient client_;

	void onConnection(const TcpConnectionPtr& conn) {
		if (conn->connected()) {
			LOG_INFO << "Connected to server: " << conn->peerAddress().toIpPort();
			conn->send("Hello, Muduo!");
		} else {
			LOG_INFO << "Disconnected from server: " << conn->peerAddress().toIpPort();
		}
	}

	void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time) {
		string msg(buf->retrieveAllAsString());
		LOG_INFO << "Received " << msg.size() << " bytes from server: " << msg;
	}
};


int main() {
	EventLoop loop;
	InetAddress serverAddr("127.0.0.1", 9877); // Server address

	Myclient client(&loop, serverAddr, "SimpleClient");

	client.connect();
	loop.loop();
	return 0;
}
