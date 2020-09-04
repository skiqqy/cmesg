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

## Testing

Run the server and then,

````
$ nc localhost <port>
````

And then type a message.
