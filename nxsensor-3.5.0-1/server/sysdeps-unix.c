/* GKrellM
|  Copyright (C) 1999-2006 Bill Wilson
|
|  Author:  Bill Wilson    billw@gkrellm.net
|  Latest versions might be found at:  http://gkrellm.net
|
|  This program is free software which I release under the GNU General Public
|  License. You may redistribute and/or modify this program under the terms
|  of that license as published by the Free Software Foundation; either
|  version 2 of the License, or (at your option) any later version.
|
|  This program is distributed in the hope that it will be useful,
|  but WITHOUT ANY WARRANTY; without even the implied warranty of
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
|  GNU General Public License for more details.  Version 2 is in the
|  COPYRIGHT file in the top level directory of this distribution.
| 
|  To get a copy of the GNU General Puplic License, write to the Free Software
|  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "gkrellmd.h"
#include "gkrellmd-private.h"

#include "../src/gkrellm-sysdeps.h"

static gboolean (*mbmon_check_func)();
static gint gkrellm_connect_to(gchar *, gint);

#if defined(__linux__)
#include "../src/sysdeps/linux.c"
#include "../src/sysdeps/sensors-common.c"
#endif

#if defined(__APPLE__)
#include "../src/sysdeps/darwin.c"
#include "../src/sysdeps/bsd-common.c"
#endif

#if defined(__FreeBSD__)
#include "../src/sysdeps/freebsd.c"
#include "../src/sysdeps/bsd-common.c"
#include "../src/sysdeps/sensors-common.c"
#endif

#if defined(__NetBSD__)
#include "../src/sysdeps/netbsd.c"
#include "../src/sysdeps/bsd-net-open.c"
#include "../src/sysdeps/bsd-common.c"
#endif

#if defined(__OpenBSD__)
#include "../src/sysdeps/openbsd.c"
#include "../src/sysdeps/bsd-net-open.c"
#include "../src/sysdeps/bsd-common.c"
#endif


#if defined(__solaris__)
#include "../src/sysdeps/solaris.c"
#endif

#if defined(USE_LIBGTOP)
#include "../src/sysdeps/gtop.c"
#endif

#include <sys/utsname.h>

gchar *
gkrellm_sys_get_host_name(void)
	{
	static gboolean	have_it;
	static gchar	buf[128];

	if (!have_it && gethostname(buf, sizeof(buf)))
		strcpy(buf, "unknown");
	have_it = TRUE;
	return buf;
	}

gchar *
gkrellm_sys_get_system_name(void)
	{
	static gchar	*sname;
	struct utsname	utsn;

	if (!sname && uname(&utsn) > -1)
		sname = g_strdup_printf("%s %s", utsn.sysname, utsn.release);
	if (!sname)
		sname = g_strdup("unknown name");
	return sname;
	}


  /* Remove embedded "-i2c-" or "-isa-" from lm_sensors chip names so
  |  there can be a chance for config name sysfs compatibility.  This function
  |  here in sensors.c is a kludge.  Give user configs a chance to get
  |  converted and then move this function to sysdeps/linux.c where it
  |  belongs.
  |  Munge names like w83627hf-isa-0290 to w83627hf-0290
  |                or w83627hf-i2c-0-0290 to w83627hf-0-0290
  */
void
gkrellm_sensors_linux_name_fix(gchar *id_name)
	{
#if defined(__linux__)
	gchar	*s;
	gint	len, bus = 0;
	guint	addr = 0;

	len = strlen(id_name) + 1;
	if ((s = strstr(id_name, "-i2c-")) != NULL)
		{
		sscanf(s + 5, "%d-%x", &bus, &addr);
		snprintf(s, len - (s - id_name), "-%d-%04x", bus, addr);
		}
	else if ((s = strstr(id_name, "-isa-")) != NULL)
		{
		*(s + 1) = '0';
		memmove(s + 2, s + 4, strlen(s + 4) + 1);
		}
#endif
	}

#ifdef SENSORS_COMMON
static gint
gkrellm_connect_to(gchar *server, gint server_port)
	{
	gint		fd	= -1;
#ifdef HAVE_GETADDRINFO
	gint 		rv	= 0;
	struct addrinfo	hints, *res, *res0;
	gchar		portnumber[6];
#else
	struct hostent	*addr;
	struct sockaddr_in s;
#endif

#ifdef HAVE_GETADDRINFO
	snprintf (portnumber, sizeof(portnumber), "%d", server_port);
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	if ((rv = getaddrinfo(server, portnumber, &hints, &res0)) != 0)
		return -1;

	for (res = res0; res; res = res->ai_next)
		{
		if ((fd = socket(res->ai_family, res->ai_socktype,
				res->ai_protocol)) < 0)
			continue;
		if (_GK.debug_level & DEBUG_SENSORS)
			printf("\t[gkrellm_connect_to: (%d,%d,%d) %s:%d]\n",
			       res->ai_family, res->ai_socktype,
			       res->ai_protocol, server, server_port);
		if (connect(fd, res->ai_addr, res->ai_addrlen) >= 0)
			break;
#ifdef WIN32
		closesocket(fd);
#else
		close(fd);
#endif
		fd = -1;
		}
	freeaddrinfo(res0);
#else
	if (_GK.debug_level & DEBUG_SENSORS)
		printf("\t[gkrellm_connect_to: %s:%d]\n", server, server_port);
	addr = gethostbyname(server);
	if (addr)
		{
		fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd >= 0)
			{
			memset(&s, 0, sizeof(s));
			memcpy(&s.sin_addr.s_addr, he->h_addr, he->h_length);
			s.sin_family = AF_INET;
			s.sin_port = htons(server_port);
			if (connect(fd, (struct sockaddr *)&s, sizeof (s)) < 0)
				{
#ifdef WIN32
				closesocket(fd);
#else
				close(fd);
#endif
				fd = -1;
				}
			}
		}
#endif
	if (fd < 0)
		return -1;

	return fd;
	}
#endif
