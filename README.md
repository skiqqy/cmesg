# cmesg
Messeging Application

This is a simple messaging server + client, the goal of this project is to allow
a simple tui mode (using nc) to chat, or using a far more complicated GUI with
advanced features.

## Compiling

````
$ make
````

## Running the server

````
$ ./bin/cmesg [options]
````

## Help

````
$ ./bin/cmesg -h
````

## Admin

When compiling with `$ make`, a templete config file is created, this templete
file is used to configure the admin user, who can login to the server and
do privilaged commands (such as ban a user, mute someone etc).

In order to enable this, one must run cmesg like so,

````
$ mesg -c /path/to/config
````

## Testing

Run the server and then,

````
$ nc localhost <port>
````

And then type a message.
