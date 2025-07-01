import os
import html

def env_vars_to_html():
    env_vars = os.environ
    html_content = """
    <html>
    <head>
        <title>Environment Variables</title>
        <style>
            table { border-collapse: collapse; width: 60%; margin: 20px auto; }
            th, td { border: 1px solid #ccc; padding: 8px; text-align: left; }
            th { background-color: #f2f2f2; }
            body { font-family: Arial, sans-serif; }
        </style>
    </head>
    <body>
        <h2 style="text-align:center;">Environment Variables</h2>
        <table>
            <tr><th>Variable</th><th>Value</th></tr>
    """

    for key, value in env_vars.items():
        safe_key = html.escape(key)
        safe_value = html.escape(value)
        html_content += f"<tr><td>{safe_key}</td><td>{safe_value}</td></tr>"

    html_content += """
        </table>
    </body>
    </html>
    """

    return html_content

# Example usage
if __name__ == "__main__":
    html_string = env_vars_to_html()
    print(html_string)
