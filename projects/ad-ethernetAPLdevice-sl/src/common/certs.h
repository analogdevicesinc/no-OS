#include "lwip_socket.h"
#include "lwip_adin1110.h"

#if (CONNECTION_TYPE == 1)
#ifndef DISABLE_SECURE_SOCKET
#define CA_CERT                                                             \
    "-----BEGIN CERTIFICATE-----\r\n"                                       \
    "MIICfDCCAiGgAwIBAgIUL0wv7pFOXhrgRA2Ojcwo5KXFs6UwCgYIKoZIzj0EAwIw\r\n"  \
    "gZIxCzAJBgNVBAYTAklFMRAwDgYDVQQIDAdNdW5zdGVyMREwDwYDVQQHDAhMaW1l\r\n"  \
    "cmljazEMMAoGA1UECgwDQURJMREwDwYDVQQLDAhTZWN1cml0eTEWMBQGA1UEAwwN\r\n"  \
    "MTkyLjE2OC4wLjEwMzElMCMGCSqGSIb3DQEJARYWc2FkaGJoLnJ5YW5AYW5hbG9n\r\n"  \
    "LmNvbTAeFw0yNTA5MTkxMzQ0NDBaFw0zNTA5MTcxMzQ0NDBaMIGSMQswCQYDVQQG\r\n"  \
    "EwJJRTEQMA4GA1UECAwHTXVuc3RlcjERMA8GA1UEBwwITGltZXJpY2sxDDAKBgNV\r\n"  \
    "BAoMA0FESTERMA8GA1UECwwIU2VjdXJpdHkxFjAUBgNVBAMMDTE5Mi4xNjguMC4x\r\n"  \
    "MDMxJTAjBgkqhkiG9w0BCQEWFnNhZGhiaC5yeWFuQGFuYWxvZy5jb20wWTATBgcq\r\n"  \
    "hkjOPQIBBggqhkjOPQMBBwNCAATrk4LNopc/v2LwrvOj8dU4HQ41phLLqtIoivt7\r\n"  \
    "ykZvsh7AEujFtrS1X1QS7pnyb2TIK17win2wgNw6wE55EA4Ao1MwUTAdBgNVHQ4E\r\n"  \
    "FgQUapFI+iiRJK7aULq01yFKFPu/cxQwHwYDVR0jBBgwFoAUapFI+iiRJK7aULq0\r\n"  \
    "1yFKFPu/cxQwDwYDVR0TAQH/BAUwAwEB/zAKBggqhkjOPQQDAgNJADBGAiEAr89F\r\n"  \
    "RYOHErV59iYkXtvVLDw5d3cDrjCCtT9EkcFiunkCIQCLttRU95BGtY2j9b3+BBJa\r\n"  \
    "4MQjRSZY7s4dNUBdbgsoVw==\r\n"  \
    "-----END CERTIFICATE-----\r\n"

#define DEVICE_CERT                                                         \
    "-----BEGIN CERTIFICATE-----\r\n"                                       \
    "MIICazCCAhCgAwIBAgIUa08G6M8bGjJJrS5a5HscA8h7jtAwCgYIKoZIzj0EAwIw\r\n"  \
    "gZIxCzAJBgNVBAYTAklFMRAwDgYDVQQIDAdNdW5zdGVyMREwDwYDVQQHDAhMaW1l\r\n"  \
    "cmljazEMMAoGA1UECgwDQURJMREwDwYDVQQLDAhTZWN1cml0eTEWMBQGA1UEAwwN\r\n"  \
    "MTkyLjE2OC4wLjEwMzElMCMGCSqGSIb3DQEJARYWc2FkaGJoLnJ5YW5AYW5hbG9n\r\n"  \
    "LmNvbTAeFw0yNTA5MTkxMzQ2MzhaFw0yNjA5MTkxMzQ2MzhaMIGSMQswCQYDVQQG\r\n"  \
    "EwJJRTEQMA4GA1UECAwHTXVuc3RlcjERMA8GA1UEBwwITGltZXJpY2sxDDAKBgNV\r\n"  \
    "BAoMA0FESTERMA8GA1UECwwIU2VjdXJpdHkxFjAUBgNVBAMMDTE5Mi4xNjguMC4x\r\n"  \
    "MDMxJTAjBgkqhkiG9w0BCQEWFnNhZGhiaC5yeWFuQGFuYWxvZy5jb20wWTATBgcq\r\n"  \
    "hkjOPQIBBggqhkjOPQMBBwNCAAQ9JLMz+wCrAuMCpe/4SjRB8Kvxg+04+UC/bTK7\r\n"  \
    "VmlU1X2mBuBi2MJv2AMlFPI17A95D2+HgPW0IFdYO0u90UIro0IwQDAdBgNVHQ4E\r\n"  \
    "FgQUXYjMOEzBcKTMhovUMxy3Sm7fQPIwHwYDVR0jBBgwFoAUapFI+iiRJK7aULq0\r\n"  \
    "1yFKFPu/cxQwCgYIKoZIzj0EAwIDSQAwRgIhAJEvs+bULFEn6z65uknWAN+8xct1\r\n"  \
    "FTdpF8cwQAYREhmiAiEAgrqLihlJfgI3XCm8OtE7VUE0j1a68flAXMPEhzzlBaw=\r\n"  \
    "-----END CERTIFICATE-----\r\n"

#define DEVICE_PRIVATE_KEY                                                  \
    "-----BEGIN EC PRIVATE KEY-----\r\n"                                    \
    "MHcCAQEEILKXXlAMD2yqPcpAAwC+mIZQQ6O9rdaVnB165Qmj9AI2oAoGCCqGSM49\r\n"  \
    "AwEHoUQDQgAEPSSzM/sAqwLjAqXv+Eo0QfCr8YPtOPlAv20yu1ZpVNV9pgbgYtjC\r\n"  \
    "b9gDJRTyNewPeQ9vh4D1tCBXWDtLvdFCKw==\r\n"  \
    "-----END EC PRIVATE KEY-----\r\n"
#endif
#endif
