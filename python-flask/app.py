from flask import Flask, request
app = Flask(__name__)

@app.route('/event-invoke', methods = ['POST'])
def invoke():
    # Get all the HTTP headers from the official documentation of Tencent
    request_id = request.headers.get("X-Scf-Request-Id", "")
    print("SCF Invoke RequestId: " + request_id)

    event = request.get_data()
    event_str = event.decode("utf-8")

    return "Hello from SCF event function, your input: " + event_str + ", request_id: " + request_id + "\n"

@app.route('/web-invoke/python-flask-http', methods = ['POST','GET'])
def web_invoke():
    # Get all the HTTP headers from the official documentation of Tencent
    request_id = request.headers.get("X-Scf-Request-Id", "")
    print("SCF Invoke RequestId: " + request_id)
    
    event = request.get_data()
    event_str = event.decode("utf-8")

    return "Hello from SCF Web function, your input: " + event_str + ", request_id: " + request_id + "\n"

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=9000)
