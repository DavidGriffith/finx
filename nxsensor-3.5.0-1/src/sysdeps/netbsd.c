/* GKrellM
|  Copyright (C) 1999-2006 Bill Wilson
|
|  Author:  Bill Wilson    billw@gkrellm.net
|  Latest versions might be found at:  http://gkrellm.net
|
|  NetBSD code: Anthony Mallet <anthony.mallet@useless-ficus.net>
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
	if (setgid(getgid()) != 0)
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

#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/sched.h>

static gint ncpus;

static gint get_ncpus(void);

void
gkrellm_sys_cpu_read_data(void)
{
   static int mib[] = { CTL_KERN, KERN_CP_TIME };
   u_int64_t cp_time[ncpus][CPUSTATES];
   int len, n;

   if (ncpus > 1) {
       len = sizeof(cp_time[0]);
       /* The sysctl() is magic -- it returns the aggregate if
	  there's not enough room for all CPU's. */
       if (sysctl(mib, 2, cp_time, &len, NULL, 0) == 0)
	   gkrellm_cpu_assign_composite_data(cp_time[0][CP_USER],
	       cp_time[0][CP_NICE], cp_time[0][CP_SYS], cp_time[0][CP_IDLE]);
   }

   len = sizeof(cp_time);
   if (sysctl(mib, 2, cp_time, &len, NULL, 0) < 0) return;
   for (n = 0; n < ncpus; n++)
	gkrellm_cpu_assign_data(n, cp_time[n][CP_USER],
	   cp_time[n][CP_NICE], cp_time[n][CP_SYS], cp_time[n][CP_IDLE]);
}

gboolean
gkrellm_sys_cpu_init(void)
    {
	ncpus = get_ncpus();
	gkrellm_cpu_set_number_of_cpus(ncpus);
	return TRUE;
	}

static gint
get_ncpus(void)
{
	static int mib[] = { CTL_HW, HW_NCPU };
	int ncpus;
	int len = sizeof(int);

	if (sysctl(mib, 2, &ncpus, &len, NULL, 0) < 0)
		return 1;
	else
		return ncpus;
}


/* ===================================================================== */
/* Proc monitor interface */

#include <sys/proc.h>
#include <sys/sysctl.h>
#include <uvm/uvm_extern.h>
#include <kvm.h>

#include <utmp.h>

static struct nlist nl[] = {
#define X_UVM_EXP    0
   { "_uvmexp" },
   { NULL }
};

extern	kvm_t	*kvmd;

void
gkrellm_sys_proc_read_data(void)
{
   static int mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL };
   double avenrun;
	guint	n_forks = 0, n_processes = 0;
   struct uvmexp *uvmexp;
   int len, i;

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
/* Sensor monitor interface */

  /* Tables of voltage correction factors and offsets derived from the
  |  compute lines in sensors.conf.  See the README file.
  */
	/* "lm78-*" "lm78-j-*" "lm79-*" "w83781d-*" "sis5595-*" "as99127f-*" */
	/* Values from LM78/LM79 data sheets	*/
#if 0
static VoltDefault	voltdefault0[] =
	{
	{ "Vcor1",	1.0,    0, NULL },
	{ "Vcor2",	1.0,    0, NULL },
	{ "+3.3V",	1.0,    0, NULL },
	{ "+5V",	1.68,   0, NULL },		/* in3 ((6.8/10)+1)*@	*/
	{ "+12V",	4.0,    0, NULL },		/* in4 ((30/10)+1)*@	*/
	{ "-12V",	-4.0,   0, NULL },		/* in5 -(240/60)*@		*/
	{ "-5V",	-1.667, 0, NULL }		/* in6 -(100/60)*@		*/
	};
#endif

/* The SENSORS_DIR is not defined as a directory, but directly points on
 * the "sysmon" device, which implements envsys(4) API. This #define is
 * not really useful for NetBSD since every sensor will use that device,
 * but it still provides the location of the device inside the GUI. */
#define SENSORS_DIR	"/dev/sysmon"

#include <sys/envsys.h>


/*
 * get_netbsd_sensor ----------------------------------------------------
 *
 * Perform sensor reading
 */
#include <sys/ioctl.h>

