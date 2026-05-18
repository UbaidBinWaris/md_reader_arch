import urllib.request
import os
import glob

# Mappings of icons to their Lucide source names if different
icon_mappings = {
    "file": "file",
    "folder-open": "folder-open",
    "save": "save",
    "printer": "printer",
    "undo": "undo",
    "redo": "redo",
    "scissors": "scissors",
    "copy": "copy",
    "clipboard": "clipboard",
    "search": "search",
    "book-open": "book-open",
    "palette": "palette",
    "sidebar": "panel-left",  # Lucide modern name
    "eye": "eye",
    "help-circle": "circle-help",  # Lucide modern name
    "bold": "bold",
    "italic": "italic",
    "strikethrough": "strikethrough",
    "code": "code",
    "link": "link",
    "image": "image",
    "table": "table",
    "terminal": "terminal",
    "quote": "quote",
    "minus": "minus",
    "chevron-down": "chevron-down",
    "chevron-right": "chevron-right",
    "heading-1": "heading-1",
    "heading-2": "heading-2",
    "heading-3": "heading-3",
    "list": "list",
    "list-ordered": "list-ordered",
    "check-square": "square-check"  # Lucide modern name
}

script_dir = os.path.dirname(os.path.abspath(__file__))
target_dir = os.path.abspath(os.path.join(script_dir, "../resources/icons"))

# Step 1: Clean/Delete all previous SVGs in target folder
print("Cleaning up previous icons...")
if os.path.exists(target_dir):
    files = glob.glob(os.path.join(target_dir, "*.svg"))
    for f in files:
        try:
            os.remove(f)
            print(f"Removed {os.path.basename(f)}")
        except Exception as e:
            print(f"Failed to remove {f}: {e}")
else:
    os.makedirs(target_dir, exist_ok=True)

# Step 2: Download each icon from updated repository
print("\nStarting fresh download of Lucide SVG icons...")
for local_name, remote_name in icon_mappings.items():
    url = f"https://raw.githubusercontent.com/lucide-icons/lucide/main/icons/{remote_name}.svg"
    dest = os.path.join(target_dir, f"{local_name}.svg")
    try:
        urllib.request.urlretrieve(url, dest)
        print(f"Successfully downloaded {local_name}.svg (from {remote_name})")
    # Fallback if circle-help or square-check fails (e.g. older versions)
    except Exception as e:
        if local_name == "help-circle":
            candidates = ["circle-help", "help-circle", "circle-question-mark", "circle-question", "help"]
            success = False
            for cand in candidates:
                url_cand = f"https://raw.githubusercontent.com/lucide-icons/lucide/main/icons/{cand}.svg"
                try:
                    urllib.request.urlretrieve(url_cand, dest)
                    print(f"Successfully downloaded help-circle.svg (using candidate {cand})")
                    success = True
                    break
                except Exception:
                    pass
            if success:
                continue
        print(f"Failed to download {local_name}.svg: {e}")

print("\nAll icons cleaned and fresh downloads complete!")
