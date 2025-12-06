import tkinter as tk
from tkinter import ttk, messagebox, scrolledtext, simpledialog
from pathlib import Path
from datetime import datetime

from file_operations import FileOperations

# Handle all GUI components and user interactions
class FileManagerGUI:

    def __init__(self, root):
        self.root = root
        self.root.title("OwlTech File Manager - CS 3502 Project 3")
        self.root.geometry("1000x700")
        self.root.minsize(800, 600)

        # File operations handler
        self.file_ops = FileOperations()

        # Current directory
        self.current_path = Path.home() / "FileManagerDemo"
        self.create_demo_directory()

        # Setup UI
        self.setup_ui()
        self.refresh_file_list()

    # Create a demo directory for safe testing
    def create_demo_directory(self):
        try:
            if not self.current_path.exists():
                self.file_ops.create_directory(str(self.current_path))
        except Exception as e:
            messagebox.showerror("Error", f"Failed to create demo directory: {str(e)}")

    # UI setup methods
    def setup_ui(self):

        # Setup all GUI components
        self.setup_menu()
        self.setup_navigation()
        self.setup_file_list()
        self.setup_action_panel()
        self.setup_status_bar()

    # Setup menu bar
    def setup_menu(self):
        menubar = tk.Menu(self.root)
        self.root.config(menu=menubar)

        file_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="File", menu=file_menu)
        file_menu.add_command(label="New File", command=self.create_file)
        file_menu.add_command(label="New Folder", command=self.create_directory)
        file_menu.add_separator()
        file_menu.add_command(label="Exit", command=self.root.quit)

    # Setup navigation bar
    def setup_navigation(self):
        top_frame = ttk.Frame(self.root, padding="10")
        top_frame.pack(fill=tk.X)

        ttk.Label(top_frame, text="Current Path:").pack(side=tk.LEFT, padx=(0, 5))

        self.path_var = tk.StringVar(value=str(self.current_path))
        path_entry = ttk.Entry(top_frame, textvariable=self.path_var)
        path_entry.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=5)

        # Button frame to keep buttons together
        button_frame = ttk.Frame(top_frame)
        button_frame.pack(side=tk.LEFT, padx=(5, 0))

        ttk.Button(button_frame, text="Go", command=self.navigate_to_path, width=8).pack(side=tk.LEFT, padx=2)
        ttk.Button(button_frame, text="Up", command=self.navigate_up, width=8).pack(side=tk.LEFT, padx=2)
        ttk.Button(button_frame, text="Refresh", command=self.refresh_file_list, width=8).pack(side=tk.LEFT, padx=2)

    # Setup file listing tree view
    def setup_file_list(self):
        self.main_frame = ttk.Frame(self.root, padding="10")
        self.main_frame.pack(fill=tk.BOTH, expand=True)

        left_frame = ttk.Frame(self.main_frame)
        left_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        ttk.Label(left_frame, text="Files and Folders", font=('Arial', 10, 'bold')).pack()

        # Treeview for files
        columns = ('Type', 'Size', 'Modified')
        self.tree = ttk.Treeview(left_frame, columns=columns, show='tree headings')
        self.tree.heading('#0', text='Name')
        self.tree.heading('Type', text='Type')
        self.tree.heading('Size', text='Size')
        self.tree.heading('Modified', text='Modified')

        self.tree.column('#0', width=300)
        self.tree.column('Type', width=80)
        self.tree.column('Size', width=100)
        self.tree.column('Modified', width=150)

        scrollbar = ttk.Scrollbar(left_frame, orient=tk.VERTICAL, command=self.tree.yview)
        self.tree.configure(yscrollcommand=scrollbar.set)

        self.tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        self.tree.bind('<Double-1>', self.on_item_double_click)

    # Setup action buttons panel
    def setup_action_panel(self):
        right_frame = ttk.Frame(self.main_frame, width=200)
        right_frame.pack(side=tk.RIGHT, fill=tk.Y, padx=(10, 0))

        # Maintain width
        right_frame.pack_propagate(False)

        ttk.Label(right_frame, text="Actions", font=('Arial', 10, 'bold')).pack(pady=(0, 10))

        ttk.Button(right_frame, text="Create File", command=self.create_file, width=20).pack(pady=5)
        ttk.Button(right_frame, text="Create Folder", command=self.create_directory, width=20).pack(pady=5)
        ttk.Button(right_frame, text="Open/Edit", command=self.open_file, width=20).pack(pady=5)
        ttk.Button(right_frame, text="Rename", command=self.rename_item, width=20).pack(pady=5)
        ttk.Button(right_frame, text="Delete", command=self.delete_item, width=20).pack(pady=5)
        ttk.Button(right_frame, text="Properties", command=self.show_properties, width=20).pack(pady=5)

    # Setup status bar
    def setup_status_bar(self):
        self.status_var = tk.StringVar(value="Ready")
        status_bar = ttk.Label(self.root, textvariable=self.status_var, relief=tk.SUNKEN, anchor=tk.W)
        status_bar.pack(side=tk.BOTTOM, fill=tk.X)

    # UTILITY METHODS

    # Format file size in human-readable format
    def format_size(self, size):
        for unit in ['B', 'KB', 'MB', 'GB']:
            if size < 1024.0:
                return f"{size:.1f} {unit}"
            size /= 1024.0
        return f"{size:.1f} TB"

    # Get the path of currently selected item
    def get_selected_item_path(self):
        selection = self.tree.selection()
        if not selection:
            return None
        return self.tree.item(selection[0])['tags'][0]

    # FILE LISTING AND NAVIGATION

    # Refresh the file listing using FileOperations
    def refresh_file_list(self):
        try:
            # Clear existing items
            for item in self.tree.get_children():
                self.tree.delete(item)

            # Update path display
            self.path_var.set(str(self.current_path))

            # Get directory listing from file operations
            items = self.file_ops.list_directory(str(self.current_path))

            # Display items
            for item in items:
                item_type = "Folder" if item['is_dir'] else "File"
                size = self.format_size(item['size']) if not item['is_dir'] else ""
                modified = datetime.fromtimestamp(item['modified']).strftime('%Y-%m-%d %H:%M')

                icon = "📁" if item['is_dir'] else "📄"
                self.tree.insert('', tk.END,
                                 text=f"{icon} {item['name']}",
                                 values=(item_type, size, modified),
                                 tags=(item['path'],))

            self.status_var.set(f"Loaded {len(items)} items from {self.current_path}")

        except PermissionError:
            messagebox.showerror("Error", "Permission denied to access this directory")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to load directory: {str(e)}")

    # Handle double click on item
    def on_item_double_click(self, event):
        item_path = self.get_selected_item_path()
        if not item_path:
            return

        path = Path(item_path)
        if path.is_dir():
            self.current_path = path
            self.refresh_file_list()
        else:
            self.open_file()

    # Navigate to parent directory
    def navigate_up(self):
        if self.current_path.parent != self.current_path:
            self.current_path = self.current_path.parent
            self.refresh_file_list()

    # Navigate to the path in the entry
    def navigate_to_path(self):
        try:
            new_path = Path(self.path_var.get())
            if new_path.exists() and new_path.is_dir():
                self.current_path = new_path
                self.refresh_file_list()
            else:
                messagebox.showerror("Error", "Invalid directory path")
        except Exception as e:
            messagebox.showerror("Error", f"Navigation failed: {str(e)}")

    # CRUD OPERATIONS

    # Create a new file - GUI wrapper around FileOperations
    def create_file(self):
        dialog = tk.Toplevel(self.root)
        dialog.title("Create New File")
        dialog.geometry("500x400")
        dialog.transient(self.root)

        ttk.Label(dialog, text="File Name:").pack(pady=(10, 0))
        name_entry = ttk.Entry(dialog, width=50)
        name_entry.pack(pady=5)
        name_entry.focus()

        ttk.Label(dialog, text="File Content:").pack(pady=(10, 0))
        content_text = scrolledtext.ScrolledText(dialog, width=60, height=15)
        content_text.pack(pady=5, padx=10)

        def save_file():
            name = name_entry.get().strip()
            if not name:
                messagebox.showerror("Error", "Please enter a file name")
                return

            file_path = self.current_path / name
            content = content_text.get("1.0", tk.END)

            try:
                # Use FileOperations to create file
                self.file_ops.create_file(str(file_path), content)
                self.refresh_file_list()
                self.status_var.set(f"Created file: {name}")
                dialog.destroy()
                messagebox.showinfo("Success", f"File '{name}' created successfully")
            except FileExistsError as e:
                messagebox.showerror("Error", str(e))
            except Exception as e:
                messagebox.showerror("Error", str(e))

        ttk.Button(dialog, text="Create", command=save_file).pack(pady=10)

    # Create a new directory - GUI wrapper around FileOperations
    def create_directory(self):
        name = simpledialog.askstring("Create Folder", "Enter folder name:")

        if not name:
            return

        dir_path = self.current_path / name

        try:
            # Use FileOperations to create directory
            self.file_ops.create_directory(str(dir_path))
            self.refresh_file_list()
            self.status_var.set(f"Created folder: {name}")
            messagebox.showinfo("Success", f"Folder '{name}' created successfully")
        except FileExistsError as e:
            messagebox.showerror("Error", str(e))
        except Exception as e:
            messagebox.showerror("Error", str(e))

    # Open and edit a file - GUI wrapper around FileOperations
    def open_file(self):
        item_path = self.get_selected_item_path()
        if not item_path:
            messagebox.showwarning("Warning", "Please select a file to open")
            return

        path = Path(item_path)
        if path.is_dir():
            messagebox.showwarning("Warning", "Cannot open a folder. Use double-click to navigate.")
            return

        # Create editor window
        editor = tk.Toplevel(self.root)
        editor.title(f"Editing: {path.name}")
        editor.geometry("700x500")

        text_widget = scrolledtext.ScrolledText(editor, width=80, height=25)
        text_widget.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        # Use FileOperations to read file
        try:
            content = self.file_ops.read_file(str(path))
            text_widget.insert("1.0", content)
        except UnicodeDecodeError:
            messagebox.showwarning("Warning", "This appears to be a binary file.")
            editor.destroy()
            return
        except Exception as e:
            messagebox.showerror("Error", str(e))
            editor.destroy()
            return

        def save_changes():
            try:
                new_content = text_widget.get("1.0", tk.END)
                # Use FileOperations to update file
                self.file_ops.update_file(str(path), new_content)
                self.status_var.set(f"Saved changes to: {path.name}")
                messagebox.showinfo("Success", "File saved successfully")
                self.refresh_file_list()
            except Exception as e:
                messagebox.showerror("Error", str(e))

        button_frame = ttk.Frame(editor)
        button_frame.pack(fill=tk.X, padx=10, pady=(0, 10))

        ttk.Button(button_frame, text="Save", command=save_changes).pack(side=tk.LEFT, padx=5)
        ttk.Button(button_frame, text="Close", command=editor.destroy).pack(side=tk.LEFT)

    # Rename a file or directory - GUI wrapper around FileOperations
    def rename_item(self):
        item_path = self.get_selected_item_path()
        if not item_path:
            messagebox.showwarning("Warning", "Please select an item to rename")
            return

        old_name = Path(item_path).name
        new_name = simpledialog.askstring("Rename", f"Rename '{old_name}' to:",
                                          initialvalue=old_name)

        if not new_name or new_name == old_name:
            return

        try:
            # Use FileOperations to rename
            new_path = self.file_ops.rename_item(item_path, new_name)
            self.refresh_file_list()
            self.status_var.set(f"Renamed '{old_name}' to '{new_name}'")
            messagebox.showinfo("Success", f"Renamed to '{new_name}'")
        except FileExistsError as e:
            messagebox.showerror("Error", str(e))
        except Exception as e:
            messagebox.showerror("Error", str(e))

    # Delete a file or directory - GUI wrapper around FileOperations
    def delete_item(self):
        item_path = self.get_selected_item_path()
        if not item_path:
            messagebox.showwarning("Warning", "Please select an item to delete")
            return

        path = Path(item_path)
        item_type = "folder" if path.is_dir() else "file"

        # Confirmation dialog
        confirm = messagebox.askyesno("Confirm Delete",
                                      f"Are you sure you want to delete the {item_type}:\n'{path.name}'?",
                                      icon='warning')

        if not confirm:
            return

        try:
            # Use FileOperations to delete
            if path.is_dir():
                self.file_ops.delete_directory(str(path))
            else:
                self.file_ops.delete_file(str(path))

            self.refresh_file_list()
            self.status_var.set(f"Deleted: {path.name}")
            messagebox.showinfo("Success", f"{item_type.capitalize()} deleted successfully")
        except Exception as e:
            messagebox.showerror("Error", str(e))

    # Show properties of selected item - GUI wrapper around FileOperations
    def show_properties(self):
        item_path = self.get_selected_item_path()
        if not item_path:
            messagebox.showwarning("Warning", "Please select an item to view properties")
            return

        try:
            # Use FileOperations to get properties
            props = self.file_ops.get_item_properties(item_path)

            props_window = tk.Toplevel(self.root)
            props_window.title(f"Properties: {props['name']}")
            props_window.geometry("400x300")
            props_window.transient(self.root)

            info_frame = ttk.Frame(props_window, padding="20")
            info_frame.pack(fill=tk.BOTH, expand=True)

            properties = [
                ("Name:", props['name']),
                ("Type:", "Folder" if props['is_dir'] else "File"),
                ("Location:", props['parent']),
                ("Size:", self.format_size(props['size']) if not props['is_dir'] else "---"),
                ("Created:", datetime.fromtimestamp(props['created']).strftime('%Y-%m-%d %H:%M:%S')),
                ("Modified:", datetime.fromtimestamp(props['modified']).strftime('%Y-%m-%d %H:%M:%S')),
                ("Accessed:", datetime.fromtimestamp(props['accessed']).strftime('%Y-%m-%d %H:%M:%S')),
            ]

            for i, (label, value) in enumerate(properties):
                ttk.Label(info_frame, text=label, font=('Arial', 9, 'bold')).grid(row=i, column=0, sticky=tk.W, pady=5)
                ttk.Label(info_frame, text=value, wraplength=250).grid(row=i, column=1, sticky=tk.W, pady=5,
                                                                       padx=(10, 0))

            ttk.Button(props_window, text="Close", command=props_window.destroy).pack(pady=10)

        except Exception as e:
            messagebox.showerror("Error", f"Failed to get properties: {str(e)}")