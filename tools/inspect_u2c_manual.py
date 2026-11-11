import argparse
import logging
from io import BytesIO
from pathlib import Path

from pypdf import PdfReader


def destinations(items):
    for item in items:
        if isinstance(item, list):
            yield from destinations(item)
        else:
            yield item


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("manual", type=Path)
    parser.add_argument("--pages", type=int, nargs="+")
    parser.add_argument("--range", type=int, nargs=2, dest="page_range")
    parser.add_argument("--layout", action="store_true")
    parser.add_argument("--attachments", action="store_true")
    parser.add_argument("--table")
    parser.add_argument("--find", nargs="+", default=["Address Space", "Flash Memory"])
    parser.add_argument("--search", nargs="+")
    args = parser.parse_args()
    logging.getLogger("pypdf").setLevel(logging.ERROR)
    reader = PdfReader(args.manual)
    if args.table:
        from openpyxl import load_workbook

        workbook = load_workbook(BytesIO(reader.attachments[args.table][0]), read_only=True, data_only=True)
        for sheet in workbook:
            print("SHEET", sheet.title)
            for row_number, row in enumerate(sheet.iter_rows(values_only=True), 1):
                cells = [f"{column}={value}" for column, value in enumerate(row, 1) if value is not None]
                text = " | ".join(cells)
                if row_number <= 8 or any(term.casefold() in text.casefold() for term in args.search or []):
                    print(row_number, text)
        return
    if args.attachments:
        print("Embedded attachments:", list(reader.attachments))
        return
    if args.page_range:
        args.pages = range(args.page_range[0], args.page_range[1] + 1)
    extraction_mode = "layout" if args.layout else "plain"
    if args.search:
        for page in args.pages or range(1, len(reader.pages) + 1):
            lines = reader.pages[page - 1].extract_text(extraction_mode=extraction_mode).splitlines()
            for index, line in enumerate(lines):
                if any(term.casefold() in line.casefold() for term in args.search):
                    print(f"PDF PAGE {page}: {' | '.join(lines[max(0, index - 2):index + 3])}")
    elif args.pages:
        for page in args.pages:
            print(f"PDF PAGE {page}")
            print(reader.pages[page - 1].extract_text(extraction_mode=extraction_mode))
    else:
        for destination in destinations(reader.outline):
            title = destination.get("/Title", "")
            if any(term.casefold() in title.casefold() for term in args.find):
                print(reader.get_destination_page_number(destination) + 1, title)


if __name__ == "__main__":
    main()