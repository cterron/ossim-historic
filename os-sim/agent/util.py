import string

def normalizeWhitespace(text):
    "Remove redundant whitespace from a string"
    return string.join(string.split(text), ' ')


def debug(module, message, mark = "", color = ""):
   
    msg = ""
   
    if color:
        if color == 'RED':      msg += "\033[01;31m"
        elif color == 'GREEN':  msg += "\033[01;32m"
        elif color == 'YELLOW': msg += "\033[01;33m"
        elif color == 'BLUE':   msg += "\033[01;34m"
        elif color == 'PURPLE': msg += "\033[01;35m"
        elif color == 'CYAN':   msg += "\033[01;36m"

    if mark:
        msg += " (%s) \033[00m" % (mark)

    # print debug message
    msg += " %s:\t%s" % (module, message)
    print msg

