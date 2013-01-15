/* GKrellM
|  Copyright (C) 1999-2006 Bill Wilson
|
|  Author:  Bill Wilson    billw@gkrellm.net
|  Latest versions might be found at:  http://gkrellm.net
|
|  NetBSD/OpenBSD code: Anthony Mallet <anthony.mallet@useless-ficus.net>
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


/* ===================================================================== */
/* Disk monitor interface */

#include <sys/dkstat.h>
#include <sys/disk.h>
#include <kvm.h>

static struct nlist nl_disk[] = {
#define X_DISK_COUNT    0
   { "_disk_count" },      /* number of disks */
#define X_DISKLIST      1
   { "_disklist" },        /* TAILQ of disks */
   { NULL },
};

static struct disk *dkdisks;	/* kernel disk list head */

extern	kvm_t	*kvmd;

static gint	n_disks;

gchar *
gkrellm_sys_disk_name_from_device(gint device_number, gint unit_number,
			gint *order)
	{
	return NULL;	/* disk data by device not implemented */
	}

gint
gkrellm_sys_disk_order_from_name(gchar *name)
	{
	return -1;  /* append disk charts as added */
	}


void
gkrellm_sys_disk_read_data(void)
{
   struct disk	d, *p;
   gint			i;
   char			buf[20];
   guint64		rbytes, wbytes;

   if (kvmd == NULL) return;
   if (n_disks <= 0) return;		/* computed by register_disks() */
   if (nl_disk[0].n_type == 0) return;	/* ditto. */

   for (i = 0, p = dkdisks; i < n_disks; p = d.dk_link.tqe_next, ++i)
	{
	if (kvm_read(kvmd, (u_long)p, &d, sizeof(d)) == sizeof(d))
		{
		if (kvm_read(kvmd, (u_long)d.dk_name, buf, sizeof(buf)) != sizeof(buf))
			/* fallback to default name if kvm_read failed */
			sprintf(buf, "%s%c", _("Disk"), 'A' + i);
	 
		/* It seems impossible to get the read and write transfers
		 * separately. Its just a matter of choice to put the total in
		 * the rbytes member but I like the blue color so much.
		*/

		/* Separate read/write stats were implemented in NetBSD 1.6K.
		*/

#if (__NetBSD_Version__ >= 106110000) || defined(__OpenBSD__)
		rbytes = d.dk_rbytes;
		wbytes = d.dk_wbytes;
#else
		rbytes = d.dk_bytes;
		wbytes = 0;
#endif

		gkrellm_disk_assign_data_by_name(buf, rbytes, wbytes, FALSE);
		}
	}
}

gboolean
gkrellm_sys_disk_init(void)
{
   struct disklist_head head;


   if (kvmd == NULL) return FALSE;

   /* get disk count */
   if (kvm_nlist(kvmd, nl_disk) >= 0 && nl_disk[0].n_type != 0)
      if (kvm_read(kvmd, nl_disk[X_DISK_COUNT].n_value,
		   (char *)&n_disks, sizeof(n_disks)) != sizeof(n_disks))
	 n_disks = 0;

   /* get first disk */
   if (n_disks > 0) {
      if (kvm_read(kvmd, nl_disk[X_DISKLIST].n_value, 
		   &head, sizeof(head)) != sizeof(head))
	 n_disks = 0;

      dkdisks = head.tqh_first;
   }
   return TRUE;
}


/* ===================================================================== */
/* Inet monitor interface */

#include "../inet.h"

/* NO IPv6 SUPPORT YET */
#include <net/route.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/in_pcb.h>

#include <netinet/ip_var.h>
#include <netinet/tcp.h>
#include <netinet/tcpip.h>
#include <netinet/tcp_seq.h>
#include <netinet/tcp_fsm.h>
#include <netinet/tcp_timer.h>
#include <netinet/tcp_var.h>

#include <kvm.h>

extern	kvm_t	*kvmd;


void
gkrellm_sys_inet_read_tcp_data(void)
{
   ActiveTCP tcp;
   gint	tcp_status;
   struct inpcbtable table;
   struct inpcb inpcb, *head, *next;
   struct tcpcb tcpcb;
   static struct nlist nl[] = {
#define X_TCBTABLE    0
   { "_tcbtable" },
   { NULL }
};


   if (kvmd == NULL) return;
   if (nl[0].n_type == 0)
      if (kvm_nlist(kvmd, nl) < 0 || nl[0].n_type == 0)	return;

   if (kvm_read(kvmd, nl[X_TCBTABLE].n_value, (char *)&table,
		sizeof(struct inpcbtable)) != sizeof(struct inpcbtable))
      return;

	head = (struct inpcb *)
		&((struct inpcbtable *)nl[X_TCBTABLE].n_value)->inpt_queue.cqh_first;
   next = (struct inpcb *)table.inpt_queue.cqh_first;
   while(next != NULL && next != head) {
      
      if (kvm_read(kvmd, (u_long)next,
		   (char *)&inpcb, sizeof(inpcb)) == sizeof(inpcb) &&
	  kvm_read(kvmd, (u_long)inpcb.inp_ppcb,
		   (char *)&tcpcb, sizeof(tcpcb)) == sizeof(tcpcb)) {

	 tcp.local_port = ntohs(inpcb.inp_lport);
	 tcp.remote_addr.s_addr = inpcb.inp_faddr.s_addr;
	 tcp.remote_port = ntohs(inpcb.inp_fport);
	 tcp_status = (tcpcb.t_state == TCPS_ESTABLISHED);
	 tcp.family = AF_INET;
	 if (tcp_status == TCP_ALIVE)
	    gkrellm_inet_log_tcp_port_data(&tcp);
      }

      next = (struct inpcb *)inpcb.inp_queue.cqe_next;
   }
}

