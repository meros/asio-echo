#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost;

class Session {
public:
	Session(io_service& io) :
			socket(io) {
	}

	void start_receive() {
		socket.async_receive(buffer(buf),
				bind(&Session::receive_done, this,
						placeholders::bytes_transferred(),
						placeholders::error()));
	}

	tcp::socket socket;

private:
	void receive_done(size_t bytes_transferred,
			const system::error_code& error) {
		if (!error) {
			async_write(socket, buffer(buf, bytes_transferred),
					bind(&Session::write_done, this, placeholders::error()));
		} else {
			delete this;
		}
	}

	void write_done(const system::error_code& error) {
		if (!error) {
			start_receive();
		} else {
			delete this;
		}
	}

	char buf[1024];
};

class Listener {
public:
	Listener(io_service& io) :
			acceptor(io, tcp::endpoint(tcp::v4(), 1234)) {
		start_listen();
	}

	tcp::acceptor acceptor;

private:
	void start_listen() {
		Session* session = new Session(acceptor.get_io_service());
		acceptor.async_accept(session->socket,
				bind(&Listener::listen_complete, this, session,
						placeholders::error()));
	}

	void listen_complete(Session* session, const system::error_code& error) {
		if (!error) {
			session->start_receive();
			start_listen();
		} else {
			delete session;
			delete this;
		}
	}
};

int main(int argc, char **argv) {
	io_service io;
	Listener listener(io);
	io.run();
}

