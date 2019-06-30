/*
multibootusb.c from Slackel <http://www.slackel.gr>
Copyright (C) 2019 Dimitris Tzemos <dijemos@gmail.com>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License at <http://www.gnu.org/licenses/> for more details.
*/

#include <gtk/gtk.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <libintl.h>
#include <locale.h>
#include <sys/wait.h> 
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#define _(String) gettext (String)
#define N_(String) String
#include "config.h"
#include "multibootusb.h"


bool is_file(const char* path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISREG(buf.st_mode);
}

bool is_dir(const char* path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}	

guint64 directorysize(gchar *dir) {
	FILE *fp;
	fp = fopen("set_dir.sh", "w+");
    fprintf(fp,"#/bin/bash\n");
	fprintf(fp,"#\n");
	fprintf(fp,"du  -BM ");
	fprintf(fp,dir);
	fprintf(fp," | cut -d'M' -f1 > dirsize\n");
	fprintf(fp,"cat dirsize");
	fclose(fp);
	gchar *output;
	guint64 dirsize;
	g_spawn_command_line_sync("sh set_dir.sh ", &output, NULL, NULL, NULL);
	dirsize = g_ascii_strtoull(output, NULL, 10);
	system("rm dirsize");
	return dirsize;
}

guint64 file_length(char* filename)
{
FILE *f = fopen ( filename, "r" );
guint64 pos;
guint64 end;

pos = ftell (f);
fseek (f, 0, SEEK_END);
end = ftell (f);
fseek (f, pos, SEEK_SET);
end=end/1024/1024;
return end;
}

void get_current_zone(gchar dest[2][30])
{ 
	gchar *output,*kzone,**lines,**dummy;
	gint status, i;
	g_spawn_command_line_sync("sh get_current_zone.sh kzone", &output, NULL, &status, NULL);
	if (status == 0) {
		dummy = g_strsplit(output, "\n", 0);
		lines = g_strsplit(dummy[0], "/", 0);
		for (i=0; lines[i] != NULL && strlen(lines[i])>0; i++) {
		strcpy(dest[0],lines[0]);
		strcpy(dest[1],lines[1]);
	}
	g_strfreev(lines);
 }
	g_free(output); 
	if (g_file_test("zone", G_FILE_TEST_EXISTS)) {
		system("rm zone");}
}

void setkeymap(gchar *keymap) {
	FILE *fp;
	fp = fopen("set_keymap.sh", "w+");
    fprintf(fp,"#/bin/bash\n");
	fprintf(fp,"#\n");
	fprintf(fp,"/usr/sbin/keyboardsetup -k ");
	fprintf(fp,keymap);
	fprintf(fp,"\n");
	fclose(fp);
}	

void settimezone(gchar *name, gchar *continent, gchar *location){
	gchar current_zone[2][30];
	get_current_zone(current_zone);
			if (symlink(name, "/etc/localtime-copied-from")!=0){
				unlink("/etc/localtime-copied-from");
				symlink(name, "/etc/localtime-copied-from");
				}
			system("rm -f /etc/localtime");
			system("cp /etc/localtime-copied-from /etc/localtime");
}

void system_locale(gchar *name)
{
FILE *f = fopen(name, "w");
if (f == NULL)
{
    printf("Error opening file!\n");
    exit(1);
}

int i;
gchar *lines[6] = {"#!/bin/sh\n#\n",	"echo `", "locale | grep LANG= | sed 's/LANG=//' "," | sed 's/UTF-8/utf8/' " ,"` > system_locale\n",
					"cat system_locale\n"
					};
  for (i=0 ; i<6; i++) {
			fprintf(f, "%s", lines[i]);
		}

fclose(f);
}

void get_system_locale(gchar *dest)
{ 
	gchar *output,*locale,**lines;
	gint status, i,count;
    count=0;
	system_locale("get_system_locale.sh");
	g_spawn_command_line_sync("sh get_system_locale.sh locale", &output, NULL, &status, NULL);
	if (status == 0) {
		lines = g_strsplit(output, "\n", 0);
		for (i=0; lines[i] != NULL && strlen(lines[i])>0; i++) {
			strcpy(dest,lines[0]);
	}
	count=i;
	g_strfreev(lines);
 }
	g_free(output); 
}

void set_locale(gchar *locale) {
	FILE *fp;
	fp = fopen("set_locale.sh", "w+");
    fprintf(fp,"#/bin/bash\n");
	fprintf(fp,"#\n");
	fprintf(fp,"localesetup ");
	fprintf(fp,locale);
	fprintf(fp,"\n");
	fclose(fp);
}	

char * create_keymaplist(char *name, char *keymaplist)
{
FILE *f = fopen(name, "w");
if (f == NULL)
{
    printf("Error opening file!\n");
    exit(1);
}

 int i;
char *lines[5] = {"#!/bin/sh\n#\n", "ls  /usr/share/kbd/keymaps/i386/",
					keymaplist,"/ | sed 's/.map.gz//' > keymaplist\n",
					"cat keymaplist\n"
					};
  for (i=0 ; i<5; i++) {
			fprintf(f, "%s", lines[i]);
		}

fclose(f);
}


void readfile(gchar registered_keymaplist[131][30]){
	static const char filename[] = "/usr/share/salixtools/keymaps";
	FILE *file = fopen ( filename, "r" );
	char dest1[133][30];
	int i,ii=0,i2=0;
	if ( file != NULL )
	{ 
		char line [ 128 ]; 
		while ( fgets ( line, sizeof line, file ) != NULL ) 
		{  size_t ln = strlen(line) - 1; for (i=0;i<ln;i++) if (line[i] == '|' || line[i] == '#' ||  line[i] == '\n') {line[i] = '\0';}
		   strcpy(dest1[ii],line);
			ii++;	
		}
		for (i=3;i<ii;i++) {	
				strcpy(registered_keymaplist[i-3],dest1[i]);
							}
		fclose ( file );
   }
   else
   {
      perror ( filename ); 
   }
}

