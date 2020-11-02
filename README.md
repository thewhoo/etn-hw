# etn-hw

Homework (devices communicating with server)

## Build

etn-hw has the following dependencies:

* CMake >= 3.10

### Compiling

Run `cmake CMakeLists.txt` followed by `make`

## Running

The build process produces two binaries, `client` and `server`

### Server

The `server` binary only takes one argument, the TCP port on which the server will listen:

```sh
./server 5555
```

The server can be killed with `Ctrl+C`

### Client

The `client` binary takes the following arguments:

```sh
./client SERVER_IP SERVER_PORT [DEVICE-TYPE DEVICE-ID] ... [DEVICE-TYPE DEVICE-ID]
```



where:

* `SERVER_IP` is the IPv4 address of the server
* `SERVER_PORT` is the port on which the server listens
* `[DEVICE-TYPE DEVICE-ID]` represents a device identified by `DEVICE-ID` of type `DEVICE-TYPE`
* The client can emulate as many devices as desired, but they must always be fully specified by a
`[DEVICE-TYPE DEVICE-ID]` pair

The following `DEVICE-TYPE` values are currently supported:

* `temp-monitor`
* `uptime-monitor`

The client can be killed with `Ctrl+C`