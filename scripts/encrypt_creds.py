#!/usr/bin/env python3
"""Encrypt MQTT creds with a shared password. Writes web/creds.enc.js.

Usage: python3 scripts/encrypt_creds.py
Prompts for the shared PWA password (NOT the MQTT password).
"""
import base64
import getpass
import json
import os
import sys
from pathlib import Path

from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.ciphers.aead import AESGCM
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC

HOST = "74df50b95e834841bd5bdb512e6ebec0.s1.eu.hivemq.cloud"
USERNAME = "messageboxuser"
MQTT_PASSWORD = "sufpa0-mycfer-wasHet"

ITERATIONS = 200_000


def main() -> int:
    pw = getpass.getpass("Shared PWA password: ")
    if len(pw) < 8:
        print("Password must be at least 8 characters.", file=sys.stderr)
        return 1
    confirm = getpass.getpass("Confirm password: ")
    if pw != confirm:
        print("Passwords do not match.", file=sys.stderr)
        return 1

    salt = os.urandom(16)
    iv = os.urandom(12)

    kdf = PBKDF2HMAC(algorithm=hashes.SHA256(), length=32, salt=salt, iterations=ITERATIONS)
    key = kdf.derive(pw.encode("utf-8"))

    plaintext = json.dumps({"host": HOST, "username": USERNAME, "password": MQTT_PASSWORD}).encode("utf-8")
    ct = AESGCM(key).encrypt(iv, plaintext, associated_data=None)

    b64 = lambda b: base64.b64encode(b).decode("ascii")
    out = (
        "export const ENC_CREDS = {\n"
        f"  salt: {json.dumps(b64(salt))},\n"
        f"  iv:   {json.dumps(b64(iv))},\n"
        f"  ct:   {json.dumps(b64(ct))},\n"
        "};\n"
    )

    out_path = Path(__file__).resolve().parent.parent / "web" / "creds.enc.js"
    out_path.write_text(out)
    print(f"Wrote {out_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