int is_registered_keymap(char *keymap) {
	char registered_keymaplist[131][30];
	int i,found=0;
	readfile(registered_keymaplist);
	for (i=0;i<131;i++) {
		 if (strcmp(keymap,registered_keymaplist[i])==0) {
							found=1;return found;
							}
						}
	return found;
}

void get_keymap(gchar *dest)
{ 
	gchar *output,*kmap,**lines;
	gint status, i,count;
    count=0;
	memset(dest, '\0', sizeof(dest));
	g_spawn_command_line_sync("sh get_current_keymap.sh kmap", &output, NULL, &status, NULL);
	if (status == 0) {
		lines = g_strsplit(output, "\n", 0);
		for (i=0; lines[i] != NULL && strlen(lines[i])>0; i++) {
			strcpy(dest,lines[0]);
	}
	count=i;
	g_strfreev(lines);
 }
	g_free(output); 
}

void get_keybtype(gchar *dest)
{ 
	gchar *output,*keybtype,**lines;
	gint status, i;
	memset(dest, '\0', sizeof(dest));
	g_spawn_command_line_sync("sh get_current_keybtype.sh keybtype", &output, NULL, &status, NULL);
	if (status == 0) {
		lines = g_strsplit(output, "\n", 0);
		for (i=0; lines[i] != NULL && strlen(lines[i])>0; i++) {
			strcpy(dest,lines[0]);
	}
	g_strfreev(lines);
 }
	g_free(output); 
}

void do_action (gboolean copy) {
	gchar *commandline, **command, *output, *fstype, *usbfstype, *isodir, *iso_arch, *usblabel, *isofile, *timezone, *language, *keyboard;
	GtkComboBox *listwidget;
	GtkTreeIter iter;
	GtkListStore *list;
	GtkWidget *usb_label_name, *isoname, *label;
	isoname = (GtkWidget *) gtk_builder_get_object(widgetstree, "isoname");
	isodir = g_strdup(gtk_entry_get_text (GTK_ENTRY(isoname)));
	//isofile = g_strdup(gtk_entry_get_text (GTK_ENTRY(isoname)));
	/*if (strstr(isofile,"64")!= NULL) {
			iso_arch="64";
		}
		else { 
			iso_arch="32";
		}
	*/
	usb_label_name = (GtkWidget *) gtk_builder_get_object(widgetstree, "usb_label_name");
	usblabel = g_strdup(gtk_entry_get_text (GTK_ENTRY(usb_label_name)));
	if (strlen(usblabel) == 0) {
		g_free(usblabel);
		usblabel = g_strdup("LIVE");
		}
	
	label = (GtkWidget *) gtk_builder_get_object(widgetstree, "zone");
	timezone = g_strdup(gtk_label_get_text (GTK_LABEL(label)));
	if (strlen(timezone) == 0) {
		g_free(timezone);
		timezone = g_strdup("Etc/GMT");
		}
	
	label = (GtkWidget *) gtk_builder_get_object(widgetstree, "label3");
	language = g_strdup(gtk_label_get_text (GTK_LABEL(label)));
	if (strlen(language) == 0) {
		g_free(language);
		language = g_strdup("en_US.utf8");
		}
	
	label = (GtkWidget *) gtk_builder_get_object(widgetstree, "label4");
	keyboard = g_strdup(gtk_label_get_text (GTK_LABEL(label)));
	if (strlen(keyboard) == 0) {
		g_free(keyboard);
		keyboard = g_strdup("us");
		}
	
	
	const gchar *DW[] = { "copydevices" };

	listwidget = (GtkComboBox *) gtk_builder_get_object(widgetstree, "usbfilesystem");
	gtk_combo_box_get_active_iter(listwidget, &iter);
	list = (GtkListStore *) gtk_combo_box_get_model(listwidget);
	gtk_tree_model_get((GtkTreeModel *) list, &iter, 0, &usbfstype, -1);
	if (strlen(usbfstype) == 0) {
		g_free(usbfstype);
		usbfstype = g_strdup("vfat");
	}
	
	fullpercent = FALSE;
	pulsebar = TRUE;
	progressbar_handler_id = g_timeout_add(100, progressbar_handler, NULL);
	if (location != NULL) {
		g_free(location);
	}
	listwidget = (GtkComboBox *) gtk_builder_get_object(widgetstree, DW[(guint) copy]);
	gtk_combo_box_get_active_iter(listwidget, &iter);
	list = (GtkListStore *) gtk_combo_box_get_model(listwidget);
	gtk_tree_model_get((GtkTreeModel *) list, &iter, 0, &location, -1);
	if (location == NULL) {
		return;
	}		
	
	
	if (copy) {
		totalsize = directorysize(isodir);
		commandline = g_strdup_printf("multiple_iso_on_usb.sh --usb %s %s %s %s %s\n", isodir, location, timezone, language, keyboard);
	}

	g_shell_parse_argv(commandline, NULL, &command, NULL);
	g_free(commandline);
	g_spawn_async(NULL, command, NULL, G_SPAWN_SEARCH_PATH|G_SPAWN_DO_NOT_REAP_CHILD, NULL, NULL, &pid, NULL);
	g_child_watch_add(pid, on_process_end, NULL);
	gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object(widgetstree, "copy_btn"), FALSE);
	gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object(widgetstree, "cancel_btn"), TRUE);
	gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object(widgetstree, "usbfilesystem"), FALSE);
	gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object(widgetstree, "copydevices"), FALSE);
	gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object(widgetstree, "isofile_button"), FALSE);
	gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object(widgetstree, "isoname"), FALSE);
	gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object(widgetstree, "usb_label_name"), FALSE);
	gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object(widgetstree, "usbbutton"), FALSE);
	gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object(widgetstree, "timezonebutton"), FALSE);
	gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object(widgetstree, "Language"), FALSE);
	gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object(widgetstree, "keyboard"), FALSE);
	
	g_strfreev(command);
}                 

