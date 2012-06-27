SeaCraft
========
Sea battle client and server. Our website is http://seacraft.i-port.su/

#### Server command line arguments
- --address - specify server addr to spawn (default '0.0.0.0')
- --port - and port (default 1234)
- --authfile - specify file with authorization info (default 'authorized')
- --statfile - specify file with statistic to save to (default 'stats')

Format of the authfile is simple:

    login:password:

Format of the statfile:

    login:roundsWon:roundsLost:

