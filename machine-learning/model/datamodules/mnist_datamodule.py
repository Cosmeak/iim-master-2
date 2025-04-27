import pytorch_lightning as pl
from torch.utils.data import DataLoader, random_split
from torchvision import datasets
from torchvision.transforms import ToTensor
import torch
import os

class MNISTDataModule(pl.LightningDataModule):
    def __init__(self, batch_size: int = 128, val_split: float = 0.2, seed=42):
        super().__init__()
        self.save_hyperparameters()
        self.train_set, self.valid_set, self.test_set = None, None, None
        self.num_workers = min(os.cpu_count(), 8)  # Ensure we don't use too many workers
        self.transform = ToTensor() # Define the transformation once

    def prepare_data(self):
        """Download the MNIST dataset only once."""
        datasets.MNIST(root="data", train=True, download=True, transform=self.transform)
        datasets.MNIST(root="data", train=False, download=True, transform=self.transform)

    def setup(self, stage=None):
        """Set up training, validation, and test datasets."""
        # Load training data only once
        if self.train_set is None or self.valid_set is None:
            train_full = datasets.MNIST(root="data", train=True, transform=self.transform)
            
            # Split into train and validation sets
            train_set_size = int(len(train_full) * (1 - self.val_split))
            valid_set_size = len(train_full) - train_set_size
            generator = torch.Generator().manual_seed(self.seed)
            self.train_set, self.valid_set = random_split(train_full, [train_set_size, valid_set_size], generator=generator)
        
        # Load the test set only once
        if self.test_set is None:
            self.test_set = datasets.MNIST(root="data", train=False, download=True, transform=self.transform)

    def train_dataloader(self):
        """Return the train DataLoader."""
        return DataLoader(self.train_set, batch_size=self.batch_size, num_workers=self.num_workers)

    def val_dataloader(self):
        """Return the validation DataLoader."""
        return DataLoader(self.valid_set, batch_size=self.batch_size, num_workers=self.num_workers)

    def test_dataloader(self):
        """Return the test DataLoader."""
        return DataLoader(self.test_set, batch_size=self.batch_size, num_workers=self.num_workers)
