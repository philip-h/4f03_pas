struct verify_init_params {
    int n;
    int l;
    int m;
};

program VERIFY_PROG {
    version VERIFY_VERS {
        int RPCInitVerifyServer(verify_init_params) = 1;
        string RPCGetSeg(int) = 2;
        string RPCGetS() = 3;
    } = 2;

} = 0x01327857;
