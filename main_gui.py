from tkinter import (
    FALSE,
    Button,
    E,
    Label,
    Menu,
    N,
    S,
    StringVar,
    Tk,
    W,
    filedialog,
    messagebox,
    ttk,
)
import threading

from memory_reader import MemoryReader

reader = MemoryReader()
reader.getProcess()
proc_open = False

def read_config():
    config = reader.readConfig()
    if config is not None:
        print("Config Values: ")
        for b in config.buttons:
            print(hex(b))

def wait_for_process():
    global reader, proc_open
    while True:
        if proc_open != reader.hasWorkingPID():
            proc_open = not proc_open
            if proc_open:
                loadstatus.set("+R Open!")
            else:
                loadstatus.set("+R Closed!")



# define ui-activated methods here
def import_recording(*args):

    filename = filedialog.askopenfilename(
        filetypes=[["AC+R Recording", ".acrec"]],
        title="Import Recording",
        defaultextension=".acrec",
    )
    if filename != "":
        yesno_answer = messagebox.askyesno(
            message="Are you sure you want to overwrite the current ingame recording?",
            icon="warning",
            title="Overwrite?",
        )

        if yesno_answer:
            messagebox.showinfo("You hit yes.", "RIP old recording.")
            loadstatus.set("Current recording: {}".format(filename))
            # do what you will with the filename and its data
        elif not yesno_answer:
            messagebox.showinfo("You hit no.", "Save the recordings!")


def export_recording(*args):

    filename = filedialog.asksaveasfilename(
        filetypes=[["AC+R Recording", ".acrec"]],
        confirmoverwrite=True,
        title="Export Recording",
        defaultextension=".acrec",
    )

    if filename != "":
        # write data to file
        messagebox.showinfo(
            title="Export Successful",
            message="Exported current recording to {}".format(filename),
        )


root = Tk()
root.title("+R Recording Manager")
root.option_add("*tearOff", FALSE)

loadstatus = StringVar()
loadstatus.set(
    "+R Closed!"
)  # may want to adjust based on game open-ness among other things

root.geometry("400x400+200+200")

menubar = Menu(root)
root["menu"] = menubar
menu_file = Menu(menubar)
menubar.add_cascade(menu=menu_file, label="File")
menu_file.add_command(label="Import...", command=import_recording)
menu_file.add_command(label="Export", command=export_recording)

mainframe = ttk.Frame(root, padding="3 3 12 12")
mainframe.grid(column=0, row=0, sticky=(N, W, E, S))
root.rowconfigure(0, weight=1)
root.columnconfigure(0, weight=1)

Button(mainframe, text="Read Config", command=read_config).grid(
    column=1, row=2, sticky=(N, W, E)
)
Label(mainframe, textvariable=loadstatus).grid(column=1, row=1, sticky=(N, W, E))

proc_status_t = threading.Thread(target=wait_for_process)
proc_status_t.daemon = True
proc_status_t.start()

root.mainloop()
# anything that executes after mainloop is post-termination