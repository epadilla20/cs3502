import tkinter as tk
from gui import FileManagerGUI

# Main function to initialize and run the file manager application
def main():

    # Create root window
    root = tk.Tk()

    # Initialize the file manager GUI
    app = FileManagerGUI(root)

    # Start the GUI event loop
    root.mainloop()


if __name__ == "__main__":
    main()