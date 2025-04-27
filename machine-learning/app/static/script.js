
let canvas, context;
let isDrawing = false;

document.addEventListener('DOMContentLoaded', () => {
    // Initialize canvas and context
    canvas = document.getElementById('drawingCanvas');
    context = canvas.getContext('2d');
    context.lineWidth = 15;
    context.lineCap = 'round';
    context.strokeStyle = 'white';
    context.fillStyle = 'black';
    context.fillRect(0, 0, canvas.width, canvas.height);

    // Set up event listeners for drawing on the canvas
    canvas.addEventListener('mousedown', startDrawing);
    canvas.addEventListener('mousemove', draw);
    canvas.addEventListener('mouseup', stopDrawing);
    canvas.addEventListener('mouseleave', stopDrawing);

    // Set up clear canvas button
    document.getElementById('clearButton').addEventListener('click', clearCanvas);
});

function startDrawing(e) {
    context.beginPath();
    context.moveTo(e.offsetX, e.offsetY);
    isDrawing = true;
}

function draw(e) {
    if (!isDrawing) return;
    context.lineTo(e.offsetX, e.offsetY);
    context.stroke();
}

function stopDrawing() {
    isDrawing = false;
    context.closePath();
    makePrediction();
}

function clearCanvas() {
    context.fillStyle = 'black';
    context.fillRect(0, 0, canvas.width, canvas.height);
    document.getElementById('predictionResult').textContent = '';
}

async function makePrediction() {
    const imageBlob = await getCanvasBlob();
    const formData = new FormData();
    formData.append('file', imageBlob, 'drawing.png');

    try {
        const response = await fetch('http://127.0.0.1:5000/predict', {
            method: 'POST',
            body: formData
        });
        
        if (response.ok) {
            const data = await response.json();
            displayPrediction(data.pred);
        } else {
            console.error("Error with prediction API");
        }
    } catch (error) {
        console.error("Error making prediction:", error);
    }
}

function getCanvasBlob() {
    return new Promise((resolve) => {
        canvas.toBlob(resolve, 'image/png');
    });
}

function displayPrediction(prediction) {
    const resultDiv = document.getElementById('predictionResult');
    resultDiv.innerHTML = `<h2>Prédiction : ${prediction}</h2>`;
}
