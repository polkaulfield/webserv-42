# /home/arnau/42/webServer/cgi/test_directory.py
#!/usr/bin/env python3

import os

print("Content-Type: text/plain")
print()
print(f"Current working directory: {os.getcwd()}")
print("Files in current directory:")
for file in os.listdir("."):
    print(f"  - {file}")