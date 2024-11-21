from flask import Flask, jsonify
from app import app

@app.route('/')
def index():
    return jsonify({'status': 200, 'message': 'API is up.'})

@app.route('/predict', methods=['POST'])
def predict():
    return jsonify({'status': 200, 'prediction': ''})