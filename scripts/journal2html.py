#!/usr/bin/env python3

import sys
import os
import re
import subprocess
from pathlib import Path
from ansi2html import Ansi2HTMLConverter

def generate_html_stream(journal_args, output_filename):
    try:
        # inline=True embeds styles directly (e.g., style="color: #aa0000")
        # instead of relying on a fragile CSS stylesheet in the header.
        conv = Ansi2HTMLConverter(dark_bg=True, inline=True)

        env = os.environ.copy()
        env['SYSTEMD_COLORS'] = '1'
        cmd = ['journalctl'] + journal_args
        print(f"Executing: {' '.join(cmd)}")

        process = subprocess.Popen(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            env=env,
            text=True,
            errors='replace'
        )

        html_header = (
            "<!DOCTYPE html>\n<html>\n<head>\n"
            "  <meta charset=\"utf-8\">\n"
            "  <title>ZENUX Log</title>\n"
            "  <style>\n"
            "    body { background-color: #000; color: #ccc; font-family: monospace; white-space: pre-wrap; word-wrap: break-word; }\n"
            "  </style>\n"
            "</head>\n<body>\n"
        )
        html_footer = "</body>\n</html>\n"

        with open(output_filename, 'w', encoding='utf-8') as outfile:
            outfile.write(html_header)

            chunk_size = 1000
            chunk = []

            def process_chunk(lines):
                text = "".join(lines)
                # Strip OSC 8 terminal hyperlinks safely across the chunk
                clean_text = re.sub(r'\x1b\]8;.*?(?:\a|\x1b\\)', '', text)
                return conv.convert(clean_text, full=False)

            for line in process.stdout:
                chunk.append(line)
                if len(chunk) >= chunk_size:
                    outfile.write(process_chunk(chunk))
                    chunk.clear()

            # Flush any remaining lines
            if chunk:
                outfile.write(process_chunk(chunk))

            outfile.write(html_footer)

        process.wait()

        if process.returncode != 0:
            print(f"Warning: journalctl exited with code {process.returncode}")
            stderr_output = process.stderr.read()
            if stderr_output:
                print(f"journalctl stderr: {stderr_output.strip()}")

        print(f"Successfully generated '{output_filename}'")
        return True

    except Exception as e:
        print(f"An error occurred: {e}")
        return False

if __name__ == "__main__":
    args = sys.argv[1:]

    # defaults / fallback
    journal_args = []
    out_file = "zenux-log.html"
    # last: filename
    if len(args) > 0:
        out_file = str(Path(args[-1]).with_suffix('.html'))
    # leading: journalctl params
    if len(args) > 1:
        journal_args = args[:-1]

    success = generate_html_stream(journal_args, out_file)
    if success:
        sys.exit(0)
    else:
        sys.exit(-1)
