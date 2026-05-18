#!/usr/bin/env python3
import os
import sys
import time
import urllib.request
import urllib.error
import json
import threading

# Add parent directory to path so we can import service
sys.path.append(os.path.dirname(os.path.abspath(__file__)))
import service

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

def test_markdown_to_html_conversion():
    print("Testing Markdown to HTML conversion...")
    md_text = "# Hello World\nThis is a *test* of the HTML rendering."
    html_output = service.convert_markdown(md_text)
    
    assert "Hello World</h1>" in html_output
    assert "This is a <em>test</em> of the HTML rendering." in html_output
    assert "<style>" in html_output
    print(f"{GREEN}✓ Markdown to HTML conversion passed!{NC}")

def test_cli_pdf_and_html_generation():
    print("Testing CLI PDF & HTML generation...")
    test_md = "temp_test_cli.md"
    test_pdf = "temp_test_cli.pdf"
    test_html = "temp_test_cli.html"
    
    # Create test markdown file
    with open(test_md, "w", encoding="utf-8") as f:
        f.write("# CLI Test\nTesting PDF and HTML file compilation.")
        
    try:
        # Render HTML
        service.generate_html("# CLI Test\nTesting PDF and HTML file compilation.", test_html)
        assert os.path.exists(test_html)
        assert os.path.getsize(test_html) > 0
        print(f"{GREEN}✓ CLI HTML Generation passed!{NC}")
        
        # Render PDF
        service.generate_pdf("# CLI Test\nTesting PDF and HTML file compilation.", test_pdf)
        assert os.path.exists(test_pdf)
        assert os.path.getsize(test_pdf) > 0
        print(f"{GREEN}✓ CLI PDF Generation passed!{NC}")
        
    finally:
        # Clean up files
        for f in (test_md, test_pdf, test_html):
            if os.path.exists(f):
                os.remove(f)

def test_microservice_http_endpoint():
    print("Testing Microservice HTTP Endpoint on test port 5099...")
    
    # Start server in a background thread
    server_thread = threading.Thread(target=service.run_server, args=(5099,), daemon=True)
    server_thread.start()
    
    # Wait for server to boot up
    time.sleep(1.0)
    
    test_output_pdf = "temp_test_service.pdf"
    payload = {
        "markdown": "# Microservice Test\nTesting HTTP PDF generation.",
        "output_path": test_output_pdf,
        "type": "pdf"
    }
    
    try:
        req = urllib.request.Request(
            "http://127.0.0.1:5099/pdf",
            data=json.dumps(payload).encode('utf-8'),
            headers={"Content-Type": "application/json"},
            method="POST"
        )
        
        with urllib.request.urlopen(req) as response:
            assert response.status == 200
            res_data = json.loads(response.read().decode('utf-8'))
            assert res_data.get("success") is True
            assert os.path.exists(test_output_pdf)
            assert os.path.getsize(test_output_pdf) > 0
            
        print(f"{GREEN}✓ HTTP Microservice Endpoint PDF Generation passed!{NC}")
        
    finally:
        # Clean up output PDF
        if os.path.exists(test_output_pdf):
            os.remove(test_output_pdf)

def main():
    print("===================================================")
    print("   NanoMark Python Exporter Integration Tests      ")
    print("===================================================")
    
    try:
        test_markdown_to_html_conversion()
        test_cli_pdf_and_html_generation()
        test_microservice_http_endpoint()
        print("===================================================")
        print(f"{GREEN}   All Python Exporter Integration Tests Passed!   {NC}")
        print("===================================================")
        sys.exit(0)
    except AssertionError as e:
        print(f"{RED}Assertion failed during tests: {str(e)}{NC}")
        sys.exit(1)
    except Exception as e:
        print(f"{RED}Error during test execution: {str(e)}{NC}")
        sys.exit(2)

if __name__ == "__main__":
    main()