char *lastnchars(int s, const char *input)
{
	char * ret;
    ret = malloc(s+1);
   
   if (ret == NULL)
	{
      printf("Unable to allocate memory.\n");
      exit(1);
	}  
	 for (int i = 0; i <= s; i++)
	{
	 ret[i] = input[strlen(input) - (s - i)];
	   }
return ret;
}

void on_copy_btn_clicked (GtkWidget *widget, gpointer user_data) {
	GtkWidget *isoname, *isofiledialog;
	gchar *isofile;
	
	isoname = (GtkWidget *) gtk_builder_get_object(widgetstree, "isoname");
	isofile = g_strdup(gtk_entry_get_text (GTK_ENTRY(isoname)));
    if (g_file_test(gtk_entry_get_text (GTK_ENTRY(isoname)),  G_FILE_TEST_IS_DIR)) {
			do_action(TRUE); 
         }
		else
			{
			isofiledialog = (GtkWidget *) gtk_builder_get_object(widgetstree, "isofiledialog");
			 gtk_widget_show(isofiledialog);
			}
}

////

void on_timezonebutton_clicked(GtkWidget *widget, gpointer user_data) {
	gchar *continents[17] = {"Africa", "America", "Antarctica", "Asia", 
			"Atlantic", "Australia", "Europe", "Indian", 
			"Pacific", "US", "Mexico", "Chile", "Mideast",
			"Canada", "Brazil", "Arctic", "Etc"};
	GtkWidget *timezonewindow;
	GtkTreeView *listwidget,*locationlistwidget;
	GtkTreeIter iter;
	GtkListStore *list,*locationliststore;
	gchar *continentlist,*locationlist, current_zone[2][30], **dummy;
    GtkWidget *scrolledwindow3,*scrolledwindow4;
    GtkWidget *label;
    GtkTreeSortable *sortable;
    gboolean valid;
    gint i, row_count = 0;
	//
	GtkTreePath *path;
	GtkTreeModel *model;
	GtkTreeViewColumn *column;
	//  
	label = (GtkWidget *) gtk_builder_get_object(widgetstree, "zone");
	timezonewindow = (GtkWidget *) gtk_builder_get_object(widgetstree, "timezonewindow");
	scrolledwindow3 = (GtkWidget *) gtk_builder_get_object(widgetstree, "scrolledwindow3");    
    listwidget = (GtkTreeView *) gtk_builder_get_object(widgetstree, "continentlist");
	list = (GtkListStore *) gtk_tree_view_get_model(listwidget);
	model = gtk_tree_view_get_model(listwidget);
	
	scrolledwindow4 = (GtkWidget *) gtk_builder_get_object(widgetstree, "scrolledwindow4");    
    locationlistwidget = (GtkTreeView *) gtk_builder_get_object(widgetstree, "locationlist");
	locationliststore = (GtkListStore *) gtk_tree_view_get_model(locationlistwidget);
	
	gtk_list_store_clear(list) ;
	for (i=0; i<17; i++) {
			gtk_list_store_append(list, &iter);
			gtk_list_store_set(list, &iter, 0, continents[i], -1);
		}
	
	if (strlen(gtk_label_get_text(GTK_LABEL(label)))==0){
	get_current_zone(current_zone);
	}
	else { 
		dummy=g_strsplit(gtk_label_get_text(GTK_LABEL(label)), "/", 0);
		strcpy(current_zone[0],dummy[0]);
		strcpy(current_zone[1],dummy[1]);
	}
	valid = gtk_tree_model_get_iter_first (model, &iter);
   
       while (valid)
         {
           gchar *str_data0;
           gtk_tree_model_get (model, &iter,
                               0, &str_data0,
                               -1);
                             
				path = gtk_tree_model_get_path (model, &iter);
				column=gtk_tree_view_get_column(listwidget , 0);
           	if  (strncmp(current_zone[0],str_data0,strlen(current_zone[0]))==0) { 
				path = gtk_tree_model_get_path (model, &iter);
				column=gtk_tree_view_get_column(listwidget , 0);
				gtk_tree_view_set_cursor (listwidget , path, column, FALSE);
				if (row_count>5){
				gtk_tree_view_scroll_to_cell(listwidget , path,  column, TRUE,0.5,0.5);
				}
			}		
           row_count ++;
           valid = gtk_tree_model_iter_next (model, &iter);
         } 
         
         //location time
         model = gtk_tree_view_get_model(locationlistwidget);
	     valid = gtk_tree_model_get_iter_first (model, &iter);
		 
	   while (valid)
         {
			gchar *klocation;
			gtk_tree_model_get (model, &iter,
                               0, &klocation,
                               -1);
				path = gtk_tree_model_get_path (model, &iter);
				column=gtk_tree_view_get_column(locationlistwidget , 0);
           	if  (strncmp(current_zone[1],klocation,strlen(current_zone[1]))==0) { 
				path = gtk_tree_model_get_path (model, &iter);
				column=gtk_tree_view_get_column(locationlistwidget , 0);
				gtk_tree_view_set_cursor (locationlistwidget , path, column, FALSE);
				if (row_count>5){
				gtk_tree_view_scroll_to_cell(locationlistwidget , path,  column, TRUE,0.5,0.5);
				}
			}
			
			row_count ++;
			valid = gtk_tree_model_iter_next (model, &iter);
         }
         
			gtk_widget_show(timezonewindow);
}


