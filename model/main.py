import torch
from torch import nn
from torch.utils.data import DataLoader
from torchvision import datasets
from torchvision.transforms import ToTensor
import lightning as L
from lightning.pytorch.tuner import Tuner
import torch.utils.data as data
from lightning.pytorch.callbacks.early_stopping import EarlyStopping

# Define Pytorch Neural Network Module
class NeuralNetwork(nn.Module):
    def __init__(self):
        super().__init__()
        self.flatten = nn.Flatten()
        self.linear_relu_stack = nn.Sequential(
            nn.Linear(28*28, 512),
            nn.ReLU(),
            nn.Linear(512, 512),
            nn.ReLU(),
            nn.Linear(512, 10)
        )

    def forward(self, x):
        x = self.flatten(x)
        logits = self.linear_relu_stack(x)
        return logits


# Define the LightningModule
class LitAutoEncoder(L.LightningModule):
    def __init__(self, model, batch_size, training_set, valid_set):
        super().__init__()
        self.model = model
        self.batch_size = batch_size
        self.training_set = training_set
        self.valid_set = valid_set

    def configure_optimizers(self):
        optimizer = torch.optim.Adam(self.parameters(), lr=1e-3)
        return optimizer

    def train_dataloader(self):
        return DataLoader(self.training_set, batch_size=self.batch_size, num_workers=11, persistent_workers=True)

    def val_dataloader(self):
        return DataLoader(self.valid_set, batch_size=self.batch_size, num_workers=11, persistent_workers=True)

    def training_step(self, batch, batch_idx):
        X, y = batch
        y_hat = self.model(X)
        loss = nn.functional.cross_entropy(y_hat, y)
        # Logging to TensorBoard (if installed) by default
        self.log("train_loss", loss)
        return loss

    def validation_step(self, batch, batch_idx):
        X, y = batch
        y_hat = self.model(X)
        val_loss = nn.functional.cross_entropy(y_hat, y)
        self.log("val_loss", val_loss)

    def test_step(self, batch, batch_idx):
        X, y = batch
        y_hat = self.model(X)
        test_loss = nn.functional.cross_entropy(y_hat, y)
        self.log("test_loss", test_loss)

def main():
    # Define datasets
    training_set = datasets.MNIST(root="data", train=True, download=True, transform=ToTensor())
    test_set = datasets.MNIST(root="data", train=False, download=True, transform=ToTensor())
    # use 20% of training data for validation
    train_set_size = int(len(training_set) * 0.8)
    valid_set_size = len(training_set) - train_set_size
    # split the train set into two
    seed = torch.Generator().manual_seed(42)
    train_set, valid_set = data.random_split(training_set, [train_set_size, valid_set_size], generator=seed)

    # Create our model
    model = NeuralNetwork()
    autoencoder = LitAutoEncoder(model=model, batch_size=2, training_set=train_set, valid_set=valid_set)

    # Train the model
    trainer = L.Trainer(default_root_dir="checkpoints/", callbacks=[EarlyStopping(monitor="val_loss", mode="min")], max_epochs=100)
    tuner = Tuner(trainer)
    tuner.scale_batch_size(autoencoder)
    trainer.fit(autoencoder)

    # Validate training
    trainer.test(autoencoder, DataLoader(test_set, num_workers=11))

if __name__ == "__main__":
    main()
