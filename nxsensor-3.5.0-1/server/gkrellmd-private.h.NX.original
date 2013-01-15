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


#include "../src/gkrellm-sysdeps.h"

#if defined(__linux__)
#if defined(__GLIBC__) && ((__GLIBC__>2)||(__GLIBC__==2 && __GLIBC_MINOR__>=1))
#define HAVE_GETADDRINFO	1
#endif
#endif

#if defined(__FreeBSD__)
#include <sys/param.h>
#if __FreeBSD_version >= 400000
#define HAVE_GETADDRINFO	1
#endif
#endif

#if defined(__OpenBSD__)
#define HAVE_GETADDRINFO	1
#endif

#if defined(__NetBSD__)
#define HAVE_GETADDRINFO	1
#  if __NetBSD_Version__ <= 105010000
#    define sa_family_t unsigned char
#  endif
#endif

#if defined(__solaris__)
# include <netconfig.h>
# if defined(NC_INET6)
#  define HAVE_GETADDRINFO	1
# endif
#endif

#if defined(__APPLE__)
# ifndef socklen_t
#  define socklen_t int
# endif
#define HAVE_GETADDRINFO   1
#endif

#ifndef	NI_WITHSCOPEID
#define	NI_WITHSCOPEID	0
#endif

#if !defined(__FreeBSD__) && !defined(__linux__) && !defined(__NetBSD__) \
    && !defined(__OpenBSD__) && !defined(__solaris__) && !defined(WIN32) \
    && !defined(__APPLE__)
#define  USE_LIBGTOP
#endif

extern int	errno;

#define DEBUG_SYSDEP	0x1
#define DEBUG_MAIL		0x10
#define DEBUG_NET		0x20
#define DEBUG_TIMER		0x40
#define DEBUG_SENSORS	0x80
#define DEBUG_REMOTE	0x100
#define	DEBUG_BATTERY	0x8000


#define SENSOR_TEMPERATURE  0
#define SENSOR_FAN          1
#define SENSOR_VOLTAGE      2

#define	SENSOR_GROUP_MAINBOARD	0
#define	SENSOR_GROUP_DISK		1


struct GkrellmdConfig
	{
	gint		update_HZ;
	gint		debug_level;
	gint		*server_fd;
	gint		max_clients;
	gint		server_port;
	gint		verbose;
	time_t		start_time;
	time_t		time_now;
	gint		io_timeout;
	gint		reconnect_timeout;
	gint		mbmon_port;

	gint		fs_interval,
				nfs_interval,
				inet_interval;

	gboolean	without_libsensors;

	gboolean	list_plugins,
				log_plugins;
	gchar		*command_line_plugin;

	gchar		*pidfile;
	gchar		*homedir;

	gchar		*net_timer;

        /*
         * Added for NX.
         */
	gint		delay_flag;
        gint            use_config_files;
        gint            use_auth_file;
        gchar           *auth_file;
        gchar           *auth_cookie;

	};

typedef struct
	{
	gchar	*name,
			*line;
	}
	PluginConfigRec;

extern struct GkrellmdConfig	_GK;

extern gchar	*plugin_install_log;

typedef	void (*GkrellmdFunc)();

extern GList	*gkrellmd_client_list,
				*gkrellmd_plugin_enable_list,
				*gkrellmd_plugin_config_list;

void			gkrellmd_client_read(gint client_fd, gint nbytes);
void			gkrellmd_load_monitors(void);
GList			*gkrellmd_plugins_load(void);
gint			gkrellmd_update_monitors(void);
gint			gkrellmd_update_monitors_delay(void);
void			gkrellmd_serve_setup(GkrellmdClient *client);

GkrellmdMonitor *gkrellmd_init_mail_monitor(void);