void on_Language_clicked (GtkWidget *widget, gpointer user_data) {
	GtkWidget *localewindow;
	GtkTreeView *listwidget;
	GtkTreeIter iter;
	GtkListStore *list;
	gchar *clist;
	gchar current_locale[10];
    GtkWidget *scrolledwindow;
    GtkWidget *label;
    GtkTreeSortable *sortable;
    gboolean valid;
    gint row_count = 0;
	//
	GtkTreePath *path;
	GtkTreeModel *model;
	GtkTreeViewColumn *column;
	// 
	label = (GtkWidget *) gtk_builder_get_object(widgetstree, "label3");
    localewindow = (GtkWidget *) gtk_builder_get_object(widgetstree, "localewindow");
	scrolledwindow = (GtkWidget *) gtk_builder_get_object(widgetstree, "scrolledwindow");    
    listwidget = (GtkTreeView *) gtk_builder_get_object(widgetstree, "clist");
	list = (GtkListStore *) gtk_tree_view_get_model(listwidget);
	model = gtk_tree_view_get_model(listwidget);
	
	//sortable = GTK_TREE_SORTABLE(list);
	//gtk_tree_sortable_set_sort_column_id(sortable, 0, GTK_SORT_ASCENDING);
	
		gtk_widget_show(localewindow);
	if (strlen(gtk_label_get_text(GTK_LABEL(label)))==0){
		get_system_locale(current_locale);
		if (g_file_test("get_system_locale.sh", G_FILE_TEST_EXISTS)) {
		system("rm get_system_locale.sh");}
		if (g_file_test("system_locale", G_FILE_TEST_EXISTS)) {
		system("rm system_locale");}
		}
	else {
		strcpy(current_locale,gtk_label_get_text(GTK_LABEL(label)));
		}

	valid = gtk_tree_model_get_iter_first (model, &iter);
    path = gtk_tree_model_get_path (model, &iter);
	column=gtk_tree_view_get_column(listwidget , 0);
	gtk_tree_view_set_cursor (listwidget , path, column, FALSE);
	while (valid)
     {
		gchar *str_data0;
		gchar   *str_data1;
		gtk_tree_model_get (model, &iter,
                               0, &str_data0,
                               1, &str_data1,
                               -1);
		path = gtk_tree_model_get_path (model, &iter);
		column=gtk_tree_view_get_column(listwidget , 0);
		if  (strncmp(current_locale,str_data1,10)==0) { 
			path = gtk_tree_model_get_path (model, &iter);
			column=gtk_tree_view_get_column(listwidget , 0);
			gtk_tree_view_set_cursor (listwidget , path, column, FALSE);
			if (row_count>5){
				gtk_tree_view_scroll_to_cell(listwidget , path,  column, TRUE,0.5,0.5);
				}
			}		
		row_count ++;
        valid = gtk_tree_model_iter_next (model, &iter);
      }         
}


void on_keyboard_clicked (GtkWidget *widget, gpointer user_data) {
	GtkWidget *keyboardwindow;
	GtkTreeView *keybtypelistwidget, *keymaplistwidget;
	GtkListStore *keybtypeliststore, *keymapliststore;
	gchar *keytypelist, keymaplist,currentkeymap[30] ;
    GtkWidget *scrolledwindow1,*scrolledwindow2, *label4, *label8;
    
    GtkTreeIter iter;
	gchar currentkeybtype[30];
    gboolean valid;
    gint row_count = 0;
	//
	GtkTreePath *path;
	GtkTreeModel *model;
	GtkTreeViewColumn *column;
	// 
    label4 = (GtkWidget *) gtk_builder_get_object(widgetstree, "label4");
    label8 = (GtkWidget *) gtk_builder_get_object(widgetstree, "label8");
    keyboardwindow = (GtkWidget *) gtk_builder_get_object(widgetstree, "keyboardwindow");
	scrolledwindow1 = (GtkWidget *) gtk_builder_get_object(widgetstree, "scrolledwindow1");    
    keybtypelistwidget = (GtkTreeView *) gtk_builder_get_object(widgetstree, "keybtypelist");
	keybtypeliststore = (GtkListStore *) gtk_tree_view_get_model(keybtypelistwidget);
	
    scrolledwindow2 = (GtkWidget *) gtk_builder_get_object(widgetstree, "scrolledwindow2");    
    keymaplistwidget = (GtkTreeView *) gtk_builder_get_object(widgetstree, "keymaplist");
	keymapliststore = (GtkListStore *) gtk_tree_view_get_model(keymaplistwidget);
 	gtk_widget_show(keyboardwindow);
 	
 	if (strlen(gtk_label_get_text(GTK_LABEL(label8)))==0){
			get_keybtype(currentkeybtype);}
		else
		{
			strcpy(currentkeybtype,gtk_label_get_text(GTK_LABEL(label8)));
		}
 	model = gtk_tree_view_get_model(keybtypelistwidget);
 	valid = gtk_tree_model_get_iter_first (model, &iter);
	while (valid)
         {
           gchar *keybtype;
           gtk_tree_model_get (model, &iter,
                               0, &keybtype,
                               -1);
				path = gtk_tree_model_get_path (model, &iter);
				column=gtk_tree_view_get_column(keybtypelistwidget , 0);
           	if  (strncmp(currentkeybtype,keybtype,strlen(currentkeybtype))==0) { 
				path = gtk_tree_model_get_path (model, &iter);
				column=gtk_tree_view_get_column(keybtypelistwidget , 0);
				gtk_tree_view_set_cursor (keybtypelistwidget , path, column, FALSE);
				if (row_count>5){
				gtk_tree_view_scroll_to_cell(keybtypelistwidget , path,  column, TRUE,0.5,0.5);
				}
			}
			
           row_count ++;
           valid = gtk_tree_model_iter_next (model, &iter);
         } 	
         
         model = gtk_tree_view_get_model(keymaplistwidget);
         if (strlen(gtk_label_get_text(GTK_LABEL(label4)))==0){
			get_keymap(currentkeymap);}
		else
			{
				strcpy(currentkeymap,gtk_label_get_text(GTK_LABEL(label4)));
			}

	    valid = gtk_tree_model_get_iter_first (model, &iter);
	   while (valid)
         {
           gchar *keymap;
           gtk_tree_model_get (model, &iter,
                               0, &keymap,
                               -1);
				path = gtk_tree_model_get_path (model, &iter);
				column=gtk_tree_view_get_column(keymaplistwidget , 0);
           	if  (strncmp(currentkeymap,keymap,strlen(currentkeymap))==0) { 
				path = gtk_tree_model_get_path (model, &iter);
				column=gtk_tree_view_get_column(keymaplistwidget , 0);
				gtk_tree_view_set_cursor (keymaplistwidget , path, column, FALSE);
				if (row_count>5){
				gtk_tree_view_scroll_to_cell(keymaplistwidget , path,  column, TRUE,0.5,0.5);
				}
			}
			
           row_count ++;
           valid = gtk_tree_model_iter_next (model, &iter);
         }
         if (g_file_test("currentkeymap", G_FILE_TEST_EXISTS)) {system("rm currentkeymap");}
}

