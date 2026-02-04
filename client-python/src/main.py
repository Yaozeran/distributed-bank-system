#!/usr/bin/env python3
# Copyright (c) 2026. Distributed Bank client.
# Main loop: text menu for all server operations (Open, Close, Check Balance, Deposit, Withdraw, Transfer, Exchange, Monitor).

import argparse
import socket
import sys
import time

from . import client as udp_client
from . import protocol


def parse_args():
    p = argparse.ArgumentParser(description="Distributed Bank UDP client (matches server protocol)")
    p.add_argument("host", help="Server host (e.g. 127.0.0.1)")
    p.add_argument("port", type=int, help="Server port (e.g. 8080)")
    p.add_argument("--timeout", type=float, default=5.0, help="Request timeout in seconds")
    return p.parse_args()


def get_currency() -> str:
    """Return currency string (USD, RMB, SGD, JPY, BPD)."""
    while True:
        s = input("Currency (usd / rmb / sgd / jpy / bpd): ").strip().lower()
        if s in protocol.CURRENCY_NAMES:
            return protocol.CURRENCY_NAMES[s]
        print("Invalid currency. Choose usd, rmb, sgd, jpy, or bpd.")


def send_and_show(sock, server_addr: tuple, request_id: int, op_code: int, content: bytes) -> int:
    """Build request, send, receive, parse response and print message. Returns next request_id."""
    # 1. Only pack the request once
    req = protocol.pack_request(request_id, op_code, content)
    # 2. Set up the retry configuration
    MAX_RETRIES = 5
    reply = None
    # 3. Retry loop
    for attempt in range(MAX_RETRIES):
        # try to send 
        reply = udp_client.send_request(sock, server_addr, req)
        if reply is not None:
            break
    if reply is None:
        print("Error: No reply from server (timeout).")
        return request_id + 1
    try:
        _resp_id, status, msg = protocol.unpack_response(reply)
    except Exception as e:
        print(f"Error: Invalid response from server: {e}")
        return request_id + 1
    if status == protocol.STATUS_SUCCESS:
        print("Success:", msg)
    elif status == protocol.STATUS_FAIL:
        print("Fail:", msg)
    elif status == protocol.STATUS_ERROR:
        print("Error:", msg)
    else:
        print("Response:", msg)
    return request_id + 1


def do_open_account(sock, server_addr: tuple, request_id: int) -> int:
    name = input("Account holder name: ").strip()
    password = input("Password: ").strip()
    currency_str = get_currency()
    while True:
        try:
            balance = float(input("Initial balance: ").strip())
            break
        except ValueError:
            print("Enter a valid number.")
    content = protocol.pack_open_account(name, password, currency_str, balance)
    return send_and_show(sock, server_addr, request_id, protocol.OP_OPEN, content)


def do_close_account(sock, server_addr: tuple, request_id: int) -> int:
    while True:
        try:
            acc_id = int(input("Account number: ").strip())
            break
        except ValueError:
            print("Enter a valid integer.")
    name = input("Account holder name: ").strip()
    password = input("Password: ").strip()
    content = protocol.pack_close_account(acc_id, name, password)
    return send_and_show(sock, server_addr, request_id, protocol.OP_CLOSE, content)


def do_check_balance(sock, server_addr: tuple, request_id: int) -> int:
    while True:
        try:
            acc_id = int(input("Account number: ").strip())
            break
        except ValueError:
            print("Enter a valid integer.")
    name = input("Account holder name: ").strip()
    password = input("Password: ").strip()
    currency_str = get_currency()
    content = protocol.pack_check_balance(acc_id, name, password, currency_str)
    return send_and_show(sock, server_addr, request_id, protocol.OP_CHECK_BALANCE, content)


def do_deposit(sock, server_addr: tuple, request_id: int) -> int:
    while True:
        try:
            acc_id = int(input("Account number: ").strip())
            break
        except ValueError:
            print("Enter a valid integer.")
    name = input("Account holder name: ").strip()
    password = input("Password: ").strip()
    currency_str = get_currency()
    while True:
        try:
            amount = float(input("Amount to deposit: ").strip())
            break
        except ValueError:
            print("Enter a valid number.")
    content = protocol.pack_deposit_or_withdraw(acc_id, name, password, currency_str, amount)
    return send_and_show(sock, server_addr, request_id, protocol.OP_DEPOSIT, content)


def do_withdraw(sock, server_addr: tuple, request_id: int) -> int:
    while True:
        try:
            acc_id = int(input("Account number: ").strip())
            break
        except ValueError:
            print("Enter a valid integer.")
    name = input("Account holder name: ").strip()
    password = input("Password: ").strip()
    currency_str = get_currency()
    while True:
        try:
            amount = float(input("Amount to withdraw: ").strip())
            break
        except ValueError:
            print("Enter a valid number.")
    content = protocol.pack_deposit_or_withdraw(acc_id, name, password, currency_str, amount)
    return send_and_show(sock, server_addr, request_id, protocol.OP_WITHDRAW, content)


