mod_secfcgid
============

- Version: 0.0.1
- Technologies:
  - C
- Compatibility:
  - Apache HTTP Server (>= 2.4)
  - mod_fcgid (>= 2.3)
- Copyright / Authors:
  - Krystian Pietruszka <kpietru@lokolab.net>
- Licenses:
  - MIT <http://opensource.org/licenses/MIT>
- Download: <https://github.com/lokolab/mod_secfcgid/releases>
- Homepage: <http://www.lokolab.net>

Module for Apache HTTP Server,
checks paths from `FcgidWrapper` and off several
directives (`FcgidAccessChecker`, `FcgidAccessCheckerAuthoritative`,
`FcgidAuthenticator`, `FcgidAuthenticatorAuthoritative`,
`FcgidAuthorizer`, `FcgidAuthorizerAuthoritative`)
declared inside ".htaccess".

Undeclared wrapper paths and directives used inside
the file ".htaccess", the server will give an error.
____________________________________________________

Installing and configuring on Linux
-----------------------------------

First, install the "apxs".

Second, install the "mod_secfcgid" Apache module,
but before edit the variable `PATH_TO_WRAPPERS`
to set their own path*:

    PATH_TO_WRAPPERS=/usr/fcgid-wrapper/*
    cd /tmp
    wget https://github.com/lokolab/mod_secfcgid/archive/v0.0.1.tar.gz -O mod_secfcgid-0.0.1.tar.gz
    tar -xzvf mod_secfcgid-0.0.1.tar.gz
    cd mod_secfcgid-0.0.1
    sed -i "s#p_ =.*;#p_ = \"${PATH_TO_WRAPPERS::-1}\";#g" mod_secfcgid.c
    apxs -i -a -c mod_secfcgid.c

Finally, restart the Apache HTTP Server via shell:

    service apache2 restart

Example
-------

".htaccess" file with the correct path*:

    FcgidWrapper /usr/fcgid-wrapper/someuser/php/php7.fcgi .php


".htaccess" file with the wrong path (server return error)*:

    FcgidWrapper /home/someuser/fcgid-wraper/php/php7.fcgi .php


".htaccess" file with an illegal directive (server return error):

    FcgidAuthenticatorAuthoritative On

References
----------

1. [FastCGI Specification][1]
2. [Apache Module "mod_fcgid" – documentation][2]
3. [Developing modules for the Apache HTTP Server 2.4][3]
4. [Bug 49220 – feature request for being able to disable `FcgidWrapper` support in ".htaccess"][4]
5. ["mod_fcgid" and ".htaccess" – discussion (browse by date)][5]
6. ["mod_fcgid" – restrict arbitrary command execution from ".htaccess" files – discussion (browse by date)][6]

[1]: http://web.archive.org/web/20160306081510/http://fastcgi.com/drupal/node/6?q=node/22
[2]: http://httpd.apache.org/mod_fcgid/mod/mod_fcgid.html
[3]: http://httpd.apache.org/docs/2.4/developer/modguide.html
[4]: http://bz.apache.org/bugzilla/show_bug.cgi?id=49220
[5]: http://mail-archives.apache.org/mod_mbox/httpd-users/201005.mbox/%3C2165C78BA8AF404CAFA91AC453E345D6@SERVERADMIN%3E
[6]: http://mail-archives.apache.org/mod_mbox/httpd-dev/201309.mbox/%3C7D9F127E-5D38-4190-80A7-58BFDC55E051@uvm.edu%3E

________________________________________________________
[*] Paths and username should be modified to the server.

