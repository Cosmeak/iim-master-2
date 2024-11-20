import torch
import torch.nn as nn
import pytorch_lightning as pl
from torchmetrics.classification import Accuracy
import torch.optim as optim
from torchvision import datasets
from torchvision.transforms import ToTensor
import torch.utils.data as data

class VGGLightning(pl.LightningModule):
    def __init__(self, train_set, valid_set, batch_size=128, num_classes=10):
        super(VGGLightning, self).__init__()
        
        # Define VGG architecture for 28x28 MNIST images (grayscale)
        self.features = nn.Sequential(
            # First block: Conv -> Conv -> MaxPool
            nn.Conv2d(1, 64, kernel_size=3, padding=1),  # 1 input channel (grayscale), 64 output channels
            nn.ReLU(inplace=True),
            nn.Conv2d(64, 64, kernel_size=3, padding=1),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(kernel_size=2, stride=2),  # Output size: 64x14x14

            # Second block: Conv -> Conv -> MaxPool
            nn.Conv2d(64, 128, kernel_size=3, padding=1),  # 64 input channels, 128 output channels
            nn.ReLU(inplace=True),
            nn.Conv2d(128, 128, kernel_size=3, padding=1),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(kernel_size=2, stride=2),  # Output size: 128x7x7

            # Third block: Conv -> Conv -> Conv -> MaxPool
            nn.Conv2d(128, 256, kernel_size=3, padding=1),  # 128 input channels, 256 output channels
            nn.ReLU(inplace=True),
            nn.Conv2d(256, 256, kernel_size=3, padding=1),
            nn.ReLU(inplace=True),
            nn.Conv2d(256, 256, kernel_size=3, padding=1),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(kernel_size=2, stride=2),  # Output size: 256x3x3

            # Fourth block: Conv -> Conv -> Conv -> MaxPool
            nn.Conv2d(256, 512, kernel_size=3, padding=1),  # 256 input channels, 512 output channels
            nn.ReLU(inplace=True),
            nn.Conv2d(512, 512, kernel_size=3, padding=1),
            nn.ReLU(inplace=True),
            nn.Conv2d(512, 512, kernel_size=3, padding=1),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(kernel_size=2, stride=2)  # Output size: 512x1x1
        )
        
        # Fully connected layers
        self.classifier = nn.Sequential(
            nn.Linear(512 * 1 * 1, 4096),  # Adjusted to 512 * 1 * 1 based on input size
            nn.ReLU(inplace=True),
            nn.Dropout(0.5),
            nn.Linear(4096, 4096),
            nn.ReLU(inplace=True),
            nn.Dropout(0.5),
            nn.Linear(4096, num_classes)  # Output 10 classes for MNIST digits
        )

        # Metric for accuracy
        self.accuracy = Accuracy(task="multiclass", num_classes=num_classes)

        # Define batch_size
        self.batch_size = batch_size

        # Define datasets
        self.train_set = train_set
        self.valid_set = valid_set

    def forward(self, x):
        x = self.features(x)
        x = x.view(x.size(0), -1)  # Flatten the output from convolutional layers
        x = self.classifier(x)
        return x

    def train_dataloader(self):
        return DataLoader(self.train_set, batch_size=self.batch_size, num_workers=11, persistent_workers=True)

    def training_step(self, batch, batch_idx):
        # Extract input images and labels
        images, labels = batch
        
        # Forward pass
        outputs = self(images)
        
        # Calculate loss (cross-entropy loss)
        loss = nn.CrossEntropyLoss()(outputs, labels)
        
        # Log training loss
        self.log('train_loss', loss, on_epoch=True)
        
        return loss

    def val_dataloader(self):
        return DataLoader(self.valid_set, batch_size=self.batch_size, num_workers=11, persistent_workers=True)

    def validation_step(self, batch, batch_idx):
        images, labels = batch
        outputs = self(images)
        
        # Calculate loss
        loss = nn.CrossEntropyLoss()(outputs, labels)
        
        # Calculate accuracy
        acc = self.accuracy(outputs, labels)
        
        self.log('val_loss', loss, on_epoch=True)
        self.log('val_acc', acc, on_epoch=True)
        
        return loss

    def test_step(self, batch, batch_idx):
        images, labels = batch
        outputs = self(images)
        
        # Calculate loss
        loss = nn.CrossEntropyLoss()(outputs, labels)
        
        # Calculate accuracy
        acc = self.accuracy(outputs, labels)
        
        self.log('test_loss', loss, on_epoch=True)
        self.log('test_acc', acc, on_epoch=True)
        
        return loss

    def configure_optimizers(self):
        # Define optimizer (e.g., Adam)
        # try with lr=0.001
        optimizer = optim.AdamW(self.parameters())
        
        # Optionally define a scheduler for learning rate (e.g., step scheduler)
        scheduler = {
            'scheduler': torch.optim.lr_scheduler.StepLR(optimizer, step_size=7, gamma=0.1),
            'monitor': 'val_loss'
        }
        
        return [optimizer], [scheduler]

# Example: how to use the model with PyTorch Lightning Trainer
if __name__ == "__main__":
    from torch.utils.data import DataLoader

    # Define datasets
    training_set = datasets.MNIST(root="data", train=True, download=True, transform=ToTensor())
    # use 20% of training data for validation
    train_set_size = int(len(training_set) * 0.8)
    valid_set_size = len(training_set) - train_set_size
    # split the train set into two
    seed = torch.Generator().manual_seed(42)
    train_set, valid_set = data.random_split(training_set, [train_set_size, valid_set_size], generator=seed)

    # Initialize model 
    model = VGGLightning(train_set, valid_set)

    # Create a Trainer object
    trainer = pl.Trainer(default_root_dir="checkpoints/", callbacks=[pl.callbacks.EarlyStopping(monitor="val_loss", mode="min")], max_epochs=-1)

    # Tune batch scale
    # tuner = pl.tuner.Tuner(trainer)
    # tuner.scale_batch_size(model)

    # Train the model
    trainer.fit(model)

    # Validate training
    test_set = datasets.MNIST(root="data", train=False, download=True, transform=ToTensor())
    test_dataloader = DataLoader(test_set, num_workers=11, persistent_workers=True)
    trainer.test(model, test_dataloader)
