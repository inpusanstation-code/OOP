import os
import re
import ssl
import subprocess
import sys
import tkinter as tk
from concurrent.futures import ThreadPoolExecutor
from tkinter import filedialog, messagebox, ttk
from urllib.request import urlopen
from io import BytesIO
from PIL import Image, ImageTk


class TyporaLikeEditor:
    def __init__(self, root):
        self.root = root
        self.root.title("Typora-like Markdown Editor")
        self.root.geometry("1100x700")
        self.root.minsize(900, 600)
        self.root.option_add("*tearOff", False)

        self.file_path = None
        self.unsaved_changes = False
        self._preview_job = None
        self._render_id = 0
        self._last_rendered_text = None
        self.link_map = {}
        self.image_refs = []
        self.image_placeholders = {}
        self.image_executor = ThreadPoolExecutor(max_workers=3)
        self.render_executor = ThreadPoolExecutor(max_workers=1)

        self._build_ui()
        self._bind_events()
        self._update_title()

    def _build_ui(self):
        self.menu_bar = tk.Menu(self.root)
        self.file_menu = tk.Menu(self.menu_bar, tearoff=0)
        self.file_menu.add_command(label="새로 만들기(N)", accelerator="Ctrl+N", command=self.new_file)
        self.file_menu.add_command(label="열기(O)...", accelerator="Ctrl+O", command=self.open_file)
        self.file_menu.add_command(label="저장(S)", accelerator="Ctrl+S", command=self.save_file)
        self.file_menu.add_command(label="다른 이름으로 저장", command=self.save_as_file)
        self.file_menu.add_separator()
        self.file_menu.add_command(label="종료(X)", command=self.on_close)
        self.menu_bar.add_cascade(label="파일(F)", menu=self.file_menu)
        self.root.config(menu=self.menu_bar)

        self.toolbar = ttk.Frame(self.root, padding=(8, 6))
        self.toolbar.pack(fill=tk.X)

        buttons = [
            ("새 파일", self.new_file),
            ("열기", self.open_file),
            ("저장", self.save_file),
            ("굵게", lambda: self.wrap_selection("**", "**")),
            ("기울임", lambda: self.wrap_selection("*", "*")),
            ("제목", lambda: self.insert_prefix("# ")),
            ("목록", lambda: self.insert_prefix("- ")),
            ("코드", lambda: self.wrap_selection("`", "`")),
            ("인용", lambda: self.insert_prefix("> "))
        ]
        for text, command in buttons:
            ttk.Button(self.toolbar, text=text, command=command).pack(side=tk.LEFT, padx=3)

        self.main_pane = tk.PanedWindow(self.root, orient=tk.HORIZONTAL, sashrelief=tk.RAISED)
        self.main_pane.pack(expand=True, fill=tk.BOTH, padx=8, pady=(0, 8))

        editor_frame = ttk.Frame(self.main_pane)
        ttk.Label(editor_frame, text="편집기", font=("맑은 고딕", 10, "bold")).pack(anchor=tk.W, padx=6, pady=(6, 2))
        self.editor = tk.Text(editor_frame, undo=True, wrap="word", font=("맑은 고딕", 11), padx=8, pady=8)
        self.editor.pack(expand=True, fill=tk.BOTH)
        self.main_pane.add(editor_frame, width=500)

        preview_frame = ttk.Frame(self.main_pane)
        ttk.Label(preview_frame, text="미리보기", font=("맑은 고딕", 10, "bold")).pack(anchor=tk.W, padx=6, pady=(6, 2))
        self.preview = tk.Text(preview_frame, wrap="word", state="disabled", font=("맑은 고딕", 11), padx=8, pady=8, bg="#f8f8f8")
        self.preview.pack(expand=True, fill=tk.BOTH)
        self.main_pane.add(preview_frame, width=500)

        self.preview.tag_configure("h1", font=("맑은 고딕", 24, "bold"), spacing3=8, foreground="#1f1f1f")
        self.preview.tag_configure("h2", font=("맑은 고딕", 20, "bold"), spacing3=6, foreground="#2c3e50")
        self.preview.tag_configure("h3", font=("맑은 고딕", 16, "bold"), spacing3=4, foreground="#34495e")
        self.preview.tag_configure("h4", font=("맑은 고딕", 14, "bold"), spacing3=3, foreground="#4a5568")
        self.preview.tag_configure("h5", font=("맑은 고딕", 12, "bold"), spacing3=2, foreground="#4a5568")
        self.preview.tag_configure("h6", font=("맑은 고딕", 10, "bold"), spacing3=2, foreground="#718096")
        self.preview.tag_configure("bold", font=("맑은 고딕", 11, "bold"))
        self.preview.tag_configure("italic", font=("맑은 고딕", 11, "italic"))
        self.preview.tag_configure("bold_italic", font=("맑은 고딕", 11, "bold", "italic"))
        self.preview.tag_configure("strike", overstrike=True)
        self.preview.tag_configure("underline", underline=True)
        self.preview.tag_configure("code", background="#f0f0f0", foreground="#c7254e", font=("Consolas", 10))
        self.preview.tag_configure("quote", foreground="#6c757d", lmargin1=20, lmargin2=20)
        self.preview.tag_configure("list", foreground="#2c3e50")
        self.preview.tag_configure("check_done", foreground="#2ecc71", font=("맑은 고딕", 11, "bold"))
        self.preview.tag_configure("check_todo", foreground="#e74c3c", font=("맑은 고딕", 11, "bold"))
        self.preview.tag_configure("link", foreground="#1e88e5", underline=True)
        self.preview.tag_configure("image", spacing3=8)
        self.preview.tag_configure("table", font=("Consolas", 11), background="#f8fafc")
        self.preview.tag_configure("table_header", font=("Consolas", 11, "bold"), background="#e2e8f0")
        self.preview.tag_configure("table_separator", font=("Consolas", 11), foreground="#94a3b8")
        self.preview.tag_configure("table_border", foreground="#94a3b8")

        self.status_var = tk.StringVar(value="준비됨")
        ttk.Label(self.root, textvariable=self.status_var, anchor=tk.W).pack(fill=tk.X, padx=8, pady=(0, 6))

    def _bind_events(self):
        self.editor.bind("<<Modified>>", self.on_editor_modified)
        self.editor.bind("<Control-s>", lambda event: self.save_file())
        self.editor.bind("<Control-o>", lambda event: self.open_file())
        self.editor.bind("<Control-n>", lambda event: self.new_file())
        self.preview.bind("<Button-1>", self.on_preview_click)
        self.preview.tag_bind("link", "<Enter>", lambda event: self.preview.config(cursor="hand2"))
        self.preview.tag_bind("link", "<Leave>", lambda event: self.preview.config(cursor=""))
        self.root.protocol("WM_DELETE_WINDOW", self.on_close)

    def on_editor_modified(self, event=None):
        self.unsaved_changes = True
        self._update_title()
        self.status_var.set("편집 중")
        if self._preview_job is not None:
            self.root.after_cancel(self._preview_job)
        self._preview_job = self.root.after(300, self._request_render)
        self.editor.edit_modified(False)

    def _request_render(self):
        self._preview_job = None
        content = self.editor.get("1.0", tk.END)
        if content == self._last_rendered_text:
            return
        self._render_id += 1
        render_id = self._render_id
        self._last_rendered_text = content
        self.render_executor.submit(self._parse_markdown, content, render_id)

    def _parse_markdown(self, content, render_id):
        ops = []
        link_map = {}
        image_placeholders = {}

        lines = content.splitlines()
        index = 0
        while index < len(lines):
            line = lines[index]
            if not line.strip():
                ops.append({"type": "text", "text": "\n"})
                index += 1
                continue

            next_line = lines[index + 1] if index + 1 < len(lines) else ""
            if self._is_table_row(line) and self._is_table_divider(next_line):
                self._parse_table_block(ops, lines, index)
                index = self._advance_table_block(lines, index)
                continue

            heading = re.match(r"^(#{1,6})\s*(.*)", line)
            if heading:
                level = len(heading.group(1))
                heading_text = heading.group(2).strip()
                if heading_text:
                    self._parse_formatted_line(ops, heading_text, tag=f"h{level}", render_id=render_id, link_map=link_map, image_placeholders=image_placeholders)
                ops.append({"type": "text", "text": "\n"})
                index += 1
                continue

            quote = re.match(r"^>\s*(.*)", line)
            if quote:
                self._parse_formatted_line(ops, f"▸ {quote.group(1)}", tag="quote", render_id=render_id, link_map=link_map, image_placeholders=image_placeholders)
                ops.append({"type": "text", "text": "\n"})
                index += 1
                continue

            checklist = re.match(r"^[-*]\s+\[(x|X| )\]\s*(.*)", line)
            if checklist:
                state = checklist.group(1).lower()
                text = checklist.group(2)
                if state == "x":
                    ops.append({"type": "text", "text": "✓ ", "tags": ["check_done"]})
                else:
                    ops.append({"type": "text", "text": "☐ ", "tags": ["check_todo"]})
                self._parse_formatted_line(ops, text, tag="list", render_id=render_id, link_map=link_map, image_placeholders=image_placeholders)
                ops.append({"type": "text", "text": "\n"})
                index += 1
                continue

            bullet = re.match(r"^[-*]\s+(.*)", line)
            if bullet:
                self._parse_formatted_line(ops, f"• {bullet.group(1)}", tag="list", render_id=render_id, link_map=link_map, image_placeholders=image_placeholders)
                ops.append({"type": "text", "text": "\n"})
                index += 1
                continue

            self._parse_formatted_line(ops, line, render_id=render_id, link_map=link_map, image_placeholders=image_placeholders)
            ops.append({"type": "text", "text": "\n"})
            index += 1

        self.root.after(0, lambda: self._render_preview(ops, render_id, link_map, image_placeholders))

    def _parse_formatted_line(self, ops, text, tag=None, render_id=None, link_map=None, image_placeholders=None):
        base_tags = [tag] if tag else []
        if render_id is None:
            render_id = self._render_id
        if link_map is None:
            link_map = {}
        if image_placeholders is None:
            image_placeholders = {}
        if not text:
            ops.append({"type": "text", "text": "", "tags": base_tags})
            return

        pattern = re.compile(r"(`[^`]+`)|(__[^_]+__)|(\*\*[^*]+\*\*)|(\*[^*]+\*)|(~~[^~]+~~)|(<u>[^<]+</u>)|(\[[^\]]+\]\((https?://[^)]+)\))|(\!\[[^\]]*\]\((https?://[^)]+)\))")
        last_index = 0

        for match in pattern.finditer(text):
            if match.start() > last_index:
                ops.append({"type": "text", "text": text[last_index:match.start()], "tags": base_tags})

            token = match.group(0)
            if token.startswith("`") and token.endswith("`"):
                content = token[1:-1]
                ops.append({"type": "text", "text": content, "tags": base_tags + ["code"]})
            elif token.startswith("~~") and token.endswith("~~"):
                content = token[2:-2]
                ops.append({"type": "text", "text": content, "tags": base_tags + ["strike"]})
            elif token.startswith("<u>") and token.endswith("</u>"):
                content = token[3:-4]
                ops.append({"type": "text", "text": content, "tags": base_tags + ["underline"]})
            elif token.startswith("!") and "[" in token and "](" in token:
                alt_text, url = re.match(r"!\[(.*?)\]\((https?://[^)]+)\)", token).groups()
                placeholder_tag = f"img_placeholder_{render_id}_{len(image_placeholders)}"
                placeholder = f"[이미지 로딩: {alt_text}]"
                ops.append({"type": "image_placeholder", "text": placeholder, "tags": base_tags + ["image", placeholder_tag], "placeholder_tag": placeholder_tag})
                image_placeholders[placeholder_tag] = (url, alt_text, render_id)
                ops.append({"type": "text", "text": "\n\n"})
            elif token.startswith("[") and "](" in token:
                label, url = re.match(r"\[(.*?)\]\((https?://[^)]+)\)", token).groups()
                ops.append({"type": "link", "text": label, "tags": base_tags + ["link"], "url": url})
            elif token.startswith("**") and token.endswith("**"):
                content = token[2:-2]
                tags = base_tags + (["bold_italic"] if "*" in content and not content.startswith("*") else ["bold"])
                ops.append({"type": "text", "text": content, "tags": tags})
            else:
                content = token[1:-1]
                tags = base_tags + (["bold_italic"] if "**" in token else ["italic"])
                ops.append({"type": "text", "text": content, "tags": tags})
            last_index = match.end()

        if last_index < len(text):
            ops.append({"type": "text", "text": text[last_index:], "tags": base_tags})
        elif last_index == 0:
            ops.append({"type": "text", "text": text, "tags": base_tags})

    def _render_preview(self, ops, render_id, link_map, image_placeholders):
        if render_id != self._render_id:
            return
        self.preview.configure(state="normal")
        self.preview.delete("1.0", tk.END)
        self.link_map = {}
        self.image_refs = []
        self.image_placeholders = {}

        for op in ops:
            if op["type"] == "text":
                self.preview.insert(tk.END, op["text"], tuple(op.get("tags", [])))
            elif op["type"] == "link":
                start_index = self.preview.index(tk.INSERT)
                self.preview.insert(tk.END, op["text"], tuple(op.get("tags", [])))
                end_index = self.preview.index(tk.INSERT)
                self.link_map[(start_index, end_index)] = op["url"]
            elif op["type"] == "image_placeholder":
                self.preview.insert(tk.END, op["text"], tuple(op.get("tags", [])))
                self.image_placeholders[op["placeholder_tag"]] = image_placeholders[op["placeholder_tag"]]
            else:
                self.preview.insert(tk.END, op.get("text", ""), tuple(op.get("tags", [])))

        self.preview.configure(state="disabled")

        for placeholder_tag, (url, alt_text, placeholder_render_id) in self.image_placeholders.items():
            self.image_executor.submit(self._download_and_replace_image, url, alt_text, placeholder_tag, placeholder_render_id)

    def _parse_table_block(self, ops, lines, start_index):
        table_rows = []
        index = start_index
        while index < len(lines) and lines[index].strip().startswith("|"):
            table_rows.append(lines[index])
            index += 1

        if len(table_rows) < 2:
            ops.append({"type": "text", "text": table_rows[0] + "\n"})
            return

        parsed = [self._split_table_row(row) for row in table_rows]
        max_columns = max(len(row) for row in parsed)
        widths = [0] * max_columns
        for row in parsed:
            for idx in range(max_columns):
                cell = row[idx] if idx < len(row) else ""
                widths[idx] = max(widths[idx], len(cell))

        self._append_table_row_ops(ops, parsed[0], widths, header=True)
        self._append_table_separator_ops(ops, widths)
        for row in parsed[2:]:
            self._append_table_row_ops(ops, row, widths)
        ops.append({"type": "text", "text": "\n"})

    def _advance_table_block(self, lines, start_index):
        index = start_index
        while index < len(lines) and lines[index].strip().startswith("|"):
            index += 1
        return index

    def _append_table_row_ops(self, ops, row, widths, header=False):
        tags = ["table_header"] if header else ["table"]
        line_text = ""
        for idx, width in enumerate(widths):
            cell = row[idx] if idx < len(row) else ""
            padded = cell.ljust(width)
            line_text += f"| {padded} "
        line_text += "|\n"
        ops.append({"type": "text", "text": line_text, "tags": tags})

    def _append_table_separator_ops(self, ops, widths):
        line_text = ""
        for width in widths:
            line_text += f"| {'-' * width} "
        line_text += "|\n"
        ops.append({"type": "text", "text": line_text, "tags": ["table_separator"]})

    def _insert_formatted_line(self, text, tag=None, render_id=None):
        base_tags = [tag] if tag else []
        if render_id is None:
            render_id = self._render_id
        if not text:
            self.preview.insert(tk.END, "", tuple(base_tags))
            return

        pattern = re.compile(r"(`[^`]+`)|(__[^_]+__)|(\*\*[^*]+\*\*)|(\*[^*]+\*)|(~~[^~]+~~)|(<u>[^<]+</u>)|(\[[^\]]+\]\((https?://[^)]+)\))|(\!\[[^\]]*\]\((https?://[^)]+)\))")
        last_index = 0

        for match in pattern.finditer(text):
            if match.start() > last_index:
                self.preview.insert(tk.END, text[last_index:match.start()], tuple(base_tags))

            token = match.group(0)
            if token.startswith("`") and token.endswith("`"):
                content = token[1:-1]
                tags = base_tags + ["code"]
                self.preview.insert(tk.END, content, tuple(tags))
            elif token.startswith("~~") and token.endswith("~~"):
                content = token[2:-2]
                tags = base_tags + ["strike"]
                self.preview.insert(tk.END, content, tuple(tags))
            elif token.startswith("<u>") and token.endswith("</u>"):
                content = token[3:-4]
                tags = base_tags + ["underline"]
                self.preview.insert(tk.END, content, tuple(tags))
            elif token.startswith("!") and "[" in token and "](" in token:
                alt_text, url = re.match(r"!\[(.*?)\]\((https?://[^)]+)\)", token).groups()
                placeholder_tag = f"img_placeholder_{render_id}_{len(self.image_placeholders)}"
                placeholder = f"[이미지 로딩: {alt_text}]"
                self.preview.insert(tk.END, placeholder, tuple(base_tags + ["image", placeholder_tag]))
                self.image_placeholders[placeholder_tag] = (url, alt_text, render_id)
                self.preview.insert(tk.END, "\n\n")
                self.image_executor.submit(self._download_and_replace_image, url, alt_text, placeholder_tag, render_id)
            elif token.startswith("[") and "](" in token:
                label, url = re.match(r"\[(.*?)\]\((https?://[^)]+)\)", token).groups()
                tags = base_tags + ["link"]
                start_index = self.preview.index(tk.INSERT)
                self.preview.insert(tk.END, label, tuple(tags))
                end_index = self.preview.index(tk.INSERT)
                self.link_map[(start_index, end_index)] = url
            elif token.startswith("**") and token.endswith("**"):
                content = token[2:-2]
                if "*" in content and not content.startswith("*"):
                    tags = base_tags + ["bold_italic"]
                else:
                    tags = base_tags + ["bold"]
                self.preview.insert(tk.END, content, tuple(tags))
            else:
                content = token[1:-1]
                if "**" in token:
                    tags = base_tags + ["bold_italic"]
                else:
                    tags = base_tags + ["italic"]
                self.preview.insert(tk.END, content, tuple(tags))
            last_index = match.end()

        if last_index < len(text):
            self.preview.insert(tk.END, text[last_index:], tuple(base_tags))
        elif last_index == 0:
            self.preview.insert(tk.END, text, tuple(base_tags))

    def _download_and_replace_image(self, url, alt_text, placeholder_tag, render_id):
        try:
            ssl_context = ssl._create_unverified_context()
            with urlopen(url, context=ssl_context, timeout=10) as response:
                image_bytes = response.read()
            image = Image.open(BytesIO(image_bytes))
            image.thumbnail((480, 260))
            photo = ImageTk.PhotoImage(image)
            self.root.after(0, lambda: self._replace_placeholder_image(placeholder_tag, photo, render_id))
        except Exception:
            self.root.after(0, lambda: self._replace_placeholder_text(placeholder_tag, alt_text, render_id))

    def _replace_placeholder_image(self, placeholder_tag, photo, render_id):
        if render_id != self._render_id:
            return
        try:
            self.preview.configure(state="normal")
            self.image_refs.append(photo)
            ranges = self.preview.tag_ranges(placeholder_tag)
            if not ranges:
                self.preview.configure(state="disabled")
                return
            self.preview.delete(ranges[0], ranges[1])
            self.preview.image_create(ranges[0], image=photo)
            self.preview.insert(ranges[0], "\n\n")
            self.preview.configure(state="disabled")
        except Exception:
            pass

    def _replace_placeholder_text(self, placeholder_tag, alt_text, render_id):
        if render_id != self._render_id:
            return
        try:
            self.preview.configure(state="normal")
            ranges = self.preview.tag_ranges(placeholder_tag)
            if ranges:
                self.preview.delete(ranges[0], ranges[1])
                self.preview.insert(ranges[0], f"[이미지: {alt_text}]")
            self.preview.configure(state="disabled")
        except Exception:
            pass

    def _is_table_divider(self, line):
        return bool(re.match(r"^\s*\|\s*[:\-]+\s*(\|\s*[:\-]+\s*)*\|\s*$", line))

    def _render_table_block(self, lines, start_index):
        table_rows = []
        index = start_index
        while index < len(lines) and lines[index].strip().startswith("|"):
            table_rows.append(lines[index])
            index += 1

        if len(table_rows) < 2:
            self.preview.insert(tk.END, "\n")
            return index

        parsed = [self._split_table_row(row) for row in table_rows]
        max_columns = max(len(row) for row in parsed)
        widths = [0] * max_columns
        for row in parsed:
            for idx in range(max_columns):
                cell = row[idx] if idx < len(row) else ""
                widths[idx] = max(widths[idx], len(cell))

        self._insert_table_row(parsed[0], widths, header=True)
        self._insert_table_separator(widths)
        for row in parsed[2:]:
            self._insert_table_row(row, widths)

        self.preview.insert(tk.END, "\n")
        return index

    def _split_table_row(self, row):
        cells = [cell.strip() for cell in row.strip().strip("|").split("|")]
        return cells

    def _insert_table_row(self, row, widths, header=False):
        for idx, width in enumerate(widths):
            cell = row[idx] if idx < len(row) else ""
            padded = cell.ljust(width)
            tag = "table_header" if header else "table"
            self.preview.insert(tk.END, f"| {padded} ", tuple([tag]))
        self.preview.insert(tk.END, "|\n")

    def _insert_table_separator(self, widths):
        for width in widths:
            self.preview.insert(tk.END, f"| {'-' * width} ", ("table_separator",))
        self.preview.insert(tk.END, "|\n")

    def _is_table_row(self, line):
        return bool(re.match(r"^\s*\|.*\|\s*$", line))

    def _is_table_divider(self, line):
        return bool(re.match(r"^\s*\|\s*[:\-]+\s*(\|\s*[:\-]+\s*)*\|\s*$", line))

    def wrap_selection(self, prefix, suffix=None):
        if suffix is None:
            suffix = prefix
        try:
            start = self.editor.index("sel.first")
            end = self.editor.index("sel.last")
            selected = self.editor.get(start, end)
            self.editor.delete(start, end)
            self.editor.insert(start, f"{prefix}{selected}{suffix}")
        except tk.TclError:
            self.editor.insert(tk.INSERT, f"{prefix}{suffix}")

    def insert_prefix(self, prefix):
        try:
            start = self.editor.index("sel.first")
            end = self.editor.index("sel.last")
            selected = self.editor.get(start, end)
            self.editor.delete(start, end)
            self.editor.insert(start, f"{prefix}{selected}")
        except tk.TclError:
            self.editor.insert(tk.INSERT, prefix)

    def new_file(self):
        if self.unsaved_changes:
            answer = messagebox.askyesnocancel("저장", "변경 내용을 저장하시겠습니까?")
            if answer is None:
                return
            if answer and not self.save_file():
                return
        self.editor.delete("1.0", tk.END)
        self.file_path = None
        self.unsaved_changes = False
        self._update_title()
        self.status_var.set("새 문서")
        self.render_preview()

    def open_file(self):
        if self.unsaved_changes:
            answer = messagebox.askyesnocancel("저장", "변경 내용을 저장하시겠습니까?")
            if answer is None:
                return
            if answer and not self.save_file():
                return

        file_path = filedialog.askopenfilename(
            title="마크다운 파일 열기",
            defaultextension=".md",
            filetypes=[("마크다운 파일 (*.md)", "*.md"), ("텍스트 파일 (*.txt)", "*.txt"), ("모든 파일", "*.*")],
        )
        if not file_path:
            return

        try:
            with open(file_path, "r", encoding="utf-8") as handle:
                content = handle.read()
            self.editor.delete("1.0", tk.END)
            self.editor.insert("1.0", content)
            self.file_path = file_path
            self.unsaved_changes = False
            self._update_title()
            self.status_var.set(os.path.basename(file_path))
            self.render_preview()
        except Exception as exc:
            messagebox.showerror("오류", f"파일을 열 수 없습니다: {exc}")

    def save_file(self):
        if self.file_path is None:
            return self.save_as_file()

        try:
            with open(self.file_path, "w", encoding="utf-8") as handle:
                handle.write(self.editor.get("1.0", tk.END).rstrip("\n"))
            self.unsaved_changes = False
            self._update_title()
            self.status_var.set(os.path.basename(self.file_path))
            messagebox.showinfo("저장 완료", "파일이 저장되었습니다.")
            return True
        except Exception as exc:
            messagebox.showerror("오류", f"파일을 저장하는 중 문제가 발생했습니다: {exc}")
            return False

    def save_as_file(self):
        file_path = filedialog.asksaveasfilename(
            initialfile="제목 없음.md",
            defaultextension=".md",
            filetypes=[("마크다운 파일 (*.md)", "*.md"), ("텍스트 파일 (*.txt)", "*.txt"), ("모든 파일", "*.*")],
        )
        if not file_path:
            return False

        self.file_path = file_path
        return self.save_file()

    def _update_title(self):
        base_name = os.path.basename(self.file_path) if self.file_path else "제목 없음"
        suffix = "*" if self.unsaved_changes else ""
        self.root.title(f"{base_name}{suffix} - Typora-like Editor")

    def render_preview(self):
        if self._preview_job is not None:
            self.root.after_cancel(self._preview_job)
        self._preview_job = self.root.after(1, self._request_render)

    def on_preview_click(self, event):
        index = self.preview.index(f"@{event.x},{event.y}")
        for (start, end), url in self.link_map.items():
            if self.preview.compare(index, ">=", start) and self.preview.compare(index, "<=", end):
                self.open_url(url)
                return

    def open_url(self, url):
        try:
            if sys.platform.startswith("win"):
                os.startfile(url)
            elif sys.platform.startswith("darwin"):
                subprocess.Popen(["open", url])
            else:
                subprocess.Popen(["xdg-open", url])
        except Exception as exc:
            messagebox.showerror("오류", f"링크를 열 수 없습니다: {exc}")

    def on_close(self):
        if self.unsaved_changes:
            answer = messagebox.askyesnocancel("저장", "변경 내용을 저장하시겠습니까?")
            if answer is None:
                return
            if answer and not self.save_file():
                return
        self.root.destroy()


if __name__ == "__main__":
    root = tk.Tk()
    app = TyporaLikeEditor(root)
    root.mainloop()