import csv


def convert(lst):
    for i in range(len(lst)):
        try:
            lst[i] = float(lst[i])
        except ValueError:
            pass

    return lst


def read(name):
    lines = []

    try:
        with open(name, "r") as f:
            reader = csv.reader(f)
            for l in reader:
                lines.append(convert(l))
    except FileNotFoundError:
        print("mskn")
    except Exception as e:
        print(str(e))

    return lines
