import asyncio
import logging

import websockets
import psutil

_logger = logging.getLogger()
_logger.setLevel(logging.DEBUG)

formatter = logging.Formatter(
    '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)

ch = logging.StreamHandler()
ch.setLevel(logging.INFO)
ch.setFormatter(formatter)
_logger.addHandler(ch)


async def ws_handler(socket, path):
    while True:
        load = psutil.getloadavg()
        payload = (";").join(map(str, load))
        _logger.info("Sending Payload: %s" % payload)
        await socket.send(payload)
        await asyncio.sleep(1)


def create_web_socket_server(path, port):
    _logger.info("Creating WebSocket Server")
    ws = websockets.serve(ws_handler, path, port)
    asyncio.get_event_loop().run_until_complete(ws)
    asyncio.get_event_loop().run_forever()


if __name__ == "__main__":
    create_web_socket_server("localhost", 5555)
