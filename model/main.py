import pytorch_lightning as pl
from lightning.pytorch.cli import LightningCLI
from models.mnist_vgg_lightning import MNISTVGGLightning
from datamodules.mnist_datamodule import MNISTDataModule

def main():
    cli = LightningCLI(MNISTVGGLightning, MNISTDataModule)

if __name__ == "__main__":
    main()
