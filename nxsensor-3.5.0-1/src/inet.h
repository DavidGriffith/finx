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
 
#if !defined(WIN32)
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#else
#include <winsock2.h>
#endif


#if defined(__linux__)
#if defined(IPPROTO_IPV6)
#define INET6
#endif
#endif /* __linux__ */

#if defined(__FreeBSD__)
#include <osreldate.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/sysctl.h>
#include <net/route.h>
#if defined(__KAME__) && !defined(INET6)
#define INET6
#endif
#endif  /* __FreeBSD__ */

#if defined(__APPLE__)
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/sysctl.h>
#include <net/route.h>
#define INET6
#endif /* __APPLE__ */

#if defined(__NetBSD__) || defined(__OpenBSD__)
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#endif	/* __NetBSD__ || __OpenBSD__ */

#if defined(__solaris__)
/* IPv6 ? */
#include <netconfig.h>
#if defined(NC_INET6)
#define INET6
#endif
#endif /* __solaris__ */


  /* Values for state.
  */
#define	TCP_DEAD	0
#define	TCP_ALIVE	1

typedef struct
	{
	gint			state;
	gint			family;
	gint			local_port;
	struct in_addr	remote_addr;
#if defined(INET6)
	struct in6_addr	remote_addr6;
#endif
	gint			remote_port;
	gint			new_hit;
	}
	ActiveTCP;

