#ifndef libevq_H
#define libevq_H

#include <netdb.h>

typedef struct evq_dispatch_context evq_dispatch_context_s;

typedef void (*evq_dispatch_cb)(
		evq_dispatch_context_s*,
		int efd,
		struct epoll_event*
		);

typedef void(*evq_dispose_user_data_fn)(void*);

struct evq_dispatch_context {
	int fd;
	void* user_data;
	evq_dispose_user_data_fn dispose_user_data_fn;
	evq_dispatch_cb dispatch_cb;
};

#ifdef __cplusplus
	extern "C" {
#endif

int evq_create_socket_and_bind(
						char* node,
						char *service
						);

evq_dispatch_context_s* evq_create_evq_dispatch_context(
											int efd,
											int fd,
											void* user_data,
											evq_dispose_user_data_fn dispose_user_data_fn,
											evq_dispatch_cb dispach_cb
											);

void evq_dispose_dispatch_context(evq_dispatch_context_s* d);

int evq_make_fd_non_blocking(int sfd);

int evq_add_to_monitoring(
						int efd,
						int fd,
						evq_dispatch_context_s* context
						);

void evq_event_loop(
					int epollfd,
					int max_events
					);

int evq_init(
		char* node,
		char* service,
		void* user_data,
		evq_dispose_user_data_fn dispose_user_data_fn,
		evq_dispatch_cb dispatch_cb);

#ifdef __cplusplus
	}
#endif

#endif
