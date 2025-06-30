#!/usr/bin/python3

import os
import sys

def main():
    # Headers HTTP necesarios para CGI
    print("Content-Type: text/html\r")
    print("\r")  # Línea vacía obligatoria entre headers y body
    
    # HTML de respuesta
    print("<!DOCTYPE html>")
    print("<html><head><title>CGI Environment Test</title></head>")
    print("<body>")
    print("<h1>Variables de entorno CGI</h1>")
    print("<table border='1' style='border-collapse: collapse;'>")
    print("<tr><th>Variable</th><th>Valor</th></tr>")
    
    # Variables CGI que implementaste
    variables_to_test = [
        "REQUEST_METHOD",
        "CONTENT_TYPE", 
        "SCRIPT_NAME",
        "QUERY_STRING",
        "CONTENT_LENGTH",
        "SERVER_NAME",
        "SERVER_PORT", 
        "DOCUMENT_ROOT",
        "HTTP_HOST",
        "GATEWAY_INTERFACE",
        "SERVER_PROTOCOL",
        "SERVER_SOFTWARE",
        "HTTP_USER_AGENT",
        "HTTP_ACCEPT",
        "HTTP_ACCEPT_LANGUAGE", 
        "HTTP_CONNECTION"
    ]
    
    # Mostrar cada variable
    for var in variables_to_test:
        value = os.environ.get(var, "NO DEFINIDA")
        print(f"<tr><td><strong>{var}</strong></td><td>{value}</td></tr>")
    
    print("</table>")
    
    # Mostrar datos POST si los hay
    if os.environ.get("REQUEST_METHOD") == "POST":
        content_length = int(os.environ.get("CONTENT_LENGTH", "0"))
        if content_length > 0:
            post_data = sys.stdin.read(content_length)
            print("<h2>Datos POST recibidos:</h2>")
            print(f"<pre>{post_data}</pre>")
    
    print("</body></html>")

if __name__ == "__main__":
    main()
