SeaCraft
========
Sea battle client and server. Our website is http://seacraft.i-port.su/

#### Client usage
Use your mouse to setup ships -- _left-click_ to set part of it, _right-click_ to clear.
Also you can fill the field randomly.

#### Server command line arguments
- --address - specify server addr to spawn (default '0.0.0.0')
- --port - and port (default 1234)
- --authfile - specify file with authorization info (default 'authorized')
- --statfile - specify file with statistic to save to (default 'stats')

PS: For now, *authfile* is _not_ created automatically. You should create it first.

Format of the *authfile* is simple:

    login:password:

Format of the *statfile*:

    login:roundsWon:roundsLost:

