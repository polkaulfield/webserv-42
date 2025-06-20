#!/usr/bin/python3

import sys

def main():
    if len(sys.argv) < 2:
        print("Usage: python script.py <page_title>", file=sys.stderr)
        sys.exit(1)

    title = sys.argv[1]

    html_content = f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>{title}</title>
</head>
<body>
    <h1>Welcome to {title}</h1>
</body>
</html>"""

    print(html_content)

if __name__ == "__main__":
    main()
