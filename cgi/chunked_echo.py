#!/usr/bin/env python3
import sys
import os

print("Content-Type: text/plain\r\n")

# Leer el body completo (funciona igual para chunked si el servidor lo maneja)
body = sys.stdin.read()
print("Body recibido:")
print(body)
