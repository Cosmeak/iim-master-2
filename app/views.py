from flask import jsonify, request
from app import app
from .util import get_prediction

@app.route('/')
def index():
    return jsonify({'status': 200, 'message': 'API is up.'})

@app.route('/predict', methods=['POST'])
def predict():
    file = request.files['file']
    img_bytes = file.read()
    pred = get_prediction(img_bytes)
    return jsonify({'status': 200, 'pred': pred})