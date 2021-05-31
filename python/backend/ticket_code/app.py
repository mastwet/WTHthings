from flask import Flask,render_template
from flask import request,make_response
import cdks
import code

app = Flask(__name__)
cd = cdks.cdk()


@app.route('/')
def index():
    return render_template('index.html')

@app.route('/gen_cdk/',methods=['GET'])
def generate_cdk():
    number = request.args.get("number")
    try:
        int(number)
    except (ValueError,TypeError):
        if number != "" and number != None:
           return render_template('generate.html', get=False, warning=True)
        else:
            return render_template('generate.html', get=False)
    else:
        if len(str(number))==19:
            cdks = cd.gen_cdk(number)
            code.make_code(cdks[0], cdks[1])
            return render_template('generate.html', get=True, number=number, cdn=cdks[1], cdk=cdks[0],img="image/{name}".format(name=cdks[1]))
        else:
            return render_template('generate.html', get=False)
    finally:
        render_template('generate.html', get=False)

@app.route('/query_cdk/')
def quering_cdk():
    return render_template('query.html')

@app.route("/gen_cdk/image/<string:imageid>")
def image(imageid):
    image_data = open("imgcode/{}.png".format(imageid), "rb").read()
    response = make_response(image_data)
    response.headers['Content-Type'] = 'image/png'
    return response

if __name__ == '__main__':
    app.run()
