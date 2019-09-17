
import pymysql
import paho.mqtt.client as mqtt
import time

x=0
def on_connect(client,userdata,flags,rc):
    print("Conectado - Código de resultado: " + str(rc))

    client.subscribe("estacion/#")
 
    
def on_message(client,userdata,msg):
    global t,t_i,h,p,vv,dv,wr,pm25,pm10,aq,x
    x+=1
    print(msg.topic+" "+str(msg.payload.decode("utf-8")))
    topic=str(msg.topic)
    valor= str(msg.payload.decode("utf-8"))

    if topic =="estacion/temperatura":
        t=float(valor)
        print(t)
    if topic == "estacion/temp_i":
        t_i=float(valor)
        print(t_i)
    if topic =="estacion/humedad":
        h=float(valor)
        print(h)
    if topic == "estacion/presion":
        p=float(valor)
        print(p)
    if topic == "estacion/vel_viento":
        vv=float(valor)
        print(vv)
    if topic == "estacion/dir_viento":
        dv=int(valor)
        print(dv)
    if topic == "estacion/lluvia":
        wr=float(valor)
        print(wr)
    if topic == "estacion/PM2.5":
        pm25=float(valor)
        print(pm25)
    if topic == "estacion/PM10":
        pm10=float(valor)
        print(pm10)
    if topic == "estacion/AQ_color":
        aq=valor
        print(aq)

    sql="INSERT INTO `cosmic` (`ID`, `FECHA`, `TEMPERATURA`, `TEMP_INTERNA`, `HUMEDAD`, `PRESION`, `VEL_VIENTO`, `DIR_VIENTO`, `LLUVIA`, `PM2.5`, `PM10`, `COLOR`) VALUES (NULL, CURRENT_TIMESTAMP,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)"

    if x==10:
        print(x)
        try:
            db=pymysql.connect("localhost","root","powerlab","estacion")
            cursor=db.cursor()
            cursor.execute(sql,(t,t_i,h,p,vv,dv,wr,pm25,pm10,aq))
            db.commit()
            print("guardado db")
    
        except:
            db.rollback()
            print("fallo db")
            db.close()

client = mqtt.Client()
client.on_connect=on_connect
client.on_message = on_message
#client.connect("powerlab.ddns.net")
#client.connect("itson.ddns.net")
client.connect("redmet.ddns.net")
try:
    client.loop_forever()
except KeyboardInterrupt:
    print("Cerrando...")
    db.close()