void on_location_button_cancel_clicked(GtkWidget *widget, gpointer user_data) {
	
}

void on_location_button_ok_clicked(GtkWidget *widget, gpointer user_data) {
	gchar *continent,*location;
	GtkWidget *label;
	GtkTreeIter iter;
	GtkTreeView *treeview;
	GtkListStore *list;
	GtkTreeModel *model;
	gchar timezonelabel_text[80],timezone[80];
	
	treeview = (GtkTreeView *) gtk_builder_get_object(widgetstree, "continentlist");
	model = gtk_tree_view_get_model(treeview);
	list = GTK_LIST_STORE(model);
	GtkTreeSelection *selection;
	selection = gtk_tree_view_get_selection(treeview);

   if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter)) {
	gtk_tree_model_get (model, &iter, 0, &continent, -1);
	label = (GtkWidget *) gtk_builder_get_object(widgetstree, "zone"); 
	strcpy(timezonelabel_text,continent);
	strcat(timezonelabel_text,"/");
	}
	
	treeview = (GtkTreeView *) gtk_builder_get_object(widgetstree, "locationlist");
	model = gtk_tree_view_get_model(treeview);
	list = GTK_LIST_STORE(model);
	selection = gtk_tree_view_get_selection(treeview);

   if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter)) {
	  gtk_tree_model_get (model, &iter, 0, &location, -1);
	  label = (GtkWidget *) gtk_builder_get_object(widgetstree, "zone"); 	  
	  strcat(timezonelabel_text,location);
	  }
	   gtk_label_set_text(GTK_LABEL(label),timezonelabel_text);
	  /* strcpy(timezone,"/usr/share/zoneinfo/");
		strcat(timezone,continent);
		strcat(timezone,"/");
		strcat(timezone,location);
		settimezone(timezone,continent,location);*/
 }

char * create_locationlist(char *name, char *locationlist)
{
FILE *f = fopen(name, "w");
if (f == NULL)
{
    printf("Error opening file!\n");
    exit(1);
}

 int i;
char *lines[5] = {"#!/bin/sh\n#\n", "ls  /usr/share/zoneinfo/",
					locationlist,"/ > locationlist\n",
					"cat locationlist\n"
					};
  for (i=0 ; i<5; i++) {
			fprintf(f, "%s", lines[i]);
		}

fclose(f);
}
	
void on_keybtypelist_cursor_changed(GtkTreeView       *treeview, 
                                GtkTreePath       *path, 
                                GtkTreeViewColumn *column,
                                gpointer userdata) {										

	GtkTreeView *listwidget;
	GtkTreeView *keybtypelistwidget, *keymaplistwidget;
	GtkListStore *keybtypeliststore, *keymapliststore;
	gchar *keymaplist,currentkeymap[30];
	gchar **lines, *output,*locale;
	gint i;
	gint status;
	GtkWidget *scrolledwindow2;
    
	gboolean valid;
	gint row_count = 0;
    
	gchar *keymap;
	GtkTreeIter iter;
	GtkTreeSortable *sortable;
	GtkSortType	order;
	GtkTreeModel *model;
	model = gtk_tree_view_get_model(treeview);
	keymapliststore = GTK_LIST_STORE(model);
	GtkTreeSelection *selection;
	selection = gtk_tree_view_get_selection(treeview);
	   
   if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter)) {
		gtk_tree_model_get (model, &iter, 0, &keymap, -1);
		create_keymaplist("sli-keymap_list-detection.sh", keymap);
	   
		scrolledwindow2 = (GtkWidget *) gtk_builder_get_object(widgetstree, "scrolledwindow2");    
		keymaplistwidget = (GtkTreeView *) gtk_builder_get_object(widgetstree, "keymaplist");
		keymapliststore = (GtkListStore *) gtk_tree_view_get_model(keymaplistwidget);
	
		/*sortable = GTK_TREE_SORTABLE(keymapliststore);
		gtk_tree_sortable_set_sort_column_id(sortable, 0, GTK_SORT_ASCENDING);*/
	
		gtk_list_store_clear(keymapliststore) ;
		
		localecount = 0;
		g_spawn_command_line_sync("sh sli-keymap_list-detection.sh locale", &output, NULL, &status, NULL);
	 if (status == 0) {
		lines = g_strsplit(output, "\n", 0);
		for (i=0; lines[i] != NULL && strlen(lines[i])>0; i++) {
			if (is_registered_keymap(lines[i])){
				gtk_list_store_append(keymapliststore, &iter);
				gtk_list_store_set(keymapliststore, &iter, 0, lines[i], -1);
			}
		}
		localecount = i;
		g_strfreev(lines);
	}
		g_free(output);
		if (g_file_test("sli-keymap_list-detection.sh", G_FILE_TEST_EXISTS)) {
		system("rm sli-keymap_list-detection.sh");}
		if (g_file_test("keymaplist", G_FILE_TEST_EXISTS)) {
		system("rm keymaplist");}
	 }
	 
	 //keymap focus
	
		model = gtk_tree_view_get_model(keymaplistwidget); 
      
		get_keymap(currentkeymap);
     
	    valid = gtk_tree_model_get_iter_first (model, &iter);
	    path = gtk_tree_model_get_path (model, &iter);
		column=gtk_tree_view_get_column(keymaplistwidget , 0);
	    gtk_tree_view_set_cursor (keymaplistwidget , path, column, FALSE);
	    
	   while (valid)
         {
           gchar *keymap;
           gtk_tree_model_get (model, &iter,
                               0, &keymap,
                               -1);
				path = gtk_tree_model_get_path (model, &iter);
				column=gtk_tree_view_get_column(keymaplistwidget , 0);
           	if  (strncmp(currentkeymap,keymap,strlen(currentkeymap))==0) { 
				path = gtk_tree_model_get_path (model, &iter);
				column=gtk_tree_view_get_column(keymaplistwidget , 0);
				gtk_tree_view_set_cursor (keymaplistwidget , path, column, FALSE);
				if (row_count>5){
				gtk_tree_view_scroll_to_cell(keymaplistwidget , path,  column, TRUE,0.5,0.5);
				}
			}
			
           row_count ++;
           valid = gtk_tree_model_iter_next (model, &iter);
         }
        if (g_file_test("currentkeymap", G_FILE_TEST_EXISTS)) {
			system("rm currentkeymap");}
}


