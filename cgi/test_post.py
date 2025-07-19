#!/usr/bin/env python3

import os
import sys
import urllib.parse

def main():
    print("Content-Type: text/html")
    print()

    method = os.environ.get("REQUEST_METHOD", "")
    content_type = os.environ.get("CONTENT_TYPE", "")
    content_length = int(os.environ.get("CONTENT_LENGTH", 0))
    body = sys.stdin.read(content_length)
    parsed = urllib.parse.parse_qs(body)

    print("""
    <!DOCTYPE html>
    <html>
    <head>
        <title>CGI Pretty POST Demo</title>
        <style>
            body {{ font-family: Arial, sans-serif; background: #f0f0f0; }}
            .container {{ background: #fff; padding: 2em; margin: 2em auto; border-radius: 8px; max-width: 500px; box-shadow: 0 0 10px #ccc; }}
            h1 {{ color: #4a90e2; }}
            table {{ width: 100%; border-collapse: collapse; margin-top: 1em; }}
            th, td {{ border: 1px solid #ddd; padding: 8px; }}
            th {{ background: #4a90e2; color: #fff; }}
            tr:nth-child(even) {{ background: #f9f9f9; }}
            .raw {{ background: #eee; padding: 1em; border-radius: 4px; }}
        </style>
    </head>
    <body>
    <div class="container">
        <h1>CGI Pretty POST Demo</h1>
        <h2>Request Info</h2>
        <ul>
            <li><strong>Method:</strong> {}</li>
            <li><strong>Content-Type:</strong> {}</li>
            <li><strong>Content-Length:</strong> {}</li>
        </ul>
        <h2>Raw POST Data</h2>
        <div class="raw">{}</div>
    """.format(method, content_type, content_length, body if body else "<em>No data received</em>"))

    if parsed:
        print("""
        <h2>Parsed Form Data</h2>
        <table>
            <tr><th>Field</th><th>Value</th></tr>
        """)
        for key, values in parsed.items():
            for value in values:
                print(f"<tr><td>{key}</td><td>{value}</td></tr>")
        print("</table>")
    else:
        print("<h2>No form data to parse</h2>")

    print("""
        <h2>Try Again</h2>
        <form method="POST" action="/cgi/test_post.py">
            <label>Name: <input type="text" name="name"></label><br>
            <label>Message: <input type="text" name="msg"></label><br>
            <label>Favorite Color: <input type="color" name="color"></label><br>
            <label>Choose a pet:
                <select name="pet">
                    <option value="dog">Dog</option>
                    <option value="cat">Cat</option>
                    <option value="bird">Bird</option>
                </select>
            </label><br>
            <input type="submit" value="Send POST">
        </form>
    </div>
    </body>
    </html>
    """)

if __name__ == "__main__":
    main()
