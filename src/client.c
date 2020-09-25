/* @Auther  -> Stephen Cochrane
 * @Github  -> skippy404
 * @License -> GPL
 */

#include "client.h"

/*
static GtkWidget *n1;
static GtkWidget *n2;
static GtkWidget *res;
*/

static GtkWidget *fixed;
static GtkWidget *send;              /* Send text button */
static GtkWidget *recv;              /* Display recv data */
static GtkWidget *text;              /* Input text */
static GtkWidget *name;              /* Name of the program */
static GtkWidget *voip_joined;       /* List of people joined */
static GtkWidget *user_online_label; /* Label for online people */
static GtkWidget *jl_voip;           /* Join/Leave voip button */
static GtkWidget *user_online;       /* List of online people */
static GtkBuilder *builder;          /* The builder for GTK */

void
on_send_clicked(GtkButton *b)
{
	char *input = (char *) gtk_entry_get_text(GTK_ENTRY(text));
	printf("DEBUG: Entered Message: %s\n", input);
	gtk_entry_set_text(GTK_ENTRY(text), "");
}

// Just test code to get used to gtk
int
main(int argc, char *argv[])
{
	GtkWidget *window, *grid, *calculate;
	gtk_init(&argc, &argv);

	builder = gtk_builder_new_from_file("./assets/client.glade");
	window = GTK_WIDGET(gtk_builder_get_object(builder, "root"));
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	/* Map the GTK Widgets from the builder to thier Counterparts */
	fixed = GTK_WIDGET(gtk_builder_get_object(builder, "fixed"));
	send = GTK_WIDGET(gtk_builder_get_object(builder, "send"));
	recv = GTK_WIDGET(gtk_builder_get_object(builder, "recv"));
	text = GTK_WIDGET(gtk_builder_get_object(builder, "text"));
	name = GTK_WIDGET(gtk_builder_get_object(builder, "name"));
	voip_joined = GTK_WIDGET(gtk_builder_get_object(builder, "voip_joined"));
	user_online_label = GTK_WIDGET(gtk_builder_get_object(builder, "user_online_label"));
	jl_voip = GTK_WIDGET(gtk_builder_get_object(builder, "jl_voip"));
	user_online = GTK_WIDGET(gtk_builder_get_object(builder, "user_online"));

	/* Map the buttons to thier handlers */
	g_signal_connect(send, "clicked", G_CALLBACK(on_send_clicked), NULL);

	gtk_widget_show(window);
	gtk_main();

	return EXIT_SUCCESS;
}
