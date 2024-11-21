from flask import jsonify, request
import torchvision.transforms as transforms 
from app import app

@app.route('/')
def index():
    return jsonify({'status': 200, 'message': 'API is up.'})

@app.route('/predict', methods=['POST'])
def predict():
    file = request.data
    transformation = transforms.Compose([
        transforms.Resize(28), 
        transforms.CenterCrop([28, 28]),
        transforms.ToTensor()
    ])
    return jsonify({'status': 200, 'prediction': ''})