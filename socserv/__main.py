import threading

from PyQt5 import QtWidgets
import sys
from PyQt5.QtWidgets import QDialog
import UI
import socket

gui=None
def main():
    app = QtWidgets.QApplication(sys.argv)

    gui = UI.MainUi()
    gui.show()
    sys.exit(app.exec_())


if __name__ == '__main__':

    main()
    
    