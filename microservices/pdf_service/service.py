#!/usr/bin/env python3
import os
import sys
import json
import argparse
from http.server import HTTPServer, BaseHTTPRequestHandler
import markdown
from weasyprint import HTML, CSS

# Premium, print-perfect CSS stylesheet designed specifically for weasyprint PDF generation
DEFAULT_CSS = """
@page {
    size: A4;
    margin: 20mm 15mm 20mm 15mm;
}

body {
    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
    color: #2c3e50;
    line-height: 1.6;
    font-size: 10.5pt;
}

h1, h2, h3, h4, h5, h6 {
    color: #1a252f;
    font-weight: 600;
    margin-top: 1.5em;
    margin-bottom: 0.5em;
    page-break-after: avoid;
}

h1 {
    font-size: 22pt;
    border-bottom: 1px solid #eee;
    padding-bottom: 0.3em;
}

h2 {
    font-size: 16pt;
    border-bottom: 1px solid #f8f9fa;
    padding-bottom: 0.2em;
}

h3 {
    font-size: 13pt;
}

p {
    margin-top: 0;
    margin-bottom: 1em;
    text-align: justify;
}

a {
    color: #3498db;
    text-decoration: none;
}

code {
    font-family: "SFMono-Regular", Consolas, "Liberation Mono", Menlo, Courier, monospace;
    font-size: 9pt;
    background-color: #f8f9fa;
    padding: 2px 4px;
    border-radius: 3px;
    border: 1px solid #eaeded;
}

pre {
    background-color: #f8f9fa;
    border: 1px solid #eaeded;
    border-radius: 4px;
    padding: 12px;
    margin-bottom: 1em;
    page-break-inside: avoid;
}

pre code {
    background-color: transparent;
    padding: 0;
    border: none;
    border-radius: 0;
    font-size: 8.5pt;
    display: block;
    white-space: pre-wrap;
    word-break: break-all;
}

blockquote {
    margin: 0 0 1em 0;
    padding: 8px 16px;
    border-left: 4px solid #3498db;
    background-color: #fcfcfc;
    color: #555;
    font-style: italic;
}

table {
    width: 100%;
    border-collapse: collapse;
    margin-bottom: 1em;
    page-break-inside: avoid;
}

th, td {
    border: 1px solid #e2e8f0;
    padding: 8px 12px;
    text-align: left;
}

th {
    background-color: #f8f9fa;
    font-weight: 600;
}

tr:nth-child(even) {
    background-color: #fcfcfc;
}

img {
    max-width: 100%;
    height: auto;
    display: block;
    margin: 1em auto;
    page-break-inside: avoid;
}

ul, ol {
    margin-top: 0;
    margin-bottom: 1em;
    padding-left: 20px;
}

li {
    margin-bottom: 0.25em;
}
"""

def convert_markdown(md_text, css_content=None):
    """Converts markdown to a styled HTML document."""
    # Render body HTML
    html_body = markdown.markdown(md_text, extensions=['extra', 'codehilite', 'tables', 'toc'])
    
    # Bundle into complete, styled HTML page
    style = css_content if css_content else DEFAULT_CSS
    full_html = f"""<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <style>
        {style}
    </style>
</head>
<body>
    {html_body}
</body>
</html>
"""
    return full_html

def generate_pdf(md_text, output_path, css_content=None):
    """Generates a premium PDF file from Markdown content."""
    html_content = convert_markdown(md_text, css_content)
    # Render PDF using WeasyPrint
    HTML(string=html_content).write_pdf(output_path)

def generate_html(md_text, output_path, css_content=None):
    """Generates a styled HTML file from Markdown content."""
    html_content = convert_markdown(md_text, css_content)
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(html_content)

class PDFServiceHandler(BaseHTTPRequestHandler):
    """HTTP Request Handler for the local Markdown/PDF microservice."""
    
    def log_message(self, format, *args):
        # Override to log cleanly
        sys.stderr.write(f"[PDF_SERVICE] {format % args}\n")

    def do_POST(self):
        if self.path in ('/pdf', '/html', '/generate'):
            try:
                # Read content length
                content_length = int(self.headers.get('Content-Length', 0))
                if content_length == 0:
                    self.send_error_response(400, "Empty payload")
                    return
                
                # Parse JSON
                post_data = self.rfile.read(content_length)
                try:
                    payload = json.loads(post_data.decode('utf-8'))
                except json.JSONDecodeError:
                    self.send_error_response(400, "Invalid JSON payload")
                    return
                
                # Extract fields
                md_text = payload.get('markdown', '')
                output_path = payload.get('output_path', '')
                css_content = payload.get('css', None)
                conv_type = payload.get('type', 'pdf')
                
                if not md_text or not output_path:
                    self.send_error_response(400, "Missing 'markdown' or 'output_path' parameters")
                    return
                
                # Execute conversion
                if conv_type == 'pdf' or self.path == '/pdf':
                    generate_pdf(md_text, output_path, css_content)
                else:
                    generate_html(md_text, output_path, css_content)
                
                # Return success response
                self.send_response(200)
                self.send_header('Content-Type', 'application/json')
                self.end_headers()
                response = {"success": True, "output_path": output_path}
                self.wfile.write(json.dumps(response).encode('utf-8'))
                
            except Exception as e:
                self.send_error_response(500, f"Conversion failed: {str(e)}")
        else:
            self.send_error_response(404, "Endpoint not found")

    def send_error_response(self, code, message):
        self.send_response(code)
        self.send_header('Content-Type', 'application/json')
        self.end_headers()
        response = {"success": False, "error": message}
        self.wfile.write(json.dumps(response).encode('utf-8'))

def run_server(port):
    """Starts the local HTTP microservice server."""
    server_address = ('127.0.0.1', port)
    httpd = HTTPServer(server_address, PDFServiceHandler)
    print(f"[PDF_SERVICE] Microservice running at http://127.0.0.1:{port}/")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\n[PDF_SERVICE] Shutting down microservice...")
        httpd.server_close()

def main():
    parser = argparse.ArgumentParser(description="NanoMark Premium PDF/HTML Generation Service")
    parser.add_argument('--server', action='store_true', help="Run as a local HTTP microservice")
    parser.add_argument('--port', type=int, default=5005, help="Port to run the HTTP microservice on")
    
    parser.add_argument('--cli', action='store_true', help="Run a single conversion via CLI")
    parser.add_argument('--input', type=str, help="Input Markdown file path")
    parser.add_argument('--output', type=str, help="Output destination file path")
    parser.add_argument('--type', type=str, choices=['pdf', 'html'], default='pdf', help="Target conversion format")
    
    args = parser.parse_args()
    
    if args.server:
        run_server(args.port)
    elif args.cli:
        if not args.input or not args.output:
            print("Error: CLI mode requires both --input and --output parameters.")
            sys.exit(1)
            
        try:
            with open(args.input, 'r', encoding='utf-8') as f:
                md_text = f.read()
                
            if args.type == 'pdf':
                generate_pdf(md_text, args.output)
                print(f"Successfully generated PDF: {args.output}")
            else:
                generate_html(md_text, args.output)
                print(f"Successfully generated HTML: {args.output}")
        except Exception as e:
            print(f"CLI Error: {str(e)}")
            sys.exit(2)
    else:
        # Default behavior: run server mode if no arguments are provided
        run_server(args.port)

if __name__ == '__main__':
    main()
