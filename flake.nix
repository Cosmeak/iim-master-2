{
  description = "Python development environment";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.11";
  outputs = { self, nixpkgs }:
    let
      forAllSystems = nixpkgs.lib.genAttrs [ "x86_64-linux" "aarch64-darwin" ];
    in
    {

      devShells = forAllSystems (system: 
        let 
          pkgs = import nixpkgs { inherit system; };
          pythonPackages = pkgs.python3Packages;
        in
          {
          default = pkgs.mkShell ({
            venvDir = "./.venv";

            buildInputs = with pythonPackages; [
              python
              pip
              venvShellHook
            ];

            postVenvCreation = ''
              unset SOURCE_DATE_EPOCH
              pip install -r requirements.txt
            '';
            postShellHook = ''
              unset SOURCE_DATE_EPOCH
              export FLASK_APP=run.py
              flask run --debug
            '';
          });
        }
      );
    };
}