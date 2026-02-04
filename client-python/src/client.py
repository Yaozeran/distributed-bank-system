# Copyright (c) 2026. Distributed Bank client.
# UDP client: send request, receive response with timeout.

import socket
from typing import Optional, Tuple

DEFAULT_TIMEOUT_SEC = 5.0
# Server request = 8 + 1200 = 1208 bytes; response buffer = 1400. Allow up to 2K.
MAX_DATAGRAM = 2048


def create_socket(server_addr: Tuple[str, int], timeout_sec: float = DEFAULT_TIMEOUT_SEC) -> socket.socket:
    """Create a UDP socket bound to an ephemeral port, ready to send to server_addr."""
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.settimeout(timeout_sec)
    return sock


def send_request(sock: socket.socket, server_addr: Tuple[str, int], data: bytes) -> None:
    """Send raw request bytes to server (e.g. 1208 bytes for id + op_code + payload)."""
    if len(data) > MAX_DATAGRAM:
        raise ValueError(f"Request size {len(data)} exceeds limit {MAX_DATAGRAM}")
    sock.sendto(data, server_addr)


def receive_response(sock: socket.socket) -> Tuple[bytes, Tuple[str, int]]:
    """Receive one response datagram. Returns (payload_bytes, (peer_host, peer_port))."""
    data, peer = sock.recvfrom(65535)
    return data, peer


def request_reply(
    sock: socket.socket,
    server_addr: Tuple[str, int],
    request: bytes,
) -> Optional[bytes]:
    """
    Send request and wait for one reply. Returns response bytes or None on timeout.
    Caller is responsible for unmarshalling (status code + payload).
    """
    send_request(sock, server_addr, request)
    try:
        data, _ = receive_response(sock)
        return data
    except socket.timeout:
        return None
