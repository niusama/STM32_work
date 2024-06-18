import os
import random
import re
import threading
from os.path import exists
import matplotlib.pyplot as plt
import requests
import datetime
from PyQt5 import QtCore, QtWidgets
import qtawesome
from PyQt5.QtCore import QUrl
from PyQt5.QtGui import QPalette, QBrush, QPixmap, QFont
from PyQt5.QtMultimedia import QMediaContent, QMediaPlayer
from PyQt5.QtWidgets import QFileDialog, QMessageBox, QDialog, QFrame, QVBoxLayout, QLineEdit, QPushButton
import socket
import pymysql

# from py_exp.seluminum import get_html_src, parse_html_page

tcpmessage=[]
conne=socket.socket()
# 弹窗提示
def pop_prompt(title, str):
    msg_box = QMessageBox(QMessageBox.Warning, title, str)
    msg_box.exec_()


class MainUi(QtWidgets.QMainWindow, QDialog):

    def __init__(self):
        # 登录界面
        super().__init__()
        self.tcpmessage=[]
        self.singers_name = []
        self.singers_url = []
        self.player = QMediaPlayer(self)
        self.playing = False
        self.play_index_now = -1
        self.singer = []
        self.Headers = {
            "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) "
                          "Chrome/119.0.0.0 Safari/537.36 Edg/119.0.0.0 "
        }

        self.init_ui()

    def init_ui(self):
        self.setFixedSize(960, 700)
        self.setWindowTitle('Music')

        # 创建窗口主部件
        self.main_widget, self.main_layout = self.build_widget()
        # 创建左侧部件
        self.left_widget, self.left_layout = self.build_widget('left_widget')
        # 创建右侧部件
        self.right_widget, self.right_layout = self.build_widget('right_widget')

        self.setCentralWidget(self.main_widget)  # 设置窗口主部件
        self.main_layout.addWidget(self.left_widget, 0, 0, 12, 2)  # 左侧部件在第0行第0列
        self.main_layout.addWidget(self.right_widget, 0, 2, 12, 10)  # 右侧部件在第0行第3列



        # 右侧顶部搜索框部件
        self.right_bar_widget, self.right_bar_layout = self.build_widget()
        # 图标
        self.search_icon = QtWidgets.QLabel(chr(0xf002) + ' ' + '消息')
        self.search_icon.setFont(qtawesome.font('fa', 20))
        # 输入框
        self.search_label = QtWidgets.QLineEdit()
        self.search_label.setPlaceholderText("输入歌手、歌曲或用户，回车进行搜索")
        self.search_label.returnPressed.connect(
            lambda: self.search(self.search_label.text()))

        self.right_bar_layout.addWidget(self.search_icon, 0, 0, 1, 1)
        self.right_bar_layout.addWidget(self.search_label, 0, 1, 1, 8)
        self.right_layout.addWidget(self.right_bar_widget, 0, 0, 1, 9)

        self.search_result = QtWidgets.QLabel("")
        self.search_result.setObjectName('right_label')
        self.right_layout.addWidget(self.search_result, 2, 0, 1, 5)

        self.operator_label = QtWidgets.QLabel("")
        self.operator_label.setObjectName('right_label')
        self.right_layout.addWidget(self.operator_label, 2, 5, 1, 3)

        # 搜索歌曲部件
        self.result_widget, self.result_layout = self.build_widget()

        #隐藏按钮
        jiayou=self.build_button(self.result_layout, 0, 1, event=lambda: self.play_music(0))
        jiayou.setText("加油啊！")
        # self.search("陈奕迅")

        # 播放歌单部件
        self.operator_widget, self.operator_layout = self.build_widget('')

        self.right_layout.addWidget(self.result_widget, 3, 0, 1, 5)
        self.right_layout.addWidget(self.operator_widget, 3, 5, 1, 3)

        self.process_bar = QtWidgets.QProgressBar()  # 播放进度部件
        self.process_value = 0
        self.process_bar.setRange(0, 101)
        self.process_bar.setValue(self.process_value)
        self.process_bar.setFixedHeight(3)  # 设置进度条高度
        self.process_bar.setTextVisible(False)  # 不显示进度条文字


        # 播放控制部件
        self.console_widget, self.console_layout = self.build_widget()
        self.console_button_1 = self.build_button(self.console_layout, 0, 0, event=self.pre_music,
                                                  icon=qtawesome.icon('fa.backward', color='red'))
        self.console_button_1.setText("开始")
        #self.console_button_1 = self.build_button(self.result_layout, 0, 0,
        #                                           event=self.pre_music)
        #self.console_button_1.setText("开始")
        self.console_button_2 = self.build_button(self.console_layout, 0, 2, event=self.next_music,
                                                  icon=qtawesome.icon('fa.forward', color='red'))
        self.console_button_2.setText("保存")
        #self.console_button_3 = self.build_button(self.console_layout, 0, 1, event=self.play_music_by_button,
        #                                          icon=qtawesome.icon('fa.play', color='red', font=18),
        #                                          icon_size=QtCore.QSize(30, 30))

        self.console_layout.setAlignment(QtCore.Qt.AlignCenter)  # 设置布局内部件居中显示
        self.right_layout.addWidget(self.process_bar, 9, 0, 1, 9)
        self.right_layout.addWidget(self.console_widget, 10, 0, 1, 9)

        self.main_layout.setSpacing(0)  # 设置内部控件间距

        self.left_widget.setStyleSheet('''
            QPushButton{border:none;color:white;}
            QPushButton#left_label{
                border:none;
                border-bottom:1px solid white;
                font-size:18px;
                font-weight:700;
            }
            QPushButton#left_button:hover{border-left:4px solid red;font-weight:700;}
            QWidget#left_widget{
                background:gray;
                border-top:1px solid white;
                border-bottom:1px solid white;
                border-left:1px solid white;
                border-top-left-radius:10px;
                border-bottom-left-radius:10px;
            }''')

        self.search_label.setStyleSheet(
            '''QLineEdit{
                border:1px solid gray;
                width:300px;
                border-radius:10px;
                padding:8px 4px;
        }''')

        self.right_widget.setStyleSheet('''
            QWidget#right_widget{
                color:#232C51;
                background:white;
                border-top:1px solid darkGray;
                border-bottom:1px solid darkGray;
                border-right:1px solid darkGray;
                border-top-right-radius:10px;
                border-bottom-right-radius:10px;
            }
            QLabel#right_label{
                border:none;
                font-size:20px;
                font-weight:700;

                text-align:center
            }
        ''')

        self.operator_widget.setStyleSheet(
            '''
                QToolButton{border:none; margin-left:15px}
                QToolButton:hover{border-bottom:2px solid #F76677;}
            ''')

        self.result_widget.setStyleSheet('''
            QPushButton{
                border:none;
                color:gray;
                font-size:18px;
                height:36px;
                padding-left:5px;
                padding-right:10px;
                text-align:left;
            }
            QPushButton:hover{
                color:black;
                border:1px solid #F3F3F5;
                border-radius:10px;
                background:LightGray;
            }
        ''')

        self.process_bar.setStyleSheet('''
            QProgressBar::chunk {
                background-color: red;
            }
        ''')

        self.console_widget.setStyleSheet('''
            QPushButton{
                border:none;
            }
        ''')
    #-*- coding:utf-8 -*-
    def play_music(self, num):

        global conne
        conne.send(b'\x10\x11\x12\x15\x16')
        print(90)
    def getmessage(sec):
        global conne
        socket_server = socket.socket()
        socket_server.bind(("192.168.76.186", 8088))
        # 监听端口
        socket_server.listen(1)
        # 等待客户端连接，accept方法返回二元元组(连接对象, 客户端地址信息)
        print(f"服务端已开始监听，正在等待客户端连接...")
        conn, address = socket_server.accept()
        print(type(conn))
        conne = conn
        print(f"接收到了客户端的连接，客户端的信息：{address}")

        # 接受客户端信息，使用客户端和服务端的本次连接对象，而非socket_server
        while True:
            # 接收消息
            #data: str = conn.recv(1024).decode("gbk")
            #print(f"客户端发来的消息是：{data}")
            global tcpmessage

            tcpstr=conn.recv(1024).decode("GBK")
            #tcpstr = conn.recv(1024).decode("utf-8")
            #tcpstr = conn.recv(1024)
            dt=datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            print("the time is ",dt)
            print("the message is ", tcpstr)
            tcpmessage.append((tcpstr,dt))
            # 回复消息
            msg = "i got !"
            if msg == 'exit':
                break
            conn.send(msg.encode("gbk"))  # encode将字符串编码为字节数组对象

        # 关闭连接
        conn.close()
        socket_server.close()
    
    t=threading.Thread(target=getmessage,args=(random.random(),))
    t.start()








    # 暂停-开始按钮✔
    def play_music_by_button(self):  # 播放按钮设置
        # 未选择歌曲，默认播放第一首
       # this is the place need to compelete
       self.search_label.setPlaceholderText("hello")
       print(1)

    # 前一首✔
    def pre_music(self):  # 播放前一首
        print(1)
        global tcpmessage
        if len(tcpmessage)==0:
            pop_prompt('温馨提示','no message')
            return  
        self.search_label.setPlaceholderText(str(tcpmessage[-1][0]))

    # 后一首✔ jump
    def next_music(self):  # 播放后一首
        global tcpmessage
        if len(tcpmessage)==0:
            pop_prompt('温馨提示','no message')
            return 

        host = 'localhost'
        user = 'root'
        password = '1735585597'
        db = 'py_exp'

            #连接数据库
        try:
            conn = pymysql.connect(host=host, user=user, password=password, db=db)
            cur = conn.cursor()
        except:
            pop_prompt('温馨提示', '数据库连接错误')
            exit(1)

        #执行sql语句
        for i in tcpmessage:
            sql = ('insert into tcpmessage values("%s", "%s")' %
               (str(i[0]),str(i[1])))
            print(i[0])
            print(i[1])
            print(type(i[1]))
            try:
                cur.execute(sql)
                conn.commit()
                print("save correct")
            except:
                conn.rollback()
                cur.close()
                conn.close()
            










    # 创建部件✔
    def build_widget(self, name=''):
        widget = QtWidgets.QWidget()
        if name:
            widget.setObjectName(name)
        layout = QtWidgets.QGridLayout()  # 创建部件的网格布局
        widget.setLayout(layout)  # 设置部件布局为网格布局
        return widget, layout

    # 创建按钮✔
    def build_button(self, layout, frow=0, fcol=0, rows=0, cols=0, event=None, text='', name='',
                     icon=None, icon_size=None):
        # button=None
        if icon:
            button = QtWidgets.QPushButton(icon, text)
        else:
            button = QtWidgets.QPushButton(text)
        if name:
            button.setObjectName(name)
        if event:
            button.clicked.connect(event)
        if icon_size:
            button.setIconSize(icon_size)
        if rows:
            layout.addWidget(button, frow, fcol, rows, cols)
        else:
            layout.addWidget(button, frow, fcol)
        return button

    # 创建工具按钮✔
    def build_tool_button(self, text, layout, frow, fcol, event=None, icon_name='', icon_color='', icon_size=None,
                          style=0):
        button = QtWidgets.QToolButton()
        button.setText(text)
        if event:
            button.clicked.connect(event)
        if icon_name:
            button.setIcon(qtawesome.icon(icon_name, color=icon_color))
        if icon_size:
            button.setIconSize(icon_size)

        button.setToolButtonStyle(style)
        layout.addWidget(button, frow, fcol)
        return button


