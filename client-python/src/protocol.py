# Copyright (c) 2026. Distributed Bank client.
# Wire format matches server: Request (id, op_code, payload[1200]), Response (id, status_code, payload[1200]).
# Server serdes uses host byte order (no htonl); we use little-endian '<' to match typical server (x86/ARM).
# String format: 8-byte length (size_t) + raw bytes (server serdes.h).

import struct
from typing import Tuple

PAYLOAD_SIZE = 1200  # rpc/protocol.h payload_size

# --- Op codes (match server rpc/protocol.h) ---
OP_OPEN = 1
OP_CLOSE = 2
OP_CHECK_BALANCE = 3
OP_DEPOSIT = 4
OP_WITHDRAW = 5
OP_TRANSFER = 6
OP_EXCHANGE = 7
OP_MONITOR = 8

# --- Status codes (match server rpc/protocol.h) ---
STATUS_SUCCESS = 1
STATUS_FAIL = 2
STATUS_ERROR = 3
STATUS_CALLBACK = 4

# --- Currency: server serializes as string (currency_to_str) ---
CURRENCY_STRINGS = ["USD", "RMB", "SGD", "JPY", "BPD"]
CURRENCY_NAMES = {"usd": "USD", "rmb": "RMB", "sgd": "SGD", "jpy": "JPY", "bpd": "BPD"}


def _pack_string(s: str) -> bytes:
    """Length-prefixed string: 8-byte size_t (little-endian) + UTF-8 bytes. Matches server serdes.h."""
    raw = s.encode("utf-8")
    return struct.pack("<Q", len(raw)) + raw


def _pack_payload(content: bytes) -> bytes:
    """Pad content to PAYLOAD_SIZE with zeros. Server expects fixed 1200-byte payload."""
    if len(content) > PAYLOAD_SIZE:
        raise ValueError(f"Payload content {len(content)} exceeds {PAYLOAD_SIZE}")
    return content + b"\x00" * (PAYLOAD_SIZE - len(content))


def pack_request(request_id: int, op_code: int, content: bytes) -> bytes:
    """Full request: id (4), op_code (4), payload (1200). Total 1208 bytes."""
    payload = _pack_payload(content)
    return struct.pack("<ii", request_id, op_code) + payload


# --- Open: user_name, password, balance, currency (string) ---
def pack_open_account(name: str, password: str, currency_str: str, initial_balance: float) -> bytes:
    return _pack_string(name) + _pack_string(password) + struct.pack("<f", initial_balance) + _pack_string(currency_str)


# --- Close: id, user_name, password ---
def pack_close_account(account_id: int, name: str, password: str) -> bytes:
    return struct.pack("<i", account_id) + _pack_string(name) + _pack_string(password)


# --- Check balance: id, user_name, password, cur_unit (string) ---
def pack_check_balance(account_id: int, name: str, password: str, currency_str: str) -> bytes:
    return struct.pack("<i", account_id) + _pack_string(name) + _pack_string(password) + _pack_string(currency_str)


# --- Deposit / Withdraw: id, user_name, password, cur_unit (string), amount ---
def pack_deposit_or_withdraw(account_id: int, name: str, password: str, currency_str: str, amount: float) -> bytes:
    return (
        struct.pack("<i", account_id)
        + _pack_string(name)
        + _pack_string(password)
        + _pack_string(currency_str)
        + struct.pack("<f", amount)
    )


# --- Transfer: sender_id, user_name, password, cur_unit, amount, receiver_id ---
def pack_transfer(
    sender_id: int, name: str, password: str, currency_str: str, amount: float, receiver_id: int
) -> bytes:
    return (
        struct.pack("<i", sender_id)
        + _pack_string(name)
        + _pack_string(password)
        + _pack_string(currency_str)
        + struct.pack("<f", amount)
        + struct.pack("<i", receiver_id)
    )


# --- Exchange: id, user_name, password, from_cur (string), to_cur (string), amount_to_exchange ---
def pack_exchange(
    account_id: int, name: str, password: str, from_currency: str, to_currency: str, amount: float
) -> bytes:
    return (
        struct.pack("<i", account_id)
        + _pack_string(name)
        + _pack_string(password)
        + _pack_string(from_currency)
        + _pack_string(to_currency)
        + struct.pack("<f", amount)
    )


# --- Monitor: int64_t duration (e.g. milliseconds) ---
def pack_monitor(duration_ms: int) -> bytes:
    return struct.pack("<q", duration_ms)


# --- Response: id (4), status_code (4), payload (1200). Server may send larger buffer (1400). ---
def unpack_response(data: bytes) -> Tuple[int, int, bytes]:
    """Unpack response: (response_id, status_code, payload_message). Payload is trimmed to string."""
    if len(data) < 8:
        raise ValueError("Response too short for id and status")
    resp_id, status = struct.unpack_from("<ii", data, 0)
    payload_raw = data[8 : 8 + PAYLOAD_SIZE] if len(data) >= 8 + PAYLOAD_SIZE else data[8:]
    # Server stores null-terminated string in payload
    msg = payload_raw.split(b"\x00")[0].decode("utf-8", errors="replace")
    return resp_id, status, msg

# --- Callback: no id, no status_code, payload (1400). ---
def unpack_callback(data: bytes) -> str:
    """Unpack callback message from server."""
    # 1. Clean the buffer
    # The server sends a fixed-size buffer
    # Only care about the payload starts from the first non-zero byte
    msg_bytes = data.split(b"\x00", 1)[0]

    # 2. Decode the bytes to a string
    return msg_bytes.decode("utf-8", errors="replace")