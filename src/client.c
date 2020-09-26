/* @Auther  -> Stephen Cochrane
 * @Github  -> skippy404
 * @License -> GPL
 */

#include "client.h"

static GtkWidget *fixed;
static GtkWidget *send_btn;          /* Send text button */
static GtkWidget *recv_box;          /* Display recv data */
static GtkWidget *text;              /* Input text */
static GtkWidget *name;              /* Name of the program */
static GtkWidget *voip_joined;       /* List of people joined */
static GtkWidget *user_online_label; /* Label for online people */
static GtkWidget *jl_voip;           /* Join/Leave voip button */
static GtkWidget *user_online;       /* List of online people */
static GtkBuilder *builder;          /* The builder for GTK */

/* For adding new text */
static GtkTextIter iter;
static GtkTextBuffer *gtk_buffer;

void
on_send_clicked(GtkButton *b)
{
	char *input = (char *) gtk_entry_get_text(GTK_ENTRY(text));
	printf("DEBUG: Entered Message: %s\n", input);
	send_mesg(input);
	gtk_entry_set_text(GTK_ENTRY(text), "");
}

void
update_messages(char *buff)
{
	/* TODO: Dont nuke the old messages. */
	gtk_text_buffer_insert(gtk_buffer, &iter, buff, -1);
	gtk_text_buffer_insert(gtk_buffer, &iter, "\n", -1);
}

/* Sends a message to the server
 * 
 * char *mesg: The message to send.
 */
void
send_mesg(char *mesg)
{
	/* TODO: Tidy up */
	send(sock, mesg, strlen(mesg), 0);
}

/* Stores the message from a server in buffer, and sets its type.
 *
 * char *buff: Stores the message into this buffer [out].
 * int *type: Stores the message type [out].
 */

int
recv_mesg(char *buff, int *type)
{
	/* TODO: Finish */
	int cnt = read(sock, buff, 256); // Buffer cant be bigger than 256
	if (cnt < 0) {
		return cnt;
	}
	buff[cnt] = 0; // Add a terminator
	return cnt;
}

int
init_sock(int port, char *host, int *sock, struct sockaddr_in *address)
{
	struct hostent *server;
	struct in_addr **list;
	
	/* First resolve the hostname */
	if ((server = gethostbyname(host)) == NULL) {
		perror("ERROR: Resolving hostname failed.\n");
		return 0;
	}

	list = (struct in_addr **) server->h_addr_list;
	if (!list[0]) {
		perror("ERROR: Hostname list empty.\n");
		return 0;
	}

	if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("ERROR: Socket creation error.\n");
		return 0;
	}

	address->sin_family = AF_INET;
	address->sin_port = htons(port);

	if (inet_pton(AF_INET, inet_ntoa(*list[0]), &address->sin_addr) <= 0) {
		perror("ERROR: Invalid address/Address not supported.\n");
		return 0;
	}

	if (connect(*sock, (struct sockaddr *) address, sizeof(*address)) < 0) {
		perror("ERROR: Connection Failed.\n");
		return 0;
	}

	/* Success */
	return 1;
}

void *
recv_thread(void *data)
{
	char buff[512];
	int type;

	// Just display recieved messages.
	while (1) {
		recv_mesg(buff, &type);
		update_messages(buff);
	}

	return 0;
}

int
main(int argc, char *argv[])
{
	GtkWidget *window, *grid, *calculate;
	int i, port = 8199, type;
	char *hostname = "localhost";
	char buff[256];
	struct sockaddr_in address;
	pthread_t worker_thread;
	/* Map the widgets to thier ID's */
	struct widge_mapping map[] = {
		{.ID = "fixed", .widget = &fixed},
		{.ID = "send", .widget = &send_btn},
		{.ID = "recv", .widget = &recv_box},
		{.ID = "text", .widget = &text},
		{.ID = "name", .widget = &name},
		{.ID = "voip_joined", .widget = &voip_joined},
		{.ID = "user_online_label", .widget = &user_online_label},
		{.ID = "jl_voip", .widget = &jl_voip},
		{.ID = "user_online", .widget = &user_online},
	};

	gtk_init(&argc, &argv);
	
	/* Parse args */
	while ((i = getopt(argc, argv, "p:H:h")) != -1) {
		switch (i) {
			case 'p':
				port = atoi(optarg);
				printf("Port: %d\n", port);
				break;
			case 'H':
				hostname = strdup(optarg);
				printf("hostname: %s\n", hostname);
				break;
			case 'h':
				printf("cmesg client v1.0 (https://github.com/skippy404/cmesg)\n\n");
				printf("Usage: cmesg_client [options].\n");
				printf("-h\tShows this message.\n");
				printf("-p\tSpecify a port to use.\n");
				printf("-H\tSpecify the hostname to connect too.\n");
				return EXIT_SUCCESS;
				break;
		}
	}

	/* Setup Sockets */
	if (!init_sock(port, hostname, &sock, &address)) {
		exit(1);
	}

	builder = gtk_builder_new_from_file("./assets/client.glade");
	window = GTK_WIDGET(gtk_builder_get_object(builder, "root"));
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	/* Map the GTK Widgets from the builder to thier Counterparts */
	for (i = 0; i < sizeof(map)/sizeof(*map); i++) {
		printf("%s\n", map[i].ID);
		*(map[i].widget) = GTK_WIDGET(gtk_builder_get_object(builder, map[i].ID));
	}

	/* Map the buttons to thier handlers */
	g_signal_connect(send_btn, "clicked", G_CALLBACK(on_send_clicked), NULL);

	/* Setup the update buffer */
	gtk_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(recv_box));
	gtk_text_buffer_get_iter_at_offset(gtk_buffer, &iter, 0);

	/* TODO: Get input via gui */
	recv_mesg(buff, &type);
	printf("%s\n", buff);
	send_mesg("skippy\n");

	recv_mesg(buff, &type);
	printf("%s\n", buff);
	send_mesg("22\n");

	/* Display welcome message */
	update_messages("Welcome to cmesg!\n");

	/* Fork recv thread */
	pthread_create(&worker_thread, NULL, recv_thread, NULL);

	gtk_widget_show(window);
	gtk_main();

	return EXIT_SUCCESS;
}
