from flask import jsonify, request, render_template
from app import app
from .util import get_prediction

@app.route('/')
def index():
    return render_template("index.html")

@app.route('/predict', methods=['POST'])
def predict():
    print(request.files)
    file = request.files['file']
    img_bytes = file.read()
    pred = get_prediction(img_bytes)
    return jsonify({'status': 200, 'pred': pred})