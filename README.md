> [!CAUTION] 
> This project is an experimental machine learning model with an web API integration as part of a course i had.

## Features

- Number drawing
- Number recognition
- Exported Trained Model ([/model/model.pt](/model/model.pt))

## Model

This model is trained using the MNIST dataset and used the VGG pattern adapted to the MNIST image size (28*28).

### Stack

- Pytorch Lightning
- Tensorboard

## Web

The web part of this project don't need to shine, just to be functional. This is why we export a API route for prediction and use it in the default web page.

![](/docs/webapp-screenshot.png)

### Stack

- Flask
- HTML
- CSS 
- JS

## Getting Started

Since i'm using Nix as my package manager, this project is also managed by it.

So you can retrieve a `flake.nix` containing all information needed to have a functional installation for this project. 

### Prerequisites

- **Nix Package Manager** (>=2.4)

### Installation

1. Clone repository

```sh
git clone git@github.com:Cosmeak/iim-m2-machine-learning.git & cd iim-m2-machine-learning
```

2. Install dependencies

Each nix shell has it own purpose and own dependencies so make sure to be in the right folder for what you want to do.

```sh
# Web
nix develop -c $SHELL

# Machine Learning Model
cd model & nix develop -c $SHELL
```

3. Test and enjoy

The shell will tell you the url for the web, just open it and try to draw a number. 

In case you want to train the model yourself, have a look inside of the [Jupyter Notebook](/model/model.ipynb).

