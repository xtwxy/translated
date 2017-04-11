#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

#include <translated/evq.h>


evq_dispatch_context_s* evq_create_evq_dispatch_context(
										int efd,
										int fd,
										void* user_data,
										evq_dispose_user_data_fn dispose_user_data_fn,
										evq_dispatch_cb dispach_cb) {

	evq_dispatch_context_s* d = (evq_dispatch_context_s*) malloc(
			sizeof(evq_dispatch_context_s));
	memset(d, 0, sizeof(evq_dispatch_context_s));

	d->fd = fd;
	d->dispose_user_data_fn = dispose_user_data_fn;
	d->dispatch_cb = dispach_cb;
	d->user_data = user_data;

	return d;
}

void evq_dispose_dispatch_context(evq_dispatch_context_s* d) {
	d->dispose_user_data_fn(d->user_data);
	free(d);
}

int evq_create_socket_and_bind(char* node, char *service) {
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s, sfd;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET; /* Return IPv4 and IPv6 choices */
	hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
	hints.ai_flags = AI_PASSIVE; /* All interfaces */

	s = getaddrinfo(node, service, &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		return -1;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue;

		s = bind(sfd, rp->ai_addr, rp->ai_addrlen);
		if (s == 0) {
			/* We managed to bind successfully! */
			break;
		}

		close(sfd);
	}

	if (rp == NULL) {
		fprintf(stderr, "Could not bind\n");
		return -1;
	}

	freeaddrinfo(result);

	return sfd;
}

int evq_make_fd_non_blocking(int sfd) {
	int flags, s;

	flags = fcntl(sfd, F_GETFL, 0);
	if (flags == -1) {
		perror("fcntl");
		return -1;
	}

	flags |= O_NONBLOCK;
	s = fcntl(sfd, F_SETFL, flags);
	if (s == -1) {
		perror("fcntl");
		return -1;
	}

	return 0;
}

int evq_add_to_monitoring(int efd, int fd, evq_dispatch_context_s* context) {
	struct epoll_event event;

	event.events = EPOLLIN | EPOLLET; //EPOLLOUT
	event.data.ptr = context;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, fd, &event) == 0) {
		// success
		return 0;
	} else {
		return -1;
	}
}

void evq_event_loop(int epollfd, int max_events) {
	int nfds;
	evq_dispatch_context_s* context;
	struct epoll_event* events = calloc(max_events, sizeof(struct epoll_event));

	for(;;) {
		nfds = epoll_wait(epollfd, events, max_events, -1);

		if(nfds == -1) {
			perror("evq_wait()");
			break;
		}
		for (int i = 0; i < nfds; ++i) {
			context = events[i].data.ptr;
			context->dispatch_cb(context, &events[i]);
		}
	}
	free(events);
}

int evq_init(
		char* node,
		char* service,
		void* user_data,
		evq_dispose_user_data_fn dispose_user_data_fn,
		evq_dispatch_cb dispatch_cb) {
	int listen_sock, epollfd;

	listen_sock = evq_create_socket_and_bind(node, service);
	if(evq_make_fd_non_blocking(listen_sock) == -1) {
		perror("make_fd_non_blocking: listen_sock");
				exit(EXIT_FAILURE);
	}
	epollfd = epoll_create1(0);
	if (epollfd == -1) {
		perror("evq_create1");
		exit(EXIT_FAILURE);
	}

	evq_dispatch_context_s* context = evq_create_evq_dispatch_context(epollfd,
			listen_sock,
			user_data,
			dispose_user_data_fn,
			dispatch_cb);

	if (evq_add_to_monitoring(epollfd, listen_sock, context) == -1) {
		perror("evq_ctl: listen_sock");
		exit(EXIT_FAILURE);
	}
	return epollfd;
}
