/* @Auther  -> Stephen Cochrane
 * @Github  -> skippy404
 * @License -> GPL
 */

#include "client.h"

static GtkWidget *n1;
static GtkWidget *n2;
static GtkWidget *res;

static GtkWidget *fixed;
static GtkWidget *send;              /* Send text button */
static GtkWidget *recv;              /* Display recv data */
static GtkWidget *text;              /* Input text */
static GtkWidget *name;              /* Name of the program */
static GtkWidget *voip_joined;       /* List of people joined */
static GtkWidget *user_online_label; /* Label for online people */
static GtkWidget *jl_voip;           /* Join/Leave voip button */
static GtkWidget *user_online;       /* List of online people */

void
calc(GtkWidget *calculate, gpointer data)
{
	int num1 = atoi((char *) gtk_entry_get_text(GTK_ENTRY(n1)));
	int num2 = atoi((char *) gtk_entry_get_text(GTK_ENTRY(n2)));
	char buff[32];
	
	sprintf(buff, "%d", num1 + num2);
	gtk_label_set_text(GTK_LABEL(res), buff);
}

// Just test code to get used to gtk
int
main(int argc, char *argv[])
{
	GtkWidget *window, *grid, *calculate;
	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	grid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(window), grid);

	n1 = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(grid), n1, 0, 0, 1, 1);

	n2 = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(grid), n2, 0, 1, 1, 1);

	calculate = gtk_button_new_with_label("calculate");
	g_signal_connect(calculate, "clicked", G_CALLBACK(calc), NULL);
	gtk_grid_attach(GTK_GRID(grid), calculate, 0, 2, 1, 1);

	res = gtk_label_new("result");
	gtk_grid_attach(GTK_GRID(grid), res, 0, 3, 1, 1);

	gtk_widget_show_all(window);
	gtk_main();
}
