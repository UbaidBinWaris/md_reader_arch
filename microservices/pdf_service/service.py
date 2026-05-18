#!/usr/bin/env python3
import os
import sys
import json
import argparse
from http.server import HTTPServer, BaseHTTPRequestHandler
import markdown
from weasyprint import HTML, CSS

# Premium, vector print-perfect CSS designed specifically for professional document PDF generation via WeasyPrint
PRINT_CSS = """
@page {
    size: A4;
    margin: 20mm;
}

* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}

body {
    font-family: 'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;
    font-size: 11pt;
    line-height: 1.7;
    color: #111111;
    background: #ffffff !important;
    -webkit-font-smoothing: antialiased;
}

h1, h2, h3, h4, h5, h6 {
    font-weight: 700;
    line-height: 1.35;
    color: #111111;
    margin-top: 1.6em;
    margin-bottom: 0.6em;
    page-break-after: avoid;
}

h1 {
    font-size: 2.2em;
    border-bottom: 1px solid #e5e5e5;
    padding-bottom: 0.3em;
}

h2 {
    font-size: 1.6em;
    border-bottom: 1px solid #f0f0f0;
    padding-bottom: 0.2em;
}

h3 {
    font-size: 1.3em;
}

p {
    margin: 0.8em 0;
    text-align: justify;
}

a {
    color: #0d8c6d;
    text-decoration: none;
    font-weight: 500;
}

code {
    font-family: 'JetBrains Mono', 'Fira Code', 'Cascadia Code', monospace;
    font-size: 9.5pt;
    padding: 3px 6px;
    border-radius: 4px;
    background: #f5f5f5 !important;
    color: #0d8c6d !important;
    border: 1px solid #e5e5e5;
}

pre {
    margin: 1.2em 0;
    padding: 16px;
    border-radius: 6px;
    background: #f8f8f8 !important;
    border: 1px solid #e5e5e5;
    page-break-inside: avoid;
    overflow-x: auto;
}

pre code {
    padding: 0;
    background: none !important;
    border: none;
    font-size: 9pt;
    color: #111111 !important;
}

blockquote {
    margin: 1.2em 0;
    padding: 10px 20px;
    border-radius: 4px;
    border-left: 4px solid #0d8c6d;
    background: rgba(13, 140, 109, 0.05) !important;
    color: #444444;
    page-break-inside: avoid;
}

ul, ol {
    margin: 0.8em 0;
    padding-left: 2.2em;
}

li {
    margin: 0.4em 0;
}

table {
    border-collapse: collapse;
    margin: 1.5em 0;
    width: 100%;
    border: 1px solid #e5e5e5;
    page-break-inside: avoid;
}

th, td {
    padding: 10px 14px;
    text-align: left;
    border: 1px solid #e5e5e5;
}

th {
    background-color: #f5f5f5 !important;
    color: #111111 !important;
    font-weight: 600;
    font-size: 10.5pt;
}

td {
    color: #2b2b2b;
    font-size: 10pt;
}

tr:nth-child(even) {
    background-color: #fcfcfc !important;
}

hr {
    margin: 2.2em 0;
    border: none;
    height: 1px;
    background: #e5e5e5;
}

img {
    max-width: 100%;
    border-radius: 6px;
    margin: 1.2em 0;
}

strong {
    font-weight: 700;
    color: #111111;
}

del {
    opacity: 0.5;
    text-decoration: line-through;
}

/* Force pure clean printing styles */
@media print {
    body {
        background: white !important;
        color: #111111 !important;
    }
    pre, code {
        background: #f5f5f5 !important;
        color: #111111 !important;
    }
    table {
        page-break-inside: avoid;
    }
    h1, h2, h3 {
        page-break-after: avoid;
    }
}
"""


from pygments.formatters import HtmlFormatter

def convert_markdown(md_text, css_content=None):
    """Converts markdown to a styled, print-perfect HTML document using Pygments for syntax highlighting."""
    # Render body HTML using Python-Markdown extensions
    html_body = markdown.markdown(md_text, extensions=['extra', 'codehilite', 'tables', 'toc'])
    
    # Generate Pygments CSS definitions for crisp code syntax highlighting on a white background
    pygments_css = HtmlFormatter(style="friendly").get_style_defs('.codehilite')
    
    # Combine premium PRINT_CSS, Pygments code syntax, and any custom Print Options CSS overlays
    full_css = PRINT_CSS + "\n" + pygments_css
    if css_content:
        full_css += "\n" + css_content
        
    full_html = f"""<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <style>
        {full_css}
    </style>
</head>
<body class="light">
    <div class="document-content">
        {html_body}
    </div>
</body>
</html>
"""
    return full_html

def generate_pdf(md_text, output_path, css_content=None):
    """Generates a high-end, publication-quality vector PDF file using the print layout engine."""
    if md_text.strip().startswith("<html") or md_text.strip().startswith("<!DOCTYPE"):
        html_content = md_text
        if css_content:
            if "</head>" in html_content:
                html_content = html_content.replace("</head>", f"<style>{css_content}</style></head>")
            else:
                html_content = html_content + f"<style>{css_content}</style>"
    else:
        html_content = convert_markdown(md_text, css_content)
        
    # Compile directly to a crisp Vector PDF with zero rasterization triggers
    HTML(string=html_content).write_pdf(
        output_path,
        presentational_hints=True,
        optimize_images=True
    )

def generate_html(md_text, output_path, css_content=None):
    """Generates a styled, responsive HTML file from Markdown content."""
    if md_text.strip().startswith("<html") or md_text.strip().startswith("<!DOCTYPE"):
        html_content = md_text
        if css_content:
            if "</head>" in html_content:
                html_content = html_content.replace("</head>", f"<style>{css_content}</style></head>")
            else:
                html_content = html_content + f"<style>{css_content}</style>"
    else:
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
    parser.add_argument('--css', type=str, help="Path to custom CSS override file")
    
    args = parser.parse_args()
    
    if args.server:
        run_server(args.port)
    elif args.cli:
        if not args.input or not args.output:
            print("Error: CLI mode requires both --input and --output parameters.")
            sys.exit(1)
            
        try:
            css_content = None
            if args.css and os.path.exists(args.css):
                with open(args.css, 'r', encoding='utf-8') as f:
                    css_content = f.read()
                    
            with open(args.input, 'r', encoding='utf-8') as f:
                md_text = f.read()
                
            if args.type == 'pdf':
                generate_pdf(md_text, args.output, css_content)
                print(f"Successfully generated PDF: {args.output}")
            else:
                generate_html(md_text, args.output, css_content)
                print(f"Successfully generated HTML: {args.output}")
        except Exception as e:
            print(f"CLI Error: {str(e)}")
            sys.exit(2)
    else:
        # Default behavior: run server mode if no arguments are provided
        run_server(args.port)

if __name__ == '__main__':
    main()
