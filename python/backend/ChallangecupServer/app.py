from flask import Flask, render_template,jsonify
import ccudp
from flask_mqtt import Mqtt

res = None

app = Flask(__name__)
mqtt_ = Mqtt(app)


app.config['MQTT_BROKER_URL'] = 'localhost'
app.config['MQTT_BROKER_PORT'] = 1883
app.config['MQTT_REFRESH_TIME'] = 1.0  # refresh time in seconds

@app.route('/message')
def message():
    result = res
    return jsonify(result)

@app.route('/')
def hello_world():
    result = ccudp.form()
    return render_template("index.html", result=result)

@app.route('/test')
def mqtt():
    return render_template('mqtt.html')

@mqtt_.on_connect()
def handle_connect(client, userdata, flags, rc):
    mqtt_.subscribe('app/topic')

@mqtt_.on_message()
def handle_mqtt_message(client, userdata, message):
    print("topic:" + message.topic + " message:" + str(message.payload))
    data = dict(
        topic=message.topic,
        payload=message.payload.decode()
    )
    global res
    res = data

if __name__ == '__main__':
    app.run()

