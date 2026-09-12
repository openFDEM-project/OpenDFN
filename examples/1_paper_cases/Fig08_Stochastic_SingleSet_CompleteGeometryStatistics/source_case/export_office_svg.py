from pathlib import Path
import re

ROOT = Path(__file__).resolve().parent
SRC = ROOT / "svg"
OUT = ROOT / "office_compatible_svg"
OUT.mkdir(exist_ok=True)

def fix(path: Path):
    text = path.read_text(encoding="utf-8")
    def repl(match):
        tag = match.group(0)
        style = re.search(r"style=\"([^\"]*)\"", tag)
        if not style:
            return tag
        css = style.group(1)
        fm = re.search(r"font-family:\s*'Times New Roman'", css)
        fs = re.search(r"font-size:\s*([0-9.]+)px", css)
        if not fm or not fs:
            return tag
        if "font-family=" not in tag:
            tag = tag.replace("<text", '<text font-family="Times New Roman"', 1)
        if "font-size=" not in tag:
            tag = tag.replace("<text", f'<text font-size="{fs.group(1)}px"', 1)
        return tag
    text = re.sub(r"<text\b[^>]*>", repl, text)
    (OUT / path.name).write_text(text, encoding="utf-8", newline="\n")

for svg in sorted(SRC.glob("*.svg")):
    fix(svg)
print(f"exported {len(list(OUT.glob('*.svg')))} SVG files")
