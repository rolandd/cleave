/*
 * libcleave: fork/exec daemon client library
 *
 * The cleaved source code is licensed to you under a BSD 2-Clause
 * license, included below.
 *
 * Copyright © 2013 Pure Storage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef CLEAVE_H
#define CLEAVE_H

#ifdef __cplusplus
extern "C" {
#endif

struct cleave_handle;
struct cleave_child;

/* Provide a callback for cleave logging */
void cleave_set_logfn(void (*log)(char const *str));

/* Fork a new cleave daemon.
 *
 * This handle must be destroyed by calling cleave_destroy(), which will cause
 * the new daemon to exit.
 *
 * The passed error_fd is connected to stderr of the child process. Specify
 * -1 if you don't want to see any errors.
 *
 * By default we expect "cleave" to be in PATH. If "cleave" is installed
 * outside of PATH, or installed with a different name, then you can override
 * this behaviour by setting CLEAVE_CLEAVED_FILENAME environment variable
 */
struct cleave_handle * cleave_create(int error_fd);

/* Attach to a running cleave daemon at the given socket
 *
 * This handle must be destroyed by calling cleave_destroy(). The ademon will
 * continue to run after cleave_destroy().
 */
struct cleave_handle * cleave_attach(char const *socket);

/* Detach from the running cleave daemon. */
void cleave_destroy(struct cleave_handle *handle);

/* Execute a child process
 *
 * The fd array is an array of three pipe fd's - stdin, stdout, stderr. Any
 * one of these fds can be -1 in which case the fd will be duped to /dev/null.
 *
 * The returned handle *must* be freed by calling cleave_wait().
 */
struct cleave_child * cleave_child(struct cleave_handle *handle,
				   char const **argv, int fd[3]);

/* Wait for the given child process to complete and free the handle.
 *
 * This call will deadlock if any of stdin, stdout, stderr fill and
 * aren't drained by the client.
 *
 * Returns the return code of the process, or -1.
 * The cleave_child is always invalid after this call, even if -1 is returned.
 */
pid_t cleave_wait(struct cleave_child *child);

/* Return the file descriptor that cleave_wait() uses to determine when the
 * child processes has exited. This file descriptor is one end of a pipe.
 *
 * Use this file descriptor with select/poll/epoll to determine when to
 * call cleave_wait(), which will read from the pipe. Do not read/write from
 * this file descriptor.
 */
int cleave_wait_fd(struct cleave_child *child);

/* Return the pid of the child */
pid_t cleave_pid(struct cleave_child *child);

#ifdef __cplusplus
}
#endif

#endif
