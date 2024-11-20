{
  description = "Python development environment";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.05";

  outputs = { self, nixpkgs }:
    let
      forAllSystems = nixpkgs.lib.genAttrs [ "x86_64-linux" "aarch64-darwin" ];
    in
    {
      devShells = forAllSystems (system: 
        let 
          pkgs = import nixpkgs { inherit system; };
          pythonPackages = pkgs.python311Packages;
        in
          {
          default = pkgs.mkShell ({
            venvDir = "./.venv";

            buildInputs = [
              pythonPackages.python
              pythonPackages.pip
              pythonPackages.venvShellHook

              # Jupyter Notebook needed dependencies
              pkgs.stdenv.cc.cc.lib
              pythonPackages.ipykernel
              pythonPackages.jupyterlab
              pythonPackages.ipython

              # Project dependencies
              pythonPackages.torch
              pythonPackages.torchvision
              pythonPackages.onnx
              pythonPackages.tensorboard
              pythonPackages.torchmetrics
              pythonPackages.matplotlib
              pythonPackages.numpy 
              pythonPackages.ipykernel
              pythonPackages.jupyterlab
            ];
            postVenvCreation = ''
              unset SOURCE_DATE_EPOCH
              
              python -m ipykernel install --user
              pip install -r requirements.txt
            '';
            postShellHook = ''
              unset SOURCE_DATE_EPOCH
              jupyter server

            '';
          });
        }
      );
    };
}