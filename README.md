SeaCraft
========
Sea battle client and server. Our website is http://seacraft.i-port.su/

#### Client usage
Use your mouse to setup ships -- _left-click_ to set part of it, _right-click_ to clear.
Also you can fill the field randomly.

#### Server command line arguments
- -s, --statfile [*FILE*]  -- use FILE for statistic (default '**stats**')
- -u, --authfile [*FILE*]  -- use FILE for authorized users (default '**authorized**')
- -a, --address  [*ADDR*]  -- server address to spawn to (default '**0.0.0.0**')
- -p, --port [*PORT*]      -- and it's port (default **1234**)
- -g, --allowguest         -- allow guest accounts
- -r, --disablereg         -- disallow registration


Format of the *authfile* is simple:

    login:password:

Format of the *statfile*:

    login:roundsWon:roundsLost:

#### Compilation remark
On *Linux* we use **alsa** output for sound effects. So you should have
the dev-package of *libasound* to compile the project.

