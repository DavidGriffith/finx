/* GKrellM
|  Copyright (C) 1999-2006 Bill Wilson
|
|  Author:  Bill Wilson    billw@gkrellm.net
|  Latest versions might be found at:  http://gkrellm.net
|
|  OpenBSD code derived from FreeBSD code by: Hajimu UMEMOTO <ume@FreeBSD.org>
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

#include <kvm.h>

kvm_t	*kvmd = NULL;
char	errbuf[_POSIX2_LINE_MAX];


void
gkrellm_sys_main_init(void)
	{
	/* We just ignore error, here.  Even if GKrellM doesn't have
	|  kmem privilege, it runs with available information.
	*/
	kvmd = kvm_openfiles(NULL, NULL, NULL, O_RDONLY, errbuf);
	if (setgid(getegid()) != 0)
		{
		fprintf(stderr, "Can't drop setgid privileges.");
		exit(1);
		}
	}

void
gkrellm_sys_main_cleanup(void)
	{
	}


/* ===================================================================== */
/* CPU monitor interface */

#include <sys/dkstat.h>
#include <kvm.h>

extern	kvm_t	*kvmd;

void
gkrellm_sys_cpu_read_data(void)
	{
	long		cp_time[CPUSTATES];
	static struct nlist nl[] = {
#define N_CP_TIME	0
		{ "_cp_time" },
		{ "" }
	};


	if (kvmd == NULL)
		return;
	if (nl[0].n_type == 0)
		if (kvm_nlist(kvmd, nl) < 0 || nl[0].n_type == 0)
			return;
	if (kvm_read(kvmd, nl[N_CP_TIME].n_value,
		     (char *)&cp_time, sizeof(cp_time)) != sizeof(cp_time))
		return;

	/* Currently, SMP is not supported */
	gkrellm_cpu_assign_data(0, cp_time[CP_USER], cp_time[CP_NICE],
				cp_time[CP_SYS], cp_time[CP_IDLE]);
	}

gboolean
gkrellm_sys_cpu_init(void)
    {
	gkrellm_cpu_set_number_of_cpus(1);
	return TRUE;
	}


/* ===================================================================== */
/* Proc monitor interface */

#include <sys/proc.h>
#include <sys/sysctl.h>
#include <uvm/uvm_extern.h>
#include <kvm.h>

#include <utmp.h>

extern	kvm_t	*kvmd;

void
gkrellm_sys_proc_read_data(void)
{
   static int mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL };
   double avenrun;
	guint	n_forks = 0, n_processes = 0;
   struct uvmexp *uvmexp;
   int i;
   size_t len;

	static struct nlist nl[] = {
#define X_UVM_EXP    0
	   { "_uvmexp" },
	   { NULL }
	};


   if (sysctl(mib, 3, NULL, &len, NULL, 0) >= 0) {
      n_processes = len / sizeof(struct kinfo_proc);
   }

   /* get name list if it is not done yet */
   if (kvmd == NULL) return;
   if (nl[0].n_type == 0) kvm_nlist(kvmd, nl);

   if (nl[0].n_type != 0) {
      uvmexp = (struct uvmexp *)nl[X_UVM_EXP].n_value;
      if (kvm_read(kvmd, (u_long)&uvmexp->forks, &i, sizeof(i)) == sizeof(i))
	 n_forks = i;
   }

   if (getloadavg(&avenrun, 1) <= 0)
		avenrun = 0;
	gkrellm_proc_assign_data(n_processes, 0, n_forks, avenrun);
}

void
gkrellm_sys_proc_read_users(void)
	{
	gint	n_users;
   static time_t utmp_mtime;
   struct utmp utmp;
   struct stat s;
   FILE *ut;

	if (stat(_PATH_UTMP, &s) == 0 && s.st_mtime != utmp_mtime)
		{
		if ((ut = fopen(_PATH_UTMP, "r")) != NULL)
			{
			n_users = 0;
			while (fread(&utmp, sizeof(utmp), 1, ut))
				{
				if (utmp.ut_name[0] == '\0') continue;
					++n_users;
				}
			(void)fclose(ut);
			gkrellm_proc_assign_users(n_users);
			}
		utmp_mtime = s.st_mtime;
		}
	}

gboolean
gkrellm_sys_proc_init(void)
	{
	return TRUE;
	}


/* ===================================================================== */
/* Sensor monitor interface - not implemented */

gboolean
gkrellm_sys_sensors_get_temperature(gchar *device_name, gint id,
		gint iodev, gint interface, gfloat *temp)
	{
	return FALSE;
	}

gboolean
gkrellm_sys_sensors_get_fan(gchar *device_name, gint id,
		gint iodev, gint interface, gfloat *fan)
	{
	return FALSE;
	}

gboolean
gkrellm_sys_sensors_get_voltage(gchar *device_name, gint id,
		gint iodev, gint interface, gfloat *volt)
	{
	return FALSE;
	}

gboolean
gkrellm_sys_sensors_init(void)
	{
	return FALSE;
	}

