# ministaller
Lightweight installer/updater for desktop Qt application

There're two tools available: **minipkgen** and **ministaller**.

### minipkgen

This tool is used for creating a 'diff' package config.

    Options:
      -b, --base-package <directory>  Path to the base package dir
      -n, --new-package <directory>   Path to the new package dir
      -o, --output <filepath>         Path to the result package file
      -f, --force-update              Don't skip same files
      -k, --keep-missing              Do not remove missing files in new package
      -v, --verbose                   Be verbose

Typical usage is either offline usage or server-side when client request an update from version X to version Y and server generates an update package from only items needed for upgrade.

`./minipkgen --verbose -b ../test_base -n ../test_new` will create a json file with three self-explanatory lists _add_, _remove_, _update_ with objects containing _path_ and _sha1_ keys. Paths are relative either to `base dir` or to the `new dir` dependingly on the context (are they files to add, remove or update). Example of the config:

    {
        "add": [
            {
                "path": "dir2/Makefile",
                "sha1": "f4217be269bec91780fe27cca6f207c15051d007"
            }
        ],
        "remove": [
            {
                "path": "dir1/diffgenerator.h",
                "sha1": "42d268d568da9cacb5e8cef4a4354a39502841c6"
            }
        ],
        "update": [
        ]
    }

### ministaller

This is the main installer application which executes an update config created by **minipkgen**
