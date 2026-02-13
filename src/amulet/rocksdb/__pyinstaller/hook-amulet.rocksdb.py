from PyInstaller.utils.hooks import collect_data_files, collect_submodules

hiddenimports = collect_submodules("rocksdb")
datas = collect_data_files("rocksdb")
