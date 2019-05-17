#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h>

#include <sys/time.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <err.h>

#include <event.h>

#include <string>

#include <glog/logging.h>
#include <libconfig.h++>

struct client {
	struct event ev_read;
};

int setnonblock(int fd)
{
	int flags = fcntl(fd, F_GETFL);
	if (flags < 0) {
        return flags;
    }
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) < 0) {
        return -1;
    }
		
    return 0;
}

void on_read(int fd, short ev, void *arg)
{
	struct client *client = (struct client *)arg;
	u_char buf[8196] = {};
	
    int len = read(fd, buf, sizeof(buf));
	if (len == 0) {
		LOG(INFO) << "Client disconnected.";
        close(fd);        
		event_del(&client->ev_read);
		free(client);
		return;
	}
	else if (len < 0) {
		LOG(WARNING) << "Socket failure, disconnecting client: " << strerror(errno);
		close(fd);
		event_del(&client->ev_read);
		free(client);		
		return;
	}

    int wlen = write(fd, buf, len);
    if (wlen < len) {
		LOG(WARNING) << "Short write, not all data echoed back to client.";
    }
}

void on_accept(int fd, short ev, void *arg)
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	
	int client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd == -1) {
		LOG(WARNING) << "accept failed";
		return;
	}

	if (setnonblock(client_fd) < 0) {
		LOG(WARNING) << "failed to set client socket non-blocking";
	}

	struct client *client = (struct client *)calloc(1, sizeof(*client));

	event_set(&client->ev_read, client_fd, EV_READ | EV_PERSIST, on_read, client);
	event_add(&client->ev_read, NULL);
	LOG(INFO) << "Accepted connection from " << inet_ntoa(client_addr.sin_addr);
}

int main(int argc, char* argv[]) {

	libconfig::Config cfg;
	cfg.readFile("/etc/testdockerecho/testdockerecho.cfg");

    std::string listenaddr = cfg.lookup("server.listenaddr");
	int listenport = cfg.lookup("server.listenport");

	LOG(INFO) << "listen: " << listenaddr << ":" << listenport;

	FLAGS_logtostderr = true;
	// FLAGS_colorlogtostderr = false;

	google::InitGoogleLogging("testdockerecho");

	LOG(INFO) << "program: " << argv[0] << "start echo";

	event_init();
	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0) {
		LOG(FATAL) << "listen failed";
	}

	int reuseaddr_on = 1;
	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_on, sizeof(reuseaddr_on)) == -1) {
		LOG(FATAL) << "setsockopt failed";
	}

	struct sockaddr_in listen_addr;
	memset(&listen_addr, 0, sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = inet_addr(listenaddr.c_str());
	listen_addr.sin_port = htons(listenport);

	if (bind(listen_fd, (struct sockaddr *)&listen_addr, sizeof(listen_addr)) < 0) {
		LOG(FATAL) << "bind failed";
	}

	if (listen(listen_fd, 5) < 0) {
		LOG(FATAL) << "listen failed";
	}

	if (setnonblock(listen_fd) < 0) {
		LOG(FATAL) << "failed to set server socket to non-blocking";
	}

	struct event ev_accept;
	event_set(&ev_accept, listen_fd, EV_READ|EV_PERSIST, on_accept, NULL);
	event_add(&ev_accept, NULL);

	event_dispatch();
    return 0;
}