void on_button_cancel_clicked(GtkWidget *widget, gpointer user_data) {
	
}

void on_keyboard_button_cancel_clicked(GtkWidget *widget, gpointer user_data) {
}

void on_button_ok_clicked(GtkWidget *widget, gpointer user_data) {
	 
	gchar *locale;
	GtkWidget *label;
	GtkTreeIter iter;
	GtkTreeView *treeview;
	GtkListStore *list;
	GtkTreeModel *model;
	
	treeview = (GtkTreeView *) gtk_builder_get_object(widgetstree, "clist");
	model = gtk_tree_view_get_model(treeview);
	list = GTK_LIST_STORE(model);
	GtkTreeSelection *selection;
	selection = gtk_tree_view_get_selection(treeview);

   if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter)) {
	  gtk_tree_model_get (model, &iter, 1, &locale, -1);
	  label = (GtkWidget *) gtk_builder_get_object(widgetstree, "label3"); 
	  gtk_label_set_text(GTK_LABEL(label),locale);}
	 // set_locale(locale);
	//  system("sh set_locale.sh");
}

void on_clist_cursor_changed(GtkTreeView       *treeview, 
                                GtkTreePath       *path, 
                                GtkTreeViewColumn *column,
                                gpointer userdata) {

   gchar *language, *locale;
   GtkWidget *label;
   GtkTreeIter iter;
   GtkListStore *list;
   GtkTreeModel *model;
   model = gtk_tree_view_get_model(treeview);
   list = GTK_LIST_STORE(model);
   GtkTreeSelection *selection;
   selection = gtk_tree_view_get_selection(treeview);
  
  /* if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter)) {
	  gtk_tree_model_get (model, &iter, 1, &locale, -1);
	  label = (GtkWidget *) gtk_builder_get_object(widgetstree, "label20"); 
	  }  */
}

void on_continentlist_cursor_changed(GtkTreeView       *treeview, 
                                GtkTreePath       *path, 
                                GtkTreeViewColumn *column,
                                gpointer userdata) {										

	GtkTreeView *listwidget;
	GtkTreeView *continentlistwidget, *locationlistwidget;
	GtkListStore *continentliststore, *locationliststore;
	gchar *continentlist,current_zone[2][30];
	gchar **lines, *output,*locale,*location;
	gint i;
	gint status;
    GtkWidget *scrolledwindow4;
    
	gboolean valid;
    gint row_count = 0;
    
	GtkTreeIter iter;
	GtkTreeSortable *sortable;
	GtkTreeModel *model;
	model = gtk_tree_view_get_model(treeview);
	locationliststore = GTK_LIST_STORE(model);
	GtkTreeSelection *selection;
	selection = gtk_tree_view_get_selection(treeview);
  
   if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter)) {
		gtk_tree_model_get (model, &iter, 0, &location, -1);
		create_locationlist("sli_location_list_detection.sh", location);
		scrolledwindow4 = (GtkWidget *) gtk_builder_get_object(widgetstree, "scrolledwindow4");    
		locationlistwidget = (GtkTreeView *) gtk_builder_get_object(widgetstree, "locationlist");
		locationliststore = (GtkListStore *) gtk_tree_view_get_model(locationlistwidget);
	
		/*sortable = GTK_TREE_SORTABLE(keymapliststore);
		gtk_tree_sortable_set_sort_column_id(sortable, 0, GTK_SORT_ASCENDING);*/
	
		gtk_list_store_clear(locationliststore) ;
		localecount = 0;
		g_spawn_command_line_sync("sh sli_location_list_detection.sh location", &output, NULL, &status, NULL);
	 if (status == 0) {
		lines = g_strsplit(output, "\n", 0);
		for (i=0; lines[i] != NULL && strlen(lines[i])>0; i++) {
			gtk_list_store_append(locationliststore, &iter);
			gtk_list_store_set(locationliststore, &iter, 0, lines[i], -1);
			}
			localecount = i;
			g_strfreev(lines);
		}
		g_free(output);
		//system("rm sli_location_list_detection.sh");
		//system("rm locationlist");  
	}
	 
	 //location focus
	
		model = gtk_tree_view_get_model(locationlistwidget); 
      
		get_current_zone(current_zone);
		valid = gtk_tree_model_get_iter_first (model, &iter);
	    
		path = gtk_tree_model_get_path (model, &iter);
		column=gtk_tree_view_get_column(locationlistwidget , 0);
		gtk_tree_view_set_cursor (locationlistwidget , path, column, FALSE);
		while (valid)
         {
			gchar *klocation;
			gtk_tree_model_get (model, &iter,
                               0, &klocation,
                               -1);
				path = gtk_tree_model_get_path (model, &iter);
				column=gtk_tree_view_get_column(locationlistwidget , 0);
           	if  (strncmp(current_zone[1],klocation,strlen(current_zone[1]))==0) { 
				path = gtk_tree_model_get_path (model, &iter);
				column=gtk_tree_view_get_column(locationlistwidget , 0);
				gtk_tree_view_set_cursor (locationlistwidget , path, column, FALSE);
				if (row_count>5){
				gtk_tree_view_scroll_to_cell(locationlistwidget , path,  column, TRUE,0.5,0.5);
				}
			}
			
           row_count ++;
           valid = gtk_tree_model_iter_next (model, &iter);
		}  
}

