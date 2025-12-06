# OwlTech File Manager
CS 3502 - Operating Systems Project 3

A graphical file management system demonstrating file system operations with a user-friendly interface.

## Author
Eliseo Padilla

## Project Structure

```
file-manager/
├── main.py                 # Entry point - runs the application
├── gui.py                  # GUI module - handles user interface
├── file_operations.py      # File operations module - handles file system operations
└── README.md              # This file
```

## Architecture

The application follows Separation of Concerns design pattern:

- **main.py**: Entry point that initializes the application
- **file_operations.py**: Pure file system logic (CRUD operations)
- **gui.py**: User interface layer that uses FileOperations

## Requirements

- Python 3.7 or higher
- tkinter (usually included with Python)

## Installation

1. Download the project files
2. Ensure Python is installed: `python --version`
3. No additional dependencies needed

## Running the Application

```bash
python main.py
```

Or on some systems:
```bash
python3 main.py
```

## Features

### Core Operations (CRUD)
- CREATE - Create new files with content or empty directories
- READ - View file contents in built-in editor
- UPDATE - Edit and save changes to files
- DELETE - Remove files and directories (with confirmation)
- RENAME - Change names of files and folders
- NAVIGATE - Browse directory structure with tree view

### Additional Features
- File properties display (size, timestamps, type)
- Visual file/folder icons
- Path navigation bar
- Comprehensive error handling
- User-friendly status messages
- Confirmation dialogs for destructive operations

## Usage Guide

### Creating Files
1. Click "Create File" button
2. Enter filename
3. Type content in the text area
4. Click "Create"

### Creating Folders
1. Click "Create Folder" button
2. Enter folder name
3. Click OK

### Opening/Editing Files
- Double-click a file to open it
- Or select a file and click "Open/Edit"
- Make changes in the editor
- Click "Save" to save changes

### Navigating
- Double-click folders to enter them
- Click "Up" button to go to parent directory
- Type a path and click "Go" for direct navigation
- Click "Refresh" to reload the current directory

### Deleting Items
1. Select a file or folder
2. Click "Delete" button
3. Confirm the deletion
4. Item is permanently removed

### Renaming Items
1. Select a file or folder
2. Click "Rename" button
3. Enter new name
4. Click OK

### Viewing Properties
1. Select any item
2. Click "Properties" button
3. View detailed information (size, dates, location)

## Testing

The application creates a safe demo directory at ~/FileManagerDemo for testing.

## Error Handling

The application gracefully handles:
- Duplicate file/folder names
- Permission denied errors
- Invalid file names
- Missing files/folders
- Read/write errors
- Non-existent paths

All errors display user-friendly messages instead of crashing.

## Code Organization

### file_operations.py
Contains the FileOperations class with static methods for:
- create_file() - Create new files
- read_file() - Read file contents
- update_file() - Modify existing files
- delete_file() - Remove files
- create_directory() - Create folders
- delete_directory() - Remove folders
- rename_item() - Rename files/folders
- list_directory() - Get directory contents
- get_item_properties() - Get file/folder metadata

### gui.py
Contains the FileManagerGUI class that:
- Sets up all GUI components
- Handles user interactions
- Delegates file operations to FileOperations
- Displays results and errors

### main.py
- Simple entry point
- Initializes Tkinter
- Creates FileManagerGUI instance
- Starts event loop
