/* $OpenBSD: readpass.c,v 1.47 2006/08/03 03:34:42 deraadt Exp $ */
/*
 * Copyright (c) 2001 Markus Friedl.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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

#include "includes.h"

#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>
#include <fcntl.h>
#ifdef HAVE_PATHS_H
# include <paths.h>
#endif
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "xmalloc.h"
#include "misc.h"
#include "pathnames.h"
#include "log.h"
#include "ssh.h"

extern int NxAuthOnlyModeEnabled;
extern int NXStdinPassEnabled;
extern int NXServerMode;
extern int NxAdminModeEnabled;

static char *
ssh_askpass(char *askpass, const char *msg)
{
	pid_t pid;
	size_t len;
	char *pass;
	int p[2], status, ret;
	char buf[1024];

	if (fflush(stdout) != 0)
		error("ssh_askpass: fflush: %s", strerror(errno));
	if (askpass == NULL)
		fatal("internal error: askpass undefined");
	if (pipe(p) < 0) {
		error("ssh_askpass: pipe: %s", strerror(errno));
		return NULL;
	}
	if ((pid = fork()) < 0) {
		error("ssh_askpass: fork: %s", strerror(errno));
		return NULL;
	}
	if (pid == 0) {
		permanently_drop_suid(getuid());
		close(p[0]);
		if (dup2(p[1], STDOUT_FILENO) < 0)
			fatal("ssh_askpass: dup2: %s", strerror(errno));
		execlp(askpass, askpass, msg, (char *) 0);
		fatal("ssh_askpass: exec(%s): %s", askpass, strerror(errno));
	}
	close(p[1]);

	len = ret = 0;
	do {
		ret = read(p[0], buf + len, sizeof(buf) - 1 - len);
		if (ret == -1 && errno == EINTR)
			continue;
		if (ret <= 0)
			break;
		len += ret;
	} while (sizeof(buf) - 1 - len > 0);
	buf[len] = '\0';

	close(p[0]);
	while (waitpid(pid, &status, 0) < 0)
		if (errno != EINTR)
			break;

	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
		memset(buf, 0, sizeof(buf));
		return NULL;
	}

	buf[strcspn(buf, "\r\n")] = '\0';
	pass = xstrdup(buf);
	memset(buf, 0, sizeof(buf));
	return pass;
}

/*
 * Reads a passphrase from /dev/tty with echo turned off/on.  Returns the
 * passphrase (allocated with xmalloc).  Exits if EOF is encountered. If
 * RP_ALLOW_STDIN is set, the passphrase will be read from stdin if no
 * tty is available
 */
char *
read_passphrase(const char *prompt, int flags)
{
	char *askpass = NULL, *ret, buf[1024] = "";
	int rppflags, use_askpass = 0, ttyfd;

        if (!NxAuthOnlyModeEnabled && !NxModeEnabled && !NXStdinPassEnabled)
        {
	rppflags = (flags & RP_ECHO) ? RPP_ECHO_ON : RPP_ECHO_OFF;
		if (flags & RP_ALLOW_STDIN) {
			if (!isatty(STDIN_FILENO))
			use_askpass = 1;
	} else {
		rppflags |= RPP_REQUIRE_TTY;
		ttyfd = open(_PATH_TTY, O_RDWR);
		if (ttyfd >= 0)
			close(ttyfd);
		else {
			debug("read_passphrase: can't open %s: %s", _PATH_TTY,
			    strerror(errno));
			use_askpass = 1;
		}
	}

	if (use_askpass && getenv("DISPLAY")) {
		if (getenv(SSH_ASKPASS_ENV))
			askpass = getenv(SSH_ASKPASS_ENV);
		else
			askpass = _PATH_SSH_ASKPASS_DEFAULT;
				return ssh_askpass(askpass, prompt);
	}

	if (readpassphrase(prompt, buf, sizeof buf, rppflags) == NULL) {
		if (flags & RP_ALLOW_EOF)
			return NULL;
		return xstrdup("");
	}
        }
        else
        {
                size_t len;
                int  retr;
                fprintf(stdout, prompt);
                fflush(stdout);

                len = retr = 0;
                do
                {
                        retr = read(STDIN_FILENO, buf + len, sizeof(buf) - 1 - len);
                        if (retr == -1 && errno == EINTR)
                                continue;
                        if (retr <= 0)
                                break;
                        len += retr;
                } while (sizeof(buf) - 1 - len > 0 && buf[len] == '\n');

                buf[len-1] = '\0';

                fprintf(stdout, "\n");
                fflush(stdout);
        }
	ret = xstrdup(buf);
	memset(buf, 'x', sizeof buf);
	return ret;

}


int
ask_permission(const char *fmt, ...)
{
	va_list args;
	char *p, prompt[1024];
	int allowed = 0;

	va_start(args, fmt);
	vsnprintf(prompt, sizeof(prompt), fmt, args);
	va_end(args);

	p = read_passphrase(prompt, RP_USE_ASKPASS|RP_ALLOW_EOF);
	if (p != NULL) {
		/*
		 * Accept empty responses and responses consisting
		 * of the word "yes" as affirmative.
		 */
		if (*p == '\0' || *p == '\n' ||
		    strcasecmp(p, "yes") == 0)
			allowed = 1;
		xfree(p);
	}

	return (allowed);
}