void on_keyboard_button_ok_clicked(GtkWidget *widget, gpointer user_data) {
	 
	gchar *keymap;
	GtkWidget *label;
	GtkTreeIter iter;
	GtkTreeView *treeview;
	GtkListStore *list;
	GtkTreeModel *model;
	
	treeview = (GtkTreeView *) gtk_builder_get_object(widgetstree, "keymaplist");
	model = gtk_tree_view_get_model(treeview);
	list = GTK_LIST_STORE(model);
	GtkTreeSelection *selection;
	selection = gtk_tree_view_get_selection(treeview);

   if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter)) {
	  gtk_tree_model_get (model, &iter, 0, &keymap, -1);
	  label = (GtkWidget *) gtk_builder_get_object(widgetstree, "label4"); 
	  gtk_label_set_text(GTK_LABEL(label),keymap);	
	  //setkeymap(keymap);
	  //system("sh set_keymap.sh");
	  }
}

//////

void on_cancel_btn_clicked (GtkWidget *widget, gpointer user_data) {
	kill (pid, SIGTERM);
}


void on_exitp (GtkWidget *widget, gpointer user_data) {
	if (pid != 0) {
		kill (pid, SIGTERM);
	}
	gtk_main_quit();
}

gboolean progressbar_handler(gpointer data) {
	GtkProgressBar *progressbar;
	gchar *output;
	guint64 installsize;
	gdouble progressfraction;
	gchar *s_progressfraction;
	
	progressbar = (GtkProgressBar *) gtk_builder_get_object(widgetstree,"progressbar");
	
	if (pulsebar && ! fullpercent && g_file_test("/media/mb",  G_FILE_TEST_IS_DIR)) {
		pulsebar = FALSE;
		g_source_remove(progressbar_handler_id);
		progressbar_handler_id = g_timeout_add(5000, progressbar_handler, NULL);
	}
	
	if (pulsebar) {
		gtk_progress_bar_pulse(progressbar);
	} else {
		g_spawn_command_line_sync("du -s -m /media/mb", &output, NULL, NULL, NULL);
		installsize = g_ascii_strtoull(output, NULL, 10);
		progressfraction = installsize * 100 / totalsize;
		
		if (progressfraction >= 100) {
			gtk_progress_bar_set_text(progressbar, "100 %");
			gtk_progress_bar_set_fraction(progressbar, 1.0);
			fullpercent = TRUE;
			pulsebar = TRUE;
			g_source_remove(progressbar_handler_id);
			progressbar_handler_id = g_timeout_add(100, progressbar_handler, NULL);
		} else {
			gtk_progress_bar_set_fraction(progressbar, progressfraction / 100);
			s_progressfraction = g_strdup_printf("%2.0f %c", progressfraction, '%');
			gtk_progress_bar_set_text(progressbar, s_progressfraction);
			g_free(s_progressfraction);
		}
	}
	return TRUE;
}


void clearlocations() {
	GtkComboBox *listwidget;
	GtkListStore *list;
	// clear copydevices
	listwidget = (GtkComboBox *) gtk_builder_get_object(widgetstree, "copydevices");
	list = (GtkListStore *) gtk_combo_box_get_model(listwidget);
	gtk_list_store_clear (list) ;
}

void initlocations() {
	GtkComboBox *listwidget;
	GtkTreeIter iter;
	GtkListStore *list;
	gchar **lines, *output, *device;
	gint i;
	gint status;
	
	listwidget = (GtkComboBox *) gtk_builder_get_object(widgetstree, "copydevices");
	list = (GtkListStore *) gtk_combo_box_get_model(listwidget);
	copydevicescount = 0;
	g_spawn_command_line_sync("multibootusb-location-detection.sh copy", &output, NULL, &status, NULL);
	if (status == 0) {
		lines = g_strsplit(output, "\n", 0);
		for (i=0; lines[i] != NULL && strlen(lines[i])>0; i++) {
			gtk_list_store_append(list, &iter);
			gtk_list_store_set(list, &iter, 0, lines[i], -1);
		}
		copydevicescount = i;
		g_strfreev(lines);
	}
	g_free(output);
	if (copydevicescount != 0){
		gtk_combo_box_set_active_iter(listwidget, &iter);
		gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object(widgetstree, "copy_btn"), TRUE);
	}
}

