# Sysrepo SNMP segfault example

This repo provides a minimal example to reproduce a segmentation fault that we
found.

## Setup

1. Clone the repository.

    ```bash
    git clone git@github.com:nullromo/sysrepo-snmp-segfault-example.git
    cd sysrepo-snmp-segfault-example/
    ```

1. Install the yang module.

    ```bash
    sysrepoctl --install=./example-module@2022-04-28.yang
    ```

1. Build the project.

    ```bash
    mkdir build
    cd build
    cmake ..
    make
    cd ..
    ```

## How to reproduce the bug

1. Run the SNMP server.

    ```bash
    ./build/example-snmp
    ```

    It should print some stuff and then say `Running main loop...` at the end.

1. Test that the server is working.

    ```bash
    snmpget -v2c -c public 127.0.0.1:4701 .1.3.6.1.4.1.1.1
    ```

    It should print `iso.3.6.1.4.1.1.1 = INTEGER: 0`, which shows that the
    value of the requested node is `0`.

1. Run the plugin.

    ```bash
    ./build/example-standalone-plugin
    ```

    It should print some stuff and then say `Plugin initialized successfully.`.

1. Test that the plugin is working.

    ```bash
    sysrepocfg --export --datastore operational --module example-module
    ```

    It should respond with the following data:

    ```xml
    <example-data xmlns="urn:example-module">
        <thing>hello</thing>
    </example-data>
    ```

    If it fills in `hello` for the `/example-module:example-data/thing` node,
    then you know it's working properly.

1. Reproduce the segfault.

    Now run the SNMP server and the plugin at the same time, and try executing
    the `snmpget` command. It should time out and the SNMP server should crash
    with a segmentation fault.

## Debugging steps

There is a `gdb_setup_file` included in the repository which will help to set
up the debugger to find the segmentation fault. Run gdb like this:

```bash
gdb -tui -x gdb_setup_file ./build/example-snmp
```

The segmentation fault occurs when trying to call `lyd_lyb_data_length()` on
line 1607 of `shm_sub.c`.

```c
parent_lyb_len = lyd_lyb_data_length(parent_lyb);
```

The fault occurs *during the calling of the function*, meaning that we never
make it into the `lyd_lyb_data_length` function. The debugger is just about to
execute the call when we step, and then the fault happens.
