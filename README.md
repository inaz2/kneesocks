# kneesocks

LD_PRELOAD based SOCKS5 proxifier

* supports only TCP CONNECT command with no authentication
  * intended to use with OpenSSH dynamic port forwarding
  * implemented by replacing connect(2)
* support proxy-end DNS resolution
  * implemented by replacing getaddrinfo(3)/gethostbyname(3)


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
$ export socks_proxy=localhost:1080
```

The default address is just above.

To output debug logs:

```
$ export DEBUG=1
```