void init_locale_list()
{
	GtkTreeView *listwidget;
	GtkTreeIter iter;
	GtkListStore *list;
	gchar *clist ;
	gchar **lines, **lines_split, *output,*locale;
	
	gchar locale_name[10];
	
	GtkTreePath *path;
    GtkTreeModel *model;
    GtkTreeViewColumn *column;
	gint i;
	gint status;
	listwidget = (GtkTreeView *) gtk_builder_get_object(widgetstree, "clist");
	list = (GtkListStore *) gtk_tree_view_get_model(listwidget);
    model = gtk_tree_view_get_model(listwidget);
    localecount = 0;
    
    get_system_locale(locale_name);
    
	g_spawn_command_line_sync("sli-locale_list-detection.sh locale", &output, NULL, &status, NULL);
	if (status == 0) {
		lines = g_strsplit(output, "\n", 0);
		for (i=0; lines[i] != NULL && strlen(lines[i])>0; i++) {
			gtk_list_store_append(list, &iter);
			lines_split=g_strsplit(lines[i],",",0);
			gtk_list_store_set(list, &iter, 0, g_strdup(lines_split[1]), -1);
			gtk_list_store_set(list, &iter, 1, g_strdup(lines_split[0]), -1);			     
		}
		localecount = i;	
		g_strfreev(lines);
	}
	g_free(output);
	if (g_file_test("locale_file", G_FILE_TEST_EXISTS)) {
		system("rm locale_file");}
}

void on_process_end (GPid thepid, gint status, gpointer data) {
	GtkWidget *dialog;
	GtkProgressBar *progressbar;

	pid = 0;
	g_free(location);
	location = NULL;
	g_source_remove(progressbar_handler_id);
	
	progressbar = (GtkProgressBar *) gtk_builder_get_object(widgetstree,"progressbar");
	gtk_progress_bar_set_fraction(progressbar, 0);
	gtk_progress_bar_set_text(progressbar, "");

	if (copydevicescount != 0){
		gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object(widgetstree, "copy_btn"), TRUE);
	}
		gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object(widgetstree, "cancel_btn"), FALSE);
		gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object(widgetstree, "usbfilesystem"), TRUE);
		gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object(widgetstree, "copydevices"), TRUE);
		gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object(widgetstree, "isofile_button"), TRUE);
		gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object(widgetstree, "isoname"), TRUE);
		gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object(widgetstree, "usb_label_name"), TRUE);
		gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object(widgetstree, "usbbutton"), TRUE);
		gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object(widgetstree, "timezonebutton"), TRUE);
		gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object(widgetstree, "Language"), TRUE);
		gtk_widget_set_sensitive ((GtkWidget *) gtk_builder_get_object(widgetstree, "keyboard"), TRUE);
	
	if (0 == status) {
		dialog = (GtkWidget *) gtk_builder_get_object(widgetstree, "dialogfinished");
	} else {
		dialog = (GtkWidget *) gtk_builder_get_object(widgetstree, "dialogerror");
	}
	
	gtk_widget_show(dialog);
}

void on_isofile_button_clicked(GtkWidget *widget, gpointer user_data){
	GtkWidget *isofilechooserdialog;
	isofilechooserdialog = (GtkWidget *) gtk_builder_get_object(widgetstree, "isofilechooserdialog");
	gtk_widget_show(isofilechooserdialog);
}


void on_isofile_ok_button_clicked(GtkWidget *widget, gpointer user_data){
	GtkWidget *isoname, *isofilechooserdialog;
	gchar *filename;
	isofilechooserdialog = (GtkWidget *) gtk_builder_get_object(widgetstree, "isofilechooserdialog");
	
	filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (isofilechooserdialog));
	//	gtk_file_chooser_get_current_folder
	isoname = (GtkWidget *) gtk_builder_get_object(widgetstree, "isoname");
	gtk_entry_set_text (GTK_ENTRY(isoname),filename);
}

void on_usbbutton_clicked (GtkWidget *widget, gpointer user_data) {
	clearlocations();
	initlocations();
}

void on_about_activate (GtkWidget *widget, gpointer user_data) {
	GtkWidget *aboutdialog;
	
	aboutdialog = (GtkWidget *) gtk_builder_get_object(widgetstree, "aboutdialog");
	gtk_widget_show(aboutdialog);
}

void on_quit_activate (GtkWidget *widget, gpointer user_data) {
	if (pid != 0) {
		kill (pid, SIGTERM);
	}
	gtk_main_quit();
}


int main (int argc, char *argv[]) {
	GtkWindow *mainwindow;
	GtkWidget *menubar1;
	GtkWidget *about;
	GtkAboutDialog *aboutdialog;
	gchar *path;
		
	setlocale(LC_ALL, "");
	bindtextdomain(PROJECT_NAME, LOCALE_DIR);
	textdomain(PROJECT_NAME);
	
	path = g_strdup_printf("/sbin:/usr/sbin:/usr/local/sbin:%s", g_getenv("PATH"));
	g_setenv("PATH", path, TRUE);
	g_free(path);
	
	gtk_init(&argc, &argv);
	widgetstree = gtk_builder_new();
	gtk_builder_add_from_file(widgetstree, UI_FILE, NULL);
	gtk_builder_connect_signals(widgetstree, NULL);
	
	pid = 0;
	location = NULL;
		
	mainwindow = (GtkWindow *) gtk_builder_get_object(widgetstree, "mainwindow");
	gtk_window_set_icon_from_file(mainwindow, APP_ICON, NULL);
	aboutdialog = (GtkAboutDialog *) gtk_builder_get_object(widgetstree, "aboutdialog");
	
	gtk_about_dialog_set_version(aboutdialog, PROJECT_VERSION);
	gtk_widget_show((GtkWidget *) mainwindow);
	
	initlocations();
	init_locale_list();

	gtk_main();
	return 0;
}
