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

#include <gmodule.h>


static GList	*plugin_list;
GList			*gkrellmd_plugin_enable_list;
gchar			*plugin_install_log;


/* ======================================================================= */
/* Plugin interface to gkrellmd.
*/


static void
gkrellmd_plugin_log(gchar *string1, ...)
	{
	va_list		args;
	gchar		*s, *old_log;

	if (!string1)
		return;
	va_start(args, string1);
	s = string1;
	while (s)
		{
		old_log = plugin_install_log;
		if (plugin_install_log)
			plugin_install_log = g_strconcat(plugin_install_log, s, NULL);
		else
			plugin_install_log = g_strconcat(s, NULL);
		g_free(old_log);
		s = va_arg(args, gchar *);
		}
	va_end(args);
	}

GkrellmdMonitor *
gkrellmd_plugin_install(gchar *plugin_name)
	{
	GModule					*module;
	GkrellmdMonitor			*m;
	GkrellmdMonitor			*(*init_plugin)();
	gchar					buf[256];

	if (!g_module_supported())
		return NULL;
	module = g_module_open(plugin_name, 0);
	gkrellmd_plugin_log(plugin_name, "\n", NULL);

	if (! module)
		{
		snprintf(buf, sizeof(buf), _("\tError: %s\n"), g_module_error());
		gkrellmd_plugin_log(buf, NULL);
		return NULL;
		}
	if (!g_module_symbol(module, "gkrellmd_init_plugin",
				(gpointer) &init_plugin))
		{
		snprintf(buf, sizeof(buf), _("\tError: %s\n"), g_module_error());
		gkrellmd_plugin_log(buf, NULL);
		g_module_close(module);
		return NULL;
		}

//	mon_tmp.name = g_strdup(plugin_name);
//	gkrellm_record_state(INIT_MONITOR, &mon_tmp);

	m = (*init_plugin)();

//	g_free(mon_tmp.name);
//	mon_tmp.name = NULL;
//	gkrellm_record_state(INTERNAL, NULL);

	if (m == NULL)
		{
		gkrellmd_plugin_log(
				_("\tOoops! plugin returned NULL, not installng\n"), NULL);
		g_module_close(module);
		return NULL;
		}

	gkrellmd_plugin_log(_("\tInstalled OK\n"), NULL);

	if (!m->privat)		/* may get set in gkrellm_config_getline() */
		{
		m->privat = g_new0(GkrellmdMonitorPrivate, 1);
		m->privat->config_list = gkrellmd_plugin_config_list;
		}

	m->privat->handle = module;
	m->privat->path = plugin_name;
	if (!m->name)
		m->name = g_path_get_basename(m->privat->path);

	return m;
	}


static gchar *
gkrellmd_string_suffix(gchar *string, gchar *suffix)
	{
	gchar	*dot;

	dot = strrchr(string, '.');
	if (dot && !strcmp(dot + 1, suffix))
		return dot + 1;
	return NULL;
	}

static gboolean
gkrellmd_plugin_enabled(gchar *name)
	{
	GList	*list;
	gchar	*check, *s;
	gint	len;

	for (list = gkrellmd_plugin_enable_list; list; list = list->next)
		{
		check = (gchar *) list->data;
		s = strstr(name, check);
		len = strlen(check);
		if (   s
			&& s == name
			&& (*(name + len) == '\0' || !strcmp(name + len, ".so"))
		   )
			return TRUE;
		}
	return FALSE;
	}

static void
gkrellmd_plugin_scan(gchar *path)
	{
    GDir			*dir;
    gchar			*name, *filename;
	GList			*list;
	GkrellmdMonitor	*m = NULL;
	gchar			*s;
	gboolean		exists;
	
	if (!path || !*path || (dir = g_dir_open(path, 0, NULL)) == NULL)
		return;
	while ((name = (gchar *) g_dir_read_name(dir)) != NULL)
		{
		if (   !gkrellmd_string_suffix(name, "so")
			&& !gkrellmd_string_suffix(name, "la")
			&& !gkrellmd_string_suffix(name, "dll")
		   )
			continue;

		/* If there's a libtool .la archive, won't want to load this .so
		*/
		if (   !gkrellmd_string_suffix(name, "la")
			&& (s = strrchr(name, '.')) != NULL
		   )
			{
			s = g_strndup(name, s - name);
			filename = g_strconcat(path, G_DIR_SEPARATOR_S, s, ".la", NULL);
			exists = g_file_test(filename, G_FILE_TEST_EXISTS);
			g_free(s);
			g_free(filename);
			if (exists)
				continue;
			}
		if (_GK.list_plugins)
			{
			printf("%s    (%s)\n", name, path);
			continue;
			}
		for (list = plugin_list; list; list = list->next)
			{
			m = (GkrellmdMonitor *) list->data;
			s = g_path_get_basename(m->privat->path);
			exists = !strcmp(s, name);
			g_free(s);
			if (exists)
				break;
			m = NULL;
			}
		s = g_strconcat(path, G_DIR_SEPARATOR_S, name, NULL);
		if (m)
			{
			gkrellmd_plugin_log(_("Ignoring duplicate plugin "), s, "\n", NULL);
			g_free(s);
			continue;
			}
		if (!gkrellmd_plugin_enabled(name))
			{
			gkrellmd_plugin_log(s, "\n",
					"\tNot enabled in gkrellmd.conf - skipping\n", NULL);
			continue;
			}
		m = gkrellmd_plugin_install(s);
		if (m)	/* s is saved for use */
			plugin_list = g_list_append(plugin_list, m);
		else
			g_free(s);
		}
	g_dir_close(dir);
	}

GList *
gkrellmd_plugins_load(void)
	{
	GkrellmdMonitor	*m;
	gchar			*path;

	if (_GK.command_line_plugin)
		{
		if (   *_GK.command_line_plugin != '.'
			&& !strchr(_GK.command_line_plugin, G_DIR_SEPARATOR)
		   )
			path = g_strconcat(".", G_DIR_SEPARATOR_S,
						_GK.command_line_plugin, NULL);
		else
			path = g_strdup(_GK.command_line_plugin);
		gkrellmd_plugin_log(_("*** Command line plugin:\n"), NULL);
		if ((m = gkrellmd_plugin_install(path)) == NULL)
			g_free(path);
		else
			plugin_list = g_list_append(plugin_list, m);
		gkrellmd_plugin_log("\n", NULL);
		}

	path = g_strconcat(_GK.homedir, G_DIR_SEPARATOR_S,
				GKRELLMD_PLUGINS_DIR, NULL);
	gkrellmd_plugin_scan(path);
	g_free(path);

#if defined(GKRELLMD_LOCAL_PLUGINS_DIR)
	gkrellmd_plugin_scan(GKRELLMD_LOCAL_PLUGINS_DIR);
#endif

#if defined(GKRELLMD_SYSTEM_PLUGINS_DIR)
	gkrellmd_plugin_scan(GKRELLMD_SYSTEM_PLUGINS_DIR);
#endif

	return plugin_list;
	}
