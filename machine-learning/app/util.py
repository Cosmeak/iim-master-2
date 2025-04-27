from app import app
import io
import torchvision.transforms as transforms 
import torch
from PIL import Image

model = torch.jit.load("./model/model.pt")

def transform_image(image_bytes):
    transformation = transforms.Compose([
        transforms.Resize(28), 
        transforms.CenterCrop(28),
        transforms.Grayscale(num_output_channels=1),
        transforms.ToTensor()
    ])
    return transformation(Image.open(io.BytesIO(image_bytes))).unsqueeze(0)

def get_prediction(image_bytes):
    tensor = transform_image(image_bytes)
    outputs = model.forward(tensor)
    _, y_hat = outputs.max(1)
    return y_hat.item()

