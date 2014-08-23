# kneesocks

LD_PRELOAD based SOCKS 5 proxifier

* intended to use with OpenSSH dynamic port forwarding
  * for now, support only TCP CONNECT command with no authentication
* support proxy-end DNS resolution


## Installation

```
$ make
$ sudo make install
```

To uninstall:

```
$ sudo make uninstall
```


## Usage

To connect via sshd on 192.168.0.2 (ssh connection is available for 1 hour):

```
$ ssh -D 1080 user@192.168.0.2 -f sleep 3600
$ kneesocks curl http://www.example.com/
```

With sudo:

```
$ sudo kneesocks apt-get update
```


## Configuration

kneesocks uses environment variables for configuration.

To change proxy server address:

```
$ export socks_proxy=127.0.0.1:1080
```

The default address is just above.

To output debug logs:

```
$ export DEBUG=1
```
