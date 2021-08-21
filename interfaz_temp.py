import tkinter as tk
import socket
import sys
import time

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
wind = tk.Tk()
wind.title("Proyecto de temperatura")
wind.geometry("400x200")

# Asignando el puerto a la comunicacion
server_address = ('192.168.0.9', 8000) #Windows. Buscar con ipconfig la direccion ipv4 #Linux desde ventana
sock.bind(server_address)
dest_addr = ("192.168.101.3", 5000)

def buttonact():
    data, address = sock.recvfrom(4096)
    temp = data.decode("ascii")
    txt_temp.insert(0,temp)

def Borrar():
    txt_temp.delete(0, tk.END)

labelgc = tk.Label(wind, text="Grados Celsius")
labelgc.place(x=240,y=20)
buttonTemp = tk.Button(wind, text="Medir Temperatura", command=buttonact)
buttonTemp.place(x=40,y=60)
buttonClear = tk.Button(wind, text="Borrar medicion", command=Borrar)
buttonClear.place(x=210,y=60)
txt_temp = tk.Entry(wind)
txt_temp.place(x=60,y=20)

wind.mainloop()