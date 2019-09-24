from http.client import HTTPConnection
import json

SAVE_PATH = "/opt/downloads"
STOP_TIMEOUT = 60
MAX_CONCURRENT = 16
MAX_MAGNETS = 256

ARIA2RPC_ADDR = "127.0.0.1"
ARIA2RPC_PORT = 6800


def exec_rpc(magnet):
    """
    使用 rpc，减少线程资源占用，关于这部分的详细信息科参考
    https://aria2.github.io/manual/en/html/aria2c.html?highlight=enable%20rpc#aria2.addUri
    """
    conn = HTTPConnection(ARIA2RPC_ADDR, ARIA2RPC_PORT)
    req = {
        "jsonrpc": "2.0",
        "id": "magnet",
        "method": "aria2.addUri",
        "params": [
            [magnet],
            {
                "bt-stop-timeout": str(STOP_TIMEOUT),
                "max-concurrent-downloads": str(MAX_CONCURRENT),
                "listen-port": "6881",
                "dir": SAVE_PATH,
            },
        ],
    }
    conn.request(
        "POST",
        "/jsonrpc",
        json.dumps(req),
        {"Content-Type": "application/json"},
    )

    res = json.loads(conn.getresponse().read())
    print(res)
    if "error" in res:
        print("Aria2c replied with an error:", res["error"])


def magnet2torrent():
    """
    磁力转种子
    """
    exec_rpc("magnet:?xt=urn:btih:B7611BAACDEA11816D0959F62617A6A88383993A")
