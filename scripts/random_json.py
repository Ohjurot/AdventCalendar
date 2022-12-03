import random
import json 
import tkinter.filedialog 

if __name__ == "__main__":
    # Input
    numberOfDays = int(input("Number of days: "))
    numberOfRows = int(input("Number of rows: "))
    numberOfColums = int(input("Number of columns: "))

    # Warning
    if numberOfRows * numberOfColums != numberOfDays:
        print("Warning: Will produce invalid json! (numberOfRows * numberOfColums != numberOfDays)")

    # Generate days
    days = [f"{i}" for i in range(1, numberOfDays + 1)]
    random.shuffle(days)

    # Generate json header
    data = { 
        "positions": {},
        "display": {
            "columns": numberOfColums, 
            "rows": numberOfRows
        }    
    }

    # Generate json data
    px = 0
    py = 0
    for day in days:
        data["positions"][f"{day}"] = {"x": px, "y": py }
        px += 1
        if(px == numberOfColums):
            px = 0
            py += 1

    # Write to file
    path = tkinter.filedialog.asksaveasfile(mode='w', defaultextension=".json", filetypes=(("JSON File", "*.json"),("All Files", "*.*")))
    with open(path.name, 'w', encoding='utf-8') as f:
        json.dump(data, f, ensure_ascii=False)
