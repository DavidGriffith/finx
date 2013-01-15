/**************************************************************************/
/*                                                                        */
/* Copyright (c) 2001, 2011 NoMachine, http://www.nomachine.com/.         */
/*                                                                        */
/* NXSSH, NX protocol compression and NX extensions to this software      */
/* are copyright of NoMachine. Redistribution and use of the present      */
/* software is allowed according to terms specified in the file LICENSE   */
/* which comes in the source distribution.                                */
/*                                                                        */
/* Check http://www.nomachine.com/licensing.html for applicability.       */
/*                                                                        */
/* NX and NoMachine are trademarks of Medialogic S.p.A.                   */
/*                                                                        */
/* All rights reserved.                                                   */
/*                                                                        */
/**************************************************************************/

/*
 * Do we share the debug log with the proxy?
 */

extern int nx_proxy_log;

/*
 * Do we check for the incoming command? This flag
 * should be bound to a command line parameter.
 */

extern int nx_check_switch;

/*
 * Did we receive the command? Switch is executed at
 * the time the first channel receives the command,
 * thus it is important to note that port forwarding
 * should always be disabled when enabling the check.
 */

extern int nx_switch_received;

/*
 * Parameters read from the switch command.
 */

extern char nx_switch_cookie[256];
extern char nx_switch_host[256];
extern int  nx_switch_proxy;
extern int  nx_switch_port;
extern int  nx_switch_in;
extern int  nx_switch_out;
extern char nx_switch_mode[256];
extern char nx_switch_options[1024];
extern int  nx_switch_internal;
extern int  nx_switch_forward;

/*
 * Buffer the input while looking for the command.
 * The buffering happens by flushing the input when
 * a newline is received. This means that all input
 * should be terminated with a newline or it will
 * remain in the buffer and will never be sent to
 * the packet interface.
 */

int nx_check_channel_input(Channel *channel, char *data, int *length, int limit);

/*
 * Replace the select() with the version managing
 * the NX descriptors.
 */

int nx_proxy_select(int maxfds, fd_set *readfds, fd_set *writefds,
                        fd_set *exceptfds, struct timeval *timeout);

/*
 * Connect to the NX transport.
 */

int nx_open_proxy_connection();

/*
 * Wait for the NX transport to terminate.
 */

int nx_close_proxy_connection();

/*
 * If cookie was passed, manage authentication.
 */

int nx_check_proxy_authentication(int proxy_fd);

/*
 * Reassign the descriptors.
 */

int nx_switch_client_side_descriptors(Channel *channel, int proxy_fd);

/*
 * Reassign the descriptors.
 */

int nx_switch_forward_descriptors(Channel *channel);

/*
 * Forward port to remote sshd.
 */

int nx_switch_forward_port(Channel *channel);

/*
 * Used in ssh.c to monitor the standard input
 * until the switch command is received.
 */

int nx_check_standard_input();

/*
 * Used in ssh.c. Connect to the proxy and run the
 * restricted loop forwarding the traffic to the
 * local proxy.
 */

void nx_switch_server_side_descriptors();

/*
 * Set the preferred options for using the socket
 * with NX.
 */

void nx_set_socket_options(int fd, int blocking);

/*
 * Wrappers used to get and set the environment.
 */

const char *nx_get_environment(const char *name);
int nx_set_environment(const char *name, const char *value);