class LoginDialog(QDialog):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self.setFixedSize(960, 700)
        self.setWindowTitle('登录')

        # 设置窗口背景
        palette = QPalette()
        palette.setBrush(QPalette.Background, QBrush(QPixmap('1.jpg')))
        self.setPalette(palette)

        # 设置界面控件
        self.frame = QFrame(self)
        self.frame.move(260, 110)
        self.mainLayout = QVBoxLayout(self.frame)

        # 设置输入框
        self.edit_name = self.set_inputbox("username")
        self.edit_pwd = self.set_inputbox("password")

        # 设置按钮
        self.set_button("Login")
        self.set_button("Register")

        # 设置控件间距
        self.mainLayout.setSpacing(60)

    

    

    # 设置输入框
    def set_inputbox(self, text):
        # 设置输入框
        edit = QLineEdit(self)  # 创建输入框
        edit.setPlaceholderText(text)  # 设置默认文字
        edit.setFont(QFont('微软雅黑', 22))  # 设置字体和大小
        if text == "password":
            edit.setEchoMode(QLineEdit.Password)
        self.mainLayout.addWidget(edit)  # 将部件加入布局
        return edit

    # 设置按钮
    def set_button(self, text):
        btn = QPushButton(text)  # 按钮值设置Login
        btn.setFont(QFont('Microsoft YaHei', 22))  # 设置字体和大小
        self.mainLayout.addWidget(btn)  # 将部件加入布局

        # 绑定按钮事件
        if text == "Login":
            btn.clicked.connect(self.login)
        elif text == "Register":
            btn.clicked.connect(self.register)
        # return self.btn

    def get_username(self):
        return self.username