def do_transfer(sock, server_addr: tuple, request_id: int) -> int:
    while True:
        try:
            sender_id = int(input("Your account number (sender): ").strip())
            break
        except ValueError:
            print("Enter a valid integer.")
    name = input("Account holder name: ").strip()
    password = input("Password: ").strip()
    currency_str = get_currency()
    while True:
        try:
            amount = float(input("Amount to transfer: ").strip())
            break
        except ValueError:
            print("Enter a valid number.")
    while True:
        try:
            receiver_id = int(input("Receiver account number: ").strip())
            break
        except ValueError:
            print("Enter a valid integer.")
    content = protocol.pack_transfer(sender_id, name, password, currency_str, amount, receiver_id)
    return send_and_show(sock, server_addr, request_id, protocol.OP_TRANSFER, content)


def do_exchange(sock, server_addr: tuple, request_id: int) -> int:
    while True:
        try:
            acc_id = int(input("Account number: ").strip())
            break
        except ValueError:
            print("Enter a valid integer.")
    name = input("Account holder name: ").strip()
    password = input("Password: ").strip()
    print("From currency:")
    from_cur = get_currency()
    print("To currency:")
    to_cur = get_currency()
    while True:
        try:
            amount = float(input("Amount (in target currency) to receive: ").strip())
            break
        except ValueError:
            print("Enter a valid number.")
    content = protocol.pack_exchange(acc_id, name, password, from_cur, to_cur, amount)
    return send_and_show(sock, server_addr, request_id, protocol.OP_EXCHANGE, content)


def do_monitor(sock, server_addr: tuple, request_id: int, timeout_sec: float = 5.0) -> int:
    while True:
        try:
            duration_ms = int(input("Monitor duration in milliseconds: ").strip())
            if duration_ms > 0:
                break
            print("Enter a positive number.")
        except ValueError:
            print("Enter a valid integer.")
    # Send monitor request
    content = protocol.pack_monitor(duration_ms)
    req = protocol.pack_request(request_id, protocol.OP_MONITOR, content)
    MAX_RETRIES = 5
    reply = None

    print(f"Sending Monitor Request (ID: {request_id})...")
    
    for attempt in range(MAX_RETRIES):
        # try to send request and wait for response
        # Notes: request_reply contains sock.sendto and sock.recvfrom
        reply = udp_client.request_reply(sock, server_addr, req)

        if reply is not None:
            # means client received a response from server
            print(f"Request received!")
            break
        else:
            print(f"Timeout (Attempt {attempt + 1}/{MAX_RETRIES}). Resending request...")

    # reply = udp_client.request_reply(sock, server_addr, req)
    if reply is None:
        print("Error: No reply from server (timeout).")
        return request_id + 1
    # this time, the reply is supposed to be a response from server
    # try:
    #     _rid, status, msg = protocol.unpack_response(reply)
    #     print("Success:" if status == protocol.STATUS_SUCCESS else "Response:", msg)
    # except Exception as e:
    #     print("Error parsing response:", e)
    #     return request_id + 1
    # Block and receive callback datagrams for the monitor window (server sends account updates).
    sock.settimeout(min(1.0, duration_ms / 1000.0) if duration_ms else 1.0)
    deadline = time.time() + (duration_ms / 1000.0)
    try:
        while time.time() < deadline:
            try:
                data, _ = udp_client.receive_response(sock)
                msg = protocol.unpack_callback(data)
                print("Update:", msg)
            except socket.timeout:
                pass
    except Exception as e:
        print("Monitor ended:", e)
    sock.settimeout(timeout_sec)
    print("Monitor window ended.")
    return request_id + 1


def main() -> int:
    args = parse_args()
    server_addr = (args.host, args.port)
    sock = udp_client.create_socket(server_addr, args.timeout)
    request_id = 1

    menu = (
        "Distributed Bank client. Commands:\n"
        "  1=Open Account  2=Close Account  3=Check Balance  4=Deposit  5=Withdraw\n"
        "  6=Transfer  7=Exchange  8=Monitor  0=Exit"
    )
    print(menu)
    while True:
        try:
            choice = input("Choice (0-8): ").strip()
        except EOFError:
            print("\nExiting.")
            break
        if choice == "0":
            print("Exiting.")
            break
        if choice == "1":
            request_id = do_open_account(sock, server_addr, request_id)
            continue
        if choice == "2":
            request_id = do_close_account(sock, server_addr, request_id)
            continue
        if choice == "3":
            request_id = do_check_balance(sock, server_addr, request_id)
            continue
        if choice == "4":
            request_id = do_deposit(sock, server_addr, request_id)
            continue
        if choice == "5":
            request_id = do_withdraw(sock, server_addr, request_id)
            continue
        if choice == "6":
            request_id = do_transfer(sock, server_addr, request_id)
            continue
        if choice == "7":
            request_id = do_exchange(sock, server_addr, request_id)
            continue
        if choice == "8":
            request_id = do_monitor(sock, server_addr, request_id, args.timeout)
            continue
        print("Unknown option. Enter 0-8.")

    sock.close()
    return 0


if __name__ == "__main__":
    sys.exit(main())
