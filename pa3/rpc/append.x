struct append_init_params {
    int f;
    int n;
    int l;
    int m;
    char c0;
    char c1;
    char c2;
    string host_verify<>;
};

program APPEND_PROG {
    version APPEND_VERS {
        int RPCInitAppendServer(append_init_params) = 1;
        int RPCAppend(char) = 2;
    } = 1;

} = 0x01310455;