gboolean
gkrellm_sys_sensors_get_temperature(gchar *path, gint id,
        gint iodev, gint interface, gfloat *temp)
{
   envsys_tre_data_t data;	/* sensor data */

   data.sensor = interface;
   if (ioctl(iodev, ENVSYS_GTREDATA, &data) < 0) return FALSE;
   if (!(data.validflags & (ENVSYS_FVALID|ENVSYS_FCURVALID))) return FALSE;

   if (data.units == ENVSYS_STEMP) {
      if (temp)	/* values in uK */
	     *temp = (data.cur.data_us / 1.0e6) - 273.15/*0K*/ ;
	 return TRUE;
   }

   return FALSE;
}

gboolean
gkrellm_sys_sensors_get_fan(gchar *path, gint id,
        gint iodev, gint interface, gfloat *fan)
{
   envsys_tre_data_t data;	/* sensor data */

   data.sensor = interface;
   if (ioctl(iodev, ENVSYS_GTREDATA, &data) < 0) return FALSE;
   if (!(data.validflags & (ENVSYS_FVALID|ENVSYS_FCURVALID))) return FALSE;

   if (data.units == ENVSYS_SFANRPM) {
      if (fan)	/* values in RPM */
	    *fan = data.cur.data_us;
	 return TRUE;
   }
   return FALSE;
}

gboolean
gkrellm_sys_sensors_get_voltage(gchar *path, gint id,
        gint iodev, gint interface, gfloat *volt)
{
   envsys_tre_data_t data;	/* sensor data */

   data.sensor = interface;
   if (ioctl(iodev, ENVSYS_GTREDATA, &data) < 0) return FALSE;
   if (!(data.validflags & (ENVSYS_FVALID|ENVSYS_FCURVALID))) return FALSE;

   if (data.units == ENVSYS_SVOLTS_DC) {
      if (volt)		/* values in uV */
	    *volt = data.cur.data_s / 1.0e6;
	 return TRUE;
   }

   return FALSE;
}

/*
 *
 * At the moment, only two chips are supported: lm78 and alike (see
 * lm(4)), and VT82C68A South Bridge for VIA chipsets (see viaenv(4)).
 * Both support the envsys(4) API.
 *
 * XXX /dev/sysmon is opened but never closed. This is a problem since
 * the driver wants an exclusive lock (e.g. envstat won't work when
 * GKrellM will be running). But, at this time, I don't want to
 * open/close sysmon each time a reading is needed. See README for
 * details.
 */

gboolean
gkrellm_sys_sensors_init(void)
{
   envsys_basic_info_t info;	/* sensor misc. info */
   int fd;			/* file desc. for /dev/sysmon */
   int id = 0;			/* incremented for each sensor */
   int type;
   char *s, base_name[32];
   gboolean	found_sensors = FALSE;

   /* check if some sensor is configured */
   fd = open(SENSORS_DIR, O_RDONLY); /* never closed */ 
   if (fd < 0) return FALSE;

   /* iterate through available sensors, until the first invalid */
   for(info.sensor=0; ; info.sensor++) {

      /* stop if we can't ioctl() */
      if (ioctl(fd, ENVSYS_GTREINFO, &info) < 0) break;
      /* stop if that sensor is not valid */
      if (!(info.validflags & ENVSYS_FVALID)) break;

      switch(info.units) {
	 case ENVSYS_STEMP:
	    type = SENSOR_TEMPERATURE;	break;
	 case ENVSYS_SFANRPM:
	    type = SENSOR_FAN;		break;
	 case ENVSYS_SVOLTS_DC:
	    type = SENSOR_VOLTAGE;		break;
	 default:
	    /* unwanted sensor type: continue */
	    continue;
      }

      /* ok, we've got one working sensor */
      sprintf(base_name, "%32s", info.desc);
      /* must map spaces into something else (for config file items) */
      for(s=strchr(base_name, ' '); s != NULL; s=strchr(s, ' '))
	 *s++ = '_';

      gkrellm_sensors_add_sensor(type, SENSORS_DIR, base_name,
           id, fd, info.sensor, 1.0,
           0.0, NULL, NULL);
      found_sensors = TRUE;
   }
   return found_sensors;
}