gboolean
gkrellm_sys_inet_init(void)
	{
	return TRUE;
	}

/* ===================================================================== */
/* Memory/Swap monitor interface */

#include <sys/vmmeter.h>
#include <sys/sysctl.h>
#include <uvm/uvm_extern.h>
#include <kvm.h>

static gulong	swapin,
				swapout;
static guint64	swap_total,
				swap_used;

void
gkrellm_sys_mem_read_data(void)
{
   static int mib[] = { CTL_VM, VM_METER };
   static int pgout, pgin;
   unsigned long	total, used, x_used, free, shared, buffers, cached;
   struct vmtotal vmt;
   struct uvmexp uvmexp;
   size_t len;
   static struct nlist nl[] = {
#define X_UVM_EXP    0
   { "_uvmexp" },
   { NULL }
};


   if (kvmd == NULL) return;

   /* get the name list if it's not done yet */
   if (nl[0].n_type == 0) kvm_nlist(kvmd, nl);

   if (nl[0].n_type != 0)
      if (kvm_read(kvmd, nl[X_UVM_EXP].n_value,
		   &uvmexp, sizeof(uvmexp)) != sizeof(uvmexp))
	 memset(&uvmexp, 0, sizeof(uvmexp));

   if (sysctl(mib, 2, &vmt, &len, NULL, 0) < 0)
      memset(&vmt, 0, sizeof(vmt));

   total = (uvmexp.npages - uvmexp.wired) << uvmexp.pageshift;

   /* not sure of what must be computed */
   x_used = (uvmexp.active + uvmexp.inactive) << uvmexp.pageshift;
   free = uvmexp.free << uvmexp.pageshift;
   shared = vmt.t_rmshr << uvmexp.pageshift;

   /* want to see only this in the chat. this could be changed */
   used = uvmexp.active << uvmexp.pageshift;

   /* don't know how to get those values */
   buffers = 0;
   cached = 0;

   gkrellm_mem_assign_data(total, used, free, shared, buffers, cached);

   /* show only the pages located on the disk and not in memory */
   swap_total = (guint64) (uvmexp.swpages << uvmexp.pageshift);
   swap_used = (guint64) (uvmexp.swpgonly << uvmexp.pageshift);

   /* For page in/out operations, uvmexp struct doesn't seem to be reliable */

   /* if the number of swapped pages that are in memory (inuse - only) is
    * greater that the previous value (pgin), we count this a "page in" */
   if (uvmexp.swpginuse - uvmexp.swpgonly > pgin)
      swapin += uvmexp.swpginuse - uvmexp.swpgonly - pgin;
   pgin = uvmexp.swpginuse - uvmexp.swpgonly;

   /* same for page out */
   if (uvmexp.swpgonly > pgout)
      swapout += uvmexp.swpgonly - pgout;
   pgout = uvmexp.swpgonly;
}

void
gkrellm_sys_swap_read_data(void)
	{
	gkrellm_swap_assign_data(swap_total, swap_used, swapin, swapout);
	}

gboolean
gkrellm_sys_mem_init(void)
	{
	return TRUE;
	}


/* ===================================================================== */
/* Battery monitor interface */
#include <sys/ioctl.h>

#if defined(__i386__) || defined(__powerpc__)

#include <machine/apmvar.h>
#define	APMDEV		"/dev/apm"

void
gkrellm_sys_battery_read_data(void)
	{
	int			f, r;
	struct apm_power_info info;
	gboolean    available, on_line, charging;
	gint        percent, time_left;

	if ((f = open(APMDEV, O_RDONLY)) == -1) return;
	memset(&info, 0, sizeof(info));
	r = ioctl(f, APM_IOC_GETPOWER, &info);
	close(f);
	if (r == -1) return;

	available = (info.battery_state != APM_BATT_UNKNOWN);
	on_line = (info.ac_state == APM_AC_ON) ? TRUE : FALSE;
	charging = (info.battery_state == APM_BATT_CHARGING) ? TRUE : FALSE;
	percent = info.battery_life;
	time_left = info.minutes_left;
	gkrellm_battery_assign_data(0, available, on_line, charging,
				percent, time_left);
	}

gboolean
gkrellm_sys_battery_init()
	{
	return TRUE;
	}

#else

void
gkrellm_sys_battery_read_data(void)
	{
	}

gboolean
gkrellm_sys_battery_init()
	{
	return FALSE;
	}
#endif
